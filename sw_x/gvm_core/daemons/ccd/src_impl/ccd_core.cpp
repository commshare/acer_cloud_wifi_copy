//
//  Copyright 2010 iGware Inc.
//  All Rights Reserved.
//
//  THIS SOFTWARE CONTAINS CONFIDENTIAL INFORMATION AND
//  TRADE SECRETS OF IGWARE INC.
//  USE, DISCLOSURE OR REPRODUCTION IS PROHIBITED WITHOUT
//  THE PRIOR EXPRESS WRITTEN PERMISSION OF IGWARE INC.
//
#include "ccd_core.h"
#include "ccd_core_priv.hpp"

#include "ans_connection.hpp"
#include "base.h"
#include "cache.h"
#include "config.h"
#include "ccd_build_info.h"
#include "ccd_storage.hpp"
#include "ccd_util.hpp"
#include "ccdiquery.h"
#include "DeviceStateCache.hpp"
#include "netman.hpp"
#include "query.h"
#include "gvm_file_utils.h"
#include "sw_update.hpp"
#include "util_open_db_handle.hpp"
#include "vcs_proxy.hpp"
#if CCD_ENABLE_MEDIA_SERVER_AGENT
#include "MediaMetadataServer.hpp"
#endif
#if CCD_ENABLE_DOC_SAVE_N_GO
#include "AsyncDatasetOps.hpp"
#endif

#include <errno.h>
#include <signal.h>
#ifdef _MSC_VER
#include <io.h>
#else
#include <unistd.h>
#endif

#ifdef VPL_PLAT_IS_WIN_DESKTOP_MODE
#include <Windows.h>
#include <Shlwapi.h>
#include <strsafe.h>
#include <Sddl.h>
#endif

#include "EventManagerPb.hpp"
#include "SyncFeatureMgr.hpp"
#include "virtual_device.hpp"
#include "vplex_file.h"
#include "vplex_powerman.h"
#include "vplex_safe_conversion.h"
#include "vplex_scoped_locks.hpp"
#include "vplex_strings.h"
#include "vplex_trace.h"
#include "vpl_fs.h"
#include "vpl_lazy_init.h"
#include "vpl_socket.h"
#include "vpl_net.h"
#include "vpl_th.h"
#include "ccdi_internal_defs.hpp"
#include "scopeguard.hpp"
#ifndef WIN32
#include <sys/socket.h>
#include <netinet/tcp.h>
#endif
#if CCD_USE_SHARED_CREDENTIALS
#include "vplex_shared_credential.hpp"
#endif

#include <string>

static bool isRunning = false;

static VPLLazyInitMutex_t inProgressMutex = VPLLAZYINITMUTEX_INIT;
static VPLLazyInitCond_t inProgressCond = VPLLAZYINITCOND_INIT;
/// Protected by #inProgressMutex.
//@{
/// If true, CCD shutdown has been requested and all new requests should be canceled.
static bool shutdownRequested = false;
/// If greater than 0, a "local login" (local activation with optional infra login) is in progress and
/// it is not currently safe to start any other logins/logouts or perform the actual shutdown steps.
static int inProgressLogin = 0;
/// If greater than 0, a "local logout" (local deactivation) is in progress and
/// it is not currently safe to start any other logins/logouts or perform the actual shutdown steps.
static int inProgressLogout = 0;
/// If greater than 0, a "starts or stops modules" is in progress and
/// it is not currently safe to start any other logins/logouts or perform the actual shutdown steps.
static int inProgressOther = 0;
/// Tracks which thread is currently performing a login or logout operation.
/// This is to detect the case when login calls logout as a subroutine; we must avoid blocking
/// (and deadlocking) since the current thread already has permission to run.
static VPLThreadId_t loginLogoutThreadId; // no init needed; this should never be read before it is assigned.
//@}

static void setShutdownRequested(bool value)
{
    MutexAutoLock lock(VPLLazyInitMutex_GetMutex(&inProgressMutex));
    shutdownRequested = value;
    if (shutdownRequested) {
        int rc = VPLCond_Broadcast(VPLLazyInitCond_GetCond(&inProgressCond));
        if (rc < 0) {
            LOG_ERROR("VPLCond_Broadcast failed: %d", rc);
        }
    }
}

#if CCD_USE_IPC_SOCKET
static bool isShutdownRequested()
{
    MutexAutoLock lock(VPLLazyInitMutex_GetMutex(&inProgressMutex));
    bool result = shutdownRequested;
    return result;
}
#endif

static void waitUntilSafeToShutdown()
{
    VPLMutex_t* mutex = VPLLazyInitMutex_GetMutex(&inProgressMutex);
    MutexAutoLock lock(mutex);
    // It is important that shutdownRequested is true, to prevent any new requests from
    // starting.
    ASSERT(shutdownRequested);
    while ((inProgressLogout > 0) || (inProgressLogin > 0)) {
        LOG_INFO("inProgressLogout=%d, inProgressLogin=%d; waiting.", inProgressLogout, inProgressLogin);
        int rv = VPLCond_TimedWait(VPLLazyInitCond_GetCond(&inProgressCond), mutex, VPL_TIMEOUT_NONE);
        if (rv < 0) {
            LOG_ERROR("VPLCond_TimedWait failed: %d", rv);
            // This isn't expected; just do a sleep to avoid meltdown.
            VPLThread_Sleep(VPLTime_FromMillisec(500));
        }
    }
    LOG_INFO("Safe to begin shutdown");
}

int CCDPrivBeginLogout(bool returnIfLogoutInProgress)
{
    ASSERT(!Cache_ThreadHasLock());
    VPLMutex_t* mutex = VPLLazyInitMutex_GetMutex(&inProgressMutex);
    MutexAutoLock lock(mutex);
    do {
        if (shutdownRequested) {
            LOG_INFO("Shutdown in progress!");
            return CCD_ERROR_SHUTTING_DOWN;
        }
        if (returnIfLogoutInProgress && (inProgressLogout > 0)) {
            LOG_INFO("Logout in progress!");
            return CCD_ERROR_ALREADY;
        }
        
        if (((inProgressLogout == 0) && (inProgressLogin == 0) && (inProgressOther == 0)) ||
                // Detect the case when login calls logout as a subroutine; we must avoid blocking
                // (and deadlocking), since the current thread already has permission to run.
            (VPLDetachableThread_Self() == loginLogoutThreadId)
           )
        {
            LOG_INFO("Safe to begin logout");
            inProgressLogout++;
            loginLogoutThreadId = VPLDetachableThread_Self();
            return 0;
        }
        LOG_INFO("inProgressLogout=%d, inProgressLogin=%d, inProgressOther=%d; waiting.", inProgressLogout, inProgressLogin, inProgressOther);
        int rv = VPLCond_TimedWait(VPLLazyInitCond_GetCond(&inProgressCond), mutex, VPL_TIMEOUT_NONE);
        if (rv < 0) {
            LOG_ERROR("VPLCond_TimedWait failed: %d", rv);
            // This isn't expected; just do a sleep to avoid meltdown.
            VPLThread_Sleep(VPLTime_FromMillisec(500));
        }
    } while (1);
}

