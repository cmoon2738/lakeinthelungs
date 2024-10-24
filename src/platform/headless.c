#include "hadopelagic.h"
#include "headless.h"

bool HadalHeadless_connect(void)
{
    log_verbose("HADAL: running headless...");

    const HadalAPI headless = {
        .id = HADAL_BACKEND_HEADLESS,
        .init = HadalHeadless_init,
        .terminate = HadalHeadless_terminate,
#ifdef AMW_NATIVE_VULKAN
        .vkPhysicalDevicePresentationSupport = HadalHeadless_vkPhysicalDevicePresentationSupport,
        .vkCreateSurface = HadalHeadless_vkCreateSurface,
#endif
    };

    if (!_hadal_debug_verify_api(&headless)) {
        log_debug("Internal API for headless mode is incomplete");
    }

    HADAL.api = headless;

    return true;
}

i32 HadalHeadless_init(void)
{
    return AMW_SUCCESS;
}

void HadalHeadless_terminate(void)
{
}
