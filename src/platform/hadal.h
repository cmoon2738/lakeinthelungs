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

typedef enum {
    WINDOW_FLAG_SHOULD_CLOSE = 0x1,
    WINDOW_FLAG_VISIBLE      = 0x2,
    WINDOW_FLAG_ICONIFIED    = 0x4,
    WINDOW_FLAG_MAXIMIZED    = 0x8,
    WINDOW_FLAG_FULLSCREEN   = 0x10,
    WINDOW_FLAG_ACTIVATED    = 0x20,
    WINDOW_FLAG_RESIZABLE    = 0x40,
    WINDOW_FLAG_IS_RESIZING  = 0x80,
    WINDOW_FLAG_AUTO_ICONIFY = 0x100,
    WINDOW_FLAG_TRANSPARENT  = 0x200,
} WindowFlags;

/** Opaque window handle. */
typedef struct Window Window;

/** Opaque output monitor handle. */
typedef struct Output Output;

extern Window *hadal_create_window(u32 width, u32 height, const char *title, Output *output, u32 hints);
extern void    hadal_destroy_window(Window *window);

extern u32     hadal_get_flags(Window *window);
extern void    hadal_set_should_close(Window *window);
extern void    hadal_set_visible(Window *window, bool visible);

#endif /* _AMW_hadal_h_ */