void CCDPrivEndLogout()
{
    MutexAutoLock lock(VPLLazyInitMutex_GetMutex(&inProgressMutex));
    ASSERT(inProgressLogout > 0);
    inProgressLogout--;
    int rc = VPLCond_Broadcast(VPLLazyInitCond_GetCond(&inProgressCond));
    if (rc < 0) {
        LOG_ERROR("VPLCond_Broadcast failed: %d", rc);
    }
}

int CCDPrivBeginLogin()
{
    ASSERT(!Cache_ThreadHasLock());
    VPLMutex_t* mutex = VPLLazyInitMutex_GetMutex(&inProgressMutex);
    MutexAutoLock lock(mutex);
    do {
        if (shutdownRequested) {
            LOG_INFO("Shutdown in progress!");
            return CCD_ERROR_SHUTTING_DOWN;
        }
        if ((inProgressLogout == 0) && (inProgressLogin == 0) && (inProgressOther == 0)) {
            LOG_INFO("Safe to begin login");
            inProgressLogin++;
            loginLogoutThreadId = VPLDetachableThread_Self();
            return 0;
        }
        LOG_INFO("inProgressLogout=%d, inProgressLogin=%d, inProgressOther=%d; waiting.", inProgressLogout, inProgressLogin, inProgressOther);
        int rv = VPLCond_TimedWait(VPLLazyInitCond_GetCond(&inProgressCond), mutex, VPL_TIMEOUT_NONE);
        if (rv < 0) {
            LOG_ERROR("VPLCond_TimedWait failed: %d", rv);
            // This isn't expected; just do a sleep to avoid meltdown.
            VPLThread_Sleep(VPLTime_FromMillisec(500));
        }
    } while (1);
}

void CCDPrivEndLogin()
{
    MutexAutoLock lock(VPLLazyInitMutex_GetMutex(&inProgressMutex));
    ASSERT(inProgressLogin > 0);
    inProgressLogin--;
    int rc = VPLCond_Broadcast(VPLLazyInitCond_GetCond(&inProgressCond));
    if (rc < 0) {
        LOG_ERROR("VPLCond_Broadcast failed: %d", rc);
    }
}

int CCDPrivBeginOther()
{
    ASSERT(!Cache_ThreadHasLock());
    VPLMutex_t* mutex = VPLLazyInitMutex_GetMutex(&inProgressMutex);
    MutexAutoLock lock(mutex);
    do {
        if (shutdownRequested) {
            LOG_INFO("Shutdown in progress!");
            return CCD_ERROR_SHUTTING_DOWN;
        }
        if ((inProgressLogout == 0) && (inProgressLogin == 0)) {
            LOG_INFO("Safe to begin other");
            inProgressOther++;
            return 0;
        }
        LOG_INFO("inProgressLogout=%d, inProgressLogin=%d; waiting.", inProgressLogout, inProgressLogin);
        int rv = VPLCond_TimedWait(VPLLazyInitCond_GetCond(&inProgressCond), mutex, VPL_TIMEOUT_NONE);
        if (rv < 0) {
            LOG_ERROR("VPLCond_TimedWait failed: %d", rv);
            // This isn't expected; just do a sleep to avoid meltdown.
            VPLThread_Sleep(VPLTime_FromMillisec(500));
        }
    } while (1);
}

void CCDPrivEndOther()
{
    MutexAutoLock lock(VPLLazyInitMutex_GetMutex(&inProgressMutex));
    ASSERT(inProgressOther > 0);
    inProgressOther--;
    int rc = VPLCond_Broadcast(VPLLazyInitCond_GetCond(&inProgressCond));
    if (rc < 0) {
        LOG_ERROR("VPLCond_Broadcast failed: %d", rc);
    }
}

#ifndef VPL_PLAT_IS_WINRT
static std::string s_osUserId;
#endif
static std::string s_processName;
static std::string s_titleId;
#ifdef ANDROID
static std::string s_brandName;
#endif

#if !CCD_USE_IPC_SOCKET
static VPLSem_t s_mainLoopSem = VPLSEM_SET_UNDEF;
#else

static char protobufSockName[CCDI_PROTOBUF_SOCKET_NAME_MAX_LENGTH];

static int init_protobufSockName()
{
    snprintf(protobufSockName, ARRAY_SIZE_IN_BYTES(protobufSockName),
            CCDI_PROTOBUF_SOCKET_NAME_FMT, __ccdConfig.testInstanceNum, s_osUserId.c_str());
    LOG_INFO("CCDI socket name: \"%s\"", protobufSockName);
    return VPL_OK;
}

# ifndef WIN32
static VPLFile_handle_t s_wakeMainLoopFd[2] = {-1, -1};

static s32 createWakeMainLoopFd()
{
    // TODO: use eventfd if supported (more lightweight and only 1 file descriptor created)
    //s_wakeMainLoopFd = eventfd(0, EFD_CLOEXEC);
    s32 rv = VPLPipe_Create(s_wakeMainLoopFd);
    if (rv < 0) {
        LOG_ERROR("pipe failed, errno %d (%s)", errno, strerror(errno));
        rv = CCD_ERROR_PIPE_FAILED;
    }
    return rv;
}

/// Get the file descriptor that the mainLoop thread will call select() on.
static int getWakeMainLoopFd()
{
    return s_wakeMainLoopFd[0];
}

/// Wake the mainLoop thread and tell it to exit.
static void wakeMainLoop()
{
    if (s_wakeMainLoopFd[1] >= 0) {
        ssize_t result = write(s_wakeMainLoopFd[1], "", 1);
        if (result != 1) {
            LOG_WARN("Possible problem");
        }
    }
}

