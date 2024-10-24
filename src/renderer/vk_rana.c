#include "vk.h"
#include "renderer.h"
#include "../amw.h" /* AMW.app-> version, name */

i32 RanaVulkan_init(void)
{
    u32         instance_version = vulkan_version();
    u32         layer_count = 0;
    u32         app_version = 0;
    const char *app_name = "unknown";

    log_verbose("RANA: initializing Vulkan...");

    if (AMW.app) {
        app_version = AMW.app->version;
        app_name = AMW.app->name;
    }

    vkEnumerateInstanceLayerProperties(&layer_count, NULL);
    if (layer_count) {
        set_bit(RANA.vk.ext_available, RANA_VK_EXT_VALIDATION_LAYERS_BIT);
    }

#ifndef LAKE_NDEBUG
    VkValidationFeatureEnableEXT validation_feature_enable[] = {
        VK_VALIDATION_FEATURE_ENABLE_BEST_PRACTICES_EXT,
    };
    VkValidationFeaturesEXT validation_features = {
        .sType = VK_STRUCTURE_TYPE_VALIDATION_FEATURES_EXT,
        .pNext = NULL,
        .enabledValidationFeatureCount = array_size(validation_feature_enable),
        .pEnabledValidationFeatures = validation_feature_enable,
        .disabledValidationFeatureCount = 0,
        .pDisabledValidationFeatures = NULL,
    };
#endif /* LAKE_NDEBUG */

    VkApplicationInfo app_info = {
        .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
        .pNext = NULL,
        .pApplicationName = app_name,
        .applicationVersion = app_version,
        .pEngineName = "A Moonlit Walk Engine",
        .engineVersion = AMW_VERSION,
        .apiVersion = instance_version,
    };

    return AMW_SUCCESS;
}

void RanaVulkan_terminate(void)
{
    log_verbose("RANA: terminating Vulkan...");
    vulkan_close_driver();
}
