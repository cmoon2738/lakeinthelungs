#ifndef _AMW_headless_h_
#define _AMW_headless_h_

#include "hadal.h"

extern bool hadal_headless_connect(void);
extern i32  hadal_headless_init(void);
extern void hadal_headless_terminate(void);

#ifdef AMW_NATIVE_VULKAN
#include "../renderer/vulkan/vk.h"

extern bool hadal_headless_vkPhysicalDevicePresentationSupport(VkPhysicalDevice pd, u32 queue_family);
extern VkResult hadal_headless_vkCreateSurface(VkInstance instance, Window *window, const VkAllocationCallbacks *allocator, VkSurfaceKHR *surface);
#endif

#endif /* _AMW_headless_h_ */
