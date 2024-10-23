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

extern u32  HadalCurrentBackendID(void);
extern bool HadalBackendIsSupported(u32 backend_id);

/** Initializing Hadal is required for all functions related to 
 *  windowing, input and the display backend. The shared state 
 *  is an internal global variable. */
extern i32  HadalInit(u32 backend_id);
extern void HadalTerminate(void);

/** Opaque window handle. */
typedef struct HadalWindow HadalWindow;

extern HadalWindow *HadalCreateWindow(u32 width, u32 height, const char *title, HadalWindow *share);
extern void         HadalDestroyWindow(HadalWindow *window);
extern u32          HadalGetFlags(HadalWindow *window);

/** Opaque output monitor handle. */
typedef struct HadalOutput HadalOutput;

#endif /* _LAKE_hadal_h_ */
