#include "wl.h"
#include "../hadopelagic.h"

#include "../../renderer/rana.h"

//#include <errno.h>
//#include <poll.h>

static void resize_framebuffer(Window *window)
{
    /* TODO add fractional scale */

    window->wl.fb_width = window->wl.width * 1.f;   // window->wl.buffer_scale
    window->wl.fb_height = window->wl.height * 1.f; // window->wl.buffer_scale
                                                    
    if (read_flags(window->flags, WINDOW_FLAG_TRANSPARENT)) {
        /* TODO set opaque content area */
    }
    /* TODO framebuffer resized event */
    rana_set_framebuffer_resized(); /* this only makes sense with only one window, so its fine :3 */
}

static bool resize_window(Window *window, i32 width, i32 height)
{
    width = max(width, 1);
    height = max(height, 1);

    if ((u32)width == window->wl.width && (u32)height == window->wl.height)
        return false;

    window->wl.width = width;
    window->wl.height = height;

    resize_framebuffer(window);

    /* TODO scaling viewport */

    return true;
}

/*
static bool flush_display(void)
{
    while (wl_display_flush(HADAL.wl.display) == -1) {
        if (errno != EAGAIN)
            return false;
        struct pollfd fd = { wl_display_get_fd(HADAL.wl.display), POLLOUT, 0 };
        while (poll(&fd, 1, -1) == -1) {
            if (errno != EINTR && errno != EAGAIN) {
                return false;
            }
        }
    }
    return true;
}
*/

static void handle_surface_enter(void *data,
                                 struct wl_surface *surface,
                                 struct wl_output *wl_output)
{
    if (wl_proxy_get_tag((struct wl_proxy *)wl_output) != &HADAL.wl.tag)
        return;

    /* unused */
    (void)surface;

    Window *window = data;
    Output *output = wl_output_get_user_data(wl_output);

    if (!window || !output)
        return;

    /* TODO handle output scale buffers */
}

static void handle_surface_leave(void *data,
                                 struct wl_surface *surface,
                                 struct wl_output *wl_output)
{
    if (wl_proxy_get_tag((struct wl_proxy *)wl_output) != &HADAL.wl.tag)
        return;

    /* unused */
    (void)surface;

    Window *window = data;

    /* TODO handle output scale buffers */
    (void)wl_output;
    (void)window;
}

static const struct wl_surface_listener surface_listener = {
    .enter = handle_surface_enter,
    .leave = handle_surface_leave,
    .preferred_buffer_scale = NULL,
    .preferred_buffer_transform = NULL,
};

static void handle_xdg_toplevel_configure(void *data,
                                          struct xdg_toplevel *toplevel,
                                          i32 width, i32 height,
                                          struct wl_array *states)
{
    /* unused */
    (void)toplevel;

    Window *window = data;
    u32 *state;

    unset_flags(window->wl.pending.flags,
            WINDOW_FLAG_ACTIVATED 
          | WINDOW_FLAG_MAXIMIZED 
          | WINDOW_FLAG_FULLSCREEN
    );

    wl_array_for_each(state, states) {
        switch (*state) {
        case XDG_TOPLEVEL_STATE_MAXIMIZED:
            set_flags(window->wl.pending.flags, WINDOW_FLAG_MAXIMIZED);
            break;
        case XDG_TOPLEVEL_STATE_FULLSCREEN:
            set_flags(window->wl.pending.flags, WINDOW_FLAG_FULLSCREEN);
            break;
        case XDG_TOPLEVEL_STATE_RESIZING:
            set_flags(window->wl.pending.flags, WINDOW_FLAG_IS_RESIZING);
            break;
        case XDG_TOPLEVEL_STATE_ACTIVATED:
            set_flags(window->wl.pending.flags, WINDOW_FLAG_ACTIVATED);
            break;
        }
    }

    if (width && height) {
        window->wl.pending.width = width;
        window->wl.pending.height = height;
    } else {
        window->wl.pending.width = window->wl.width;
        window->wl.pending.height = window->wl.height;
    }
}

