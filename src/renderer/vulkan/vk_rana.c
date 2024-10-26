#include "common.h"
#include "vk.h"
#include "../renderer.h"
#include "../../amw.h" /* AMW.app-> version, name */

static const char *validation_layers[] = {
    "VK_LAYER_KHRONOS_validation"
};

static void enumerate_instance_extensions(Arena *temp_arena, char **extensions, u32 *extension_count)
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
        VkExtensionProperties *available_extensions = (VkExtensionProperties *)arena_alloc(temp_arena, sizeof(VkExtensionProperties) * count);
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
    u32 instance_version = vulkan_version();
    u32 i, extension_count;
    u32 layer_count = 0;
    u32 app_version = 0;
    const char *app_name = "unknown";

    Arena *temp_arena = &RANA.temporary_arena;
    Arena *swapchain_arena = &RANA.swapchain_arena;

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
    enumerate_instance_extensions(temp_arena, NULL, &extension_count);
    if (extension_count == 0) {
        log_error("Needed Vulkan instance extensions are not available, can't continue.");
        return AMW_ERROR_STUB;
    }
    arena_reset(temp_arena);

    char **instance_extensions = (char **)arena_alloc(temp_arena, sizeof(char *) * extension_count);
    enumerate_instance_extensions(temp_arena, instance_extensions, NULL);

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
    arena_reset(temp_arena);

    if (read_bit(RANA.vk.ext_enabled, RANA_VK_EXT_VALIDATION_LAYERS_BIT))
        vulkan_create_validation_layers(RANA.vk.instance);

    RANA_VK_VERIFY(vulkan_create_surface(RANA.vk.instance, RANA.window, NULL, &RANA.vk.surface));

    /* this will only give the count of extensions saved in RANA.vk.ext_available */
    extension_count = 0;
    if (!rana_vk_select_physical_device(temp_arena, 0, &extension_count))
        return AMW_ERROR_STUB;
    arena_reset(temp_arena);

    char **device_extensions = (char **)arena_alloc(temp_arena, sizeof(char *) * extension_count);
    {   i = 0;
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
    }
    log_debug("Enabled Vulkan device extensions:");
    for (i = 0; i < extension_count; i++)
        log_debug(" %3d %s", i, device_extensions[i]);

    bool graphics_family_valid = false;
    bool transfer_family_valid = false;
    bool present_family_valid = false;

    vkGetPhysicalDeviceQueueFamilyProperties(RANA.vk.physical_device, &RANA.vk.queue_family_count, NULL);
    
    VkQueueFamilyProperties *queue_families = (VkQueueFamilyProperties *)arena_alloc(temp_arena, sizeof(VkQueueFamilyProperties) * RANA.vk.queue_family_count);
    vkGetPhysicalDeviceQueueFamilyProperties(RANA.vk.physical_device, &RANA.vk.queue_family_count, queue_families);
    if (!queue_families) {
        log_error("Can't find any queue families for this physical device.");
        return AMW_ERROR_STUB;
    }

    for (i = 0; i < RANA.vk.queue_family_count ; i++) {
        if (queue_families[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            RANA.vk.queue_family_indices[0] = i;
            graphics_family_valid = true;
        }

        if (queue_families[i].queueFlags & VK_QUEUE_TRANSFER_BIT) {
            RANA.vk.queue_family_indices[1] = i;
            transfer_family_valid = true;
        }

        VkBool32 present_support = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(RANA.vk.physical_device, i, RANA.vk.surface, &present_support);
        if (present_support) {
            RANA.vk.queue_family_indices[2] = i;
            present_family_valid = true;
        }
        if (graphics_family_valid && transfer_family_valid && present_family_valid)
            break;
    }

    if (!graphics_family_valid || !present_family_valid) {
        log_error("No necessary queue families found: (graphics:%b)(transfer:%b)(present:%b).",
                graphics_family_valid, transfer_family_valid, present_family_valid);
        return AMW_ERROR_STUB;
    }
    RANA.vk.queue_family_count = 3;
    if (RANA.vk.queue_family_indices[0] == RANA.vk.queue_family_indices[1] && 
        RANA.vk.queue_family_indices[0] == RANA.vk.queue_family_indices[2]) 
    {
        RANA.vk.queue_family_count = 1;
    } else if (RANA.vk.queue_family_indices[0] == RANA.vk.queue_family_indices[1] || 
               RANA.vk.queue_family_indices[1] == RANA.vk.queue_family_indices[2] ||
               RANA.vk.queue_family_indices[2] == RANA.vk.queue_family_indices[0])
    {
        RANA.vk.queue_family_count = 2;
    }

    f32 queue_priority = 1.f;
    VkDeviceQueueCreateInfo queue_create_infos[3];

    for (i = 0; i < RANA.vk.queue_family_count; i++) {
        queue_create_infos[i] = (VkDeviceQueueCreateInfo){
            .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
                .queueFamilyIndex = RANA.vk.queue_family_indices[i],
                .queueCount = 1,
                .pQueuePriorities = &queue_priority,
        };
    }

    VkPhysicalDeviceFeatures device_features = {0};
    device_features.samplerAnisotropy = RANA.vk.physical_device_features.samplerAnisotropy;
    device_features.fillModeNonSolid = RANA.vk.physical_device_features.fillModeNonSolid;
    device_features.sampleRateShading = RANA.vk.physical_device_features.sampleRateShading;
    device_features.geometryShader = RANA.vk.physical_device_features.geometryShader;

    VkPhysicalDeviceScalarBlockLayoutFeatures scalar_block_layout_features = {
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SCALAR_BLOCK_LAYOUT_FEATURES,
        .scalarBlockLayout = VK_TRUE,
    };

    VkPhysicalDeviceDynamicRenderingFeatures dynamic_rendering_features = {
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_FEATURES,
        .pNext = &scalar_block_layout_features,
        .dynamicRendering = VK_TRUE,
    };

    VkDeviceCreateInfo device_create_info = {
        .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
        .pNext = &dynamic_rendering_features,
        .flags = 0,
        .queueCreateInfoCount = RANA.vk.queue_family_count,
        .pQueueCreateInfos = queue_create_infos,
        .enabledExtensionCount = extension_count,
        .ppEnabledExtensionNames = (const char * const *)device_extensions,
        .pEnabledFeatures = &device_features,
    };

    RANA_VK_VERIFY(vkCreateDevice(RANA.vk.physical_device, &device_create_info, NULL, &RANA.vk.device));
    vulkan_load_device_pointers(RANA.vk.device);

    vkGetDeviceQueue(RANA.vk.device, RANA.vk.queue_family_indices[0], 0, &RANA.vk.graphics_queue);
    vkGetDeviceQueue(RANA.vk.device, RANA.vk.queue_family_indices[1], 0, &RANA.vk.transfer_queue);
    vkGetDeviceQueue(RANA.vk.device, RANA.vk.queue_family_indices[2], 0, &RANA.vk.present_queue);

    log_debug("Created a Vulkan device and %d VkQueue(s): graphics:idx%d, transfer:idx%d, present:idx%d",
            RANA.vk.queue_family_count, RANA.vk.queue_family_indices[0], 
            RANA.vk.queue_family_indices[1], RANA.vk.queue_family_indices[2]);
    arena_reset(temp_arena);

    log_verbose("Creating swapchain...");
    i = rana_vk_create_swapchain(temp_arena, swapchain_arena);
    if (i != AMW_SUCCESS) {
        log_error("Failed to create a swapchain");
        return i;
    }
    arena_reset(temp_arena);

    log_verbose("Creating image views...");
    i = rana_vk_create_image_views(swapchain_arena);
    if (i != AMW_SUCCESS) {
        log_error("Failed to create a image views");
        return i;
    }

    if (RANA.vk.msaa_samples != VK_SAMPLE_COUNT_1_BIT) {
        i = rana_vk_create_color_resources();
        if (i != AMW_SUCCESS) {
            log_error("Failed to create the color resources.");
            return i;
        }
    }

    VkDescriptorSetLayoutBinding ubo_layout_binding = {
        .binding = 0,
        .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
        .descriptorCount = 1,
        .stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
    };
    VkDescriptorSetLayoutCreateInfo desc_set_layout_create_info = {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
        .bindingCount = 1,
        .pBindings = &ubo_layout_binding,
    };

    log_verbose("Creating the descriptor set layout...");
    RANA_VK_VERIFY(vkCreateDescriptorSetLayout(RANA.vk.device, 
                &desc_set_layout_create_info, NULL, &RANA.vk.descriptor_set_layout));

    log_verbose("Creating the graphics pipeline...");
    i = rana_vk_create_graphics_pipeline();
    if (i != AMW_SUCCESS) {
        log_error("Failed to create the graphics pipeline.");
        return i;
    }
    arena_reset(temp_arena);

    log_verbose("Creating the command pool...");
    VkCommandPoolCreateInfo cmd_pool_create_info = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
        .flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
        .queueFamilyIndex = RANA.vk.queue_family_indices[0], /* graphics queue idx */
    };
    RANA_VK_VERIFY(vkCreateCommandPool(RANA.vk.device,
            &cmd_pool_create_info, NULL, &RANA.vk.command_pool));

    log_verbose("Creating the vertex buffer...");
    i = rana_vk_create_vertex_buffer();
    if (i != AMW_SUCCESS) {
        log_error("Failed to create the vertex buffer.");
        return i;
    }

    log_verbose("Creating the index buffer...");
    i = rana_vk_create_index_buffer();
    if (i != AMW_SUCCESS) {
        log_error("Failed to create the index buffer.");
        return i;
    }

    log_verbose("Creating uniform buffers...");
    i = rana_vk_create_uniform_buffers();
    if (i != AMW_SUCCESS) {
        log_error("Failed to create uniform buffers.");
        return i;
    }

    log_verbose("Creating the descriptor pool...");
    i = rana_vk_create_descriptor_pool();
    if (i != AMW_SUCCESS) {
        log_error("Failed to create the descriptor pool.");
        return i;
    }

    log_verbose("Creating descriptor sets...");
    i = rana_vk_create_descriptor_sets();
    if (i != AMW_SUCCESS) {
        log_error("Failed to create descriptor sets.");
        return i;
    }

    log_verbose("Creating command buffers...");
    i = rana_vk_create_command_buffers();
    if (i != AMW_SUCCESS) {
        log_error("Failed to create command buffers.");
        return i;
    }

    log_verbose("Creating synchronization objects...");
    i = rana_vk_create_synchronization_objects();
    if (i != AMW_SUCCESS) {
        log_error("Failed to create synchronization objects.");
        return i;
    }

    arena_reset(temp_arena);
    return AMW_SUCCESS;
}

