#ifndef _LAKE_hadopelagic_h_
#define _LAKE_hadopelagic_h_

//#include "system.h"
#include "hadal.h"
#include "headless.h"

#ifdef LAKE_PLATFORM_UNIX
    #include <unistd.h>
    #include <poll.h>
#endif

#ifdef LAKE_NATIVE_WAYLAND
    #include "linux/wl.h"
    #define HADOPELAGIC_WAYLAND_WINDOW_STATE HadalWindowWayland wl;
    #define HADOPELAGIC_WAYLAND_GLOBAL_STATE HadopelagicWayland wl;
#else
    #define HADOPELAGIC_WAYLAND_WINDOW_STATE
    #define HADOPELAGIC_WAYLAND_GLOBAL_STATE
#endif

#ifdef LAKE_NATIVE_VULKAN
    #include "../renderer/vk.h"
#endif

struct HadalWindow {
    HadalWindow *next;

    char        *title;
    u32          flags;

    HadalOutput *output;

    HADOPELAGIC_WAYLAND_WINDOW_STATE
};

typedef struct HadalAPI {
    u32 id;

    i32  (*init)(void);
    void (*terminate)(void);

#ifdef LAKE_NATIVE_VULKAN
    bool     (*vkPhysicalDevicePresentationSupport)(VkPhysicalDevice pd, u32 queue_family);
    VkResult (*vkCreateSurface)(VkInstance instance, HadalWindow *window, const VkAllocationCallbacks *allocator, VkSurfaceKHR *surface);
#endif
} HadalAPI;

typedef struct Hadopelagic {
    bool          initialized;
    HadalAPI      api;

    HadalWindow  *window_list_head;

    HadalOutput **outputs;
    i32           output_count;

    HADOPELAGIC_WAYLAND_GLOBAL_STATE
} Hadopelagic;

/** Global platform state context. */
extern Hadopelagic HADAL;

/* INTERNAL API */

extern bool _HadalDebugVerifyAPI(const HadalAPI *api);

#endif /* _LAKE_hadopelagic_h_ */
