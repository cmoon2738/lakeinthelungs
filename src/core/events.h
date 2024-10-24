#ifndef _AMW_events_h_
#define _AMW_events_h_

#include "../common.h"

typedef enum {
    EVENT_TYPE_INVALID = 0,
    EVENT_TYPE_USER = 0x100,

    EVENT_ENUM_PADDING = 0x7FFFFFFF,
} EventType;

/** Fields shared by every event */
typedef struct EventCommon {
    u32 type;             /**< The EventType code indicated what event structure it is. */
    u32 flags;            /**< Populated by a structures flag set, or 0. */
    u64 ticks_ns;         /**< Populated using ticks_ns(). */
} EventCommon;

/** Custom app-defined events */
typedef struct EventUser {
    u32   type;           /**< EVENT_TYPE_USER */
    u32   flags;          /**< The user may pass in his boolean bit flags for checks. */
    u64   ticks_ns;       /**< Populated using ticks_ns(). */
    i32   code;           /**< User defined event code. */
    void *data1;          /**< User defined data pointer. */
    void *data2;          /**< User defined data pointer. */
    void *data3;          /**< User defined data pointer. */
} EventUser;

typedef union Event {
    u32 type;

    EventCommon common;
    EventUser   user;

    u8 padding[48];
} Event;

static_assertion(sizeof(Event) == sizeof(((Event *)NULL)->padding), "Event union padding is not large enough");

#endif /* _AMW_events_h_ */
