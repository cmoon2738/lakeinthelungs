#include "../common.h"

#include <stdio.h>
#include <time.h>

static struct Logger {
    //amw_mutex  *lock;
    int32_t     level;
    bool        quiet;
    bool        initialized;
} logger = {0};

static const char *level_strings[] = {
    "V", 
    "D",
    "I",
    "W",
    "E",
    "F"
};

#if LOG_USE_COLOR
static const char *level_colors[] = {
    "\033[38;5;180m",
    "\033[38;5;215m",
    "\033[38;5;209m", 
    "\033[38;5;167m", 
    "\033[38;5;160m",
    "\033[38;5;68m",
};
#endif /* LOG_USE_COLOR */

static void default_callback(LogMessage *log)
{
    char timestamp[22];
    timestamp[strftime(timestamp, sizeof(timestamp), "%H:%M:%S", log->time)] = '\0';

#if LOG_USE_COLOR
    fprintf(log->output, "%s %s%s \x1b[0m", timestamp, 
            level_colors[log->level], level_strings[log->level]);
    if (log->file != NULL) {
        fprintf(log->output, "\033[38;5;8m%26s:%-5d\x1b[0m ", log->file, log->line);
    }
#else
    fprintf(log->output, "%s %s ", timestamp, level_strings[log->level]);
    if (log->file != NULL) {
        fprintf(log->output, "(%s:%d) ", log->file, log->line);
    }
#endif /* LOG_USE_COLOR */
    vfprintf(log->output, log->fmt, log->ap);
    fprintf(log->output, "\n");
    fflush(log->output);
}

static void initialize_logger(void)
{
    if (logger.initialized)
        return;

    //logger.lock = MutexCreate();
    //if (!logger.lock) {
    //    fprintf(stderr, "DEBUG PRINT failed to create a logger mutex lock !!!");
    //}
    logger.initialized = true;
}

/* FIXME
static void terminate_logger(void)
{
    if (logger.initialized) {
        mutex_destroy(logger.lock);
        zero(logger);
    }
}
*/

static void process_message(LogMessage *log)
{
    //mutex_lock(logger.lock);
    if (!logger.quiet && (log->level >= logger.level)) {
        if (!log->time) {
            time_t t = time(NULL);
            log->time = localtime(&t);
        }
        log->output = stderr;
        default_callback(log);
    }
    //mutex_unlock(logger.lock);
}

void log_function_raw(char *fmt, ...)
{
    if (logger.quiet)
        return;
    if (!logger.initialized)
        initialize_logger();

    assert_debug(fmt != NULL);

    va_list ap;
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    va_end(ap);
}

void log_function(i32 level, const char *file, i32 line, const char *fmt, ...)
{
    if (logger.quiet)
        return;
    if (!logger.initialized)
        initialize_logger();

    assert_debug(fmt != NULL);

    LogMessage log = {
        .fmt = fmt,
        .file = file,
        .line = line,
        .level = level,
    };

    va_start(log.ap, fmt);
    process_message(&log);
    va_end(log.ap);
}

i32 log_get_level(void)
{
    return logger.level;
}

void log_set_level(i32 level)
{
    assert_debug(level <= LOG_FATAL && level >= 0);
    logger.level = level;
}

bool log_is_quiet(void)
{
    return logger.quiet;
}

void log_set_quiet(bool quiet)
{
    logger.quiet = quiet;
}
