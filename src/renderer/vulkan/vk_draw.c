#include "renderer/rana.h"
#include "vk.h"
#include "../renderer.h"

#include "common.h"
#include <vulkan/vulkan_core.h>

i32 rana_vk_draw_frame(Arena *swapchain_arena, Arena *temp_arena)
{
    VkResult result = vkWaitForFences(RANA.vk.device, 1, &RANA.vk.fences[RANA.current_frame], VK_TRUE, 1024 * 1024);
    if (result != VK_SUCCESS) {
        if (result == VK_TIMEOUT)
            return AMW_SUCCESS;
        return AMW_ERROR_STUB;
    }

    u32 image_idx;
    result = vkAcquireNextImageKHR(
            RANA.vk.device,
            RANA.vk.swapchain,
            1024 * 1024,
            RANA.vk.image_available_semaphores[RANA.current_frame],
            VK_NULL_HANDLE,
            &image_idx);
    if (result == VK_ERROR_OUT_OF_DATE_KHR) {
        return _rana_recreate_swapchain(swapchain_arena, temp_arena);
    } else if (result == VK_TIMEOUT) {
        return AMW_SUCCESS;
    } else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
        return AMW_ERROR_STUB;
    }

    vulkan_update_uniform_buffer(RANA.current_frame);
    vkResetFences(RANA.vk.device, 1, &RANA.vk.fences[RANA.current_frame]);
    vkResetCommandBuffer(RANA.vk.command_buffers[RANA.current_frame], 0);

    i32 err = vulkan_record_command_buffer(RANA.vk.command_buffers[RANA.current_frame], image_idx);
    if (err != AMW_SUCCESS)
        return err;

    VkSemaphore wait_semaphores[] = {
        RANA.vk.image_available_semaphores[RANA.current_frame],
    };

    VkPipelineStageFlags wait_stages[] = {
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
    };
    assert_static(array_size(wait_semaphores) == array_size(wait_stages), 
            "Array size of wait semaphores doesn't match pipeline wait stages");
    
    VkSemaphore signal_semaphores[] = {
        RANA.vk.render_finished_semaphores[RANA.current_frame],
    };

    VkSubmitInfo submit_info = {
        .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
        .waitSemaphoreCount = array_size(wait_semaphores),
        .pWaitSemaphores = wait_semaphores,
        .pWaitDstStageMask = wait_stages,
        .commandBufferCount = 1,
        .pCommandBuffers = &RANA.vk.command_buffers[RANA.current_frame],
        .signalSemaphoreCount = array_size(signal_semaphores),
        .pSignalSemaphores = signal_semaphores,
    };

    RANA_VK_VERIFY(vkQueueSubmit(RANA.vk.graphics_queue, 1, &submit_info, RANA.vk.fences[RANA.current_frame]));
    
    VkSwapchainKHR swapchain = RANA.vk.swapchain;

    VkPresentInfoKHR present_info = {
        .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
        .waitSemaphoreCount = array_size(signal_semaphores),
        .pWaitSemaphores = signal_semaphores,
        .swapchainCount = 1,
        .pSwapchains = &swapchain,
        .pImageIndices = &image_idx,
        .pResults = NULL,
    };

    RANA.current_frame = (RANA.current_frame + 1) % RANA_VK_MAX_FRAMES;

    result = vkQueuePresentKHR(RANA.vk.present_queue, &present_info);
    if (result == VK_ERROR_OUT_OF_DATE_KHR || 
        result == VK_SUBOPTIMAL_KHR || 
        read_flags(RANA.flags, RANA_FLAG_FRAMEBUFFER_RESIZED))
    {
        return _rana_recreate_swapchain(swapchain_arena, temp_arena);
    } else if (result != VK_SUCCESS) {
        return AMW_ERROR_STUB;
    }
    return AMW_SUCCESS;
}
