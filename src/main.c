#include "common.h"
#include "internal.h"
#include "lake/lake.h"
#include "platform/system.h"

#include <stdio.h>

#ifdef LAKE_PLATFORM_UNIX
#include <unistd.h>
#endif

AMWEngine AMW = {0};

#define APP_MAIN_LAKE 1
static u32 option_app_main = APP_MAIN_LAKE; 
static u32 option_log_level = LOG_VERBOSE; 

static void cleanup(void)
{
    AMW.app->clean(AMW.app->data);

    TicksQuit();
    Zero(AMW);
}

static void run_mainloop(void)
{
    u64 time_now = SysTimerCounter();
    u64 time_last = 0;

    while (!ReadFlags(AMW.flags, AMW_FLAG_SHOULD_QUIT)) {
        time_last = time_now;
        time_now = SysTimerCounter();
        AMW.delta_time = (f64)((time_now - time_last)*1000 / (f64)SysTimerFrequency());

        Event *event = NULL;
        /* TODO poll from the event loop */
        while (event) {
            switch (event->type) {
            default:
                break;
            }
            AMW.app->event(AMW.app->data, event);
        }

        AMW.app->frame(AMW.app->data, AMW.delta_time);

        SetFlags(AMW.flags, AMW_FLAG_SHOULD_QUIT);
    }
    /* When the main loop returns, the fiber job system 
     * will destroy all threads (except the main one) */
}

static void AMoonlitWalk(AppDescription *app_desc)
{
    AMW.app = app_desc;

    TicksInit();
    LogSetLevel(option_log_level);

    AMW.app->init(AMW.app->data);

    run_mainloop();
    cleanup();
}

NORETURN static void print_version(FILE *f) 
{
    fprintf(f, "Lake in the Lungs, A MoonlitWalk Engine ver. %d.%d.%d", 
            LAKE_VERSION_MAJOR, LAKE_VERSION_MINOR, LAKE_VERSION_REVISION);
    SysExit(0);
}

NORETURN static void print_usage(FILE *f) 
{
    const char *usage = 
        "usage: jezioro [-l <log_level>] [-m <application>]"
        "\n\n"
        "   -l <log_level>      Set a log scope, where <log_level> is one of:\n"
        "                       'verbose' (default), 'debug', 'info', 'warn', 'error', 'fatal'.\n"
        "\n"
        "   -m <application>    What demo to run from the main entry point, where <application>\n"
        "                       is one of: 'lake' (default).\n"
        "\n"
        "   -h                  Displays this help message.\n"
        "\n"
        "   -v                  Prints the version.\n"
        ;
    fprintf(f, "%s", usage);
    SysExit(0);
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

static AppDescription AppMain(int32_t argc, char **argv)
{
    AppDescription app_desc;

#ifdef LAKE_PLATFORM_UNIX
    /* Setting '+' in the optstring is the same as setting POSIXLY_CORRECT in
     * the enviroment. It tells getopt to stop parsing argv when it encounters
     * the first non-option argument; it also prevents getopt from permuting
     * argv during parsing.
     *
     * The initial ':' in the optstring makes getopt return ':' when an option
     * is missing a required argument. */
    static const char *optstring = "+:hvl:m:";
    i32 opt;

    while ((opt = getopt(argc, argv, optstring)) != -1) {
        switch (opt) {
        case 'h': print_usage(stderr);
        case 'v': print_version(stderr);
        case 'l':
            if (!log_level_from_string(optarg, &option_log_level)) {
                fprintf(stderr, "Option '-%c' given bad log level.\n", optopt);
                print_usage(stderr);
            }
            break;
        case 'm':
            if (!app_main_from_string(optarg, &option_app_main)) {
                fprintf(stderr, "Option '-%c' given bad display mode.\n", optopt);
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
            SysExit(0);
        }
    }
    if (optind != argc) {
        fprintf(stderr, "Error trailing arguments.\n");
        print_usage(stderr);
    }
#elif LAKE_PLATFORM_WINDOWS
    /* TODO */
    (void)argc;
    (void)argv;
#endif /* LAKE_PLATFORM_UNIX */

    switch (option_app_main) {
    case APP_MAIN_LAKE:
    default:
        app_desc.init = LakeInit;
        app_desc.frame = LakeFrame;
        app_desc.event = LakeEvent;
        app_desc.clean = LakeClean;
        app_desc.data = NULL;
        app_desc.name = "Lake in the Lungs";
        app_desc.version = LAKE_VERSION;
        break;
    }

    return app_desc;
}

/* WINDOWS */
#if defined(LAKE_PLATFORM_WINDOWS)
#include <windows.h>
#include <wchar.h>

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
        Zerop(ptr_argv);
        argv = (char **)ptr_argv;
        AssertRelease(argv);
        args = (char *)&argv[argc + 1];

        i32 n;
        for (i32 i = 0; i < argc; ++i) {
            n = WideCharToMultiByte(CP_UTF8, 0, w_argv[i], -1, args, (int32_t)size, NULL, NULL);
            if (n == 0) {
                LogError("Win32 got a 0 length argument");
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
    AppDescription app_desc = AppMain(argc, argv);
    AMoonlitWalk(&app_desc);
    SysExit(0);
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

    AppDescription app_desc = AppMain(argc_utf8, argv_utf8);
    AMoonlitWalk(&app_desc);

    free(argv_utf8);
    SysExit(0);
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
int main(int argc, char **argv)
{
    AppDescription app_desc = AppMain(argc, argv);
    AMoonlitWalk(&app_desc);
    SysExit(0);
}
#endif