static void handle_xdg_toplevel_close(void *data, struct xdg_toplevel *toplevel)
{
    /* unused */
    (void)toplevel;

    Window *window = data;
    set_flags(window->flags, WINDOW_FLAG_SHOULD_CLOSE);
}

static const struct xdg_toplevel_listener xdg_toplevel_listener = {
    .configure = handle_xdg_toplevel_configure,
    .close = handle_xdg_toplevel_close,
};

static void handle_xdg_surface_configure(void *data,
                                         struct xdg_surface *surface,
                                         u32 serial)
{
    Window *window = data;

    xdg_surface_ack_configure(surface, serial);
    if (read_flags(window->flags, WINDOW_FLAG_ACTIVATED) 
        != read_flags(window->wl.pending.flags, WINDOW_FLAG_ACTIVATED))
    {
        toggle_flags(window->flags, WINDOW_FLAG_ACTIVATED);
        if (!read_flags(window->flags, WINDOW_FLAG_ACTIVATED)) {
            if (window->output && read_flags(window->flags, WINDOW_FLAG_AUTO_ICONIFY)) {
                xdg_toplevel_set_minimized(window->wl.xdg.toplevel);
            }
        }
    }
    if (read_flags(window->flags, WINDOW_FLAG_MAXIMIZED)
        != read_flags(window->wl.pending.flags, WINDOW_FLAG_MAXIMIZED))
    {
        toggle_flags(window->flags, WINDOW_FLAG_MAXIMIZED);
        /* TODO window maximized event */
    }
    if (read_flags(window->wl.pending.flags, WINDOW_FLAG_FULLSCREEN)) {
        set_flags(window->flags, WINDOW_FLAG_FULLSCREEN);
    } else {
        unset_flags(window->flags, WINDOW_FLAG_FULLSCREEN);
    }
            
    i32 width = window->wl.pending.width;
    i32 height = window->wl.pending.height;

    if (!read_flags(window->flags, WINDOW_FLAG_MAXIMIZED | WINDOW_FLAG_FULLSCREEN)) {
        if (window->numer != DONT_CARE && window->denom != DONT_CARE) {
            const f32 aspect_ratio = (f32)width / (f32)height;
            const f32 target_ratio = (f32)window->numer / (f32)window->denom;
            if (aspect_ratio < target_ratio) {
                height = width / target_ratio;
            } else if (aspect_ratio > target_ratio) {
                width = height * target_ratio;
            }
        }
    }

    if (resize_window(window, width, height)) {
        /* TODO window resized event */
        if (read_flags(window->flags, WINDOW_FLAG_VISIBLE)) {
            /* TODO window content damaged */
        }
    }
}

static const struct xdg_surface_listener xdg_surface_listener = {
    .configure = handle_xdg_surface_configure,
};

static void update_size_limits(Window *window)
{
    i32 minwidth, minheight, maxwidth, maxheight;

    if (read_flags(window->flags, WINDOW_FLAG_RESIZABLE)) {
        if (window->minwidth == DONT_CARE || window->minheight == DONT_CARE) {
            minwidth = minheight = 0;
        } else {
            minwidth = window->minwidth;
            minheight = window->minheight;
        }
        if (window->maxwidth == DONT_CARE || window->maxheight == DONT_CARE) {
            maxwidth = maxheight = 0;
        } else {
            maxwidth = window->maxwidth;
            maxheight = window->maxheight;
        }
    } else {
        minwidth = maxwidth = window->wl.width;
        minheight = maxheight = window->wl.height;
    }
    xdg_toplevel_set_min_size(window->wl.xdg.toplevel, minwidth, minheight);
    xdg_toplevel_set_max_size(window->wl.xdg.toplevel, maxwidth, maxheight);
}

