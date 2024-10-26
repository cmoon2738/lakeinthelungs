#ifndef _AMW_renderer_h_
#define _AMW_renderer_h_

#include "rana.h"
#include "../core/memory.h"

#ifdef AMW_NATIVE_VULKAN
    #include "vulkan/vk.h"
    #define RANA_VK_GLOBAL_STATE RanaVulkan vk;
#else
    #define RANA_VK_GLOBAL_STATE
#endif /* LAKE_NATIVE_VULKAN */

typedef struct Vertex {
    vec2 pos;
    vec3 color;
} Vertex;

typedef struct {
    mat2 view;
} UniformBufferObject;

typedef struct RanaAPI {
    u32   id;

    i32  (*init)(void);
    void (*terminate)(void);
    i32  (*recreate_swapchain)(Arena *swapchain_arena, Arena *temp_arena);
    i32  (*begin_frame)(void);
    void (*end_frame)(void);
} RanaAPI;

typedef struct RanaRenderer {
    bool            initialized;
    RanaAPI         api;

    Window         *window;
    Arena           temporary_arena;
    Arena           swapchain_arena;

    u32 flags;
    u32 current_frame;

    RANA_VK_GLOBAL_STATE
} RanaRenderer;

/** Global renderer state context. */
extern RanaRenderer RANA;

/* Internal API */
extern bool _rana_debug_verify_api(const RanaAPI *api);
extern i32  _rana_recreate_swapchain(Arena *swapchain_arena);

#endif /* _AMW_renderer_h_ */
