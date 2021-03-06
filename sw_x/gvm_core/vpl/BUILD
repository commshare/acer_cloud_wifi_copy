vpl_minimal_includes = [
     'include'
    ] + {
        'android':[
            'posix_include',
            'platform_linux/include',
            ],
        'linux':[
            'posix_include',
            'platform_linux/include',
            ],
        'linux_emb':[
            'posix_include',
            'platform_linux/include',
            ],
        'cloudnode':[
            'posix_include',
            'platform_linux/include',
            ],
        'gvm_rt':[
            'posix_include',
            'platform_gvm/include',
            ],
        'win':[
            'platform_win32/include',
            ],
    }[platform]

static_cpp_lib(
    name = 'libvpl_minimal',
    srcs = [],
    includes = vpl_minimal_includes,
    deps = [],
)

static_cpp_lib(
    name = 'libvplcore',
    srcs = [
            'src/vpl_debug.c',
            'src/vpl_version.c',
           ]
           + ([
               'posix_src/vpl__error.c',
               'posix_src/vpl__fs.c',
               'posix_src/vpl__monitor_dir.cpp',
               'posix_src/vpl__net.c',
               'posix_src/vpl__socket.c',
               'posix_src/vpl__strerror.c',
               'posix_src/vpl__th.c',
               'posix_src/vpl__thread_once.c',
               'posix_src/vpl__thread_synch.c',
               'posix_src/vpl__time.c',
              ] if build_target_is_posix else [])
           + {
              'android':[
                         'src_android/vpl__java.c',
                         'src_android/vpl__plat.c',
                         'src_android/vpl__srwlock.c',
                        ],
              'linux':[
                       'platform_linux/src/vpl__plat.c',
                       'posix_src/vpl__dynamic.c',
                       'posix_src/vpl__srwlock.c',
                       'posix_src/vpl__shm.c',
                      ],
              'linux_emb':[
                       'platform_linux/src/vpl__plat.c',
                       'posix_src/vpl__dynamic.c',
                       'posix_src/vpl__srwlock.c',
                       'posix_src/vpl__shm.c',
                      ],
              'cloudnode':[
                       'platform_linux/src/vpl__plat.c',
                       'posix_src/vpl__dynamic.c',
                       'posix_src/vpl__srwlock.c',
                       'posix_src/vpl__shm.c',
                      ],
              'gvm_rt':[
                        'platform_gvm/src/vpl__plat.c',
                        'posix_src/vpl__dynamic.c',
                        'posix_src/vpl__srwlock.c',
                       ],
              'win':[
                       'platform_win32/src/vpl__cond.c',
                       'platform_win32/src/vpl__error.c',
                       'platform_win32/src/vpl__fs.cpp',
                       'platform_win32/src/vpl__lazy_init_condvar.cpp',
                       'platform_win32/src/vpl__lazy_init_mutex.cpp',
                       'platform_win32/src/vpl__monitor_dir.cpp',
                       'platform_win32/src/vpl__mutex.c',
                       'platform_win32/src/vpl__net.c',
                       'platform_win32/src/vpl__plat.cpp',
                       'platform_win32/src/vpl__sem.c',
                       'platform_win32/src/vpl__shellext.cpp',
                       'platform_win32/src/vpl__socket.cpp',
                       'posix_src/vpl__srwlock.c', # Okay because we use the "pthread_win32" library for mingw.
                       'platform_win32/src/vpl__string.c',
                       'platform_win32/src/vpl__th.c',
                       'posix_src/vpl__thread_once.c', # Okay because we use the "pthread_win32" library for mingw.
                       'platform_win32/src/vpl__time.c',
                       'platform_win32/src/vpl__wstring.cpp',
                       'platform_win32/src/vplu__strerror_r.c',
                       'src/vplu_strnlen.c',
                      ],
              }[platform],
    includes = vpl_minimal_includes,
    private_includes = [
        'src'
    ],
    deps = [
           ]
           + {
              'android':[
                       '/:libm',
                       '/:libc',
                        ],
              'linux':[
                       '/:libm',
                       '/:librt',
                       '/:libpthread',
                       '/:libdl',
                       '/:libopenssl',
                      ],
              'linux_emb':[
                       '/:libm',
                       '/:librt',
                       '/:libpthread',
                       '/:libdl',
                       '/:libcrypto',
                       '/:libopenssl',
                      ],
              'cloudnode':[
                       '/:libm',
                       '/:librt',
                       '/:libpthread',
                       '/:libdl',
                       '/:libcrypto',
                       '/:libopenssl',
                      ],
              'gvm_rt':[
                        '/:libm',
                        '/:librt',
                        '/:libpthread',
                        '/:libdl',
                       ],
              'win':[
                     '/:libiphlpapi',
                     '/:libws2_32',
                     '/:libpthread',
                     '/:libadvapi32',
                    ],
              }[platform],
    cflags = '-D_GNU_SOURCE',
)

shared_cpp_lib(
    name = 'libvplcore-shr',
    srcs = [],
    deps = [':libvplcore'],
    soname = 'libvplcore.so.1'
)
