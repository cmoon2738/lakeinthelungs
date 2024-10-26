#include "common.h"
#include "viewporter-protocol.h"
#include "wl.h"
#include "../hadopelagic.h"
#include "xdg-shell-protocol.h"
#include <wayland-client-core.h>
#include <wayland-client-protocol.h>

static void handle_wm_base_ping(void *data,
                                struct xdg_wm_base *wm_base,
                                u32 serial)
{
    /* unused */
    (void)data;
    xdg_wm_base_pong(wm_base, serial);
}

static const struct xdg_wm_base_listener xdg_wm_base_listener = {
    .ping = handle_wm_base_ping,
};

static void handle_registry_global(void *data,
                                   struct wl_registry *registry,
                                   u32 name,
                                   char const *interface,
                                   u32 version)
{
    /* unused */
    (void)data;

    if (!strcmp(interface, "wl_compositor")) {
        HADAL.wl.compositor = wl_registry_bind(registry, name, &wl_compositor_interface, min(3, version));
    } else if (!strcmp(interface, "wl_subcompositor")) {
        HADAL.wl.subcompositor = wl_registry_bind(registry, name, &wl_subcompositor_interface, 1);
    } else if (!strcmp(interface, "wl_shm")) {
        HADAL.wl.shm = wl_registry_bind(registry, name, &wl_shm_interface, 1);
    } else if (!strcmp(interface, "wl_output")) {
        /* TODO add output */
        log_warn("TODO add output: %d, %d", name, version);
    } else if (!strcmp(interface, "wl_seat")) {
        HADAL.wl.seat = wl_registry_bind(registry, name, &wl_seat_interface, min(4, version));
        /* TODO add seat listener */
        /* TODO key repeat timerfd */
    } else if (!strcmp(interface, "xdg_wm_base")) {
        HADAL.wl.wm_base = wl_registry_bind(registry, name, &xdg_wm_base_interface, 1);
        xdg_wm_base_add_listener(HADAL.wl.wm_base, &xdg_wm_base_listener, NULL);
    } else if (!strcmp(interface, "wp_viewporter")) {
        HADAL.wl.viewporter = wl_registry_bind(registry, name, &wp_viewporter_interface, 1);
    }
}

static void handle_registry_global_remove(void *data,
                                          struct wl_registry *registry,
                                          u32 name)
{
    /* unused */
    (void)data;
    (void)registry;

    for (i32 i = 0; i < HADAL.output_count; i++) {
        Output *output = HADAL.outputs[i];
        if (output->wl.name == name) {
            /* TODO disconnect output */
            return;
        }
    }
}

static const struct wl_registry_listener registry_listener = {
    .global = handle_registry_global,
    .global_remove = handle_registry_global_remove,
};

bool hadal_wayland_connect(void)
{
    log_verbose("Try connecting to Wayland...");

    const HadalAPI wayland = {
        .id = HADAL_BACKEND_WAYLAND,
        .init = hadal_wayland_init,
        .terminate = hadal_wayland_terminate,
        .create_window = hadal_wayland_create_window,
        .destroy_window = hadal_wayland_destroy_window,
        .show_window = hadal_wayland_show_window,
        .hide_window = hadal_wayland_hide_window,
#ifdef AMW_NATIVE_VULKAN
        .vkPhysicalDevicePresentationSupport = hadal_wayland_vkPhysicalDevicePresentationSupport,
        .vkCreateSurface = hadal_wayland_vkCreateSurface,
#endif /* AMW_NATIVE_VULKAN */
    };
    if (!_hadal_debug_verify_api(&wayland)) {
        log_debug("Internal API for Wayland is not up to date.");
        return false;
    }

    struct wl_display *display = wl_display_connect(NULL);
    if (!display) {
        log_debug("Can't connect to a Wayland display.");
        return false;
    }
    HADAL.wl.display = display;
    HADAL.api = wayland;

    log_verbose("Connected to a Wayland display!");
    return true;
}

i32 hadal_wayland_init(void)
{
    HADAL.wl.tag = a_moonlit_walk_version_string();

    HADAL.wl.registry = wl_display_get_registry(HADAL.wl.display);
    wl_registry_add_listener(HADAL.wl.registry, &registry_listener, NULL);

    /* TODO key tables, scancodes, xkb_context */

    /* sync to get all registry objects */
    wl_display_roundtrip(HADAL.wl.display);

    /* sync to get initial output events */
    wl_display_roundtrip(HADAL.wl.display);

    if (!HADAL.wl.wm_base) {
        log_error("Wayland compositor is missing xdg-wm-base protocol support.");
        return AMW_ERROR_STUB;
    } else if (!HADAL.wl.shm) {
        log_error("Wayland compositor is missing wl-shm.");
        return AMW_ERROR_STUB;
    }

    /* TODO data device manager */

    return AMW_SUCCESS;
}

void hadal_wayland_terminate(void)
{
    if (HADAL.wl.subcompositor)
        wl_subcompositor_destroy(HADAL.wl.subcompositor);
    if (HADAL.wl.compositor)
        wl_compositor_destroy(HADAL.wl.compositor);
    if (HADAL.wl.shm)
        wl_shm_destroy(HADAL.wl.shm);
    if (HADAL.wl.viewporter)
        wp_viewporter_destroy(HADAL.wl.viewporter);
    if (HADAL.wl.wm_base)
        xdg_wm_base_destroy(HADAL.wl.wm_base);
    if (HADAL.wl.seat)
        wl_seat_destroy(HADAL.wl.seat);
    if (HADAL.wl.registry)
        wl_registry_destroy(HADAL.wl.registry);
    if (HADAL.wl.display) {
        wl_display_flush(HADAL.wl.display);
        wl_display_disconnect(HADAL.wl.display);
    }
}
