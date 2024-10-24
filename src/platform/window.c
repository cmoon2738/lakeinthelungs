#include "hadopelagic.h"

Window *HadalCreateWindow(u32 width, u32 height, const char *title, Window *share)
{
    LogVerbose("HADAL: creating a new window!");
    (void)width;
    (void)height;
    (void)title;
    (void)share;
    return NULL;
}

void HadalDestroyWindow(Window *window)
{
    LogVerbose("HADAL: destroying a window!");
    if (window) {
        /* TODO */
    }    
}

u32 HadalGetFlags(Window *window)
{
    if (window)
        return window->flags;
    return 0;
}
