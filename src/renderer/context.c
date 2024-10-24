#include "renderer.h"

RanaContext *rana_create_context(Window *window)
{
    log_verbose("RANA: creating a new rendering context!");
    (void)window;
    return NULL;
}

void rana_destroy_context(RanaContext *rana)
{
    log_verbose("RANA: destroying a rendering context!");
    if (rana) {
        /* TODO */
    }
}

u32 rana_get_flags(RanaContext *rana)
{
    if (rana)
        return rana->flags;
    return 0;
}
