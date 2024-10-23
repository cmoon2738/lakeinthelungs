#include "renderer.h"

RanaContext *RanaCreateContext(HadalWindow *window)
{
    LogVerbose("RANA: creating a new rendering context!");
    (void)window;
    return NULL;
}

void RanaDestroyContext(RanaContext *rana)
{
    LogVerbose("RANA: destroying a rendering context!");
    if (rana) {
        /* TODO */
    }
}


u32 RanaGetFlags(RanaContext *rana)
{
    if (rana)
        return rana->flags;
    return 0;
}