static void cleanupWakeMainLoopFd()
{
    if (s_wakeMainLoopFd[0] >= 0) {
        close(s_wakeMainLoopFd[0]);
        s_wakeMainLoopFd[0] = -1;
    }
    if (s_wakeMainLoopFd[1] >= 0) {
        close(s_wakeMainLoopFd[1]);
        s_wakeMainLoopFd[1] = -1;
    }
}
# endif
#endif

#if CCD_USE_IPC_SOCKET
static int
handleReadReady(fd_set* selectSet, VPLNamedSocket_t* ccdiServerSock)
{
    s32 rv = 0;

    if (FD_ISSET(VPLNamedSocket_GetFd(ccdiServerSock), selectSet)) {
        CCDIQueryHandler2Args* args = (CCDIQueryHandler2Args*)malloc(sizeof(CCDIQueryHandler2Args));
# ifdef WIN32
        LOG_INFO("Waiting for connection on CCDI socket");
# else
        LOG_INFO("Accepting connection on CCDI socket");
# endif
        rv = VPLNamedSocket_Accept(ccdiServerSock, &(args->connectedSock));
        if (rv < 0) {
            LOG_ERROR("Failed to accept on system socket 2");
            free(args);
        } else {
            LOG_INFO("Accepted connection on CCDI socket");
            args->requestFrom = CCDI_CONNECTION_SYSTEM;
            rv = CCDIQuery_SyncHandler2(args);
            if (rv < 0) {
                LOG_ERROR("Unable to handle request, shutting down CCD!");
                setShutdownRequested(true);
                goto out;
            }
        }
        rv = VPLNamedSocket_Reactivate(ccdiServerSock, protobufSockName, CCDGetOsUserId());
        if (rv < 0) {
            LOG_ERROR("Unable to reactivate socket, shutting down CCD!");
            setShutdownRequested(true);
        }
    }

out:
    return rv;
}
#endif

#ifdef VPL_PLAT_IS_WIN_DESKTOP_MODE

# ifdef _MSC_VER
#   pragma comment(lib, "User32.lib")
#   pragma comment(lib, "psapi.lib")
# endif

static HWND s_msgWindow = NULL;

/// A #WNDPROC.
LRESULT CALLBACK myWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    LOG_DEBUG("Got msg 0x%04x", uMsg);

    switch(uMsg) {
    case WM_DESTROY: // 0x0002
        LOG_INFO("Got WM_DESTROY");
        // Allow the GetMessage loop to exit.
        PostQuitMessage(0);
        return 0;
    case WM_CLOSE: // 0x0010
        LOG_INFO("Got WM_CLOSE; destroying %p", hWnd);
        if (hWnd != s_msgWindow) {
            LOG_ERROR("hWnd (%p) != s_msgWindow (%p)", hWnd, s_msgWindow);
        }
        // Must be called by the same thread that created the window!
        DestroyWindow(hWnd);
        return 0;
    case WM_POWERBROADCAST: // 0x0218
        LOG_INFO("Got WM_POWERBROADCAST");
        if (wParam == PBT_APMSUSPEND) {
            LOG_INFO("System is going to suspend");
            ANSConn_GoingToSleepCb();
# if CCD_ENABLE_STORAGE_NODE
            Cache_GoingToSuspend();
# endif
            LOG_INFO("System suspend callbacks completed.");
        } else if (wParam == PBT_APMRESUMEAUTOMATIC) {
            LOG_INFO("PBT_APMRESUMEAUTOMATIC: Resumed from suspend (normal)");
            ANSConn_ResumeFromSleepCb();
            NetMan_CheckWakeReason();
        } else if (wParam == PBT_APMRESUMESUSPEND) {
            LOG_INFO("PBT_APMRESUMESUSPEND: Resumed from suspend (user input)");
            ANSConn_ResumeFromSleepCb();
            NetMan_CheckWakeReason();
        } else {
            LOG_DEBUG("Ignoring power-management event "FMTu64, (u64)wParam);
        }
        break;
# if 0
    // May potentially be useful in the future (although I think NLM probably has us covered).
    case WM_DEVICECHANGE: //0x0219
        LOG_INFO("Got WM_DEVICECHANGE");
        break;
# endif
    default:
        break;
    }
    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

static s32
win32EventLoop_Start()
{
    s32 rv = VPL_OK;
#  if CCD_IS_WINDOWS_SERVICE
#    error "See RegisterServiceCtrlHandlerEx, SERVICE_CONTROL_POWEREVENT"
#  else
    {
        HWND hwndConsole = GetConsoleWindow();
        LOG_DEBUG("GetConsoleWindow = %p", hwndConsole);
        HINSTANCE hInstance = (HINSTANCE)GetWindowLongPtr(hwndConsole, GWLP_HINSTANCE);
        LOG_DEBUG("hInstance = %p", hInstance);
        WNDCLASS wnd = {
                WS_DISABLED, // UINT      style;
                myWndProc, // WNDPROC   lpfnWndProc;
                0, // int       cbClsExtra;
                0, // int       cbWndExtra;
                hInstance, // HINSTANCE hInstance;
                NULL, // HICON     hIcon;
                NULL, // HCURSOR   hCursor;
                NULL, // HBRUSH    hbrBackground;
                NULL, // LPCTSTR   lpszMenuName;
                TEXT("MessageReceiverClass")  // LPCTSTR   lpszClassName;
        };
        ATOM classAtom = RegisterClass(&wnd);
        if (classAtom == 0) {
            rv = VPLError_XlatWinErrno(GetLastError());
            LOG_ERROR("%s failed: %d", "RegisterClass", rv);
            goto fail;
        }
        s_msgWindow = CreateWindowEx(
                WS_DISABLED, // __in      DWORD dwExStyle,
                MAKEINTATOM(classAtom), // __in_opt  LPCTSTR lpClassName,
                TEXT("MessageReceiver"), // __in_opt  LPCTSTR lpWindowName,
                0, // __in      DWORD dwStyle,
                0, // __in      int x,
                0, // __in      int y,
                0, // __in      int nWidth,
                0, // __in      int nHeight,
                NULL, // __in_opt  HWND hWndParent, // Don't use HWND_MESSAGE; it doesn't get broadcasts
                NULL, // __in_opt  HMENU hMenu,
                NULL, // __in_opt  HINSTANCE hInstance,
                NULL  // __in_opt  LPVOID lpParam
                );
        if (s_msgWindow == NULL) {
            rv = VPLError_XlatWinErrno(GetLastError());
            LOG_ERROR("%s failed: %d", "CreateWindowEx", rv);
            goto fail;
        }
        LOG_DEBUG("Created msgWindow %p", s_msgWindow);
    }
#  endif
fail:
    return rv;
}
#endif

