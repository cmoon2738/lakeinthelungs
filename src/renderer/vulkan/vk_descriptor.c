#include "vk.h"
#include "../renderer.h"

i32 rana_vk_create_descriptor_pool(void)
{
    VkDescriptorPoolSize pool_size = {
        .type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
        .descriptorCount = RANA_VK_MAX_FRAMES,
    };
    VkDescriptorPoolCreateInfo pool_create_info = {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO, 
        .poolSizeCount = 1,
        .pPoolSizes = &pool_size,
        .maxSets = RANA_VK_MAX_FRAMES,
    };

    RANA_VK_VERIFY(vkCreateDescriptorPool(
            RANA.vk.device, &pool_create_info, NULL, &RANA.vk.descriptor_pool));

    return AMW_SUCCESS;
}

i32 rana_vk_create_descriptor_sets(void)
{
    VkDescriptorSetLayout layouts[RANA_VK_MAX_FRAMES];
    for (u32 i = 0; i < RANA_VK_MAX_FRAMES; i++) {
        layouts[i] = RANA.vk.descriptor_set_layout;
    }

    VkDescriptorSetAllocateInfo alloc_info = {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
        .descriptorPool = RANA.vk.descriptor_pool,
        .descriptorSetCount = RANA_VK_MAX_FRAMES,
        .pSetLayouts = layouts,
    };

    RANA_VK_VERIFY(vkAllocateDescriptorSets(RANA.vk.device, &alloc_info, RANA.vk.descriptor_sets));

    for (u32 i = 0; i < RANA_VK_MAX_FRAMES; i++) {
        VkDescriptorBufferInfo buffer_info = {
            .buffer = RANA.vk.uniform_buffers[i],
            .offset = 0,
            .range = sizeof(UniformBufferObject),
        };

        VkWriteDescriptorSet descriptor_write = {
            .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
            .dstSet = RANA.vk.descriptor_sets[i],
            .dstBinding = 0,
            .dstArrayElement = 0,
            .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
            .descriptorCount = 1,
            .pBufferInfo = &buffer_info,
        };
        vkUpdateDescriptorSets(RANA.vk.device, 1, &descriptor_write, 0, NULL);
    }
    return AMW_SUCCESS;
}
