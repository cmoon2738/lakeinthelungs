#ifndef _AMW_hadal_h_
#define _AMW_hadal_h_

#include "../common.h"

typedef enum {
    HADAL_ANY_BACKEND = 0,
    HADAL_BACKEND_WIN32,
    HADAL_BACKEND_COCOA,
    HADAL_BACKEND_IOS,
    HADAL_BACKEND_ANDROID,
    HADAL_BACKEND_HTML5,
    HADAL_BACKEND_WAYLAND,
    HADAL_BACKEND_XCB,
    HADAL_BACKEND_KMS,
    HADAL_BACKEND_HEADLESS,
} HadalBackendID;

extern u32  hadal_current_backend_id(void);
extern bool hadal_backend_is_supported(u32 backend_id);

/** Initializing Hadal is required for all functions related to 
 *  windowing, input and the display backend. The shared state 
 *  is an internal global variable. */
extern i32  hadal_init(u32 backend_id);
extern void hadal_terminate(void);

/** Opaque window handle. */
typedef struct Window Window;

extern Window *hadal_create_window(u32 width, u32 height, const char *title, Window *share);
extern void    hadal_destroy_window(Window *window);
extern u32     hadal_get_flags(Window *window);

/** Opaque output monitor handle. */
typedef struct Output Output;

#endif /* _AMW_hadal_h_ */
