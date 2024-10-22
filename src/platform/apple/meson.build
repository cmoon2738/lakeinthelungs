if sys == 'darwin'
    #sources = files('')

    defs += [
        '-DLAKE_PLATFORM_APPLE=1',
        '-DVK_USE_PLATFORM_METAL_EXT' ]
    platform += [ dep_threads, dep_math, dep_rt, dep_dl ]
    platform += declare_dependency(link_args: [
        '-framework', 'Cocoa',
        '-framework', 'IOKit',
        '-framework', 'CoreFoundation' ])
    add_languages('objc')

    if host_machine.subsystem() == 'ios'
        defs += '-DLAKE_PLATFORM_IOS=1'
        platform += declare_dependency(link_args: [
            '-framework', 'UIKit' ])
        enabled_display_backends += [ 'cocoa (ios)' ]
    else
        defs += '-DLAKE_PLATFORM_MACOSX=1'
        enabled_display_backends += [ 'cocoa' ]
    endif
endif
