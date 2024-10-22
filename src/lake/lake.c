#include "lake.h"

void LakeInit(void *data)
{
    (void)data;

    LogInfo("init mlem");
}

void LakeFrame(void *data, f64 delta_time)
{
    (void)data;
    (void)delta_time;

    LogDebug("frame mlem");
}

void LakeEvent(void *data, Event *event)
{
    (void)data;
    (void)event;
}

void LakeClean(void *data)
{
    (void)data;

    LogVerbose("clean mlem");
}
