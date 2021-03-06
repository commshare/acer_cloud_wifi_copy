/*
 *               Copyright (C) 2009, BroadOn Communications Corp.
 *
 *  These coded instructions, statements, and computer programs contain
 *  unpublished  proprietary information of BroadOn Communications Corp.,
 *  and  are protected by Federal copyright law. They may not be disclosed
 *  to  third  parties or copied or duplicated in any form, in whole or in
 *  part, without the prior written consent of BroadOn Communications Corp.
 *
 */
#include <vplex_ipc_socket.h>

#if !defined(WIN32) && !defined(ANDROID)

#include <vplex_private.h>
#include <vplu_format.h>

#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#ifndef IOS
#include <sys/sendfile.h>
#endif
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <unistd.h>

#define IPCSOCKET_UNINIT  -15026
#define IPCSOCKET_FAILED  -15028
#define IPCSOCKET_CLOSED  -15030

IPCError
IPCSocket_Accept(IPCSocket *sock, IPCSocket *client, u8 nonBlock)
{
    VPL_LIB_LOG_FINEST(VPL_SG_SOCKET, "IPCSocket_Accept %d", nonBlock);

    socklen_t length = 0;
    socklen_t peerLength;
    int fd = -1;
    int options = -1;
    struct sockaddr_in addr;
    struct sockaddr_in peer;
    struct sockaddr_un unixAddr;
    IPCError rv = IPC_ERROR_OK;

    if (sock == NULL || client == NULL) {
        VPL_LIB_LOG_ERR(VPL_SG_SOCKET, "sock = NULL | client = NULL");
        rv = IPC_ERROR_PARAMETER;
        goto out;
    }

    switch (sock->type) {
    case IPC_SOCKET_TCP:
        length = sizeof(addr);
        fd = accept(sock->fd, (struct sockaddr*)&addr, &length);
        break;

    case IPC_SOCKET_UNIX:
        length = sizeof(unixAddr);
        fd = accept(sock->fd, (struct sockaddr*)&unixAddr, &length);
        break;

    case IPC_SOCKET_UDP:
        /*FALLTHROUGH*/
    default:
        VPL_LIB_LOG_ERR(VPL_SG_SOCKET, "sock->type != IPC_SOCKET_TCP | IPC_SOCKET_UNIX");
        rv = IPC_ERROR_PARAMETER;
        goto out;
    }

    if (fd < 0) {
        VPL_LIB_LOG_ERR(VPL_SG_SOCKET, "accept failed: %s", strerror(errno));
        rv = IPC_ERROR_SOCKET_ACCEPT;
        goto out;
    }

    peerLength = sizeof(peer);
    // TODO: Passing in sockaddr_in isn't valid for the Unix socket case.
    // TODO: Why are we even calling this?  Shouldn't addr or unixAddr have the info already?
    if (getpeername(fd, (struct sockaddr*)&peer, &peerLength) < 0) {
        VPL_LIB_LOG_ERR(VPL_SG_SOCKET, "getpeername failed: %s", strerror(errno));
        rv = IPC_ERROR_SOCKET_PEERNAME;
        goto out;
    }

    if (nonBlock) {

        options = fcntl(fd, F_GETFL);
        if (options < 0) {
            VPL_LIB_LOG_ERR(VPL_SG_SOCKET, "fcntl F_GETFL failed: %s", strerror(errno));
            rv = IPC_ERROR_SOCKET_ACCEPT;
            goto out;
        }

        if (fcntl(fd, F_SETFL, options | O_NONBLOCK) < 0) {
            VPL_LIB_LOG_ERR(VPL_SG_SOCKET, "fcntl F_SETFL failed: %s", strerror(errno));
            rv = IPC_ERROR_SOCKET_ACCEPT;
            goto out;
        }
    }

    memset(client, 0, sizeof(IPCSocket));
    client->fd = fd;
    client->nonBlock = nonBlock;
    client->type = sock->type;
    // TODO: This isn't valid for the Unix socket case.
    client->port = ntohs(peer.sin_port);
    //strncpy((char *)client->address, inet_ntoa(peer.sin_addr), IPC_SOCKET_ADDRESS_LENGTH);
    IPCSocket_SetDelay(client, 0, 0);

out:
    if (rv < 0) {
        close(fd);
    }

    return rv;
}

