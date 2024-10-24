#include "platform/hadal.h"
#include "vk.h"
#include "../platform/hadopelagic.h"

char *vulkan_get_surface_extension(void)
{
    u32 id = hadal_current_backend_id();
    switch (id) {
#ifdef VK_USE_PLATFORM_WIN32_KHR
        case HADAL_BACKEND_WIN32:
            return VK_KHR_WIN32_SURFACE_EXTENSION_NAME;
#endif
#ifdef VK_USE_PLATFORM_METAL_EXT
        case HADAL_BACKEND_COCOA:
        case HADAL_BACKEND_IOS:
            return VK_EXT_METAL_SURFACE_EXTENSION_NAME;
#endif
#ifdef VK_USE_PLATFORM_ANDROID_KHR
        case HADAL_BACKEND_ANDROID:
            return VK_KHR_ANDROID_SURFACE_EXTENSION_NAME;
#endif
#ifdef VK_USE_PLATFORM_WAYLAND_KHR
        case HADAL_BACKEND_WAYLAND:
            return VK_KHR_WAYLAND_SURFACE_EXTENSION_NAME;
#endif
#ifdef VK_USE_PLATFORM_XCB_KHR
        case HADAL_BACKEND_XCB:
            return VK_KHR_XCB_SURFACE_EXTENSION_NAME;
#endif
#ifdef VK_USE_PLATFORM_KMS_KHR
        case HADAL_BACKEND_KMS:
            return VK_KHR_DISPLAY_EXTENSION_NAME;
#endif
        case HADAL_BACKEND_HEADLESS:
            return VK_EXT_HEADLESS_SURFACE_EXTENSION_NAME;
        case HADAL_ANY_BACKEND: 
        default:
            return NULL;
    }
}

bool vulkan_physical_device_presentation_support(VkPhysicalDevice pd, u32 queue_family)
{
    if (!HADAL.initialized)
        return false;
    assert_debug(pd != VK_NULL_HANDLE);
    return HADAL.api.vkPhysicalDevicePresentationSupport(pd, queue_family);
}

VkResult vulkan_create_surface(VkInstance instance, Window *window, const VkAllocationCallbacks *allocator, VkSurfaceKHR *surface)
{
    assert_debug(surface != NULL);
    *surface = VK_NULL_HANDLE;

    if (!HADAL.initialized)
        return VK_ERROR_INITIALIZATION_FAILED;
    assert_debug(window != NULL);
    assert_debug(instance != VK_NULL_HANDLE);

    return HADAL.api.vkCreateSurface(instance, window, allocator, surface);
}

#if defined(AMW_PLATFORM_WINDOWS)
#elif defined(AMW_PLATFORM_MACOSX)
#elif defined(AMW_PLATFORM_IOS)
#elif defined(AMW_PLATFORM_ANDROID)
#endif /* WINDOWS / APPLE / ANDROID */

#if defined(AMW_NATIVE_WAYLAND)
bool HadalWayland_vkPhysicalDevicePresentationSupport(VkPhysicalDevice pd, u32 queue_family)
{
    return vkGetPhysicalDeviceWaylandPresentationSupportKHR ?
        vkGetPhysicalDeviceWaylandPresentationSupportKHR(pd, queue_family, HADAL.wl.display) : false;
}

VkResult HadalWayland_vkCreateSurface(VkInstance instance, Window *window, const VkAllocationCallbacks *allocator, VkSurfaceKHR *surface)
{
    VkResult out;
    VkWaylandSurfaceCreateInfoKHR wlsc_info;

    zero(wlsc_info);
    wlsc_info.sType = VK_STRUCTURE_TYPE_WAYLAND_SURFACE_CREATE_INFO_KHR;
    wlsc_info.display = HADAL.wl.display;
    wlsc_info.surface = window->wl.surface;

    out = vkCreateWaylandSurfaceKHR(instance, &wlsc_info, allocator, surface);
    if (out != VK_SUCCESS)
        log_error("Failed to create a Wayland VkSurface: %s", vulkan_result(out));
    return out;
}
#endif /* AMW_NATIVE_WAYLAND */

#if defined(AMW_NATIVE_XCB)
#endif /* AMW_NATIVE_XCB */

#if defined(AMW_NATIVE_KMS)
#endif /* AMW_NATIVE_KMS */

bool HadalHeadless_vkPhysicalDevicePresentationSupport(VkPhysicalDevice pd, u32 queue_family)
{
    (void)pd;
    (void)queue_family;
    return false;
}

VkResult HadalHeadless_vkCreateSurface(VkInstance instance, Window *window, const VkAllocationCallbacks *allocator, VkSurfaceKHR *surface)
{
    (void)instance;
    (void)window;
    (void)allocator;
    (void)surface;
    return VK_ERROR_FEATURE_NOT_PRESENT;
}
