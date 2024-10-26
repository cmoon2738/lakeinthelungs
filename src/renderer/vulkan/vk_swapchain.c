#include "vk.h"
#include "../renderer.h"

static VkSurfaceFormatKHR choose_swap_surface_format(VkSurfaceFormatKHR *available_formats, u32 count)
{
    assert_debug(available_formats);
    for (u32 i = 0; i < count; i++) {
        VkSurfaceFormatKHR format = available_formats[i];
        if (format.format == VK_FORMAT_B8G8R8A8_SRGB && format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
            return format;
        }
    }
    return available_formats[0];
}

static VkPresentModeKHR choose_swap_present_mode(VkPresentModeKHR *available_present_modes, u32 count)
{
    assert_debug(available_present_modes);
    for (u32 i = 0; i < count; i++) {
        VkPresentModeKHR present_mode = available_present_modes[i];
        if (present_mode == VK_PRESENT_MODE_MAILBOX_KHR) {
            return present_mode;
        }
    }
    return VK_PRESENT_MODE_FIFO_KHR;
}

static VkExtent2D choose_swap_extent(VkSurfaceCapabilitiesKHR *capabilities)
{
    assert_debug(capabilities);
    if (capabilities->currentExtent.width != UINT32_MAX)
        return capabilities->currentExtent;

    u32 width, height;
    hadal_get_framebuffer_size(RANA.window, &width, &height);

    VkExtent2D actual_extent = {
        .width = width,
        .height = height,
    };
    actual_extent.width = max(actual_extent.width, capabilities->minImageExtent.width);
    actual_extent.width = min(actual_extent.width, capabilities->maxImageExtent.width);
    actual_extent.height = max(actual_extent.height, capabilities->minImageExtent.height);
    actual_extent.height = min(actual_extent.height, capabilities->maxImageExtent.height);
    return actual_extent;
}

i32 rana_vk_create_swapchain(Arena *temp_arena, Arena *swapchain_arena)
{
    assert_debug(temp_arena);

    VkSurfaceCapabilitiesKHR capabilities;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(RANA.vk.physical_device, RANA.vk.surface, &capabilities);

    u32 format_count = 0;
    VkSurfaceFormatKHR *formats = NULL;
    vkGetPhysicalDeviceSurfaceFormatsKHR(RANA.vk.physical_device, RANA.vk.surface, &format_count, NULL);

    if (format_count != 0) {
        formats = (VkSurfaceFormatKHR *)arena_alloc(temp_arena, sizeof(VkSurfaceFormatKHR) * format_count);
        if (formats == NULL) {
            return AMW_ERROR_STUB;
        }
    } 
    vkGetPhysicalDeviceSurfaceFormatsKHR(RANA.vk.physical_device, RANA.vk.surface, &format_count, formats);

    u32 present_mode_count = 0;
    VkPresentModeKHR *present_modes = NULL;
    vkGetPhysicalDeviceSurfacePresentModesKHR(RANA.vk.physical_device, RANA.vk.surface, &present_mode_count, NULL);

    if (present_mode_count != 0) {
        present_modes = (VkPresentModeKHR *)arena_alloc(temp_arena, sizeof(VkPresentModeKHR) * present_mode_count);
        if (present_modes == NULL) {
            return AMW_ERROR_STUB;
        }
    } 
    vkGetPhysicalDeviceSurfacePresentModesKHR(RANA.vk.physical_device, RANA.vk.surface, &present_mode_count, present_modes);

    VkSurfaceFormatKHR surface_format = choose_swap_surface_format(formats, format_count);
    VkPresentModeKHR present_mode = choose_swap_present_mode(present_modes, present_mode_count);
    VkExtent2D extent = choose_swap_extent(&capabilities);

    u32 image_count = capabilities.minImageCount + 1;
    if (capabilities.maxImageCount > 0 && image_count > capabilities.maxImageCount)
        image_count = capabilities.maxImageCount;

    VkSwapchainCreateInfoKHR swapchain_create_info = {
        .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
        .surface = RANA.vk.surface,
        .minImageCount = image_count,
        .imageFormat = surface_format.format,
        .imageColorSpace = surface_format.colorSpace,
        .imageExtent = extent,
        .imageArrayLayers = 1,
        .imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT,
        .preTransform = capabilities.currentTransform,
        .compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
        .presentMode = present_mode,
        .clipped = true,
        .oldSwapchain = VK_NULL_HANDLE,
    };

    if (RANA.vk.queue_family_count > 1) {
        swapchain_create_info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        swapchain_create_info.queueFamilyIndexCount = RANA.vk.queue_family_count;
        swapchain_create_info.pQueueFamilyIndices = RANA.vk.queue_family_indices;
    } else {
        swapchain_create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        swapchain_create_info.queueFamilyIndexCount = 0;
        swapchain_create_info.pQueueFamilyIndices = NULL;
    }

    RANA_VK_VERIFY(vkCreateSwapchainKHR(RANA.vk.device, &swapchain_create_info, NULL, &RANA.vk.swapchain));

    vkGetSwapchainImagesKHR(RANA.vk.device, RANA.vk.swapchain, &image_count, NULL);

    RANA.vk.swapchain_images = (VkImage *)arena_alloc(swapchain_arena, sizeof(VkImage) * image_count);
    if (RANA.vk.swapchain_images == NULL) {
        log_error("Could not allocate swapchain images.");
        return AMW_ERROR_STUB;
    }
    vkGetSwapchainImagesKHR(RANA.vk.device, RANA.vk.swapchain, &image_count, RANA.vk.swapchain_images);

    RANA.vk.swapchain_image_count = image_count;
    RANA.vk.swapchain_image_format = surface_format.format;
    RANA.vk.swapchain_extent = extent;

    return AMW_SUCCESS;
}