#if CCD_USE_IPC_SOCKET
static VPLNamedSocket_t ccdiServerSock;
#endif

// Forward declaration to break cyclic dependency.
static int ccdStartOrStop(const char* processName, const char* localAppDataPath, const char* osUserId, const char* titleId, bool doShutdown);

static void mainLoop()
{
#if !CCD_USE_IPC_SOCKET
    VPLSem_Wait(&s_mainLoopSem);
#else
    int rv;
    while (true) {
        fd_set selectSet;

        // create our fd set
        FD_ZERO(&selectSet);
        FD_SET(VPLNamedSocket_GetFd(&ccdiServerSock), &selectSet);

# ifdef WIN32
        // Always pretend that there is something to read.
        // The loop will simply block within VPLNamedSocket_Accept (within handleReadReady) until
        // a request comes in.  To perform reliable shutdown, CCDI clients on Windows should send
        // an extra dummy request after their shutdown request.
        rv = 1;
# else
        FD_SET(getWakeMainLoopFd(), &selectSet);
        rv = select(FD_SETSIZE, &selectSet, NULL, NULL, NULL);
# endif
        if (rv < 0) {
            LOG_ERROR("select returned %d: %s", rv, strerror(errno));
            break;
        } else if (rv > 0) {
            if (isShutdownRequested()) {
                break;
            }
            rv = handleReadReady(&selectSet, &ccdiServerSock);
            if (rv < 0) {
                LOG_INFO("handleReadReady returned %d", rv);
                // Most likely cause is that there are too many unprocessed requests; give them a
                // chance now.
                VPLThread_Yield();
            }
        }
        if (isShutdownRequested()) {
            break;
        }
    }
#endif
    LOG_INFO("CCD main loop thread shutting down");
    waitUntilSafeToShutdown();
    ccdStartOrStop(NULL, NULL, NULL, NULL, true);
    LOG_INFO("CCD main loop thread has finished shutting down");
}

static VPLTHREAD_FN_DECL mainLoopThreadFn(void*)
{
    mainLoop();
#ifdef VPL_PLAT_IS_WIN_DESKTOP_MODE
    // Post a close message to break the other thread out of GetMessage().
    BOOL success = PostMessage(s_msgWindow, WM_CLOSE, NULL, NULL);
    if (success) {
        LOG_INFO("Posted WM_CLOSE to msgWindow (%p)", s_msgWindow);
    } else {
        LOG_ERROR("Failed to post WM_CLOSE to msgWindow (%p)", s_msgWindow);
    }
#endif
    return VPLTHREAD_RETURN_VALUE;
}

static VPLDetachableThreadHandle_t mainLoopThread;

static int startCount = 0;

#ifdef VPL_PLAT_IS_WIN_DESKTOP_MODE
// Do not log yet, see NOTE in ccdStartOrStop.
static bool IsASubdirectory(const char* subdirOrig, const char* parentOrig)
{
    string subdirStr = Util_CleanupPath(subdirOrig);
    string parentStr = Util_CleanupPath(parentOrig);
    subdirStr.append("/"); // Without this, we would incorrectly accept something like
    parentStr.append("/"); // "C:/Users/steveo/blah" as being under "C:/Users/steve".
    const char* subdir = subdirStr.c_str();
    const char* parent = parentStr.c_str();
    if (!VPLString_StartsWith(subdir, parent)) {
        return false;
    }

    const char* currPos = subdir + strlen(parent);
    const char* end = subdir + strlen(subdir);
    while (1) {
        while (*currPos == '/') {
            currPos++;
        }
        if (currPos >= end) {
            return true;
        }
        const char* nextSegEnd = strchr(currPos, '/');
        if (nextSegEnd == NULL) {
            nextSegEnd = end;
        }
        size_t nextSegLen = nextSegEnd - currPos;
        if (VPLString_Equal("..", currPos, nextSegLen)) {
            return false;
        }
        currPos = nextSegEnd;
    }
}
#endif

