# makegen entry point for building devtools.

external_phony(
    name = 'android_devtools',
    srcdir = '/devtools/android',
)

external_phony(
    name = 'win32_devtools',
    srcdir = '/devtools/win32',
)

external_phony(
    name = 'cloudnode_devtools',
    srcdir = '/devtools/cloudnode',
)

###
# Top-level makegen targets for supported build environments.
# The appropriate one will be invoked when building devtools.
##

pseudo(
    name = 'devtools_to_build_for_android_on_ubuntu_linux',
    deps = [
        '/gvm_core/devtools/protorpcgen',
        ':android_devtools',
    ],
)
pseudo(
    name = 'devtools_to_build_for_android_on_redhat_linux',
    deps = [
        '/gvm_core/devtools/protorpcgen',
        ':android_devtools',
    ],
)
pseudo(
    name = 'devtools_to_build_for_android_on_centos_linux',
    deps = [
        '/gvm_core/devtools/protorpcgen',
        ':android_devtools',
    ],
)
pseudo(
    name = 'devtools_to_build_for_ios_on_osx_mac',
    deps = [
        '/gvm_core/devtools/protorpcgen',
    ],
)
pseudo(
    name = 'devtools_to_build_for_linux_on_ubuntu_linux',
    deps = [
        '/gvm_core/devtools/protorpcgen',
    ],
)
pseudo(
    name = 'devtools_to_build_for_cloudnode_on_ubuntu_linux',
    deps = [
        '/gvm_core/devtools/protorpcgen',
        ':cloudnode_devtools',
    ],
)
pseudo(
    name = 'devtools_to_build_for_cloudnode_on_centos_linux',
    deps = [
        '/gvm_core/devtools/protorpcgen',
        ':cloudnode_devtools',
    ],
)
pseudo(
    name = 'devtools_to_build_for_linux_on_redhat_linux',
    deps = [
        '/gvm_core/devtools/protorpcgen',
    ],
)
pseudo(
    name = 'devtools_to_build_for_linux_on_centos_linux',
    deps = [
        '/gvm_core/devtools/protorpcgen',
    ],
)
pseudo(
    name = 'devtools_to_build_for_linux_emb_on_centos_linux',
    deps = [
        '/gvm_core/devtools/protorpcgen',
    ],
)
pseudo(
    name = 'devtools_to_build_for_win_on_ubuntu_linux',
    deps = [
        '/gvm_core/devtools/protorpcgen',
        ':win32_devtools',
    ],
)
pseudo(
    name = 'devtools_to_build_for_msvc_on_any_cygwin',
    deps = [
        '/gvm_core/devtools/protorpcgen',
        '/external/protobuf_dotnet',
    ],
)