void rana_vk_cleanup_swapchain(Arena *swapchain_arena)
{
    log_verbose("Destroying swapchain...");

    if (RANA.vk.msaa_samples != VK_SAMPLE_COUNT_1_BIT) {
        vkDestroyImageView(RANA.vk.device, RANA.vk.color_image_view, NULL);
        vkDestroyImage(RANA.vk.device, RANA.vk.color_image, NULL);
        vkFreeMemory(RANA.vk.device, RANA.vk.color_image_memory, NULL);
    }

    if (RANA.vk.swapchain_image_views != NULL) {
        for (u32 i = 0; i < RANA.vk.swapchain_image_count; i++) {
            vkDestroyImageView(RANA.vk.device, RANA.vk.swapchain_image_views[i], NULL);
        }
    }

    if (RANA.vk.swapchain)
        vkDestroySwapchainKHR(RANA.vk.device, RANA.vk.swapchain, NULL);
    arena_reset(swapchain_arena);
}

i32 rana_vk_create_image_views(Arena *swapchain_arena)
{
    RANA.vk.swapchain_image_views = (VkImageView *)arena_alloc(swapchain_arena, sizeof(VkImageView) * RANA.vk.swapchain_image_count);
    if (RANA.vk.swapchain_image_views == NULL) {
        log_error("Could not allocate swapchain image views.");
        return AMW_ERROR_STUB;
    }

    for (u32 i = 0; i < RANA.vk.swapchain_image_count; i++) {
        VkImageViewCreateInfo create_info = {
            .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
            .image = RANA.vk.swapchain_images[i],
            .viewType = VK_IMAGE_VIEW_TYPE_2D,
            .format = RANA.vk.swapchain_image_format,
            .components = {
                .r = VK_COMPONENT_SWIZZLE_IDENTITY, 
                .g = VK_COMPONENT_SWIZZLE_IDENTITY, 
                .b = VK_COMPONENT_SWIZZLE_IDENTITY, 
                .a = VK_COMPONENT_SWIZZLE_IDENTITY, 
            },
            .subresourceRange = {
                .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                .baseMipLevel = 0,
                .levelCount = 1,
                .baseArrayLayer = 0,
                .layerCount = 1,
            },
        };
        RANA_VK_VERIFY(vkCreateImageView(RANA.vk.device, &create_info, NULL, &RANA.vk.swapchain_image_views[i]))
    }

    return AMW_SUCCESS;
}

i32 rana_vk_create_color_resources(void)
{
    VkImageCreateInfo image_info = {
        .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
        .imageType = VK_IMAGE_TYPE_2D,
        .extent.width = RANA.vk.swapchain_extent.width,
        .extent.height = RANA.vk.swapchain_extent.height,
        .extent.depth = 1,
        .mipLevels = 1,
        .arrayLayers = 1,
        .format = RANA.vk.swapchain_image_format,
        .tiling = VK_IMAGE_TILING_OPTIMAL,
        .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
        .usage = VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT |
                 VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
        .samples = RANA.vk.msaa_samples,
        .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
    };

    RANA_VK_VERIFY(vkCreateImage(RANA.vk.device, &image_info, NULL, &RANA.vk.color_image));

    VkMemoryRequirements memory_requirements;
    vkGetImageMemoryRequirements(RANA.vk.device, RANA.vk.color_image, &memory_requirements);

    u32 memory_type_idx = vulkan_find_memory_type(
        memory_requirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    if (memory_type_idx == UINT32_MAX)
        return AMW_ERROR_STUB;

    VkMemoryAllocateInfo alloc_info = {
        .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
        .allocationSize = memory_requirements.size,
        .memoryTypeIndex = memory_type_idx,
    };

    RANA_VK_VERIFY(vkAllocateMemory(RANA.vk.device, &alloc_info, NULL, &RANA.vk.color_image_memory));

    vkBindImageMemory(RANA.vk.device, RANA.vk.color_image, RANA.vk.color_image_memory, 0);
    
    VkImageViewCreateInfo image_view_info = {
        .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
        .image = RANA.vk.color_image,
        .viewType = VK_IMAGE_VIEW_TYPE_2D,
        .format = RANA.vk.swapchain_image_format,
        .subresourceRange = {
            .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
            .baseMipLevel = 0,
            .levelCount = 1,
            .baseArrayLayer = 0,
            .layerCount = 1,
        },
    };

    RANA_VK_VERIFY(vkCreateImageView(RANA.vk.device, &image_view_info, NULL, &RANA.vk.color_image_view));

    return AMW_SUCCESS;
}

i32 rana_vk_recreate_swapchain(Arena *swapchain_arena, Arena *temp_arena)
{
    u32 width = 0;
    u32 height = 0;
    i32 result = AMW_SUCCESS;
    hadal_get_framebuffer_size(RANA.window, &width, &height);
    while (width == 0 || height == 0) {
        if (read_flags(hadal_get_flags(RANA.window), WINDOW_FLAG_SHOULD_CLOSE))
            return result;
        // TODO wait for events..
    }
    vkDeviceWaitIdle(RANA.vk.device);

    rana_vk_cleanup_swapchain(swapchain_arena);

    result = rana_vk_create_swapchain(temp_arena, swapchain_arena);
    if (result != AMW_SUCCESS)
        return result;
    result = rana_vk_create_image_views(swapchain_arena);
    if (result != AMW_SUCCESS)
        return result;

    if (RANA.vk.msaa_samples != VK_SAMPLE_COUNT_1_BIT) {
        result = rana_vk_create_color_resources();
    }
    return result;
}