static int
ccdStartOrStop(const char* processName, const char* localAppDataPath, const char* osUserId, const char* titleId, bool stopCcd)
{
    int rv = 0;
    // If localAppDataPath is not NULL, we will clean it up and temporarily store it here.
    std::string tempAppDataPath;
#ifdef ANDROID
    std::string androidlogPath;
#endif

    bool usingDefaultLocalAppDataPath = false;
    if (stopCcd) {
        if (isRunning) {
            goto do_shutdown;
        } else {
            LOG_INFO("Already shut down; ignoring call");
            return 0;
        }
    } else {
        if (isRunning) {
            LOG_INFO("Already running; ignoring call");
            return 0;
        }
        // else, proceed.
    }

    // NOTE: Logging may not have been initialized yet, so don't log anything other than ERRORs,
    // since those imply that our process is going to exit anyway.

    if (startCount == 0) {
        rv = VPL_Init();
        if ((rv != VPL_OK) && (rv != VPL_ERR_IS_INIT)) {
            LOG_ERROR("%s failed: %d", "VPL_Init", rv);
            goto done;
        }
    }
    startCount++;

#ifndef WIN32
    // This is to avoid terminating CCD when a client makes an RPC request and
    // then closes the socket before we deliver the result.
    signal(SIGPIPE, SIG_IGN);
#endif

    if (localAppDataPath == NULL) {
        localAppDataPath = GVM_DEFAULT_LOCAL_APP_DATA_PATH;
        usingDefaultLocalAppDataPath = true;
    } else {
        tempAppDataPath = Util_CleanupPath(localAppDataPath);
        localAppDataPath = tempAppDataPath.c_str();
    }

    s_processName.assign(processName);
    
#ifndef VPL_PLAT_IS_WINRT
    if (osUserId != NULL) {
        s_osUserId.assign(osUserId);
        // Do not log yet, see NOTE above.
# ifdef WIN32
        _VPL__SetUserSid(s_osUserId.c_str());
# endif // WIN32
    } else {
        char* vplOsUserId;
        int rv = VPL_GetOSUserId(&vplOsUserId);
        if (rv < 0) {
            LOG_ERROR("%s failed: %d", "VPL_GetOSUserId", rv);
            goto fail_get_os_user_id;
        }
        ON_BLOCK_EXIT(VPL_ReleaseOSUserId, vplOsUserId);
        s_osUserId.assign(vplOsUserId);
        // Do not log yet, see NOTE above.
    }
#endif // VPL_PLAT_IS_WINRT

#ifdef VPL_PLAT_IS_WIN_DESKTOP_MODE
    // Validate that localAppDataPath belongs to osUserId.
    // Read the "ProfileImagePath" value under "HKLM\SOFTWARE\Microsoft\Windows NT\CurrentVersion\ProfileList\%SID%"
    {
        // Do not log yet, see NOTE above.
        char keyPath[256];
        snprintf(keyPath, ARRAY_SIZE_IN_BYTES(keyPath),
                "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\ProfileList\\%s", CCDGetOsUserId());
        HKEY key;
        LONG errCode;
        if ((errCode = RegOpenKeyExA(HKEY_LOCAL_MACHINE, keyPath, 0, KEY_READ, &key)) != ERROR_SUCCESS) {
            rv = VPLError_XlatWinErrno(errCode);
            LOG_ERROR("Failed to open \"%s\": %d", keyPath, rv);
            goto fail_validate_app_data_path;
        }
        wchar_t profileImagePath[1024];
        DWORD length = ARRAY_SIZE_IN_BYTES(profileImagePath);
        errCode = RegGetValueW(key, NULL, L"ProfileImagePath", RRF_RT_REG_SZ, NULL, profileImagePath, &length);
        if (errCode != ERROR_SUCCESS) {
            rv = VPLError_XlatWinErrno(errCode);
            LOG_ERROR("Failed to get ProfileImagePath from \"%s\": %d", keyPath, rv);
            goto fail_validate_app_data_path;
        }
        char* profilePathUtf8 = NULL;
        rv = _VPL__wstring_to_utf8_alloc(profileImagePath, &profilePathUtf8);
        ON_BLOCK_EXIT(free, profilePathUtf8);
        if (rv != 0) {
            LOG_ERROR("Failed to convert path string");
            goto fail_validate_app_data_path;
        }
        // Make sure that localAppDataPath is beneath profileImagePath.
        if (!IsASubdirectory(localAppDataPath, profilePathUtf8)) {
            LOG_ERROR("localAppDataPath \"%s\" cannot be verified within \"%s\"", localAppDataPath, profilePathUtf8);
            rv = CCD_ERROR_UNAUTHORIZED_DATA_DIR;
            goto fail_validate_app_data_path;
        }
    }
#endif

#ifdef ANDROID
    androidlogPath.append("/sdcard/AOP/").append(s_brandName.c_str());
    LOGInit(processName, androidlogPath.c_str()); // "/logs/..." will be appended to this.
    LOGSetWriteToStdout(VPL_FALSE);
#else
    LOGInit(processName, localAppDataPath);
#endif
    LOG_DISABLE_LEVEL(LOG_LEVEL_DEBUG);
    LOGStartSpecialLog("CCDStart.log", 256 * 1024);
    LOG_INFO(" ");
    LOG_INFO(" ");
#ifdef CLOUDNODE
    LOG_INFO("CCD(cloudnode) START (count = %d)", startCount);
#else
    LOG_INFO("CCD(regular) START (count = %d)", startCount);
#endif
    LOG_INFO("BUILD: %s", CCD_BUILD_INFO);
    LOG_INFO("localAppDataPath%s: \"%s\"",
            (usingDefaultLocalAppDataPath ? " (default)" : ""),
            localAppDataPath);

    if (titleId != NULL) {
        LOG_INFO("Title ID: %s", titleId);
        s_titleId.assign(titleId);
    } else {
        LOG_INFO("Title ID not specified, use default value: 0");
        s_titleId.assign("0");
    }

#ifdef ANDROID
    CCDConfig_SetBrandName(s_brandName.c_str());
#endif

    CCDConfig_Init(localAppDataPath);
    CCDConfig_Print();

    LOGSetMax(__ccdConfig.maxTotalLogSize); // Set the max after ccd config is parsed;

#ifdef ANDROID
    LOG_INFO("Logging to logcat=%s, file=%s",
            __ccdConfig.writeToSystemLog ? "YES" : "NO",
            __ccdConfig.enableLogFiles ? "YES" : "NO");
    LOGSetWriteToSystemLog(__ccdConfig.writeToSystemLog);
    LOGSetWriteToFile(__ccdConfig.enableLogFiles);
#endif

    setShutdownRequested(false);

    if (__ccdConfig.debugLog) {
        LOG_ENABLE_LEVEL(LOG_LEVEL_DEBUG);
    }

#if defined CLOUDNODE && defined __CLOUDNODE__
    // Enable in-memory logging before spawning the main thread. Also, flush the logs
    // in case the previous incarnation of CCD crashed
    LOG_INFO("Enabling in-memory logging");
    LOGSetEnableInMemoryLogging(true, __ccdConfig.testInstanceNum);
    LOGFlushInMemoryLogs();
#endif

    CCDStorage_Init(localAppDataPath);

    // For the platforms below, the default sqlite temp directory is not
    // acceptable for us, so we override it.
#if defined(VPL_PLAT_IS_WINRT)
    {
        char* ccdTempDir = NULL;
        Platform::String^ tmpdir = Windows::Storage::ApplicationData::Current->LocalFolder->Path + "\\.ccd_Temp";
        rv = _VPL__wstring_to_utf8_alloc(tmpdir->Data(), &ccdTempDir);
        for (int i = 0; i < strlen(ccdTempDir) ; i++) {
            if (ccdTempDir[i] == '\\') {
                ccdTempDir[i] = '/';
            }
        }
        LOG_INFO("Setting sqlite3 temp path(%s)", ccdTempDir);
        rv = Util_InitSqliteTempDir(ccdTempDir);
        if (rv != 0) {
            LOG_ERROR("Util_InitSqliteTempDir(%s):%d", ccdTempDir, rv);
        }
    }
#elif defined(CLOUDNODE) || defined(ANDROID)
    {
        std::string ccdTempDir = std::string(localAppDataPath) +
                                 std::string("/sqlite_tmp");
        LOG_INFO("Setting sqlite3 temp path(%s)", ccdTempDir.c_str());
        rv = Util_InitSqliteTempDir(ccdTempDir);
        if (rv != 0) {
            LOG_ERROR("Util_InitSqliteTempDir(%s):%d", ccdTempDir.c_str(), rv);
        }
    }
#endif

    LOG_INFO("Storage Root = \"%s\"", CCDStorage_GetRoot());
#ifndef VPL_PLAT_IS_WINRT
    if (osUserId != NULL) {
        LOG_INFO("Using specified osUserId \"%s\"", osUserId);
    } else {
        LOG_INFO("Retrieved osUserId \"%s\"", CCDGetOsUserId());
    }
#endif

#if CCD_USE_SHARED_CREDENTIALS
    // Migrate shared credentials written from CCD version prior to 2.6.0
    {
        MigrateCredential();
    }
#endif

#ifdef VPL_PLAT_IS_WIN_DESKTOP_MODE
    rv = win32EventLoop_Start();
    if (rv < 0) {
        LOG_ERROR("%s failed: %d", "win32EventLoop_Start", rv);
        goto fail_win32_event_loop_start;
    }
#endif

    rv = EventManagerPb_Init();
    if ((rv != CCD_OK) && (rv != CCD_ERROR_ALREADY_INIT)) {
        LOG_ERROR("EventManagerPb_Init failed: %d", rv);
        goto fail_event_manager_pb_init;
    }

#if CCD_ENABLE_IOAC
    rv = NetMan_Start();
    if (rv < 0) {
        LOG_ERROR("%s failed: %d", "NetMan_Start", rv);
        goto fail_netman_start;
    }
#endif

#ifndef VPL_PLAT_IS_WINRT
    rv = VPLPowerMan_Start();
    if (rv < 0) {
        LOG_ERROR("%s failed: %d", "VPLPowerMan_Start", rv);
        goto fail_powerman_start;
    }
    // TODO: temp:
    VPLPowerMan_SetDefaultPostponeDuration(VPLTime_FromSec(__ccdConfig.serveDataKeepAwakeTimeSec));
#endif

#if CCD_USE_IPC_SOCKET
    {
        rv = init_protobufSockName();
        if (rv < 0) {
            LOG_ERROR("%s failed: %d", "init_protobufSockName", rv);
            goto fail_create_socket2;
        }
#  ifdef VPL_PLAT_IS_WIN_DESKTOP_MODE
        // Detect multiple instances of CCD.
        // TODO: Bug 620: should probably use a locked file instead:
        // "
        // If you are using a named mutex to limit your application to a single instance, a malicious 
        // user can create this mutex before you do and prevent your application from starting. To 
        // prevent this situation, create a randomly named mutex and store the name so that it can only 
        // be obtained by an authorized user. Alternatively, you can use a file for this purpose.
        // To limit your application to one instance per user, create a locked file in the user's profile 
        // directory.
        // "
        {
            char tempMutexName[MAX_PATH];
            // Without the "Global\" prefix, each user session has its own namespace.
            // I'm a bit worried that it might be possible for this to return ERROR_ACCESS_DENIED
            // even if the mutex doesn't exist yet.  But I tried removing all entries from
            // "Create global objects" and it still worked fine.
            // gpedit.msc -> Computer Configuration/Windows Settings/Security Settings/Local Policies/User Rights Assignment
            // Then I rebooted the machine.  CCD ran as expected as a regular user or as SYSTEM.
            // I also tried it from within a Remote Desktop connection.
            // Even so, it's one more reason to use a lock file within the user's profile directory instead of a mutex.
            snprintf(tempMutexName, ARRAY_SIZE_IN_BYTES(tempMutexName), "Global\\%s.LOCK", protobufSockName);
            HANDLE tempResult = CreateMutexA(NULL, FALSE, tempMutexName);
            if (tempResult == NULL) {
                if (GetLastError() == ERROR_ACCESS_DENIED) {
                    LOG_ERROR("Unable to create mutex for named socket \"%s\"; check for another instance of CCD running as SYSTEM or as administrator.",
                            protobufSockName);
                    rv = CCD_ERROR_OTHER_INSTANCE;
                    goto fail_create_socket2;
                }
                rv = VPLError_XlatWinErrno(GetLastError());
                LOG_ERROR("%s failed: %d", "CreateMutexA", rv);
                goto fail_create_socket2;
            }
            if (GetLastError() == ERROR_ALREADY_EXISTS) {
                LOG_ERROR("Another process on this machine is already serving named socket \"%s\"",
                        protobufSockName);
                // Note: for an alternative approach using retry, see https://bugs.ctbg.acer.com/show_bug.cgi?id=15601#c9.
                rv = CCD_ERROR_OTHER_INSTANCE;
                goto fail_create_socket2;
            }
        }
#  endif // VPL_PLAT_IS_WIN_DESKTOP_MODE
        rv = VPLNamedSocket_OpenAndActivate(&ccdiServerSock, protobufSockName, CCDGetOsUserId());
        if (rv < 0) {
            LOG_ERROR("Failed to createServerSocket2 %s", protobufSockName);
            goto fail_create_socket2;
        }
        LOG_INFO("Activated \"%s\"", protobufSockName);
    }
#endif // CCD_USE_IPC_SOCKET
    
    // Need to do this before VirtualDevice_LoadCredentials.
    rv = DeviceStateCache_Init();
    if (rv < 0) {
        LOG_ERROR("DeviceStateCache_Init failed: %d", rv);
        goto fail_device_state_cache_init;
    }

    // Initialize ESCore with device-specific credentials.
    rv = VirtualDevice_LoadCredentials();
    // If the load failed, VirtualDevice_LoadCredentials() will delete any device credential files.
    // We will try again later in cache.cpp.
    if (rv == 0) {  // successfully loaded device credentials
        u64 deviceId;
        rv = ESCore_GetDeviceGuid(&deviceId);
        if (rv != 0) {
            LOG_ERROR("ESCore_GetDeviceGuid:%d", rv);
            goto fail_vs_core_init;
        }
        rv = Util_InitVsCore(deviceId);
        if (rv < 0) {
            LOG_ERROR("Failed to init VsCore: %d", rv);
            goto fail_vs_core_init;
        }
    }

    if (VPLFile_CheckAccess(CCDStorage_GetRoot(), VPLFILE_CHECKACCESS_EXISTS) != VPL_OK) {
        rv = VPLFS_Mkdir(CCDStorage_GetRoot());
        if (rv < 0) {
            LOG_ERROR("Failed to create dir at %s", CCDStorage_GetRoot());
        }
    }

    rv = Query_Init();
    if (rv < 0) {
        LOG_ERROR("Query_Init failed with error code %d", rv);
        goto fail_query_init;
    }

    rv = SWUpdate_Init();
    if ( rv < 0 ) {
        LOG_ERROR("SWUpdate_Init failed with error code %d", rv);
        goto fail_swUpdate_init;
    }

#if CCD_ENABLE_DOC_SAVE_N_GO
    DocSNGQueue_SetVcsHandlerFunction(VcsCloudDoc_HandleTicket);
    rv = ADO_Init_DocSaveNGo();
    if (rv != 0) {
        LOG_ERROR("Failed to initialize CloudDoc support: %d", rv);
        goto fail_dsng_init;
    }
#endif // CCD_ENABLE_DOC_SAVE_N_GO

    rv = SyncFeatureMgr_Start();
    if (rv < 0) {
        LOG_ERROR("SyncFeatureMgr_Start returned %d", rv);
        goto fail_sync_config_mgr_start;
    }

    // Note: this assumes that most everything else is already initialized, so it should come after
    //   everything else (other than enabling incoming CCDI requests).
    rv = Cache_Init();
    if (rv < 0) {
        LOG_ERROR("Cache_Init failed with error code %d", rv);
        goto fail_cache_init;
    }

    Cache_LogSummary();

#if CCD_USE_IPC_SOCKET
# ifndef WIN32
    rv = createWakeMainLoopFd();
    if (rv < 0) {
        goto fail_createWakeMainLoopObj;
    }
# endif
#else
    rv = VPLSem_Init(&s_mainLoopSem, 1, 0);
    if (rv < 0) {
        LOG_ERROR("%s failed: %d", "VPLSem_Init", rv);
        goto fail_createWakeMainLoopObj;
    }
#endif

    // Threads init

    rv = Util_SpawnThread(mainLoopThreadFn, NULL,
            MAIN_LOOP_THREAD_STACK_SIZE, VPL_TRUE, &mainLoopThread);
    if (rv < 0) {
        LOG_ERROR("Util_SpawnThread returned %d", rv);
        goto fail_spawn_main_thread;
    }
    
    // Success!
    isRunning = true;
    goto done;

do_shutdown:
fail_spawn_main_thread:
#if CCD_USE_IPC_SOCKET
# ifndef WIN32
    LOG_INFO("Calling cleanupWakeMainLoopFd()");
    cleanupWakeMainLoopFd();
    LOG_INFO("Back from cleanupWakeMainLoopFd()");
fail_createWakeMainLoopObj:
# endif
#else
    VPLSem_Destroy(&s_mainLoopSem);
fail_createWakeMainLoopObj:
#endif
    LOG_INFO("Calling Cache_Quit");
    Cache_Quit();
    LOG_INFO("Back from Cache_Quit");
fail_cache_init:
    LOG_INFO("Calling SyncFeatureMgr_Stop()");
    SyncFeatureMgr_Stop();
    LOG_INFO("Back from SyncFeatureMgr_Stop()");
fail_sync_config_mgr_start:
#if CCD_ENABLE_DOC_SAVE_N_GO
    LOG_INFO("Calling ADO_Release_DocSaveNGo");
    ADO_Release_DocSaveNGo();
    LOG_INFO("Back from ADO_Release_DocSaveNGo");
fail_dsng_init:
#endif // CCD_ENABLE_DOC_SAVE_N_GO
    LOG_INFO("Calling SWUpdate_Quit");
    SWUpdate_Quit();
    LOG_INFO("Back from SWUpdate_Quit");
fail_swUpdate_init:
    LOG_INFO("Calling Query_Quit");
    Query_Quit();
    LOG_INFO("Back from Query_Quit");
fail_query_init:
    LOG_INFO("Util_CloseVsCore begin");
    Util_CloseVsCore();
    LOG_INFO("Util_CloseVsCore end");
fail_vs_core_init:
    //doesn't exist: VirtualDevice_UnloadCredentials();
    LOG_INFO("Calling DeviceStateCache_Shutdown");
    DeviceStateCache_Shutdown();
fail_device_state_cache_init:
#if CCD_USE_IPC_SOCKET
    LOG_INFO("Closing CCDI named socket");
    VPLNamedSocket_Close(&ccdiServerSock);
fail_create_socket2:
#endif
#ifndef VPL_PLAT_IS_WINRT
    LOG_INFO("Calling VPLPowerMan_Stop");
    VPLPowerMan_Stop();
#endif
fail_powerman_start:
#if CCD_ENABLE_IOAC
    LOG_INFO("Calling NetMan_Stop");
    NetMan_Stop();
fail_netman_start:
#endif
    LOG_INFO("Calling EventManagerPb_Quit");
    EventManagerPb_Quit();
    LOG_INFO("Back from EventManagerPb_Quit");
fail_event_manager_pb_init:
#ifdef WIN32
fail_win32_event_loop_start:
#endif
#if defined CLOUDNODE && defined __CLOUDNODE__
    // Disable in-memory logging, which would also cause a flush automatically
    LOG_INFO("Disabling in-memory logging");
    LOGSetEnableInMemoryLogging(false, __ccdConfig.testInstanceNum);
    LOG_INFO("Disabled in-memory logging");
#endif
#ifdef _MSC_VER
fail_validate_app_data_path:
#endif
fail_get_os_user_id:
    LOG_INFO("Calling LOGStopSpecialLogs");
    LOGStopSpecialLogs();
    LOG_INFO("Back from LOGStopSpecialLogs");

done:
    return rv;
}

