#include "../hadal.h"

#include <wayland-client.h>

typedef struct HadalWindowWayland {
    struct wl_surface *surface;
} HadalWindowWayland;

typedef struct HadopelagicWayland {
    struct wl_display *display;
} HadopelagicWayland;

extern bool HadalWayland_Connect(void);
extern i32  HadalWayland_Init(void);
extern void HadalWayland_Terminate(void);

#ifdef LAKE_NATIVE_VULKAN
#include "../../renderer/vk.h"

extern bool HadalWayland_vkPhysicalDevicePresentationSupport(VkPhysicalDevice pd, u32 queue_family);
extern VkResult HadalWayland_vkCreateSurface(VkInstance instance, HadalWindow *window, const VkAllocationCallbacks *allocator, VkSurfaceKHR *surface);
#endif
