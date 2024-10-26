#include "vk.h"
#include "../renderer.h"

i32 rana_vk_create_command_buffers(void)
{
    VkCommandBufferAllocateInfo alloc_info = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .commandPool = RANA.vk.command_pool,
        .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        .commandBufferCount = array_size(RANA.vk.command_buffers),
    };

    RANA_VK_VERIFY(vkAllocateCommandBuffers(RANA.vk.device, &alloc_info, RANA.vk.command_buffers));

    return AMW_SUCCESS;
}

i32 rana_vk_record_command_buffer(VkCommandBuffer cmd_buffer, u32 image_idx)
{

    return AMW_SUCCESS;
}