#ifdef VPL_PLAT_IS_WIN_DESKTOP_MODE
int CCDHandleStartParams(const char* pszTrusteesCmdLine, const char* pszMemSize)
{
    int rv = CCD_OK;
    ccd::CCDWin32StartParams input;
    LPVOID data;
    LONGLONG llHandle;
    int szMem;
    HANDLE hMapping = NULL;

    if (strlen(pszMemSize) == 0 || strcmp(pszMemSize, "0") == 0) {
        return CCD_OK;
    }

    if (!StrToInt64ExA(pszTrusteesCmdLine, STIF_DEFAULT, &llHandle)) {
        return CCD_ERROR_PARAMETER;
    }
    
    if (!StrToIntExA(pszMemSize, STIF_DEFAULT, &szMem)) {
        return CCD_ERROR_PARAMETER;
    }
    
    hMapping = (HANDLE)(INT_PTR)llHandle;
    data = (LPVOID) MapViewOfFile(hMapping, FILE_MAP_READ, 0, 0, 0);
    if (data) {
        // after mapping it to ccd::CCDWin32StartParams, do validation
        MEMORY_BASIC_INFORMATION mbi;
        if (VirtualQuery(data, &mbi, sizeof(mbi)) >= sizeof(mbi) &&
            mbi.State == MEM_COMMIT &&
            mbi.BaseAddress == data &&
            mbi.RegionSize >= szMem) {
            // format is valid after mapped
            input.ParseFromArray(data, szMem);
            if (input.trustees_size() > 0) {
                for (int i=0; i < input.trustees_size(); i++) {
                    PSID pSid;
                    BOOL rt = ConvertStringSidToSidA(input.trustees(i).sid().c_str(), &pSid);
                    if (rt) {
                        SID_AND_ATTRIBUTES trustee = {pSid, (DWORD)input.trustees(i).attr()};
                        rv = _VPLFS__InsertTrustees(trustee);
                        if (rv != VPL_OK) {
                            // return error when insert any of trustees
                            break;
                        }
                    }
                    else {
                        // return error when failed to convert string sid to sid
                        rv = VPLError_XlatWinErrno(GetLastError());
                        break;
                    }
                }
            }
        }
        else {
            // Invalid memory block
            rv = CCD_ERROR_PARSE_CONTENT;
        }
    }
    else {
        DWORD rt = GetLastError();
    }

    UnmapViewOfFile(data);
    CloseHandle(hMapping);
    return rv;
}
#endif

