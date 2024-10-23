#ifndef _LAKE_rana_h_
#define _LAKE_rana_h_

#include "../common.h"

typedef enum {
    RANA_ANY_BACKEND = 0,
    RANA_BACKEND_VULKAN,
    RANA_BACKEND_NULL,
} RanaBackendID;

typedef enum {
    RANA_FLAG_INITIALIZED           = 0x1,
    RANA_FLAG_FRAMEBUFFER_RESIZED   = 0x2,
} RanaFlags;

i32 RanaInit(u32 backend_id);

#endif /* _LAKE_rana_h_ */
