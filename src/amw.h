#ifndef _LAKE_amw_h_
#define _LAKE_amw_h_

#include "common.h"
#include "core/events.h"

typedef void (*PFN_AppInit)(void *data);
typedef void (*PFN_AppFrame)(void *data, f64 delta_time);
typedef void (*PFN_AppEvent)(void *data, Event *event);
typedef void (*PFN_AppClean)(void *data);

typedef struct AppDescription {
    PFN_AppInit  init;
    PFN_AppFrame frame;
    PFN_AppEvent event;
    PFN_AppClean clean;
    void        *data;
    const char  *name;
    u32          version;
} AppDescription;

typedef enum {
    AMW_FLAG_SHOULD_QUIT = 0x1,
} AMWEngineFlags;

typedef struct AMWEngine {
    u32 flags;
    f64 delta_time;

    AppDescription *app;
} AMWEngine;

/** Global 'A Moonlit Walk' engine state context */
extern AMWEngine AMW;

#endif /* _LAKE_amw_h_ */