int CCDStart(const char* processName, const char* localAppDataPath, const char* osUserId, const char* titleId)
{
    return ccdStartOrStop(processName, localAppDataPath, osUserId, titleId, false);
}

const char* CCDGetProcessName()
{
    return s_processName.c_str();
}

#ifndef VPL_PLAT_IS_WINRT
const char* CCDGetOsUserId()
{
    return s_osUserId.c_str();
}
#endif

const char* CCDGetTitleId()
{
    return s_titleId.c_str();
}

#ifdef ANDROID
void CCDSetBrandName(const char* brandName)
{
    if (brandName != NULL) {
        s_brandName.assign(brandName);
    } else {
        s_brandName.assign("AcerCloud");
    }
}
#endif

void CCDShutdown()
{
    LOG_INFO("Exit request received");
    setShutdownRequested(true);
#if CCD_USE_IPC_SOCKET
# ifndef WIN32
    // Wake up the mainLoop thread to complete shutdown.
    wakeMainLoop();
# endif
#else
    VPLSem_Post(&s_mainLoopSem);
#endif
}

void CCDWaitForExit()
{
    if (!isRunning) {
        LOG_INFO("Not running; ignoring call.");
    } else {
#ifdef VPL_PLAT_IS_WIN_DESKTOP_MODE
        // This must be called by the same thread that created s_msgWindow!
        BOOL bRet;
        MSG msg;
        LOG_INFO("Entering GetMessage loop.");
        while ((bRet = GetMessage(&msg, NULL, 0, 0)) != 0) {
            if (bRet == -1) {
                int temp_rv = VPLError_XlatWinErrno(GetLastError());
                LOG_ERROR("%s failed: %d", "GetMessage", temp_rv);
                break;
            }
            //TranslateMessage(&msg);
            DispatchMessage(&msg); // this calls #myWndProc()
        }
#endif
        LOG_INFO("Joining mainLoopThread");
        VPLDetachableThread_Join(&mainLoopThread);
        isRunning = false;
        LOG_INFO("Exited");
    }
}

