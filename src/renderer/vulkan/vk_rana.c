#include "common.h"
#include "core/memory.h"
#include "vk.h"
#include "../renderer.h"
#include "../../amw.h" /* AMW.app-> version, name */
#include <vulkan/vulkan_core.h>

static const char *validation_layers[] = {
    "VK_LAYER_KHRONOS_validation"
};

static void enumerate_instance_extensions(Arena *a, char **extensions, u32 *extension_count)
{
    u32 count = 0;
    u32 wanted_count = 0;
    bool native_surface = false;
    char *native_surface_extension = vulkan_get_surface_extension();

#ifndef AMW_NDEBUG
    bool wanted_debug = read_bit(RANA.vk.ext_available, RANA_VK_EXT_VALIDATION_LAYERS_BIT);
#else
    bool wanted_debug = false;
#endif /* AMW_NDEBUG */

    RANA_VK_VERIFY(vkEnumerateInstanceExtensionProperties(NULL, &count, NULL));

    if (count > 0 && native_surface_extension != NULL) {
        VkExtensionProperties *available_extensions = (VkExtensionProperties *)arena_alloc(a, sizeof(VkExtensionProperties) * count);
        RANA_VK_VERIFY(vkEnumerateInstanceExtensionProperties(NULL, &count, available_extensions));

        if (vulkan_has_extension(available_extensions, count, "VK_KHR_surface")) {
            set_bit(RANA.vk.ext_available, RANA_VK_EXT_SURFACE_BIT);
            wanted_count++;
            if ((native_surface = vulkan_has_extension(available_extensions, count, native_surface_extension)))
                wanted_count++;
        }
        if (vulkan_has_extension(available_extensions, count, "VK_EXT_debug_utils")) {
            set_bit(RANA.vk.ext_available, RANA_VK_EXT_DEBUG_UTILS_BIT);
            wanted_count += wanted_debug;
        }
        if (vulkan_has_extension(available_extensions, count, "VK_KHR_get_physical_device_properties2")) {
            set_bit(RANA.vk.ext_available, RANA_VK_EXT_GET_PHYSICAL_DEVICE_PROPERTIES2_BIT);
            wanted_count++;
        }
    }
    count = 0;

    /* don't continue if necessary extensions have not been found */
    if (!native_surface || !read_flags(RANA.vk.ext_available, 
              (1 << RANA_VK_EXT_SURFACE_BIT) 
/*          | (1 << RANA_VK_EXT_GET_PHYSICAL_DEVICE_PROPERTIES2_BIT) */
        )) {
        log_error("Not all required instance extensions were found. ext_available: %lu", RANA.vk.ext_available);
        if (extension_count) 
            *extension_count = 0;    
        if (extensions) 
            extensions = NULL;
        return;
    }

    if (extension_count)
        *extension_count = wanted_count;

    if (extensions) {
        /* surface */
        if (read_bit(RANA.vk.ext_available, RANA_VK_EXT_SURFACE_BIT)) {
            set_bit(RANA.vk.ext_enabled, RANA_VK_EXT_SURFACE_BIT);
            extensions[count++] = "VK_KHR_surface";
            extensions[count++] = native_surface_extension;
        }
        /* debug_utils */
        if (wanted_debug && read_bit(RANA.vk.ext_available, RANA_VK_EXT_DEBUG_UTILS_BIT)) {
            set_bit(RANA.vk.ext_enabled, RANA_VK_EXT_DEBUG_UTILS_BIT);
            extensions[count++] = "VK_EXT_debug_utils";
        }
        /* get_physical_device_properties2 */
        if (read_bit(RANA.vk.ext_available, RANA_VK_EXT_GET_PHYSICAL_DEVICE_PROPERTIES2_BIT)) {
            set_bit(RANA.vk.ext_enabled, RANA_VK_EXT_GET_PHYSICAL_DEVICE_PROPERTIES2_BIT);
            extensions[count++] = "VK_KHR_get_physical_device_properties2";
        }
        assert_debug(wanted_count == count);
    }
}

