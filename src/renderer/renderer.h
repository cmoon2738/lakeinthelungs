#ifndef _AMW_renderer_h_
#define _AMW_renderer_h_

#include "rana.h"
#include "../core/memory.h"

#ifdef AMW_NATIVE_VULKAN
    #include "vulkan/vk.h"
    #define RANA_VK_GLOBAL_STATE RanaVulkanRenderer vk;
#else
    #define RANA_VK_GLOBAL_STATE
#endif /* LAKE_NATIVE_VULKAN */


typedef struct RanaAPI {
    u32   id;

    i32  (*init)(void);
    void (*terminate)(void);
} RanaAPI;

typedef struct RanaRenderer {
    bool            initialized;
    RanaAPI         api;

    Window         *window;
    Arena           temporary_arena;

    RANA_VK_GLOBAL_STATE
} RanaRenderer;

/** Global renderer state context. */
extern RanaRenderer RANA;

/* Internal API */
extern bool _rana_debug_verify_api(const RanaAPI *api);

#endif /* _AMW_renderer_h_ */
