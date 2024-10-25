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

extern bool hadal_wayland_connect(void);
extern i32  hadal_wayland_init(void);
extern void hadal_wayland_terminate(void);

#ifdef AMW_NATIVE_VULKAN
#include "../../renderer/vulkan/vk.h"

extern bool hadal_wayland_vkPhysicalDevicePresentationSupport(VkPhysicalDevice pd, u32 queue_family);
extern VkResult hadal_wayland_vkCreateSurface(VkInstance instance, Window *window, const VkAllocationCallbacks *allocator, VkSurfaceKHR *surface);
#endif

#endif /* _AMW_wayland_h_ */
