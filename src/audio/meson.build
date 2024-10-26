#sources += files('')

# CoreAudio
dep_core = dependency('appleframeworks', modules : [ 'CoreAudio' ], required : get_option('core'))
if dep_core.found()
    #sources += files('')

    defs += '-DAMW_NATIVE_COREAUDIO=1'
    deps += dep_core
    enabled_audio_backends += [ 'coreaudio' ]
endif

# Steinberg ASIO
# TODO

# WASAPI
found_wasapi = cc.check_header('audioclient.h', required : get_option('wasapi'))
if found_wasapi
    #sources += files('')

    defs += '-DAMW_NATIVE_WASAPI=1'
    deps += cc.find_library('mfplat', required : true)
    deps += cc.find_library('mfuuid', required : true)
    deps += cc.find_library('ksuser', required : true)
    deps += cc.find_library('wmcodecdspuuid', required : true)
    enabled_audio_backends += [ 'wasapi' ]
endif

# XAudio2
dep_xaudio = cc.find_library('xaudio2', required : get_option('xaudio2'))
if dep_xaudio.found()
    #sources += files('')
    
    defs += '-DAMW_NATIVE_XAUDIO2=1'
    deps += dep_xaudio
    enabled_audio_backends += [ 'xaudio2' ]
endif

# AAudio
dep_aaudio = cc.find_library('aaudio', required : get_option('aaudio'))
if dep_aaudio.found()
    #sources += files('')

    defs += '-DAMW_NATIVE_AAUDIO=1'
    deps += dep_aaudio
    enabled_audio_backends += [ 'aaudio' ]
endif

# WebAudio
# TODO

# ALSA
dep_alsa = dependency('alsa', required : get_option('alsa'))
if dep_alsa.found()
    #sources += files('')

    defs += '-DAMW_NATIVE_ALSA=1'
    deps += dep_alsa
    enabled_audio_backends += [ 'alsa' ]
endif

# JACK
dep_jack = dependency('jack', required : get_option('jack'))
if dep_jack.found()
    #sources += files('')

    defs += '-DAMW_NATIVE_JACK=1'
    deps += dep_jack
    enabled_audio_backends += [ 'jack' ]
endif

# PulseAudio
dep_pulse = dependency('libpulse-simple', required : get_option('pulse'))
if dep_pulse.found()
    #sources += files('')

    defs += '-DAMW_NATIVE_PULSEAUDIO=1'
    deps += dep_pulse
    enabled_audio_backends += [ 'pulseaudio' ]
endif

# PipeWire
dep_pipewire = dependency('libpipewire-0.3', required : get_option('pipewire'))
if dep_pipewire.found()
    #sources += files('')

    defs += [ 
        '-DAMW_NATIVE_PIPEWIRE=1', 
        '-D_GNU_SOURCE' ]
    deps += dep_pipewire
    enabled_audio_backends += [ 'pipewire' ]
endif

# OSS
found_oss = not get_option('oss').disabled() and ((sys == 'linux') or (sys == 'bsd'))
if found_oss
    #sources += files('')

    defs += '-DAMW_NATIVE_OSS=1'
    enabled_audio_backends += [ 'oss' ]
endif
