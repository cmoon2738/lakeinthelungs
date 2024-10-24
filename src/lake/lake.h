#ifndef _LAKE_lake_h_
#define _LAKE_lake_h_

#include "../common.h"
#include "../core/events.h"

extern void lake_init(void *data);
extern void lake_frame(void *data, f64 delta_time);
extern void lake_event(void *data, Event *event);
extern void lake_clean(void *data);

#endif /* _LAKE_lake_h_ */
