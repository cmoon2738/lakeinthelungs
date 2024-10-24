#ifndef _AMW_hadopelagic_h_
#define _AMW_hadopelagic_h_

#include "../common.h"
#include "system.h"
#include "hadal.h"
#include "headless.h"

#ifdef AMW_PLATFORM_UNIX
    #include <unistd.h>
    #include <poll.h>
#endif

#ifdef AMW_NATIVE_WAYLAND
    #include "linux/wl.h"
    #define HADOPELAGIC_WAYLAND_WINDOW_STATE WindowWayland wl;
    #define HADOPELAGIC_WAYLAND_GLOBAL_STATE HadopelagicWayland wl;
#else
    #define HADOPELAGIC_WAYLAND_WINDOW_STATE
    #define HADOPELAGIC_WAYLAND_GLOBAL_STATE
#endif

#ifdef AMW_NATIVE_VULKAN
    #include "../renderer/vk.h"
#endif

struct Window {
    Window *next;

    char   *title;
    u32     flags;

    /* Binded output in fullscreen mode */
    Output *output;

    HADOPELAGIC_WAYLAND_WINDOW_STATE
};

typedef struct HadalAPI {
    u32 id;

    i32  (*init)(void);
    void (*terminate)(void);

#ifdef AMW_NATIVE_VULKAN
    bool     (*vkPhysicalDevicePresentationSupport)(VkPhysicalDevice pd, u32 queue_family);
    VkResult (*vkCreateSurface)(VkInstance instance, Window *window, const VkAllocationCallbacks *allocator, VkSurfaceKHR *surface);
#endif
} HadalAPI;

typedef struct Hadopelagic {
    bool          initialized;
    HadalAPI      api;

    Window       *window_list_head;

    Output      **outputs;
    i32           output_count;

    HADOPELAGIC_WAYLAND_GLOBAL_STATE
} Hadopelagic;

/** Global platform state context. */
extern Hadopelagic HADAL;

/* INTERNAL API */

extern bool _hadal_debug_verify_api(const HadalAPI *api);

#endif /* _AMW_hadopelagic_h_ */
