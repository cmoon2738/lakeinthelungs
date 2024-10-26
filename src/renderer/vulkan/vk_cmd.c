#include "vk.h"
#include "../renderer.h"

#include "common.h"
#include <vulkan/vulkan_core.h>

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

i32 vulkan_record_command_buffer(VkCommandBuffer command_buffer, u32 image_idx)
{
    VkCommandBufferBeginInfo cmd_buffer_begin = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .flags = 0,
        .pInheritanceInfo = NULL,
    };

    RANA_VK_VERIFY(vkBeginCommandBuffer(command_buffer, &cmd_buffer_begin));

    VkImageMemoryBarrier image_memory_barriers[] = {
        {
            .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
            .dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
            .oldLayout = VK_IMAGE_LAYOUT_UNDEFINED,
            .newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
            .image = RANA.vk.swapchain_images[image_idx],
            .subresourceRange = {
                .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                .baseMipLevel = 0,
                .levelCount = 1,
                .baseArrayLayer = 0,
                .layerCount = 1,
            },
        }
    };

    vkCmdPipelineBarrier(command_buffer,
            VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
            VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
            0, 0, NULL, 0, NULL, array_size(image_memory_barriers),
            image_memory_barriers);

    VkClearValue clear_color = {
        .color = {
            .float32 = { 0.0f, 0.0f, 0.0f, 1.0f },
        },
    };

    VkRenderingAttachmentInfo attachment_info = {
        .sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
        .imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
        .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
        .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
        .clearValue = clear_color,
    };
    if (RANA.vk.msaa_samples == VK_SAMPLE_COUNT_1_BIT) {
        attachment_info.imageView = RANA.vk.swapchain_image_views[image_idx];
    } else {
        attachment_info.imageView = RANA.vk.color_image_view;
        attachment_info.resolveMode = VK_RESOLVE_MODE_AVERAGE_BIT;
        attachment_info.resolveImageView = RANA.vk.swapchain_image_views[image_idx];
        attachment_info.resolveImageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    }

    VkRenderingInfo rendering_info = {
        .sType = VK_STRUCTURE_TYPE_RENDERING_INFO,
        .renderArea = {
            .offset = { 0, 0 },
            .extent = RANA.vk.swapchain_extent,
        },
        .layerCount = 1,
        .colorAttachmentCount = 1,
        .pColorAttachments = &attachment_info,
    };

    vkCmdBeginRendering(command_buffer, &rendering_info);
    vkCmdBindPipeline(command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, RANA.vk.graphics_pipeline);

    VkViewport viewport = {
        .x = 0.0f,
        .y = 0.0f,
        .width = (f32)RANA.vk.swapchain_extent.width,
        .height = (f32)RANA.vk.swapchain_extent.height,
        .minDepth = 0.0f,
        .maxDepth = 1.0f,
    };
    vkCmdSetViewport(command_buffer, 0, 1, &viewport);

    VkRect2D scissor = {
        .offset = { 0, 0 },
        .extent = RANA.vk.swapchain_extent,
    };
    vkCmdSetScissor(command_buffer, 0, 1, &scissor);

    VkBuffer vertex_buffers[] = { RANA.vk.vertex_buffer };
    VkDeviceSize offsets[] = { 0 };
    assert_static(array_size(vertex_buffers) == array_size(offsets), 
            "Vertex buffers size doesn't match offsets size");

    vkCmdBindVertexBuffers(command_buffer, 0, array_size(vertex_buffers), vertex_buffers, offsets);
    vkCmdBindIndexBuffer(command_buffer, RANA.vk.index_buffer, 0, VK_INDEX_TYPE_UINT16);

    vkCmdBindDescriptorSets(command_buffer,
            VK_PIPELINE_BIND_POINT_GRAPHICS,
            RANA.vk.pipeline_layout,
            0, 1, &RANA.vk.descriptor_sets[RANA.current_frame],
            0, NULL);

    vkCmdDrawIndexed(command_buffer, CUBE_DEMO_INDICES_SIZE, 1, 0, 0, 0);
    vkCmdEndRendering(command_buffer);

    VkImageMemoryBarrier image_memory_barriers2[] = {
        {
            .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
            .dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
            .oldLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
            .newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
            .image = RANA.vk.swapchain_images[image_idx],
            .subresourceRange = {
                .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                .baseMipLevel = 0,
                .levelCount = 1,
                .baseArrayLayer = 0,
                .layerCount = 1,
            },
        }
    };

    vkCmdPipelineBarrier(command_buffer,
            VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
            VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
            0, 0, NULL, 0, NULL, 
            array_size(image_memory_barriers2),
            image_memory_barriers2);

    RANA_VK_VERIFY(vkEndCommandBuffer(command_buffer));

    return AMW_SUCCESS;
}
