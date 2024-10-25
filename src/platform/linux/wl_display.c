#include "common.h"
#include "wl.h"
#include "../hadopelagic.h"

bool hadal_wayland_connect(void)
{
    log_verbose("Try connecting to Wayland...");

    const HadalAPI wayland = {
        .id = HADAL_BACKEND_WAYLAND,
        .init = hadal_wayland_init,
        .terminate = hadal_wayland_terminate,
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
    log_verbose("Initializing Wayland display backend...");
    return AMW_SUCCESS;
}

void hadal_wayland_terminate(void)
{
    log_verbose("Terminating Wayland display backend...");
}
