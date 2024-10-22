amw_cpu = host_machine.cpu_family()
amw_os = 'null'
amw_abi = 'null'
amw_asm = 'null'

if amw_cpu == 'x86' or amw_cpu == 'x86_64'
    if get_option('avx')
        defs += '-mavx' # AVX intrinsics
    endif
elif amw_cpu == 'aarch64' or amw_cpu == 'arm'
    amw_os = 'aapcs'
elif amw_cpu == 'mips' 
    amw_os = 'o32'
elif amw_cpu == 'mips64'
    amw_os = 'n64'
elif amw_cpu == 'wasm' or amw_cpu == 'wasm32' or amw_cpu == 'wasm64'
    amw_os = 'web'
    amw_abi = 'ems'
    amw_asm = 'wabt.wat'
endif

if sys == 'windows'
    amw_abi = 'pe'
    if amw_os.contains('null')
        amw_os = 'ms'
    endif
    if cc.get_id() == 'msvc'
        add_languages('masm')
        if amw_cpu == 'arm' or amw_cpu == 'arm64'
            amw_asm = 'armasm.asm'
        else
            amw_asm = 'masm.asm'
        endif
    else
        amw_asm = 'gas.asm'
    endif
elif sys == 'aix'
    amw_abi = 'xcoff'
endif

if amw_os.contains('null')
    amw_os = 'sysv'
endif
if amw_abi.contains('null')
    amw_abi = 'elf'
endif
if amw_asm.contains('null')
    amw_asm = 'gas.S'
endif

assembly_config = amw_cpu + '_' + amw_os + '_' + amw_abi + '_' + amw_asm
assembly += files(
    'jump_' + assembly_config,
    'make_' + assembly_config,
)
