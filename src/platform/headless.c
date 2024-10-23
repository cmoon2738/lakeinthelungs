#include "hadopelagic.h"
#include "headless.h"

bool HadalHeadless_connect(void)
{
    LogVerbose("HADAL: running headless...");

    const HadalAPI headless = {
        .id = HADAL_BACKEND_HEADLESS,
        .init = HadalHeadless_init,
        .terminate = HadalHeadless_terminate,
#ifdef LAKE_NATIVE_VULKAN
        .vkPhysicalDevicePresentationSupport = HadalHeadless_vkPhysicalDevicePresentationSupport,
        .vkCreateSurface = HadalHeadless_vkCreateSurface,
#endif
    };

    if (!_HadalDebugVerifyAPI(&headless)) {
        LogDebug("Internal API for headless mode is incomplete");
    }

    HADAL.api = headless;

    return true;
}

i32 HadalHeadless_init(void)
{
    return LAKE_SUCCESS;
}

void HadalHeadless_terminate(void)
{
}
