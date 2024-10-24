#include "../system.h"

#include <unistd.h>
#include <sys/time.h>

/* TODO check for 'clock_gettime' in meson */
#define HAVE_CLOCK_GETTIME 1

#ifdef HAVE_CLOCK_GETTIME
    #include <time.h>
#endif

#ifdef AMW_PLATFORM_APPLE
    #include <mach/mach_time.h>
#endif

static bool checked_monotonic = false;
static bool has_monotonic = false;

#if !defined(HAVE_CLOCK_GETTIME) && defined(AMW_PLATFORM_APPLE)
static mach_timebase_info_data_t mach_base_info;
#endif

static void check_monotonic(void)
{
#ifdef HAVE_CLOCK_GETTIME
    struct timespec value;
    if (clock_gettime(CLOCK_MONOTONIC, &value) == 0) {
        has_monotonic = true;
    }
#elif defined(AMW_PLATFORM_APPLE)
    if (mach_timebase_info(&mach_base_info) == 0) {
        has_monotonic = true;
    }
#endif
    checked_monotonic = true;
}

u64 sys_timer_counter(void)
{
    u64 ticks;

    if (!checked_monotonic)
        check_monotonic();

    if (has_monotonic) {
#ifdef HAVE_CLOCK_GETTIME
        struct timespec now;

        clock_gettime(CLOCK_MONOTONIC, &now);
        ticks = now.tv_sec;
        ticks *= NS_PER_SECOND;
        ticks += now.tv_nsec;
#elif defined(AMW_PLATFORM_APPLE)
        ticks = mach_absolute_time();
#else
        assert_debug(false);
        ticks = 0;
#endif
    } else {
        struct timeval now;

        gettimeofday(&now, NULL);
        ticks = now.tv_sec;
        ticks *= US_PER_SECOND;
        ticks += now.tv_usec;
    }
    return ticks;
}

u64 sys_timer_frequency(void)
{
    if (!checked_monotonic)
        check_monotonic();

    if (has_monotonic) {
#ifdef HAVE_CLOCK_GETTIME
        return NS_PER_SECOND;
#elif defined(LAKE_PLATFORM_APPLE)
        u64 freq = mach_base_info.denom;
        freq *= NS_PER_SECOND;
        freq /= mach_base_info.numer;
        return freq;
#endif
    } 
    return US_PER_SECOND;
}
