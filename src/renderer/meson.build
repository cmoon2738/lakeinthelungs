sources += files(
    'renderer.c',
)

if sys == 'emscripten'
    #sources += files('')
 
    defs += '-DAMW_NATIVE_WEBGPU=1'
    enabled_render_backends += 'webgpu'
else
    sources += files(
        'vulkan/vk_buffer.c',
        'vulkan/vk_cmd.c',
        'vulkan/vk_descriptor.c',
        'vulkan/vk_draw.c',
        'vulkan/vk_driver.c',
        'vulkan/vk_gpu.c',
        'vulkan/vk_pipeline.c',
        'vulkan/vk_rana.c',
        'vulkan/vk_shaders.c',
        'vulkan/vk_surface.c',
        'vulkan/vk_swapchain.c',
        'vulkan/vk_sync.c',
        'vulkan/vk_validation.c',
    )
    defs += '-DAMW_NATIVE_VULKAN=1'
    enabled_render_backends += 'vulkan'
endif
