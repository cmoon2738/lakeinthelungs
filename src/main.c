#include "common.h"
#include "platform/system.h"
#include "platform/hadal.h"
#include "renderer/rana.h"
#include "amw.h"

#include "lake/lake.h"

#include <stdio.h>

#ifdef AMW_PLATFORM_UNIX
    #include <unistd.h>
#elif AMW_PLATFORM_WINDOWS
    #include <windows.h>
    #include <wchar.h>
#endif

AMWEngine AMW = {0};

#define APP_MAIN_LAKE 1
static u32 option_app_main = APP_MAIN_LAKE; 
static u32 option_log_level = LOG_VERBOSE; 
static u32 option_hadal_backend = HADAL_ANY_BACKEND;

static void cleanup(void)
{
    AMW.app->clean(AMW.app->data);

    rana_terminate();

    if (AMW.window)
        hadal_destroy_window(AMW.window);

    hadal_terminate();

    ticks_quit();
    zero(AMW);
}

static void run_mainloop(void)
{
    u64 time_now = sys_timer_counter();
    u64 time_last = 0;

    f32 timer = 3000.f;

    while (!read_flags(AMW.flags, AMW_FLAG_SHOULD_QUIT)) {
        time_last = time_now;
        time_now = sys_timer_counter();
        AMW.delta_time = (f64)((time_now - time_last)*1000 / (f64)sys_timer_frequency());

        Event *event = NULL;
        /* TODO poll from the event loop */
        while (event) {
            switch (event->type) {
            default:
                break;
            }
            AMW.app->event(AMW.app->data, event);
        }
        rana_begin_frame();

        AMW.app->frame(AMW.app->data, AMW.delta_time);

        /* if (read_flags(hadal_get_flags(AMW.window), WINDOW_FLAG_SHOULD_CLOSE))
                set_flags(AMW.flags, AMW_FLAG_SHOULD_QUIT); */

        rana_end_frame();

        /* TODO testing */
        //set_flags(AMW.flags, AMW_FLAG_SHOULD_QUIT);
        sys_sleep(10000);
        timer = (f32)timer - AMW.delta_time;
        if (timer < 0 || read_flags(hadal_get_flags(AMW.window), WINDOW_FLAG_SHOULD_CLOSE))
            set_flags(AMW.flags, AMW_FLAG_SHOULD_QUIT);
    }
    /* When the main loop returns, the fiber job system 
     * will destroy all threads (except the main one) */
}

static void a_moonlit_walk(AppDescription *app_desc)
{
    AMW.app = app_desc;

    ticks_init();
    log_set_level(option_log_level);

    hadal_init(option_hadal_backend);
    AMW.window = hadal_create_window(800, 600, AMW.app->name, NULL, 
            WINDOW_FLAG_VISIBLE
          | WINDOW_FLAG_RESIZABLE
    );

    rana_init(RANA_ANY_BACKEND, AMW.window);

    AMW.app->init(AMW.app->data);

    run_mainloop();
    cleanup();
}

NORETURN static void print_version(FILE *f) 
{
    fprintf(f, "%s", a_moonlit_walk_version_string());
    sys_exit(0);
}

NORETURN static void print_usage(FILE *f) 
{
    const char *usage = 
        "usage: lakeinthelungs [-m <application>] [-d <display>] [-l <log_level>]"
        "\n\n"
        "   -m <application>    What demo to run from the main entry point, where <application>\n"
        "                       is one of: 'lake' (default).\n"
        "\n"
        "   -l <log_level>      Set a log scope, where <log_level> is one of:\n"
        "                       'verbose' (default), 'debug', 'info', 'warn', 'error', 'fatal'.\n"
        "\n"
        "   -d <display>        What display/platform backend to run, where <display> is one of:\n"
        "                       'auto' (default), 'headless'"
#if defined(AMW_PLATFORM_WINDOW)
        ", 'win32'"
#elif defined(AMW_PLATFORM_MACOSX)
        ", 'cocoa'"
#elif defined(AMW_PLATFORM_IOS)
        ", 'ios'"
#elif defined(AMW_PLATFORM_ANDROID)
        ", 'android'"
#elif defined(AMW_PLATFORM_EMSCRIPTEN)
        ", 'wasm'"
#endif
#if defined(AMW_NATIVE_WAYLAND)
        ", 'wayland'"
#endif
#if defined(AMW_NATIVE_XCB)
        ", 'xcb'"
#endif
#if defined(AMW_NATIVE_KMS)
        ", 'kms'"
#endif
        "\n\n"
        "   -h                  Displays this help message.\n"
        "\n"
        "   -v                  Prints the version.\n"
        ;
    fprintf(f, "%s", usage);
    sys_exit(0);
}

