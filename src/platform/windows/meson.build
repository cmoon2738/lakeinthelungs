# Windows
if sys == 'windows'
    #sources = files('')

    defs += [
        '-DAMW_PLATFORM_WINDOWS',
        '-DVK_USE_PLATFORM_WIN32_KHR' ]
    deps += dep_threads
    deps += cc.find_library('ole32', required: true)
	deps += cc.find_library('winmm', required: true)
    enabled_display_backends += [ 'win32' ]
endif
