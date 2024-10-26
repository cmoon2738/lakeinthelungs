#include "../common.h"
#include "hadopelagic.h"

Window *hadal_create_window(u32 width, u32 height, const char *title, Output *output, u32 hints)
{
    if (!HADAL.initialized)
        return NULL;
    assert_debug(title != NULL);

    Window *window = (Window *)malloc(sizeof(Window));
    if (!window) {
        log_error("Failed to allocate resources to create a window.");
        return NULL;
    }
    zerop(window);

    log_debug("Creating a window: %ix%i '%s'", width, height, title);

    window->title  = strdup(title);
    window->output = output;
    window->next = HADAL.window_list_head;
    HADAL.window_list_head = window;

    window->minwidth = DONT_CARE;
    window->minheight = DONT_CARE;
    window->maxwidth = DONT_CARE;
    window->maxheight = DONT_CARE;
    window->numer = DONT_CARE;
    window->denom = DONT_CARE;

    /* setup flags required at initialization */
    set_flags(window->flags, read_flags(hints, WINDOW_FLAG_RESIZABLE));

    if (!HADAL.api.create_window(window, width, height)) {
        hadal_destroy_window(window);
        return NULL;
    }

    /* setup flags for the created window state */
    hadal_set_visible(window, read_flags(hints, WINDOW_FLAG_VISIBLE));

    return window;
}

void hadal_destroy_window(Window *window)
{
    assert_debug(HADAL.initialized);

    if (window == NULL)
        return;

    log_debug("Destroying the window titled '%s'.", window->title);
    HADAL.api.destroy_window(window);
    {
        Window **prev = &HADAL.window_list_head;
        while (*prev != window)
            prev = &((*prev)->next);
        *prev = window->next;
    }
    free(window->title);
    free(window);
}

void hadal_get_framebuffer_size(Window *window, u32 *width, u32 *height)
{
    if (width) *width = 0;
    if (height) *height = 0;

    if (!HADAL.initialized)
        return;
    assert_debug(window);

    HADAL.api.get_framebuffer_size(window, width, height);
}

u32 hadal_get_flags(Window *window)
{
    if (window)
        return window->flags;
    return 0;
}

void hadal_set_should_close(Window *window)
{
    if (window) {
        set_flags(window->flags, WINDOW_FLAG_SHOULD_CLOSE);
        /* TODO window should close event */
    }
}

void hadal_set_visible(Window *window, bool visible)
{
    if (!HADAL.initialized || !window)
        return;

    if (read_flags(window->flags, WINDOW_FLAG_VISIBLE) != visible) {
        toggle_flags(window->flags, WINDOW_FLAG_VISIBLE);
        if (visible) {
            HADAL.api.show_window(window); 
        } else {
            HADAL.api.hide_window(window); 
        }
        /* TODO window visible event */
    }
}
