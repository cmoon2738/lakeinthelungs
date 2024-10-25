#include "hadopelagic.h"
#include "headless.h"

bool hadal_headless_connect(void)
{
    log_verbose("Running headless display mode...");

    const HadalAPI headless = {
        .id = HADAL_BACKEND_HEADLESS,
        .init = hadal_headless_init,
        .terminate = hadal_headless_terminate,
#ifdef AMW_NATIVE_VULKAN
        .vkPhysicalDevicePresentationSupport = hadal_headless_vkPhysicalDevicePresentationSupport,
        .vkCreateSurface = hadal_headless_vkCreateSurface,
#endif
    };

    if (!_hadal_debug_verify_api(&headless)) {
        log_debug("Internal API for headless display mode is incomplete");
    }

    HADAL.api = headless;

    return true;
}

i32 hadal_headless_init(void)
{
    return AMW_SUCCESS;
}

void hadal_headless_terminate(void)
{
}
