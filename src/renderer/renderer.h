#ifndef _AMW_renderer_h_
#define _AMW_renderer_h_

#include "rana.h"
#include "../core/memory.h"

#ifdef AMW_NATIVE_VULKAN
    #include "vulkan/vk.h"
    #define RANA_VK_CONTEXT_STATE RanaVulkanContext vk;
    #define RANA_VK_GLOBAL_STATE RanaVulkanRenderer vk;
#else
    #define RANA_VK_CONTEXT_STATE
    #define RANA_VK_GLOBAL_STATE
#endif /* LAKE_NATIVE_VULKAN */

struct RanaContext {
    RanaContext *next;
    u32          flags;

    /* window whose surface is binded to the rendering context */
    Window      *window;

    RANA_VK_CONTEXT_STATE
};

typedef struct RanaAPI {
    u32   id;

    i32  (*init)(void);
    void (*terminate)(void);
} RanaAPI;

typedef struct RanaRenderer {
    bool            initialized;
    RanaAPI         api;

    RanaContext    *context_list_head;
    Arena           temporary_arena;

    RANA_VK_GLOBAL_STATE
} RanaRenderer;

/** Global renderer state context. */
extern RanaRenderer RANA;

/* Internal API */
extern bool _rana_debug_verify_api(const RanaAPI *api);

#endif /* _AMW_renderer_h_ */
