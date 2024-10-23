#include "hadal.h"

extern bool HadalHeadless_Connect(void);
extern i32  HadalHeadless_Init(void);
extern void HadalHeadless_Terminate(void);

#ifdef LAKE_NATIVE_VULKAN
#include "../renderer/vk.h"

extern bool HadalHeadless_vkPhysicalDevicePresentationSupport(VkPhysicalDevice pd, u32 queue_family);
extern VkResult HadalHeadless_vkCreateSurface(VkInstance instance, HadalWindow *window, const VkAllocationCallbacks *allocator, VkSurfaceKHR *surface);
#endif