static bool log_level_from_string(const char *str, u32 *level)
{
    if ((strcmp(str, "verbose")) == 0) {
        *level = LOG_VERBOSE;
    } else if (strcmp(str, "debug") == 0) {
        *level = LOG_DEBUG;
    } else if (strcmp(str, "info") == 0) {
        *level = LOG_INFO;
    } else if (strcmp(str, "warn") == 0) {
        *level = LOG_WARN;
    } else if (strcmp(str, "error") == 0) {
        *level = LOG_ERROR;
    } else if (strcmp(str, "fatal") == 0) {
        *level = LOG_FATAL;
    } else {
        return false;
    }
    return true;
}

static bool app_main_from_string(const char *str, u32 *app)
{
    if ((strcmp(str, "lake")) == 0) {
        *app = APP_MAIN_LAKE;
    } else {
        return false;
    }
    return true;
}

static bool hadal_backend_from_string(const char *str, u32 *id)
{
    if ((strcmp(str, "auto")) == 0) {
        *id = HADAL_ANY_BACKEND;
    } else if ((strcmp(str, "win32")) == 0) {
        *id = HADAL_BACKEND_WIN32;
    } else if ((strcmp(str, "cocoa")) == 0) {
        *id = HADAL_BACKEND_COCOA;
    } else if ((strcmp(str, "ios")) == 0) {
        *id = HADAL_BACKEND_IOS;
    } else if ((strcmp(str, "android")) == 0) {
        *id = HADAL_BACKEND_ANDROID;
    } else if ((strcmp(str, "wayland")) == 0) {
        *id = HADAL_BACKEND_WAYLAND;
    } else if ((strcmp(str, "xcb")) == 0) {
        *id = HADAL_BACKEND_XCB;
    } else if ((strcmp(str, "kms")) == 0) {
        *id = HADAL_BACKEND_KMS;
    } else if ((strcmp(str, "headless")) == 0) {
        *id = HADAL_BACKEND_HEADLESS;
    } else {
        return false;
    }
    return true;
}

static AppDescription amw_main(i32 argc, char **argv)
{
    AppDescription app_desc;

#ifdef AMW_PLATFORM_UNIX
    /* Setting '+' in the optstring is the same as setting POSIXLY_CORRECT in
     * the enviroment. It tells getopt to stop parsing argv when it encounters
     * the first non-option argument; it also prevents getopt from permuting
     * argv during parsing.
     *
     * The initial ':' in the optstring makes getopt return ':' when an option
     * is missing a required argument. */
    static const char *optstring = "+:hvm:d:l:";
    i32 opt;

    while ((opt = getopt(argc, argv, optstring)) != -1) {
        switch (opt) {
        case 'h': print_usage(stderr);
        case 'v': print_version(stderr);
        case 'l':
            if (!log_level_from_string(optarg, &option_log_level)) {
                fprintf(stderr, "Option '-l' given bad log level.\n");
                print_usage(stderr);
            }
            break;
        case 'd':
            if (!hadal_backend_from_string(optarg, &option_hadal_backend)) {
                fprintf(stderr, "Option '-d' given bad display backend.\n");
                print_usage(stderr);
            }
            break;
        case 'm':
            if (!app_main_from_string(optarg, &option_app_main)) {
                fprintf(stderr, "Option '-m' given bad application entry point.\n");
                print_usage(stderr);
            }
            break;
        case '?':
            fprintf(stderr, "Unknown option '-%C'.\n", optopt);
            print_usage(stderr);
        case ':':
            fprintf(stderr, "Option '-%c' requires an argument.\n", optopt);
            print_usage(stderr);
        default:
            sys_exit(0);
        }
    }
    if (optind != argc) {
        fprintf(stderr, "Error trailing arguments.\n");
        print_usage(stderr);
    }
#elif AMW_PLATFORM_WINDOWS
    /* TODO */
    (void)argc;
    (void)argv;
#endif /* windows/unix, parsing arguments */

    switch (option_app_main) {
    case APP_MAIN_LAKE:
    default:
        app_desc.init = lake_init;
        app_desc.frame = lake_frame;
        app_desc.event = lake_event;
        app_desc.clean = lake_clean;
        app_desc.data = NULL;
        app_desc.name = "Lake in the Lungs";
        app_desc.version = AMW_VERSION;
        break;
    }
    return app_desc;
}

