#include "vk.h"
#include "../../platform/hadopelagic.h"

char *vulkan_get_surface_extension(void)
{
    u32 id = hadal_current_backend_id();
    switch (id) {
        case HADAL_BACKEND_WIN32:
            return "VK_KHR_win32_surface";
        case HADAL_BACKEND_COCOA: /* return "VK_MVK_macos_surface" */
        case HADAL_BACKEND_IOS: /* return "VK_MVK_ios_surface" */
            return "VK_EXT_metal_surface";
        case HADAL_BACKEND_ANDROID:
            return "VK_KHR_android_surface";
        case HADAL_BACKEND_WAYLAND:
            return "VK_KHR_wayland_surface";
        case HADAL_BACKEND_XCB:
            return "VK_KHR_xcb_surface";
        case HADAL_BACKEND_KMS:
            return "VK_KHR_display";
        case HADAL_BACKEND_HEADLESS:
            return "VK_EXT_headless_surface";
        case HADAL_ANY_BACKEND: 
        case HADAL_BACKEND_HTML5:
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
bool hadal_wayland_vkPhysicalDevicePresentationSupport(VkPhysicalDevice pd, u32 queue_family)
{
    return vkGetPhysicalDeviceWaylandPresentationSupportKHR ?
        vkGetPhysicalDeviceWaylandPresentationSupportKHR(pd, queue_family, HADAL.wl.display) : false;
}

VkResult hadal_wayland_vkCreateSurface(VkInstance instance, Window *window, const VkAllocationCallbacks *allocator, VkSurfaceKHR *surface)
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

bool hadal_headless_vkPhysicalDevicePresentationSupport(VkPhysicalDevice pd, u32 queue_family)
{
    (void)pd;
    (void)queue_family;
    return false;
}

VkResult hadal_headless_vkCreateSurface(VkInstance instance, Window *window, const VkAllocationCallbacks *allocator, VkSurfaceKHR *surface)
{
    (void)instance;
    (void)window;
    (void)allocator;
    (void)surface;
    return VK_ERROR_FEATURE_NOT_PRESENT;
}