IPCError
IPCSocket_Bind(IPCSocket *sock, const char *address, u16 port)
{
    VPL_LIB_LOG_FINEST(VPL_SG_SOCKET, "IPCSocket_Bind %s %d", address, port);

    int opt;
    struct sockaddr_in addr;
    struct sockaddr_un unixAddr;
    IPCError rv = IPC_ERROR_OK;

    if (sock == NULL) {
        VPL_LIB_LOG_ERR(VPL_SG_SOCKET, "sock = NULL");
        rv = IPC_ERROR_PARAMETER;
        goto out;
    }

    if (sock->type == IPC_SOCKET_UNIX && address == NULL) {
        VPL_LIB_LOG_ERR(VPL_SG_SOCKET, "sock->type = IPC_SOCKET_UNIX & address = NULL");
        rv = IPC_ERROR_PARAMETER;
        goto out;
    }

    switch (sock->type) {
    case IPC_SOCKET_TCP:
    case IPC_SOCKET_UDP:
        addr.sin_family = AF_INET;
        addr.sin_port = htons(port);

        if (address == NULL) {
            addr.sin_addr.s_addr = INADDR_ANY;
        } else {
            if (inet_pton(AF_INET, address, &addr.sin_addr) <= 0) {
                VPL_LIB_LOG_ERR(VPL_SG_SOCKET, "inet_pton failed: %s", strerror(errno));
                rv = IPC_ERROR_SOCKET_BIND;
                goto out;
            }
        }

        if (setsockopt(sock->fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
            VPL_LIB_LOG_ERR(VPL_SG_SOCKET, "setsockopt SO_REUSEADDR failed: %s", strerror(errno));
        }

        if (bind(sock->fd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
            VPL_LIB_LOG_ERR(VPL_SG_SOCKET, "bind failed on %s:%d: %s", address, port, strerror(errno));
            rv = IPC_ERROR_SOCKET_BIND;
            goto out;
        }

        break;

    case IPC_SOCKET_UNIX:
        unixAddr.sun_family = AF_UNIX;
        strncpy(unixAddr.sun_path, address, 108);
        unlink(unixAddr.sun_path);

        if (bind(sock->fd, (struct sockaddr*)&unixAddr, sizeof(unixAddr)) < 0) {
            VPL_LIB_LOG_ERR(VPL_SG_SOCKET, "bind failed on %s: %s", address, strerror(errno));
            rv = IPC_ERROR_SOCKET_BIND;
            goto out;
        }

        break;

    default:
        VPL_LIB_LOG_ERR(VPL_SG_SOCKET, "sock->type != IPC_SOCKET_TCP | IPC_SOCKET_UDP | IPC_SOCKET_UNIX");
        rv = IPC_ERROR_PARAMETER;
        break;
    }

    sock->port = port;
//    if (address != NULL) {
//        memcpy(sock->address, address, sizeof(sock->address));
//    }

out:
    return rv;
}

IPCError
IPCSocket_Close(IPCSocket *sock)
{
    VPL_LIB_LOG_FINEST(VPL_SG_SOCKET, "IPCSocket_Close");

    IPCError rv = IPC_ERROR_OK;

    if (sock == NULL) {
        VPL_LIB_LOG_ERR(VPL_SG_SOCKET, "sock = NULL");
        rv = IPC_ERROR_PARAMETER;
        goto out;
    }
    if (sock->fd >= 0) {
        if (close(sock->fd) < 0) {
            VPL_LIB_LOG_ERR(VPL_SG_SOCKET, "close failed: %s", strerror(errno));
            rv = IPC_ERROR_SOCKET_CLOSE;
            goto out;
        }
        sock->fd = IPCSOCKET_CLOSED;
    }

out:
    return rv;
}

IPCError
IPCSocket_Connect(IPCSocket *sock, const s8 *address, u16 port)
{
	printf("111111111\n");
    VPL_LIB_LOG_FINEST(VPL_SG_SOCKET, "IPCSocket_Connect %s %d", address, port);

    struct sockaddr_in addr;
    struct sockaddr_un unixAddr;
	printf("22222222\n");
    IPCError rv = IPC_ERROR_OK;
	printf("33333333\n");

    if (sock == NULL) {
		printf("444444444\n");
        VPL_LIB_LOG_ERR(VPL_SG_SOCKET, "sock = NULL");
        rv = IPC_ERROR_PARAMETER;
        goto out;
    }

    if (sock->type == IPC_SOCKET_UNIX && address == NULL) {
		printf("555555555\n");
        VPL_LIB_LOG_ERR(VPL_SG_SOCKET, "sock->type = IPC_SOCKET_UNIX & address = NULL");
        rv = IPC_ERROR_PARAMETER;
        goto out;
    }

	printf("6666666666\n");
    switch (sock->type) {
    case IPC_SOCKET_TCP:
		printf("7777777777\n");
    case IPC_SOCKET_UDP:
		printf("888888888\n");
        addr.sin_family = AF_INET;
        addr.sin_port = htons(port);

        if (address == NULL) {
            addr.sin_addr.s_addr = INADDR_ANY;
        } else {
            if (inet_pton(AF_INET, (const char *)address, &addr.sin_addr) <= 0) {
                VPL_LIB_LOG_ERR(VPL_SG_SOCKET, "inet_pton failed: %s", strerror(errno));
                rv = IPC_ERROR_SOCKET_CONNECT;
                goto out;
            }
        }

        if (connect(sock->fd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
            VPL_LIB_LOG_ERR(VPL_SG_SOCKET, "connect failed on %s:%d: %s", address, port, strerror(errno));
            rv = IPC_ERROR_SOCKET_CONNECT;
            goto out;
        }

        break;

    case IPC_SOCKET_UNIX:
		printf("99999999999\n");
        unixAddr.sun_family = AF_UNIX;
        strncpy(unixAddr.sun_path, (const char *)address, 108);

        if (connect(sock->fd, (struct sockaddr*)&unixAddr, sizeof(unixAddr)) < 0) {
            if (errno == 111) { // 111 => "Connection refused" (named socket file exists, but no process is listening)
				printf("10101010101010\n");
                VPL_LIB_LOG_WARN(VPL_SG_SOCKET, "named socket \"%s\" is unavailable; is the server process running?", address);
                rv = VPL_ERR_NAMED_SOCKET_NOT_EXIST;
            } else if (errno == 2) { // 2 => "No such file or directory" (named socket file doesn't exist yet)
				printf("11   11    11   11\n");
                VPL_LIB_LOG_WARN(VPL_SG_SOCKET, "named socket \"%s\" doesn't exist yet; is the server process running?", address);
                rv = VPL_ERR_NAMED_SOCKET_NOT_EXIST;
            } else {
				printf("12121212121212121212\n");
                VPL_LIB_LOG_ERR(VPL_SG_SOCKET, "connect failed on %s: %d (%s)", address, errno, strerror(errno));
                rv = IPC_ERROR_SOCKET_CONNECT;
            }
            goto out;
        }

        break;

    default:
		printf("131313131313131313\n");
        VPL_LIB_LOG_ERR(VPL_SG_SOCKET, "sock->type != IPC_SOCKET_TCP | IPC_SOCKET_UDP | IPC_SOCKET_UNIX");
        rv = IPC_ERROR_PARAMETER;
        break;
    }
	printf("141414141414141414");	
    sock->port = port;
    //memcpy(sock->address, address, sizeof(sock->address));

out:
	printf("out\n");
    return rv;
}

s8*
IPCSocket_GetAddress(const s8* hostName)
{
    VPL_LIB_LOG_FINEST(VPL_SG_SOCKET, "IPCSocket_GetAddress %s", hostName);

    s8* address = NULL;
    struct hostent* host = NULL;

    if (hostName == NULL) {
        VPL_LIB_LOG_ERR(VPL_SG_SOCKET, "hostName == NULL");
        goto out;
    }

    host = gethostbyname((const char *)hostName);
    if (host == NULL || host->h_addr_list[0] == NULL) {
        VPL_LIB_LOG_ERR(VPL_SG_SOCKET, "host == NULL | host->h_addr_list[0] == NULL");
        goto out;
    }

    address = (s8 *)inet_ntoa(*(struct in_addr*)(host->h_addr_list[0]));

out:
    return address;
}

IPCError
IPCSocket_Listen(IPCSocket *sock, s32 backlog)
{
    VPL_LIB_LOG_FINEST(VPL_SG_SOCKET, "IPCSocket_Listen %d", backlog);

    IPCError rv = IPC_ERROR_OK;

    if (sock == NULL) {
        VPL_LIB_LOG_ERR(VPL_SG_SOCKET, "sock = NULL");
        rv = IPC_ERROR_PARAMETER;
        goto out;
    } else if (sock->type == IPC_SOCKET_UDP) {
        VPL_LIB_LOG_ERR(VPL_SG_SOCKET, "sock->type = IPC_SOCKET_UDP");
        rv = IPC_ERROR_PARAMETER;
        goto out;
    } else if (backlog < 2) {
        VPL_LIB_LOG_ERR(VPL_SG_SOCKET, "backlog < 2");
        rv = IPC_ERROR_PARAMETER;
        goto out;
    }

    if (listen(sock->fd, backlog) < 0) {
        VPL_LIB_LOG_ERR(VPL_SG_SOCKET, "listen failed: %s", strerror(errno));
        rv = IPC_ERROR_SOCKET_LISTEN;
        goto out;
    }

out:
    return rv;
}

IPCError
IPCSocket_Open(IPCSocket *sock, IPCSocketType type, u8 nonBlock)
{
    VPL_LIB_LOG_FINEST(VPL_SG_SOCKET, "IPCSocket_Open %d %d", type, nonBlock);

    int fd = IPCSOCKET_UNINIT;
    int options = -1;
    IPCError rv = IPC_ERROR_OK;

    if (sock == NULL) {
        VPL_LIB_LOG_ERR(VPL_SG_SOCKET, "sock = NULL");
        rv = IPC_ERROR_PARAMETER;
        goto out;
    }
    sock->fd = IPCSOCKET_FAILED;
    
    switch (type) {
    case IPC_SOCKET_TCP:
        fd = socket(AF_INET, SOCK_STREAM, 0);
        break;

    case IPC_SOCKET_UDP:
        fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
        break;

    case IPC_SOCKET_UNIX:
        fd = socket(AF_UNIX, SOCK_STREAM, 0);
        break;

    default:
        VPL_LIB_LOG_ERR(VPL_SG_SOCKET, "sock->type != IPC_SOCKET_TCP | IPC_SOCKET_UDP | IPC_SOCKET_UNIX");
        rv = IPC_ERROR_PARAMETER;
        goto out;
        break;
    }

    if (fd < 0) {
        VPL_LIB_LOG_ERR(VPL_SG_SOCKET, "socket failed: %s", strerror(errno));
        rv = IPC_ERROR_SOCKET_OPEN;
        goto out;
    }

    if (nonBlock) {

        options = fcntl(fd, F_GETFL);
        if (options < 0) {
            VPL_LIB_LOG_ERR(VPL_SG_SOCKET, "fcntl F_GETFL failed: %s", strerror(errno));
            rv = IPC_ERROR_SOCKET_OPEN;
            goto out;
        }

        if (fcntl(fd, F_SETFL, options | O_NONBLOCK) < 0) {
            VPL_LIB_LOG_ERR(VPL_SG_SOCKET, "fcntl F_SETFL failed: %s", strerror(errno));
            rv = IPC_ERROR_SOCKET_OPEN;
            goto out;
        }
    }

    memset(sock, 0, sizeof(IPCSocket));
    sock->fd = fd;
    sock->nonBlock = nonBlock;
    sock->type = type;
    sock->port = 0;
    //memset(sock->address, 0, sizeof(sock->address));
    IPCSocket_SetDelay(sock, 0, 0);

out:
    if (rv < 0) {
        close(fd);
    }

    return rv;
}

s32
IPCSocket_Receive(IPCSocket *sock, void *buffer, u32 length, s32 flags)
{
    VPL_LIB_LOG_FINEST(VPL_SG_SOCKET, "IPCSocket_Receive %u %d", length, flags);

    s32 rv = IPC_ERROR_OK;

    if (sock == NULL || buffer == NULL || length <= 0) {
        VPL_LIB_LOG_ERR(VPL_SG_SOCKET, "sock = NULL | buffer = NULL | length <= 0");
        rv = IPC_ERROR_PARAMETER;
        goto out;
    }

    if ((sock->recvDelay) > 0) {
        VPL_LIB_LOG_FINEST(VPL_SG_SOCKET, "recv delay = %u microseconds", sock->recvDelay);
        usleep(sock->recvDelay);
    }

    rv = recv(sock->fd, buffer, length, flags);
    if (rv < 0) {
        if ( (errno == EAGAIN || errno == EWOULDBLOCK) && ((flags & MSG_DONTWAIT) || sock->nonBlock) ) {
            rv = IPC_ERROR_SOCKET_NONBLOCK;
            goto out;
        } else {
            VPL_LIB_LOG_ERR(VPL_SG_SOCKET, "recv failed: %s", strerror(errno));
            rv = IPC_ERROR_SOCKET_RECEIVE;
            goto out;
        }
    }

out:
    return rv;
}

s32
IPCSocket_ReceiveFrom(IPCSocket *sock, const s8* address, u16 port, void *buffer, u32 length, s32 flags)
{
    VPL_LIB_LOG_FINEST(VPL_SG_SOCKET, "IPCSocket_ReceiveFrom %s %d %u %d", address, port, length, flags);

    socklen_t size;
    struct sockaddr_in addr;
    s32 rv = IPC_ERROR_OK;

    if (sock == NULL || sock->type != IPC_SOCKET_UDP || !address || buffer == NULL || length <= 0) {
        VPL_LIB_LOG_ERR(VPL_SG_SOCKET, "sock = NULL | sock->type != IPC_SOCKET_UDP | address = NULL | buffer = NULL | length <= 0");
        rv = IPC_ERROR_PARAMETER;
        goto out;
    }

    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    if (inet_pton(AF_INET, (const char *)address, &addr.sin_addr) <= 0) {
        VPL_LIB_LOG_ERR(VPL_SG_SOCKET, "inet_pton failed: %s", strerror(errno));
        rv = IPC_ERROR_SOCKET_RECEIVE_FROM;
        goto out;
    }

    size = sizeof(addr);

    if ((sock->recvDelay) > 0) {
        VPL_LIB_LOG_FINEST(VPL_SG_SOCKET, "recv delay = %u microseconds", sock->recvDelay);
        usleep(sock->recvDelay);
    }

    rv = recvfrom(sock->fd, buffer, length, flags, (struct sockaddr*)&addr, &size);
    if (rv < 0) {
        if ( (errno == EAGAIN || errno == EWOULDBLOCK) && ((flags & MSG_DONTWAIT) || sock->nonBlock) ) {
            rv = IPC_ERROR_SOCKET_NONBLOCK;
            goto out;
        } else {
            VPL_LIB_LOG_ERR(VPL_SG_SOCKET, "recvfrom failed: %s", strerror(errno));
            rv = IPC_ERROR_SOCKET_RECEIVE_FROM;
            goto out;
        }
    }

out:
    return rv;
}

#ifdef IOS
s32
IPCSocket_Send(IPCSocket *sock, const void *buffer, u32 length, s32 flags)
{
    return -1; //TODO: Porting for iOS.
}

s32
IPCSocket_SendFile(IPCSocket *sock, s32 fd, off_t *offset, size_t length)
{
    return -1; //TODO: Porting for iOS.
}

s32
IPCSocket_SendTo(IPCSocket *sock, const s8* address, u16 port, const void *buffer, u32 length, s32 flags)
{
    return -1; //TODO: Porting for iOS.
}
#else
s32
IPCSocket_Send(IPCSocket *sock, const void *buffer, u32 length, s32 flags)
{
    VPL_LIB_LOG_FINEST(VPL_SG_SOCKET, "IPCSocket_Send %u %d", length, flags);

    s32 rv = IPC_ERROR_OK;

    if (sock == NULL || buffer == NULL || length <= 0) {
        VPL_LIB_LOG_ERR(VPL_SG_SOCKET, "sock = NULL | buffer = NULL | length <= 0");
        rv = IPC_ERROR_PARAMETER;
        goto out;
    }

    if ((sock->sendDelay) > 0) {
        VPL_LIB_LOG_FINEST(VPL_SG_SOCKET, "send delay = %u microseconds", sock->sendDelay);
        usleep(sock->sendDelay);
    }

    rv = send(sock->fd, buffer, length, flags | MSG_NOSIGNAL);
    if (rv < 0) {
        VPL_LIB_LOG_ERR(VPL_SG_SOCKET, "send failed: %s", strerror(errno));
        rv = IPC_ERROR_SOCKET_SEND;
        goto out;
    }

out:
    return rv;
}

s32
IPCSocket_SendFile(IPCSocket *sock, s32 fd, off_t *offset, size_t length)
{
    VPL_LIB_LOG_FINEST(VPL_SG_SOCKET, "IPCSocket_SendFile %d %u", fd, length);

    s32 rv = IPC_ERROR_OK;

    if (sock == NULL || fd < 0 || offset == NULL || length <= 0) {
        VPL_LIB_LOG_ERR(VPL_SG_SOCKET, "sock = NULL | fd < 0 | offset = NULL | length <= 0");
        rv = IPC_ERROR_PARAMETER;
        goto out;
    }

    if (sendfile(sock->fd, fd, offset, length) < 0) {
        VPL_LIB_LOG_ERR(VPL_SG_SOCKET, "sendfile failed: %s", strerror(errno));
        rv = IPC_ERROR_SOCKET_SEND_FILE;
    }

out:
    return rv;
}

s32
IPCSocket_SendTo(IPCSocket *sock, const s8* address, u16 port, const void *buffer, u32 length, s32 flags)
{
    VPL_LIB_LOG_FINEST(VPL_SG_SOCKET, "IPCSocket_SendTo %s %d %u %d", address, port, length, flags);

    struct sockaddr_in addr;
    s32 rv = IPC_ERROR_OK;

    if (sock == NULL || sock->type != IPC_SOCKET_UDP || address == NULL || buffer == NULL || length <= 0) {
        VPL_LIB_LOG_ERR(VPL_SG_SOCKET, "sock = NULL | sock->type != IPC_SOCKET_UDP | address = NULL | buffer = NULL | length <= 0");
        rv = IPC_ERROR_PARAMETER;
        goto out;
    }

    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    if (inet_pton(AF_INET, (const char *)address, &addr.sin_addr) <= 0) {
        VPL_LIB_LOG_ERR(VPL_SG_SOCKET, "inet_pton failed: %s", strerror(errno));
        rv = IPC_ERROR_SOCKET_SEND_TO;
        goto out;
    }

    if ((sock->sendDelay) > 0) {
        VPL_LIB_LOG_FINEST(VPL_SG_SOCKET, "send delay = %u microseconds", sock->sendDelay);
        usleep(sock->sendDelay);
    }

    rv = sendto(sock->fd, buffer, length, flags | MSG_NOSIGNAL, (struct sockaddr*)&addr, sizeof(addr));
    if (rv < 0) {
        VPL_LIB_LOG_ERR(VPL_SG_SOCKET, "sendto failed: %s", strerror(errno));
        rv = IPC_ERROR_SOCKET_SEND_TO;
        goto out;
    }

out:
    return rv;
}
#endif

IPCError
IPCSocket_SetDelay(IPCSocket *sock, u32 send, u32 receive) // microseconds
{
    VPL_LIB_LOG_FINEST(VPL_SG_SOCKET, "IPCSocket_SetDelay %u %u", send, receive);

    IPCError rv = IPC_ERROR_OK;

    if (sock == NULL) {
        VPL_LIB_LOG_ERR(VPL_SG_SOCKET, "sock = NULL");
        rv = IPC_ERROR_PARAMETER;
        goto out;
    }

    sock->sendDelay = send;
    sock->recvDelay = receive;

out:
    return rv;
}

#endif // !defined(WIN32) && !defined(ANDROID)
