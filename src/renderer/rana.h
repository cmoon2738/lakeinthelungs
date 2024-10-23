#ifndef _LAKE_rana_h_
#define _LAKE_rana_h_

#include "../common.h"
#include "../platform/hadal.h"

typedef enum {
    RANA_ANY_BACKEND = 0,
    RANA_BACKEND_VULKAN,
    RANA_BACKEND_NULL,
} RanaBackendID;

typedef enum {
    RANA_FLAG_FRAMEBUFFER_RESIZED   = 0x1,
} RanaFlags;

extern u32  RanaCurrentBackendID(void);
extern bool RanaBackendIsSupported(u32 backend_id);

extern i32  RanaInit(u32 backend_id);
extern void RanaTerminate(void);

typedef struct RanaContext RanaContext;

extern RanaContext *RanaCreateContext(HadalWindow *window);
extern void         RanaDestroyContext(RanaContext *rana);
extern u32          RanaGetFlags(RanaContext *rana);


#endif /* _LAKE_rana_h_ */
