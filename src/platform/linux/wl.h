#ifndef _AMW_wayland_h_
#define _AMW_wayland_h_

#include "../hadal.h"

#include <wayland-client.h>

typedef struct WindowWayland {
    struct wl_surface *surface;
} WindowWayland;

typedef struct HadopelagicWayland {
    struct wl_display *display;
} HadopelagicWayland;

extern bool HadalWayland_connect(void);
extern i32  HadalWayland_init(void);
extern void HadalWayland_terminate(void);

#ifdef AMW_NATIVE_VULKAN
#include "../../renderer/vk.h"

extern bool HadalWayland_vkPhysicalDevicePresentationSupport(VkPhysicalDevice pd, u32 queue_family);
extern VkResult HadalWayland_vkCreateSurface(VkInstance instance, Window *window, const VkAllocationCallbacks *allocator, VkSurfaceKHR *surface);
#endif

#endif /* _AMW_wayland_h_ */