/* WINDOWS */
#if defined(AMW_PLATFORM_WINDOWS)
static char **command_line_to_utf8_argv(LPWSTR w_command_line, i32 *o_argc)
{
    i32 argc = 0;
    char **argv = 0;
    char  *args;

    LPWSTR *w_argv = CommandLineToArgvW(w_command_line, &argc);
    if (w_argv == NULL) {
        LogError("Win32 couldn't fetch command line arguments");
    } else {
        size_t size = wcslen(w_command_line) * 4;
        void *ptr_argv = malloc(((size_t)argc + 1) * sizeof(char *) + size);
        zerop(ptr_argv);
        argv = (char **)ptr_argv;
        assert_release(argv);
        args = (char *)&argv[argc + 1];

        i32 n;
        for (i32 i = 0; i < argc; ++i) {
            n = WideCharToMultiByte(CP_UTF8, 0, w_argv[i], -1, args, (i32)size, NULL, NULL);
            if (n == 0) {
                log_error("Win32 got a 0 length argument");
                break;
            }
            argv[i] = args;
            size -= (size_t)n;
            args += n;
        }
        LocalFree(w_argv);
    }
    *o_argc = argc;
    return argv;
}

#ifdef PLATFORM_WINDOWS_FORCE_MAIN
i32 main(i32 argc, char **argv)
{
    AppDescription app_desc = amw_main(argc, argv);
    a_moonlit_walk(&app_desc);
    sys_exit(0);
}
#else
i32 WINAPI WinMain(_In_ HINSTANCE hInstance, 
                   _In_opt_ HINSTANCE hPrevInstance, 
                   _In_ LPSTR lpCmdLine, 
                   _In_ int32_t nCmdShow)
{
    (void)hInstance;
    (void)hPrevInstance;
    (void)lpCmdLine;
    (void)nCmdShow;

    i32 argc_utf8 = 0;
    char **argv_utf8 = command_line_to_utf8_argv(GetCommandLineW(), &argc_utf8);

    AppDescription app_desc = amw_main(argc_utf8, argv_utf8);
    a_moonlit_walk(&app_desc);

    free(argv_utf8);
    sys_exit(0);
}
#endif /* PLATFORM_WINDOWS_FORCE_MAIN */

/* APPLE MAC OSX */
#elif LAKE_PLATFORM_MACOSX
    /* TODO */

/* APPLE IOS */
#elif LAKE_PLATFORM_IOS
    /* TODO */

/* ANDROID */
#elif LAKE_PLATFORM_ANDROID
#include <android/log.h>
#include <android_native_app_glue.h>
#include <jni.h>

JNIEXPORT void ANativeActivity_onCreate(ANativeActivity* activity, 
                                        void* saved_state, 
                                        size_t saved_state_size) 
{
    /* TODO */
}

/* LINUX BSD UNIX */
#else
i32 main(i32 argc, char **argv)
{
    AppDescription app_desc = amw_main(argc, argv);
    a_moonlit_walk(&app_desc);
    sys_exit(0);
}
#endif

