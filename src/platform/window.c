#include "hadopelagic.h"

HadalWindow *HadalCreateWindow(u32 width, u32 height, const char *title, HadalWindow *share)
{
    LogVerbose("HADAL: creating a new window!");
    (void)width;
    (void)height;
    (void)title;
    (void)share;
    return NULL;
}

void HadalDestroyWindow(HadalWindow *window)
{
    LogVerbose("HADAL: destroying a window!");
    if (window) {
        /* TODO */
    }    
}

u32 HadalGetFlags(HadalWindow *window)
{
    if (window)
        return window->flags;
    return 0;
}
