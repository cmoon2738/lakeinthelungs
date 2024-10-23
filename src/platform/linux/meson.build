if sys == 'android' or sys == 'linux' or sys == 'bsd'
    sources += files(
        'linux_os.c',
    )

    # DRM/KMS
    dep_udev = dependency('libudev', required : get_option('kms'))
    dep_drm = dependency('libdrm', required : get_option('kms'))
    dep_gbm = dependency('gbm', required : get_option('kms'))
    build_drmkms = dep_drm.found() and dep_gbm.found() and not get_option('kms').disabled()

    if build_drmkms
        #sources += files('')

        defs += [
            '-DLAKE_NATIVE_KMS=1',
            '-DVK_USE_PLATFORM_KMS_KHR' ]
        deps += [ dep_udev, dep_drm, dep_gbm ]
        enabled_display_backends += [ 'kms' ]
    endif

    defs += '-DLAKE_PLATFORM_LINUX=1'
endif

if sys == 'linux' or sys == 'bsd'
    # Wayland
    dep_wayland_client = dependency('wayland-client', required : get_option('wayland'))
    dep_wayland_protocols = dependency('wayland-protocols', version : '>= 1.12', required : get_option('wayland'))
    dep_xkbcommon = dependency('xkbcommon', required : get_option('wayland'))
    wayland_scanner = find_program('wayland-scanner', required : get_option('wayland'))
    build_wayland = (dep_wayland_client.found() and dep_wayland_protocols.found() and
                        wayland_scanner.found() and dep_xkbcommon.found())
    if build_wayland
        sources += files(
            'wl_display.c',
            'wl_output.c',
            'wl_window.c',
        )

        defs += [ 
            '-DLAKE_NATIVE_WAYLAND=1', 
            '-DVK_USE_PLATFORM_WAYLAND_KHR' ]
        deps += [ dep_wayland_client, dep_xkbcommon ]

        wayland_protocols_dir = dep_wayland_protocols.get_variable('pkgdatadir')
        client_header_args = [ wayland_scanner, 'client-header', '@INPUT@', '@OUTPUT@' ]
        private_code_args = [ wayland_scanner, 'private-code', '@INPUT@', '@OUTPUT@' ]

        wayland_protocols = {
            'xdg-shell'           : '',
            #'xdg-output'          : '-unstable-v1',
            #'xdg-decoration'      : '-unstable-v1',
            #'xdg-activation'      : '-v1',
            #'fractional-scale'    : '-v1',
            #'relative-pointer'    : '-unstable-v1',
            #'pointer-constraints' : '-unstable-v1',
            #'pointer-gestures'    : '-unstable-v1',
            #'idle-inhibit'        : '-unstable-v1',
            'viewporter'          : '',
            #'tablet'              : '-v2'
        }

        foreach name, version : wayland_protocols
            if version == '' or name == 'tablet'
                type = 'stable'
            elif version == '-v1'
                type = 'staging'
            else
                type = 'unstable'
            endif

            protocol_xml_path = wayland_protocols_dir + '/@0@/@1@/@1@@2@.xml'.format(type, name, version)
            client_header = custom_target(
                ['@0@ client-header'.format(name)],
                command : client_header_args,
                input : protocol_xml_path,
                output : name + version + '-protocol.h',
                install : false)
            private_code = custom_target(
                ['@0@ private-code'.format(name)],
                command : private_code_args,
                input : protocol_xml_path,
                output : name + version + '-protocol.c',
                install : false)
            sources += [ client_header, private_code ]
        endforeach

        enabled_display_backends += [ 'wayland' ]
    endif

    # X11 (xcb)
    dep_xcb = dependency('xcb', required : get_option('xcb'))
    dep_xcb_icccm = dependency('xcb-icccm', required : get_option('xcb'))
    build_xcb = dep_xcb.found() and dep_xcb_icccm.found()
    if build_xcb
        #sources += files('')

        defs += [ 
            '-DLAKE_NATIVE_XCB=1', 
            '-DVK_USE_PLATFORM_XCB_KHR' ]
        deps += [ dep_xcb, dep_xcb_icccm ]
        enabled_display_backends += [ 'xcb' ]
    endif
endif
