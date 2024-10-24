#include "wl.h"
#include "../hadopelagic.h"

bool HadalWayland_connect(void)
{
    log_verbose("HADAL: try connecting to Wayland...");

    const HadalAPI wayland = {
        .id = HADAL_BACKEND_WAYLAND,
        .init = HadalWayland_init,
        .terminate = HadalWayland_terminate,
#ifdef AMW_NATIVE_VULKAN
        .vkPhysicalDevicePresentationSupport = HadalWayland_vkPhysicalDevicePresentationSupport,
        .vkCreateSurface = HadalWayland_vkCreateSurface,
#endif /* AMW_NATIVE_VULKAN */
    };
    if (!_hadal_debug_verify_api(&wayland)) {
        log_debug("HADAL: internal API for Wayland is not up to date.");
        return false;
    }

    struct wl_display *display = wl_display_connect(NULL);
    if (!display) {
        log_debug("HADAL: can't connect to a Wayland display.");
        return false;
    }
    HADAL.wl.display = display;
    HADAL.api = wayland;
    return true;
}

i32 HadalWayland_init(void)
{
    log_verbose("HADAL: initializing Wayland...");
    return AMW_SUCCESS;
}

void HadalWayland_terminate(void)
{
    log_verbose("HADAL: terminating Wayland...");
}