char *a_moonlit_walk_version_string(void)
{
    return "A Moonlit Walk Engine ver. "
        MAKE_VERSIONSTR(AMW_VERSION_MAJOR, AMW_VERSION_MINOR, AMW_VERSION_REVISION)

/* Compiler */
        ","
#if defined(CC_GCC_VERSION)
        " GCC"
#elif defined(CC_CLANG_VERSION)
        " Clang"
#elif defined(CC_ARM_VERSION)
        " ARMC"
#elif defined(CC_EMSCRIPTEN_VERSION)
        " Emscripten"
#elif defined(CC_MSVC_VERSION)
        " MSVC"
#elif defined(__MINGW64_VERSION_MAJOR) /* TODO add mingw version to CC_ defines */
        " MinGW-w64"
#elif defined(__MINGW32__)
        " MinGW"
#elif defined(CC_INTEL_VERSION)
        " IntelC"
#elif defined(CC_PGI_VERSION)
        " PGIC"
#elif defined(CC_SUNPRO_VERSION)
        " SUNPRO"
#elif defined(CC_IBM_VERSION)
        " IBMXL"
#endif

/* Platform */
#if defined(AMW_PLATFORM_WINDOWS)
        " Windows"
#elif defined(AMW_PLATFORM_MACOSX)
        " MacOSX"
#elif defined(AMW_PLATFORM_IOS)
        " iOS"
#elif defined(AMW_PLATFORM_ANDROID)
        " Android"
#elif defined(AMW_PLATFORM_EMSCRIPTEN)
        " HTML5"
#elif defined(AMW_PLATFORM_LINUX)
        " Linux"
    #if defined(__FreeBSD__) || defined(__OpenBSD__)
        " BSD"
    #endif
#endif
#if defined(AMW_PLATFORM_UNIX)
        " (POSIX)"
#endif
        ","

/* Architecture */
#if defined(ARCH_AARCH64)
        " Aarch64"
#elif defined(ARCH_ARM)
        " ARM Arch"
#elif defined(ARCH_ALPHA)
        " ALpha"
#elif defined(ARCH_AVR)
        " Atmel AVR"
#elif defined(ARCH_BLACKFIN)
        " Blackfin"
#elif defined(ARCH_CRIS)
        " Etrax CRIS"
#elif defined(ARCH_CONVEX)
        " Convex"
#elif defined(ARCH_EPIPHANY)
        " Adapteva Epiphany"
#elif defined(ARCH_FRV)
        " Fujitsu FR-V"
#elif defined(ARCH_H8300)
        " H8/300"
#elif defined(ARCH_HPPA)
        " HP/PA"
#elif defined(__amd64__) || defined(__amd64)
        " amd64"
#elif defined(__x86_64__) || defined(__x86_64)
        " x86_64"
#elif defined(ARCH_X86)
        " x86"
#elif defined(ARCH_E2K)
        " Elbrus"
#elif defined(ARCH_IA64)
        " Itanium"
#elif defined(ARCH_M32R)
        " Renesas M32R"
#elif defined(ARCH_M68K)
        " Motorola 68000"
#elif defined(ARCH_MIPS)
        " MIPS"
#elif defined(ARCH_MN10300)
        " Matsushita MN10300"
#elif defined(ARCH_POWER)
        " PowerPC"
#elif defined(ARCH_RISCV)
        " RISC-V"
#elif defined(ARCH_SPARC)
        " SPARC"
#elif defined(ARCH_SUPERH)
        " SuperH"
#elif defined(ARCH_ZARCH)
        " IBM System z"
#elif defined(ARCH_TMS320)
        " TMS320 DSP"
#elif defined(ARCH_WASM)
        " WebAssembly"
#elif defined(ARCH_XTENSA)
        " Xtensa"
#elif defined(ARCH_LOONGARCH)
        " LoongArch"
#endif

/* Math SIMD implementation */
#if defined(ARCH_ARM_NEON)
        " Neon"
#elif defined(ARCH_X86_AVX)
        " AVX"
#elif defined(ARCH_X86_SSE2)
        " SSE2" 
#elif defined(ARCH_LOONGARCH_LSX)
        " LSX"
#elif defined(ARCH_WASM_SIMD128)
        " simd128"
#endif
        ","

/* Display */
#if defined(AMW_PLATFORM_WINDOWS)
        " Win32"
#endif
#if defined(AMW_PLATFORM_APPLE)
        " Cocoa"
#endif 
#if defined(AMW_NATIVE_WAYLAND)
        " Wayland"
#endif
#if defined(AMW_NATIVE_XCB)
        " XCB"
#endif 
#if defined(AMW_NATIVE_KMS)
        " DRM/KMS"
#endif

/* Renderer */
#if defined(AMW_NATIVE_VULKAN)
        " Vulkan"

#elif defined(AMW_NATIVE_WEBGPU)
        " WebGPU"
#endif

/* Audio */
#if defined(AMW_NATIVE_COREAUDIO)
        " CoreAudio"
#endif
#if defined(AMW_NATIVE_ASIO)
        " Steinberg ASIO"
#endif
#if defined(AMW_NATIVE_WASAPI)
        " WASAPI"
#endif
#if defined(AMW_NATIVE_XAUDIO2)
        " XAudio2"
#endif
#if defined(AMW_NATIVE_AAUDIO)
        " AAudio"
#endif
#if defined(AMW_NATIVE_WEBAUDIO)
        " WebAudio"
#endif
#if defined(AMW_NATIVE_ALSA)
        " ALSA"
#endif
#if defined(AMW_NATIVE_JACK)
        " JACK"
#endif
#if defined(AMW_NATIVE_PIPEWIRE)
        " PipeWire"
#endif
#if defined(AMW_NATIVE_PULSEAUDIO)
        " PulseAudio"
#endif
#if defined(AMW_NATIVE_OSS)
        " OSS"
#endif

/* DLL build */
#if defined(AMW_BUILD_DLL)
    #if defined(AMW_PLATFORM_WINDOWS)
        " DLL"
    #elif defined(AMW_PLATFORM_APPLE)
        " dynamic"
    #else
        " shared"
    #endif
#endif
        ;
}