static bool create_xdg_shell_objects(Window *window)
{
    window->wl.xdg.surface = xdg_wm_base_get_xdg_surface(HADAL.wl.wm_base, window->wl.surface);
    if (!window->wl.xdg.surface) {
        log_error("Failed to create Wayland xdg-surface.");
        return false;
    }
    xdg_surface_add_listener(window->wl.xdg.surface, &xdg_surface_listener, window);

    window->wl.xdg.toplevel = xdg_surface_get_toplevel(window->wl.xdg.surface);
    if (!window->wl.xdg.toplevel) {
        log_error("Failed to create Wayland xdg-toplevel.");
        return false;
    }
    xdg_toplevel_add_listener(window->wl.xdg.toplevel, &xdg_toplevel_listener, window);

    if (window->wl.app_id)
        xdg_toplevel_set_app_id(window->wl.xdg.toplevel, window->wl.app_id);
    xdg_toplevel_set_title(window->wl.xdg.toplevel, window->title);

    if (window->output) {
        xdg_toplevel_set_fullscreen(window->wl.xdg.toplevel, window->output->wl.output);
        // TODO set idle inhibitor, true
    } else {
        if (read_flags(window->flags, WINDOW_FLAG_MAXIMIZED))
            xdg_toplevel_set_maximized(window->wl.xdg.toplevel);
        // TODO set idle inhibitor, false
    }
    update_size_limits(window);
    wl_surface_commit(window->wl.surface);
    wl_display_roundtrip(HADAL.wl.display);
    return true;
}

static void destroy_xdg_shell_objects(Window *window)
{
    if (window->wl.xdg.toplevel)
        xdg_toplevel_destroy(window->wl.xdg.toplevel);
    if (window->wl.xdg.surface)
        xdg_surface_destroy(window->wl.xdg.surface);
    window->wl.xdg.toplevel = NULL;
    window->wl.xdg.surface = NULL;
}

static bool create_surface(Window *window, u32 width, u32 height)
{
    window->wl.surface = wl_compositor_create_surface(HADAL.wl.compositor);
    if (!window->wl.surface) {
        log_error("Failed to create a Wayland window surface.");
        return false;
    }
    wl_proxy_set_tag((struct wl_proxy *)window->wl.surface, &HADAL.wl.tag);
    wl_surface_add_listener(window->wl.surface, &surface_listener, window);

    window->wl.width = width;
    window->wl.height = height;
    window->wl.fb_width = width;
    window->wl.fb_height = height;

    if (!read_flags(window->flags, WINDOW_FLAG_TRANSPARENT)) {
        /* TODO set opaque content area */
    }

    /* TODO fractional scale */

    return true;
}

bool hadal_wayland_create_window(Window *window, u32 width, u32 height)
{
    if (!create_surface(window, width, height))
        return false;

    if (window->output || read_flags(window->flags, WINDOW_FLAG_VISIBLE)) {
        log_debug("Creating xdg shell objects.");
        if (!create_xdg_shell_objects(window)) {
            return false;
        }
    }

    log_debug("Created a Wayland window & surface.");
    return true;
}

void hadal_wayland_destroy_window(Window *window)
{
    /* TODO destroy pointers, scaling viewport and such */

    destroy_xdg_shell_objects(window);

    if (window->wl.surface)
        wl_surface_destroy(window->wl.surface);
    free(window->wl.app_id);
}

void hadal_wayland_get_framebuffer_size(Window *window, u32 *width, u32 *height)
{
    if (width) *width = window->wl.width;
    if (height) *height = window->wl.height;
}

void hadal_wayland_show_window(Window *window)
{
    if (!window->wl.xdg.toplevel) {
        set_flags(window->flags, WINDOW_FLAG_VISIBLE);
        create_xdg_shell_objects(window);
    }
}

void hadal_wayland_hide_window(Window *window)
{
    if (read_flags(window->flags, WINDOW_FLAG_VISIBLE)) {
        unset_flags(window->flags, WINDOW_FLAG_VISIBLE);
        destroy_xdg_shell_objects(window);
        wl_surface_attach(window->wl.surface, NULL, 0, 0);
        wl_surface_commit(window->wl.surface);
    }
}
