#ifndef _LAKE_lake_h_
#define _LAKE_lake_h_

#include "../common.h"
#include "../core/events.h"

extern void LakeInit(void *data);
extern void LakeFrame(void *data, f64 delta_time);
extern void LakeEvent(void *data, Event *event);
extern void LakeClean(void *data);

#endif /* _LAKE_lake_h_ */
