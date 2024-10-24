#include "hadopelagic.h"

Window *hadal_create_window(u32 width, u32 height, const char *title, Window *share)
{
    log_verbose("HADAL: creating a new window!");
    (void)width;
    (void)height;
    (void)title;
    (void)share;
    return NULL;
}

void hadal_destroy_window(Window *window)
{
    log_verbose("HADAL: destroying a window!");
    if (window) {
        /* TODO */
    }    
}

u32 hadal_get_flags(Window *window)
{
    if (window)
        return window->flags;
    return 0;
}