i32 rana_vk_init(void)
{
    u32         instance_version = vulkan_version();
    u32         layer_count = 0;
    u32         app_version = 0;
    u32         i, extension_count;
    const char *app_name = "unknown";

    Arena *arena = &RANA.temporary_arena;

    log_verbose("Initializing Vulkan...");
    if (AMW.app) {
        app_version = AMW.app->version;
        app_name = AMW.app->name;
    }

    /* validation layers */
    vkEnumerateInstanceLayerProperties(&layer_count, NULL);
    if (layer_count > 0) {
        set_bit(RANA.vk.ext_available, RANA_VK_EXT_VALIDATION_LAYERS_BIT);
    }
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

    VkApplicationInfo app_info = {
        .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
        .pNext = NULL,
        .pApplicationName = app_name,
        .applicationVersion = app_version,
        .pEngineName = "A Moonlit Walk Engine",
        .engineVersion = AMW_VERSION,
        .apiVersion = instance_version,
    };

    extension_count = 0;
    enumerate_instance_extensions(arena, NULL, &extension_count);
    if (extension_count == 0) {
        log_error("Needed Vulkan instance extensions are not available, can't continue.");
        return AMW_ERROR_STUB;
    }
    arena_reset(arena);

    char **instance_extensions = (char **)arena_alloc(arena, sizeof(char *) * extension_count);
    enumerate_instance_extensions(arena, instance_extensions, NULL);

    log_debug("Enabled Vulkan instance extensions:");
    for (i = 0; i < extension_count; i++)
        log_debug(" %3d %s", i, instance_extensions[i]);

    VkInstanceCreateInfo instance_create_info = {
        .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
        .pNext = NULL,
        .flags = 0,
        .pApplicationInfo = &app_info,
        .enabledLayerCount = 0,
        .ppEnabledLayerNames = NULL,
        .enabledExtensionCount = extension_count,
        .ppEnabledExtensionNames = (const char * const *)instance_extensions,
    };

    /* if debug utils extension is enabled, enable validation layers */
    if (read_bit(RANA.vk.ext_enabled, RANA_VK_EXT_DEBUG_UTILS_BIT)) {
        set_bit(RANA.vk.ext_enabled, RANA_VK_EXT_VALIDATION_LAYERS_BIT);
        instance_create_info.pNext = &validation_features;
        instance_create_info.enabledLayerCount = 1;
        instance_create_info.ppEnabledLayerNames = (const char * const *)validation_layers;
    }

    RANA_VK_VERIFY(vkCreateInstance(&instance_create_info, NULL, &RANA.vk.instance));
    vulkan_load_instance_pointers(RANA.vk.instance);

    log_verbose("Created a Vulkan instance!");
    arena_reset(arena);

    if (read_bit(RANA.vk.ext_enabled, RANA_VK_EXT_VALIDATION_LAYERS_BIT))
        vulkan_create_validation_layers(RANA.vk.instance);

    RANA_VK_VERIFY(vulkan_create_surface(RANA.vk.instance, RANA.window, NULL, &RANA.vk.surface));

    /* this will only give the count of extensions saved in RANA.vk.ext_available */
    extension_count = 0;
    if (!rana_vk_select_physical_device(arena, 0, &extension_count))
        return AMW_ERROR_STUB;
    arena_reset(arena);

    f32 queue_priority = 1.f;
    u32 queue_family_count = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(RANA.vk.physical_device, &queue_family_count, NULL);
    
    VkQueueFamilyProperties *queue_families = (VkQueueFamilyProperties *)arena_alloc(arena, sizeof(VkQueueFamilyProperties) * queue_family_count);
    vkGetPhysicalDeviceQueueFamilyProperties(RANA.vk.physical_device, &queue_family_count, queue_families);

    i32 queue_graphics_family = -1;
    for (i32 i = 0; i < (i32)queue_family_count ; i++) {
        if (queue_families[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            queue_graphics_family = i;
        }
    }

    if (queue_graphics_family < 0)
        return AMW_ERROR_STUB;
    RANA.vk.queue_family_index = queue_graphics_family;

    if (!vulkan_physical_device_presentation_support(RANA.vk.physical_device, queue_graphics_family)) {
        log_error("kurwa");
        return AMW_ERROR_STUB;
    }

    /* for now, just one queue */
    VkDeviceQueueCreateInfo queue_create_info[1];

    queue_create_info[0].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queue_create_info[0].pNext = NULL;
    queue_create_info[0].flags = 0;
    queue_create_info[0].queueFamilyIndex = RANA.vk.queue_family_index;
    queue_create_info[0].queueCount = 1;
    queue_create_info[0].pQueuePriorities = &queue_priority;

    i = 0;
    char **device_extensions = (char **)arena_alloc(arena, sizeof(char *) * extension_count);
    if (read_bit(RANA.vk.ext_enabled, RANA_VK_EXT_SWAPCHAIN_BIT))
        device_extensions[i++] = "VK_KHR_swapchain";
    if (read_bit(RANA.vk.ext_enabled, RANA_VK_EXT_MULTI_DRAW_INDIRECT_BIT))
        device_extensions[i++] = "VK_EXT_multi_draw";
    if (read_bit(RANA.vk.ext_enabled, RANA_VK_EXT_PUSH_DESCRIPTOR_BIT))
        device_extensions[i++] = "VK_KHR_push_descriptor";
    if (read_bit(RANA.vk.ext_enabled, RANA_VK_EXT_EXTENDED_DYNAMIC_STATE3_BIT))
        device_extensions[i++] = "VK_EXT_extended_dynamic_state3";
    if (read_bit(RANA.vk.ext_enabled, RANA_VK_EXT_DEFERRED_HOST_OPERATIONS_BIT))
        device_extensions[i++] = "VK_KHR_deferred_host_operations";
    if (read_bit(RANA.vk.ext_enabled, RANA_VK_EXT_ACCELERATION_STRUCTURE_BIT))
        device_extensions[i++] = "VK_KHR_acceleration_structure";
    if (read_bit(RANA.vk.ext_enabled, RANA_VK_EXT_RAY_TRACING_PIPELINE_BIT))
        device_extensions[i++] = "VK_KHR_ray_tracing_pipeline";
    if (read_bit(RANA.vk.ext_enabled, RANA_VK_EXT_DYNAMIC_RENDERING_BIT))
        device_extensions[i++] = "VK_KHR_dynamic_rendering";
    if (read_bit(RANA.vk.ext_enabled, RANA_VK_EXT_SYNCHRONIZATION2_BIT))
        device_extensions[i++] = "VK_KHR_synchronization2";
    if (read_bit(RANA.vk.ext_enabled, RANA_VK_EXT_TIMELINE_SEMAPHORE_BIT))
        device_extensions[i++] = "VK_KHR_timeline_semaphore";
    if (read_bit(RANA.vk.ext_enabled, RANA_VK_EXT_DESCRIPTOR_INDEXING_BIT))
        device_extensions[i++] = "VK_EXT_descriptor_indexing";
    if (read_bit(RANA.vk.ext_enabled, RANA_VK_EXT_EXTENDED_DYNAMIC_STATE_BIT))
        device_extensions[i++] = "VK_EXT_extended_dynamic_state";
    if (read_bit(RANA.vk.ext_enabled, RANA_VK_EXT_EXTENDED_DYNAMIC_STATE2_BIT))
        device_extensions[i++] = "VK_EXT_extended_dynamic_state2";
    if (read_bit(RANA.vk.ext_enabled, RANA_VK_EXT_SHADER_NON_SEMANTIC_INFO_BIT))
        device_extensions[i++] = "VK_KHR_shader_non_semantic_info";

    log_debug("Enabled Vulkan device extensions:");
    for (i = 0; i < extension_count; i++)
        log_debug(" %3d %s", i, device_extensions[i]);

    /* select the wanted physical device features to enable
     * TODO this should be done in a better way */
    VkPhysicalDeviceFeatures device_features = {0};
    device_features.samplerAnisotropy = RANA.vk.physical_device_features.samplerAnisotropy;
    device_features.fillModeNonSolid = RANA.vk.physical_device_features.fillModeNonSolid;
    device_features.sampleRateShading = RANA.vk.physical_device_features.sampleRateShading;
    device_features.geometryShader = RANA.vk.physical_device_features.geometryShader;

    VkDeviceCreateInfo device_create_info = {
        .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
        .pNext = NULL,
        .flags = 0,
        .queueCreateInfoCount = 1,
        .pQueueCreateInfos = queue_create_info,
        .enabledExtensionCount = extension_count,
        .ppEnabledExtensionNames = (const char * const *)device_extensions,
        .pEnabledFeatures = &device_features,
    };

    RANA_VK_VERIFY(vkCreateDevice(RANA.vk.physical_device, &device_create_info, NULL, &RANA.vk.device));
    vulkan_load_device_pointers(RANA.vk.device);

    log_verbose("Created a Vulkan device!");
    arena_reset(arena);

    /* TODO ... */

    arena_reset(arena);
    return AMW_SUCCESS;
}

/* any RanaContext should be terminated by this point */
void rana_vk_terminate(void)
{
    log_verbose("Terminating Vulkan...");

    if (RANA.vk.device != VK_NULL_HANDLE)
        vkDestroyDevice(RANA.vk.device, NULL);

    if (RANA.vk.surface != VK_NULL_HANDLE)
        vkDestroySurfaceKHR(RANA.vk.instance, RANA.vk.surface, NULL);

    if (read_bit(RANA.vk.ext_enabled, RANA_VK_EXT_VALIDATION_LAYERS_BIT))
        vulkan_destroy_validation_layers(RANA.vk.instance);

    if (RANA.vk.instance != VK_NULL_HANDLE)
        vkDestroyInstance(RANA.vk.instance, NULL);
    vulkan_close_driver();
}
