#ifndef _LAKE_hadal_h_
#define _LAKE_hadal_h_

#include "../common.h"

typedef enum {
    HADAL_ANY_BACKEND = 0,
    HADAL_BACKEND_WIN32,
    HADAL_BACKEND_COCOA,
    HADAL_BACKEND_IOS,
    HADAL_BACKEND_ANDROID,
    HADAL_BACKEND_WAYLAND,
    HADAL_BACKEND_XCB,
    HADAL_BACKEND_KMS,
    HADAL_BACKEND_HEADLESS,
} HadalBackendID;

u32  HadalCurrentBackend(void);
bool HadalBackendSupported(u32 backend_id);

/** Initializing Hadal is required for all functions related to 
 *  windowing, input and the display backend. The shared state 
 *  is an internal global variable. */
i32  HadalInit(u32 backend_id);
void HadalTerminate(void);

/** Opaque window handle. */
typedef struct HadalWindow HadalWindow;

/** Opaque output monitor handle. */
typedef struct HadalOutput HadalOutput;

#endif /* _LAKE_hadal_h_ */