/////////////////////////////////////////////////////////////////////
////////// TODO: could potentially go in its own file. //////////////

#include "ccd_core_service.hpp"
#include "ProtoRpc.h"

namespace ccd {

void CCDProtobufRpcDebugGenericCallback(const char* msg)
{
    LOG_INFO("%s", msg);
}

static const char* PROTOMSG_START = "------------------------------\n";
static const char* PROTOMSG_END =   "------------------------------";

void CCDProtobufRpcDebugRequestCallback(const std::string& methodName,
                                        bool isValid,
                                        const ProtoMessage& reqMsg)
{
    std::string prefix;
    {
        std::ostringstream temp;
        temp << methodName << " request" << (isValid ? "" : " (not valid)") << ":\n" <<
                PROTOMSG_START;
        prefix = temp.str();
    }
    UTIL_LOG_SENSITIVE_STRING_INFO(prefix.c_str(),
            reqMsg.DebugString().c_str(),
            PROTOMSG_END);
}

void CCDProtobufRpcDebugResponseCallback(const std::string& methodName,
                                         const RpcStatus& status,
                                         const ProtoMessage* respMsg)
{
    // prefix will not be filtered.
    std::string prefix;
    // msg will be filtered for sensitive data.
    std::string msg;
    {
        std::ostringstream tempPrefix;
        tempPrefix << methodName << " response:\n";
        if (status.status() != RpcStatus::OK) {
            tempPrefix << "RPC status was " << status.status() <<
                    " (" << RpcStatus_Status_Name(status.status()) << "):\n" <<
                    status.errordetail() << "\n";
        } else {
            tempPrefix << "Return code = " << status.appstatus() << ".\n";
            if (status.appstatus() >= 0) {
                tempPrefix << PROTOMSG_START;
                if (respMsg == NULL) {
                    // This case should not happen!
                    tempPrefix << "(no response)\n";
                } else if (std::string::npos != methodName.find("MCAQueryMetadataObjects")) {
                    // This can return a LARGE number of objects.  Suppress the printing.
                    const ccd::MCAQueryMetadataObjectsOutput* recastResponse =
                            static_cast<const ccd::MCAQueryMetadataObjectsOutput*>(respMsg);
                    tempPrefix << "(condensing potentially large response log)\n"
                        "Number of content_objects: " <<
                        recastResponse->content_objects_size() << "\n";
                } else {
                    // Note: DebugString() is expensive for large responses.
                    msg = respMsg->DebugString();
                }
            }
        }
        prefix = tempPrefix.str();
    }
    UTIL_LOG_SENSITIVE_STRING_INFO(prefix.c_str(),
            msg.c_str(),
            PROTOMSG_END);
}

////////// TODO: could potentially go in its own file. //////////////
/////////////////////////////////////////////////////////////////////


} // end namespace ccd
