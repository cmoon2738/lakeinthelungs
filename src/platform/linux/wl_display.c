#include "../hadopelagic.h"
#include "common.h"

bool HadalWayland_Connect(void)
{
    LogVerbose("HADAL: try connecting to Wayland...");

    const HadalAPI wayland = {
        .id = HADAL_BACKEND_WAYLAND,
        .init = HadalWayland_Init,
        .terminate = HadalWayland_Terminate,
#ifdef LAKE_NATIVE_VULKAN
        .vkPhysicalDevicePresentationSupport = HadalWayland_vkPhysicalDevicePresentationSupport,
        .vkCreateSurface = HadalWayland_vkCreateSurface,
#endif
    };

    if (!_HadalDebugVerifyAPI(&wayland)) {
        LogDebug("HADAL: Internal API for Wayland is not up to date.");
    }

    struct wl_display *display = wl_display_connect(NULL);
    if (!display) {
        LogDebug("HADAL: Can't connect to a Wayland display.");
        return false;
    }
    HADAL.wl.display = display;
    HADAL.api = wayland;
    return true;
}

i32 HadalWayland_Init(void)
{
    LogVerbose("HADAL: initializing Wayland...");
    return LAKE_SUCCESS;
}

void HadalWayland_Terminate(void)
{
    LogVerbose("HADAL: terminating Wayland...");
}
