project('lakeinthelungs', 'c',
    version : '2024',
    default_options : [ 
        'c_std=c11',
        'buildtype=debugoptimized',
        'default_library=static',
        'warning_level=3',
    ])

cc = meson.get_compiler('c')
sys = host_machine.system()
fs = import('fs')

sources = []
assembly = []
shaders = []

defs = [
    '-Wall',
    '-Wno-pedantic',
    '-Werror=implicit-function-declaration',
    '-Werror=incompatible-pointer-types',
    '-Werror=return-type']
cross = []

if get_option('buildtype') == 'debug' or get_option('buildtype') == 'debugoptimized'
    defs += '-DLAKE_DEBUG'
else 
    defs += '-DLAKE_NDEBUG'
endif

dep_threads = dependency('threads')
dep_math = cc.find_library('m', required : false)
dep_rt = cc.find_library('rt', required : false)
dep_dl = cc.find_library('dl', required : false)

deps = [ dep_threads, dep_math, dep_rt, dep_dl ]

have_posix = cc.has_header_symbol('unistd.h', '_POSIX_VERSION')
systype = (have_posix ? '@0@ (POSIX)' : '@0@').format(host_machine.system())
systype = '@0@, @1@'.format(systype, host_machine.cpu())
if meson.is_cross_build()
    systype = '@0@ (cross-compiling)'.format(systype)
endif

# For summary:
enabled_display_backends = []
enabled_audio_backends = []
enabled_render_backends = []

subdir('assets')
subdir('docs')
subdir('external')
subdir('src')

lakeinthelungs = executable(
    'lakeinthelungs',
    sources, assembly, shaders,
    c_args : defs,
    link_args : cross,
    dependencies : deps,
    include_directories : [ 'src', 'external' ],
    install : false)

summary({
    'System type' : systype,
    'Compiler' : cc.get_id(),
    'Linker' : cc.get_linker_id(),
    'Assembly config' : assembly_config,
    'Build type' : get_option('buildtype'),
}, bool_yn : true, section : 'Lake in the Lungs')

summary({
    'Display backends' : '@0@'.format(', '.join(enabled_display_backends)),
    'Audio backends' : '@0@'.format(', '.join(enabled_audio_backends)),
    'Render backends' : '@0@'.format(', '.join(enabled_render_backends)),
}, bool_yn : true, section : 'Engine Features')
