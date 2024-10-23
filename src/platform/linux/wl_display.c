#include "../hadopelagic.h"
#include "common.h"

bool HadalWayland_connect(void)
{
    LogVerbose("HADAL: try connecting to Wayland...");

    const HadalAPI wayland = {
        .id = HADAL_BACKEND_WAYLAND,
        .init = HadalWayland_init,
        .terminate = HadalWayland_terminate,
#ifdef LAKE_NATIVE_VULKAN
        .vkPhysicalDevicePresentationSupport = HadalWayland_vkPhysicalDevicePresentationSupport,
        .vkCreateSurface = HadalWayland_vkCreateSurface,
#endif /* LAKE_NATIVE_VULKAN */
    };
    if (!_HadalDebugVerifyAPI(&wayland)) {
        LogDebug("HADAL: internal API for Wayland is not up to date.");
        return false;
    }

    struct wl_display *display = wl_display_connect(NULL);
    if (!display) {
        LogDebug("HADAL: can't connect to a Wayland display.");
        return false;
    }
    HADAL.wl.display = display;
    HADAL.api = wayland;
    return true;
}

i32 HadalWayland_init(void)
{
    LogVerbose("HADAL: initializing Wayland...");
    return LAKE_SUCCESS;
}

void HadalWayland_terminate(void)
{
    LogVerbose("HADAL: terminating Wayland...");
}