/* any RanaContext should be terminated by this point */
void rana_vk_terminate(void)
{
    vkDeviceWaitIdle(RANA.vk.device);

    rana_vk_cleanup_swapchain(&RANA.swapchain_arena);

    log_verbose("Destroying uniform buffers...");
    for (u32 i = 0; i < RANA_VK_MAX_FRAMES; i++) {
        if (RANA.vk.uniform_buffers[i] != VK_NULL_HANDLE)
            vkDestroyBuffer(RANA.vk.device, RANA.vk.uniform_buffers[i], NULL);
        if (RANA.vk.uniform_buffers_memory[i] != VK_NULL_HANDLE)
            vkFreeMemory(RANA.vk.device, RANA.vk.uniform_buffers_memory[i], NULL);
    }

    if (RANA.vk.descriptor_pool != VK_NULL_HANDLE) {
        log_verbose("Destroying descriptor pool...");
        vkDestroyDescriptorPool(RANA.vk.device, RANA.vk.descriptor_pool, NULL);
    }

    if (RANA.vk.descriptor_set_layout != VK_NULL_HANDLE) {
        log_verbose("Destroying descriptor set layout...");
        vkDestroyDescriptorSetLayout(RANA.vk.device, RANA.vk.descriptor_set_layout, NULL);
    }

    if (RANA.vk.index_buffer != VK_NULL_HANDLE) {
        log_verbose("Destroying index buffer...");
        vkDestroyBuffer(RANA.vk.device, RANA.vk.index_buffer, NULL);
        vkFreeMemory(RANA.vk.device, RANA.vk.index_buffer_memory, NULL);
    }

    if (RANA.vk.vertex_buffer != VK_NULL_HANDLE) {
        log_verbose("Destroying vertex buffer...");
        vkDestroyBuffer(RANA.vk.device, RANA.vk.vertex_buffer, NULL);
        vkFreeMemory(RANA.vk.device, RANA.vk.vertex_buffer_memory, NULL);
    }

    /* TODO ray tracing pipeline */

    if (RANA.vk.graphics_pipeline != VK_NULL_HANDLE) {
        log_verbose("Destroying graphics pipeline...");
        vkDestroyPipeline(RANA.vk.device, RANA.vk.graphics_pipeline, NULL);
    }

    if (RANA.vk.pipeline_layout != VK_NULL_HANDLE) {
        log_verbose("Destroying graphics pipeline layout...");
        vkDestroyPipelineLayout(RANA.vk.device, RANA.vk.pipeline_layout, NULL);
    }

    log_verbose("Destroying synchronization objects...");
    for (u32 i = 0; i < RANA_VK_MAX_FRAMES; i++) {
        if (RANA.vk.image_available_semaphores[i] != VK_NULL_HANDLE)
            vkDestroySemaphore(RANA.vk.device, RANA.vk.image_available_semaphores[i], NULL);
        if (RANA.vk.render_finished_semaphores[i] != VK_NULL_HANDLE)
            vkDestroySemaphore(RANA.vk.device, RANA.vk.render_finished_semaphores[i], NULL);
        if (RANA.vk.fences[i] != VK_NULL_HANDLE)
            vkDestroyFence(RANA.vk.device, RANA.vk.fences[i], NULL);
    }

    if (RANA.vk.command_pool) {
        log_verbose("Destroying command pool...");
        vkDestroyCommandPool(RANA.vk.device, RANA.vk.command_pool, NULL);
    }

    if (RANA.vk.device != VK_NULL_HANDLE) {
        log_verbose("Destroying device...");
        vkDestroyDevice(RANA.vk.device, NULL);
    }

    if (RANA.vk.surface != VK_NULL_HANDLE) {
        log_verbose("Destroying surface...");
        vkDestroySurfaceKHR(RANA.vk.instance, RANA.vk.surface, NULL);
    }

    if (read_bit(RANA.vk.ext_enabled, RANA_VK_EXT_VALIDATION_LAYERS_BIT))
        vulkan_destroy_validation_layers(RANA.vk.instance);

    if (RANA.vk.instance != VK_NULL_HANDLE) {
        log_verbose("Destroying instance...");
        vkDestroyInstance(RANA.vk.instance, NULL);
    }
    vulkan_close_driver();
}

void rana_vk_begin_frame(void)
{
    /* for now, do nothing */
}

void rana_vk_end_frame(void)
{
    rana_vk_draw_frame(&RANA.swapchain_arena, &RANA.temporary_arena);
}
