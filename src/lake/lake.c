#include "lake.h"

void lake_init(void *data)
{
    (void)data;

    log_info("init mlem");
}

void lake_frame(void *data, f64 delta_time)
{
    (void)data;
    (void)delta_time;
}

void lake_event(void *data, Event *event)
{
    (void)data;
    (void)event;
}

void lake_clean(void *data)
{
    (void)data;

    log_warn("clean mlem");
}
