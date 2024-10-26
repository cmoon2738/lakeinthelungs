#include "vk.h"
#include "../renderer.h"

static const char *device_type_string(VkPhysicalDeviceType type)
{
#define PDTYPESTR(r) case VK_ ##r: return "VK_"#r;
    switch (type) {
        PDTYPESTR(PHYSICAL_DEVICE_TYPE_OTHER)
        PDTYPESTR(PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU)
        PDTYPESTR(PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
        PDTYPESTR(PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU)
        PDTYPESTR(PHYSICAL_DEVICE_TYPE_CPU)
        default: return "<unknown>";
    }
#undef PDTYPESTR
    return "UNKNOWN DEVICE";
}

static const char *vendor_name_string(u32 vendor_id)
{
    switch (vendor_id) {
        case 0x1002: return "AMD";
        case 0x1010: return "ImgTec";
        case 0x106B: return "Apple";
        case 0x10DE: return "NVIDIA";
        case 0x13B5: return "ARM";
        case 0x5143: return "Qualcomm";
        case 0x8086: return "Intel";
        default: return "<unknown>";
    }
}

/* will return bits for supported extensions
 * if a necessary extension is not to be found, it will return 0.
 * This 0 indicated that the GPU is not suitable to use. */
static u64 supported_device_extensions(Arena *arena, const VkPhysicalDevice *pd)
{
    u64 ext_bits = 0;
    u32 count = 0;

    RANA_VK_VERIFY(vkEnumerateDeviceExtensionProperties(*pd, NULL, &count, NULL));

    if (count > 0) {
        VkExtensionProperties *extensions = (VkExtensionProperties *)arena_alloc(arena, sizeof(VkExtensionProperties) * count);
        RANA_VK_VERIFY(vkEnumerateDeviceExtensionProperties(*pd, NULL, &count, extensions));

        if (vulkan_has_extension(extensions, count, "VK_KHR_swapchain"))
            set_bit(ext_bits, RANA_VK_EXT_SWAPCHAIN_BIT);
        if (vulkan_has_extension(extensions, count, "VK_EXT_multi_draw"))
            set_bit(ext_bits, RANA_VK_EXT_MULTI_DRAW_INDIRECT_BIT);
        if (vulkan_has_extension(extensions, count, "VK_KHR_push_descriptor"))
            set_bit(ext_bits, RANA_VK_EXT_PUSH_DESCRIPTOR_BIT);
        if (vulkan_has_extension(extensions, count, "VK_EXT_extended_dynamic_state3"))
            set_bit(ext_bits, RANA_VK_EXT_EXTENDED_DYNAMIC_STATE3_BIT);
        if (vulkan_has_extension(extensions, count, "VK_KHR_ray_tracing_pipeline") &&
            vulkan_has_extension(extensions, count, "VK_KHR_acceleration_structure") &&
            vulkan_has_extension(extensions, count, "VK_KHR_deferred_host_operations")) {
            set_bit(ext_bits, RANA_VK_EXT_RAY_TRACING_PIPELINE_BIT);
            set_bit(ext_bits, RANA_VK_EXT_ACCELERATION_STRUCTURE_BIT);
            set_bit(ext_bits, RANA_VK_EXT_DEFERRED_HOST_OPERATIONS_BIT);
        }
        if (vulkan_has_extension(extensions, count, "VK_KHR_dynamic_rendering"))
            set_bit(ext_bits, RANA_VK_EXT_DYNAMIC_RENDERING_BIT);
        if (vulkan_has_extension(extensions, count, "VK_KHR_synchronization2"))
            set_bit(ext_bits, RANA_VK_EXT_SYNCHRONIZATION2_BIT);
        if (vulkan_has_extension(extensions, count, "VK_KHR_timeline_semaphore"))
            set_bit(ext_bits, RANA_VK_EXT_TIMELINE_SEMAPHORE_BIT);
        if (vulkan_has_extension(extensions, count, "VK_EXT_descriptor_indexing"))
            set_bit(ext_bits, RANA_VK_EXT_DESCRIPTOR_INDEXING_BIT);
        if (vulkan_has_extension(extensions, count, "VK_EXT_extended_dynamic_state"))
            set_bit(ext_bits, RANA_VK_EXT_EXTENDED_DYNAMIC_STATE_BIT);
        if (vulkan_has_extension(extensions, count, "VK_EXT_extended_dynamic_state2"))
            set_bit(ext_bits, RANA_VK_EXT_EXTENDED_DYNAMIC_STATE2_BIT);
        if (vulkan_has_extension(extensions, count, "VK_KHR_shader_non_semantic_info"))
            set_bit(ext_bits, RANA_VK_EXT_SHADER_NON_SEMANTIC_INFO_BIT);
    }
    
    /* check for required extensions */
    if(!read_flags(ext_bits, 
            (1 << RANA_VK_EXT_SWAPCHAIN_BIT)
          | (1 << RANA_VK_EXT_DYNAMIC_RENDERING_BIT)
    )) {
        return 0;
    }

    return ext_bits;
}

/*
u32 vulkan_find_device_memory_type(const VkPhysicalDevice pd, VkMemoryPropertyFlags flags, u32 req_bits)
{
    VkPhysicalDeviceMemoryProperties props;
    vkGetPhysicalDeviceMemoryProperties(pd, &props);

    for (u32 i = 0u; i < props.memoryTypeCount; i++) {
        if (req_bits & (1 << i)) {
            if ((props.memoryTypes[i].propertyFlags & flags) == flags) {
                return i;
            }
        }
    }
    return 0;
}
*/

static u32 count_bits(u64 n) {
    u32 count = 0;
    while (n) {
        n &= (n - 1);
        count++;
    }
    return count;
}

/* The function will try to find the best suitable GPU, 
 * unless the user specifies a GPU he wants to use.
 * It will enumerate the device extensions, check if the necessary ones
 * are supported, and check optional extensions. Then the 
 * physical device will be saved to the global state, and 
 * extension bits & count will be returned to initialize 
 * the logical device with desired GPU extensions. */
static i32 select_best_gpu(Arena *arena, 
                           VkPhysicalDevice *pds, 
                           i32 preferred_idx, 
                           i32 pd_count,
                           u64 *ext_bits_out)
{
    VkPhysicalDeviceProperties pd_props;
    u32 queue_family_count = 0;

    u64 ext_bits[pd_count];
    i32 score[pd_count];
    i32 best_idx = -1;

    assert_debug(pds);

    for (i32 i = 0; i < (i32)pd_count; i++) {
        score[i] = -1;
        ext_bits[i] = supported_device_extensions(arena, &pds[i]);

        /* no required device extensions? */
        if (ext_bits[i] == 0)
            continue;

        vkGetPhysicalDeviceProperties(pds[i], &pd_props);
        vkGetPhysicalDeviceQueueFamilyProperties(pds[i], &queue_family_count, NULL);

        /* no queue properties? */
        if (queue_family_count == 0)
            continue;

        //if (!vulkan_physical_device_presentation_support(pds[i], queue_family_count)) {
        //    log_debug("Vulkan device no presentation support, make sure Hadal is initialized.");
        //    continue;
        //}

        /* prefer discrete GPU, but if it's the only one available then don't be picky...
         * also if the user specifies a preferred device, select it */
        bool is_discrete = pd_props.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU;

        /* evaluate some arbitraty rating, to compare multiple physical devices */
        score[i] = 2048 * is_discrete + (read_bit(ext_bits[i], RANA_VK_EXT_RAY_TRACING_PIPELINE_BIT) ? 2024 : 0);

        if (preferred_idx == i || (is_discrete && preferred_idx < 0) || pd_count == 1) {
            best_idx = i;
            break;
        } else if (i == 0) {
            best_idx = i;
            continue;
        }

        /* compare with current best physical device to choose the better one */
        if (score[i] > score[best_idx]) {
            best_idx = i;
        }
    }

    if (best_idx >= 0) {
        if (ext_bits_out)
            *ext_bits_out = ext_bits[best_idx];
    } else {
        /* no suitable device found */
        if (ext_bits_out)
            *ext_bits_out = 0;
    }
    return best_idx;
}

u32 vulkan_find_memory_type(u32 type_filter, VkMemoryPropertyFlags properties)
{
    VkPhysicalDeviceMemoryProperties memory_properties;
    vkGetPhysicalDeviceMemoryProperties(RANA.vk.physical_device, &memory_properties);

    for (u32 i = 0; i < memory_properties.memoryTypeCount; i++) {
        if (!(type_filter & (1U << i))) {
            continue;
        }

        VkMemoryPropertyFlags type_properties = memory_properties.memoryTypes[i].propertyFlags;
        if ((type_properties & properties) == properties) {
            return i;
        }
    }

    return UINT32_MAX;
}


bool rana_vk_select_physical_device(Arena *arena, i32 preferred_device_idx, u32 *ext_count)
{
    u32 pd_count = 0;
    u64 ext_bits = 0;

    assert_debug(arena);

    RANA_VK_VERIFY(vkEnumeratePhysicalDevices(RANA.vk.instance, &pd_count, NULL));
    if (pd_count == 0) {
        log_error("No Vulkan-capable graphics devices found.");
        return false;
    } else if (pd_count == 1) {
        /* check if the only available GPU is already selected, if yes then return */
        if (RANA.vk.physical_device != VK_NULL_HANDLE) {
            return true;
        }
    }
    VkPhysicalDevice *pds = (VkPhysicalDevice *)arena_alloc(arena, pd_count * sizeof(VkPhysicalDevice));
    RANA_VK_VERIFY(vkEnumeratePhysicalDevices(RANA.vk.instance, &pd_count, pds));

    i32 idx = select_best_gpu(arena, pds,
                    preferred_device_idx < (i32)pd_count ? preferred_device_idx : -1, 
                    pd_count, &ext_bits);

    if (idx < 0 || ext_bits == 0) {
        log_error("Could not find a suitable GPU.");
        return false;
    } 

    /* mask all available device extensions, set new physical device */
    mask_flags(RANA.vk.ext_available, bitmask(RANA_VK_EXT_SWAPCHAIN_BIT));
    set_flags(RANA.vk.ext_available, ext_bits);
    mask_flags(RANA.vk.ext_enabled, bitmask(RANA_VK_EXT_SWAPCHAIN_BIT));

    /* resolve extensions and backwards compatibility */
    if (read_bit(RANA.vk.ext_available, RANA_VK_EXT_SWAPCHAIN_BIT))
        set_bit(RANA.vk.ext_enabled, RANA_VK_EXT_SWAPCHAIN_BIT);
    if (read_bit(RANA.vk.ext_available, RANA_VK_EXT_MULTI_DRAW_INDIRECT_BIT))
        set_bit(RANA.vk.ext_enabled, RANA_VK_EXT_MULTI_DRAW_INDIRECT_BIT);
    if (read_bit(RANA.vk.ext_available, RANA_VK_EXT_PUSH_DESCRIPTOR_BIT))
        set_bit(RANA.vk.ext_enabled, RANA_VK_EXT_PUSH_DESCRIPTOR_BIT);
    if (read_bit(RANA.vk.ext_available, RANA_VK_EXT_EXTENDED_DYNAMIC_STATE3_BIT))
        set_bit(RANA.vk.ext_enabled, RANA_VK_EXT_EXTENDED_DYNAMIC_STATE3_BIT);
    if (read_bit(RANA.vk.ext_available, RANA_VK_EXT_RAY_TRACING_PIPELINE_BIT)) {
        set_flags(RANA.vk.ext_enabled, 
                (1 << RANA_VK_EXT_RAY_TRACING_PIPELINE_BIT)
              | (1 << RANA_VK_EXT_ACCELERATION_STRUCTURE_BIT)
              | (1 << RANA_VK_EXT_DEFERRED_HOST_OPERATIONS_BIT));
    }
    /* https://registry.khronos.org/vulkan/specs/1.3-extensions/man/html/VK_VERSION_1_3.html */
    if (vulkan_version() < VK_API_VERSION_1_3) {
        if (read_bit(RANA.vk.ext_available, RANA_VK_EXT_DYNAMIC_RENDERING_BIT))
            set_bit(RANA.vk.ext_enabled, RANA_VK_EXT_DYNAMIC_RENDERING_BIT);
        if (read_bit(RANA.vk.ext_available, RANA_VK_EXT_SYNCHRONIZATION2_BIT))
            set_bit(RANA.vk.ext_enabled, RANA_VK_EXT_SYNCHRONIZATION2_BIT);
        if (read_bit(RANA.vk.ext_available, RANA_VK_EXT_EXTENDED_DYNAMIC_STATE_BIT))
            set_bit(RANA.vk.ext_enabled, RANA_VK_EXT_EXTENDED_DYNAMIC_STATE_BIT);
        if (read_bit(RANA.vk.ext_available, RANA_VK_EXT_EXTENDED_DYNAMIC_STATE2_BIT))
            set_bit(RANA.vk.ext_enabled, RANA_VK_EXT_EXTENDED_DYNAMIC_STATE2_BIT);
#ifndef AMW_NDEBUG
        if (read_bit(RANA.vk.ext_available, RANA_VK_EXT_SHADER_NON_SEMANTIC_INFO_BIT))
            set_bit(RANA.vk.ext_enabled, RANA_VK_EXT_SHADER_NON_SEMANTIC_INFO_BIT);
#endif /* AMW_NDEBUG */
    /* https://registry.khronos.org/vulkan/specs/1.3-extensions/man/html/VK_VERSION_1_2.html */
    } else if (vulkan_version() < VK_API_VERSION_1_2) {
        if (read_bit(RANA.vk.ext_available, RANA_VK_EXT_TIMELINE_SEMAPHORE_BIT))
            set_bit(RANA.vk.ext_enabled, RANA_VK_EXT_TIMELINE_SEMAPHORE_BIT);
        if (read_bit(RANA.vk.ext_available, RANA_VK_EXT_DESCRIPTOR_INDEXING_BIT))
            set_bit(RANA.vk.ext_enabled, RANA_VK_EXT_DESCRIPTOR_INDEXING_BIT);
    }
    RANA.vk.physical_device = pds[idx];

    if (ext_count)
        *ext_count = count_bits((RANA.vk.ext_enabled) & ~(bitmask(RANA_VK_EXT_SWAPCHAIN_BIT)));

    vkGetPhysicalDeviceFeatures(RANA.vk.physical_device, &RANA.vk.physical_device_features);
    vkGetPhysicalDeviceProperties(RANA.vk.physical_device, &RANA.vk.physical_device_properties);

    VkSampleCountFlags counts = 
        RANA.vk.physical_device_properties.limits.framebufferColorSampleCounts &
        RANA.vk.physical_device_properties.limits.framebufferDepthSampleCounts;

    if (counts & VK_SAMPLE_COUNT_64_BIT) {
        RANA.vk.msaa_samples = VK_SAMPLE_COUNT_64_BIT;
    } else if (counts & VK_SAMPLE_COUNT_32_BIT) {
        RANA.vk.msaa_samples = VK_SAMPLE_COUNT_32_BIT;
    } else if (counts & VK_SAMPLE_COUNT_16_BIT) {
        RANA.vk.msaa_samples = VK_SAMPLE_COUNT_16_BIT;
    } else if (counts & VK_SAMPLE_COUNT_8_BIT) {
        RANA.vk.msaa_samples = VK_SAMPLE_COUNT_8_BIT;
    } else if (counts & VK_SAMPLE_COUNT_4_BIT) {
        RANA.vk.msaa_samples = VK_SAMPLE_COUNT_4_BIT;
    } else if (counts & VK_SAMPLE_COUNT_2_BIT) {
        RANA.vk.msaa_samples = VK_SAMPLE_COUNT_2_BIT;
    } else {
        RANA.vk.msaa_samples = VK_SAMPLE_COUNT_1_BIT;
    }

    u32 vv_major = (RANA.vk.physical_device_properties.apiVersion >> 22U);
    u32 vv_minor = (RANA.vk.physical_device_properties.apiVersion >> 12U) & 0x3ffU;
    u32 vv_patch = (RANA.vk.physical_device_properties.apiVersion) & 0xfffU;
    u32 dv_major = (RANA.vk.physical_device_properties.driverVersion >> 22U);
    u32 dv_minor = (RANA.vk.physical_device_properties.driverVersion >> 12U) & 0x3ffU;
    u32 dv_patch = (RANA.vk.physical_device_properties.driverVersion) & 0xfffU;
    const char *device_type = device_type_string(RANA.vk.physical_device_properties.deviceType);

    log_info("Selected Vulkan device: %s", RANA.vk.physical_device_properties.deviceName);
    log_info("    Device type: %s, ID: %X", device_type, RANA.vk.physical_device_properties.deviceID);
    log_info("    Vendor: %s, ID: %X", vendor_name_string(RANA.vk.physical_device_properties.vendorID), RANA.vk.physical_device_properties.vendorID);
    log_info("    Supported API version: %u.%u.%u", vv_major, vv_minor, vv_patch);
    log_info("    Driver version: %u.%u.%u", dv_major, dv_minor, dv_patch);
    return true;
}
