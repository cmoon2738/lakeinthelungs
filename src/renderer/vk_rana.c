#include "vk.h"
#include "renderer.h"

i32 RanaVulkan_init(void)
{
    LogVerbose("RANA: initializing Vulkan...");
    return LAKE_SUCCESS;
}

void RanaVulkan_terminate(void)
{
    LogVerbose("RANA: terminating Vulkan...");
    VulkanCloseDriver();
}
