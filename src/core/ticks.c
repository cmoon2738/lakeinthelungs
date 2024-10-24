#include "../common.h"
#include "../platform/system.h"

static u64 tick_start = 0;
static u32 tick_numerator_ms;
static u32 tick_denominator_ms;
static u32 tick_numerator_ns;
static u32 tick_denominator_ns;

static u32 calculate_gcd(u32 a, u32 b)
{
    if (b == 0)
        return a;
    return calculate_gcd(b, (a % b));
}

void ticks_init(void)
{
    u64 tick_freq;
    u32 gcd;

    if (tick_start)
        return;

    tick_freq = sys_timer_frequency();
    assert_debug(tick_freq > 0 && tick_freq <= (u64)UINT32_MAX);

    gcd = calculate_gcd(MS_PER_SECOND, (u32)tick_freq);
    tick_numerator_ms = (MS_PER_SECOND / gcd);
    tick_denominator_ms = (u32)(tick_freq / gcd);

    gcd = calculate_gcd(NS_PER_SECOND, (u32)tick_freq);
    tick_numerator_ns = (NS_PER_SECOND / gcd);
    tick_denominator_ns = (u32)(tick_freq / gcd);

    tick_start = sys_timer_counter();

    if (!tick_start)
        --tick_start;
}

void ticks_quit(void)
{
    tick_start = 0;
}

u64 ticks_ms(void)
{
    u64 starting_value, value;

    if (!tick_start)
        ticks_init();

    starting_value = (sys_timer_counter() - tick_start);
    value = (starting_value * tick_numerator_ms);
    assert_debug(value >= starting_value);
    value /= tick_denominator_ms;
    return value;
}

u64 ticks_ns(void)
{
    u64 starting_value, value;

    if (!tick_start)
        ticks_init();

    starting_value = (sys_timer_counter() - tick_start);
    value = (starting_value * tick_numerator_ns);
    assert_debug(value >= starting_value);
    value /= tick_denominator_ns;
    return value;
}
