#include "vk.h"
#include "../renderer.h"

#include "common.h"
#include <vulkan/vulkan_core.h>

i32 rana_vk_create_synchronization_objects(void)
{
    VkSemaphoreCreateInfo semaphore_info = {
        .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
    };
    VkFenceCreateInfo fence_info = {
        .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
        .flags = VK_FENCE_CREATE_SIGNALED_BIT,
    };

    for (u32 i = 0; i < RANA_VK_MAX_FRAMES; i++) {
        RANA_VK_VERIFY(vkCreateSemaphore(RANA.vk.device, 
                &semaphore_info, NULL, &RANA.vk.image_available_semaphores[i]));
        RANA_VK_VERIFY(vkCreateSemaphore(RANA.vk.device, 
                &semaphore_info, NULL, &RANA.vk.render_finished_semaphores[i]));
        RANA_VK_VERIFY(vkCreateFence(RANA.vk.device, 
                &fence_info, NULL, &RANA.vk.fences[i]));
    }

    return AMW_SUCCESS;
}
