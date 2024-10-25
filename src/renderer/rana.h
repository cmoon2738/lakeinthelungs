#ifndef _LAKE_rana_h_
#define _LAKE_rana_h_

#include "../common.h"
#include "../platform/hadal.h"

typedef enum {
    RANA_ANY_BACKEND = 0,
    RANA_BACKEND_VULKAN,
    RANA_BACKEND_WEBGPU,
    RANA_BACKEND_NULL,
} RanaBackendID;

typedef enum {
    RANA_FLAG_FRAMEBUFFER_RESIZED   = 0x1,
} RanaFlags;

extern u32  rana_current_backend_id(void);
extern bool rana_backend_is_supported(u32 backend_id);

extern i32  rana_init(u32 backend_id);
extern void rana_terminate(void);

/** Opaque handle for the rendering context. */
typedef struct RanaContext RanaContext;

extern RanaContext *rana_create_context(Window *window);
extern void         rana_destroy_context(RanaContext *rana);
extern u32          rana_get_flags(RanaContext *rana);


#endif /* _LAKE_rana_h_ */
