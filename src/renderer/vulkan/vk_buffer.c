#include "vk.h"
#include "../renderer.h"

#include "common.h"
#include <vulkan/vulkan_core.h>

static const Vertex CUBE_DEMO_VERTICES[] = {
    { .pos = {  0.0f,  0.0f }, .color = { 0.25f, 0.25f, 0.25f } },
    { .pos = { -0.5f, -0.5f }, .color = { 1.00f, 0.00f, 0.25f } },
    { .pos = {  0.5f, -0.5f }, .color = { 0.76f, 0.25f, 0.25f } },
    { .pos = {  0.5f,  0.5f }, .color = { 0.05f, 0.50f, 0.25f } },
    { .pos = { -0.5f,  0.5f }, .color = { 0.25f, 0.75f, 0.25f } },
};

static const u16 CUBE_DEMO_INDICES[] = {
    0, 1, 2,
    0, 2, 3,
    0, 3, 4,
    0, 4, 1
};

i32 rana_vk_create_buffer(VkDeviceSize size, 
                          VkBufferUsageFlags usage, 
                          VkMemoryPropertyFlags properties,
                          VkBuffer *buffer,
                          VkDeviceMemory *buffer_memory)
{
    VkBufferCreateInfo buffer_info = {
        .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
        .size = size,
        .usage = usage,
        .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
    };

    RANA_VK_VERIFY(vkCreateBuffer(RANA.vk.device, &buffer_info, NULL, buffer));

    VkMemoryRequirements memory_requirements;
    vkGetBufferMemoryRequirements(RANA.vk.device, *buffer, &memory_requirements);

    u32 memory_type_idx = vulkan_find_memory_type(
            memory_requirements.memoryTypeBits, properties);
    if (memory_type_idx == UINT32_MAX)
        return AMW_ERROR_STUB;

    VkMemoryAllocateInfo alloc_info = {
        .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
        .allocationSize = memory_requirements.size,
        .memoryTypeIndex = memory_type_idx,
    };

    RANA_VK_VERIFY(vkAllocateMemory(RANA.vk.device, &alloc_info, NULL, buffer_memory));

    vkBindBufferMemory(RANA.vk.device, *buffer, *buffer_memory, 0);

    return AMW_SUCCESS;
}

i32 rana_vk_copy_buffer(VkBuffer dst_buffer, VkBuffer src_buffer, VkDeviceSize size)
{
    VkCommandBufferAllocateInfo alloc_info = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        .commandPool = RANA.vk.command_pool,
        .commandBufferCount = 1,
    };

    VkCommandBuffer command_buffer;
    RANA_VK_VERIFY(vkAllocateCommandBuffers(RANA.vk.device, &alloc_info, &command_buffer));

    VkCommandBufferBeginInfo cmd_buffer_begin_info = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
    };
    vkBeginCommandBuffer(command_buffer, &cmd_buffer_begin_info);

    VkBufferCopy copy_region = {
        .srcOffset = 0,
        .dstOffset = 0,
        .size = size,
    };
    vkCmdCopyBuffer(command_buffer, src_buffer, dst_buffer, 1, &copy_region);

    RANA_VK_VERIFY(vkEndCommandBuffer(command_buffer));

    VkSubmitInfo submit_info = {
        .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
        .commandBufferCount = 1,
        .pCommandBuffers = &command_buffer,
    };
    RANA_VK_VERIFY(vkQueueSubmit(RANA.vk.graphics_queue, 1, &submit_info, VK_NULL_HANDLE));

    vkQueueWaitIdle(RANA.vk.graphics_queue);
    vkFreeCommandBuffers(RANA.vk.device, RANA.vk.command_pool, 1, &command_buffer);

    return AMW_SUCCESS;
}

i32 rana_vk_create_vertex_buffer(void)
{
    i32 result = AMW_SUCCESS;
    VkBuffer staging_buffer; 
    VkDeviceMemory staging_buffer_memory;

    result = rana_vk_create_buffer(
            sizeof(CUBE_DEMO_VERTICES),
            VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
            &staging_buffer,
            &staging_buffer_memory);
    if (result != AMW_SUCCESS)
        return result;

    void *buffer;
    vkMapMemory(RANA.vk.device, 
                staging_buffer_memory, 
                0, sizeof(CUBE_DEMO_VERTICES),
                0, &buffer);
    memcpy(buffer, CUBE_DEMO_VERTICES, (size_t)sizeof(CUBE_DEMO_VERTICES));
    vkUnmapMemory(RANA.vk.device, staging_buffer_memory);

    rana_vk_create_buffer(
            sizeof(CUBE_DEMO_VERTICES),
            VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
            &RANA.vk.vertex_buffer,
            &RANA.vk.vertex_buffer_memory);
    result = rana_vk_copy_buffer(
            RANA.vk.vertex_buffer,
            staging_buffer,
            sizeof(CUBE_DEMO_VERTICES));
    if (result != AMW_SUCCESS)
        return result;

    vkDestroyBuffer(RANA.vk.device, staging_buffer, NULL);
    vkFreeMemory(RANA.vk.device, staging_buffer_memory, NULL);

    return result;
}

i32 rana_vk_create_index_buffer(void)
{
    i32 result = AMW_SUCCESS;
    VkBuffer staging_buffer; 
    VkDeviceMemory staging_buffer_memory;

    result = rana_vk_create_buffer(
            sizeof(CUBE_DEMO_INDICES),
            VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
            &staging_buffer,
            &staging_buffer_memory);
    if (result != AMW_SUCCESS)
        return result;

    void *buffer;
    vkMapMemory(RANA.vk.device, 
                staging_buffer_memory, 
                0, sizeof(CUBE_DEMO_INDICES),
                0, &buffer);
    memcpy(buffer, CUBE_DEMO_VERTICES, (size_t)sizeof(CUBE_DEMO_INDICES));
    vkUnmapMemory(RANA.vk.device, staging_buffer_memory);

    rana_vk_create_buffer(
            sizeof(CUBE_DEMO_INDICES),
            VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
            &RANA.vk.index_buffer,
            &RANA.vk.index_buffer_memory);
    result = rana_vk_copy_buffer(
            RANA.vk.index_buffer,
            staging_buffer,
            sizeof(CUBE_DEMO_INDICES));
    if (result != AMW_SUCCESS)
        return result;

    vkDestroyBuffer(RANA.vk.device, staging_buffer, NULL);
    vkFreeMemory(RANA.vk.device, staging_buffer_memory, NULL);

    return result;
}

i32 rana_vk_create_uniform_buffers(void)
{
    for (u32 i = 0; i < RANA_VK_MAX_FRAMES; i++) {
        i32 result = rana_vk_create_buffer(
                sizeof(UniformBufferObject),
                VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                &RANA.vk.uniform_buffers[i],
                &RANA.vk.uniform_buffers_memory[i]);
        if (result != AMW_SUCCESS)
            return result;

        vkMapMemory(
                RANA.vk.device,
                RANA.vk.uniform_buffers_memory[i],
                0, sizeof(UniformBufferObject),
                0, &RANA.vk.uniform_buffers_mapped[i]);
    }
    return AMW_SUCCESS;
}
