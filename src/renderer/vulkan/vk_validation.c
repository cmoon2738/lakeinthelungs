#include "vk.h"

#ifdef VK_EXT_debug_utils
static VkDebugUtilsMessengerEXT validation_messenger = VK_NULL_HANDLE;

/* layer message to string */
static const char* msg_to_string(VkDebugUtilsMessageTypeFlagsEXT type)
{
	int g = (type & VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT) != 0 ? 1 : 0;
	int p = (type & VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT) != 0 ? 1 : 0;
	int v = (type & VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT) != 0 ? 1 : 0;

	if (g) return "";
	if (p && !v) return "(performance)";
	if (p &&  v) return "(performance and validation)";
	if (v) return "(validation)";

	return "?";
}

/* validation layer callback function (VK_EXT_debug_utils) */
static VKAPI_ATTR VkBool32 VKAPI_CALL debug_utils_callback(VkDebugUtilsMessageSeverityFlagBitsEXT msg_severity,
													  	   VkDebugUtilsMessageTypeFlagsEXT msg_type,
														   const VkDebugUtilsMessengerCallbackDataEXT *callback_data,
														   void *userdata)
{
    /* unused, it may be the structure for Vulkan backend, but doesn't matter rn */
    (void)userdata;

	switch (msg_severity) {
#if LOG_USE_COLOR
	case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
        log_debug("\033[38;5;215m%s \x1b[0m%s", callback_data->pMessage, msg_to_string(msg_type));
		break;
	case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
        log_verbose("\033[38;5;180m%s \x1b[0m%s", callback_data->pMessage, msg_to_string(msg_type));
		break;
	case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
        log_warn("\033[38;5;167m%s \x1b[0m%s", callback_data->pMessage, msg_to_string(msg_type));
		break;
	default:
        log_error("\033[38;5;160m%s \x1b[0m%s", callback_data->pMessage, msg_to_string(msg_type));
#else
	case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
        log_debug("%s %s", callback_data->pMessage, msg_to_string(msg_type));
		break;
	case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
        log_verbose("%s %s", callback_data->pMessage, msg_to_string(msg_type));
		break;
	case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
        log_warn("%s %s", callback_data->pMessage, msg_to_string(msg_type));
		break;
	default:
        log_error("%s %s", callback_data->pMessage, msg_to_string(msg_type));
#endif /* LOG_USE_COLOR */
		assert_debug(!"vulkan validation error");
	}
	return VK_FALSE;
}

void vulkan_create_validation_layers(VkInstance instance)
{
    if (validation_messenger == VK_NULL_HANDLE) {
        VkDebugUtilsMessengerCreateInfoEXT callback_info = {
            .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
            .pNext = NULL,
            .flags = 0,
            .messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                               VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT |
                               VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT |
                               VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT,
            .messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                           VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                           VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
            .pfnUserCallback = debug_utils_callback,
            .pUserData = NULL
        };
        RANA_VK_VERIFY(vkCreateDebugUtilsMessengerEXT(instance, &callback_info, NULL, &validation_messenger));
        log_debug("Vulkan validation layers created!");
    }
}

void vulkan_destroy_validation_layers(VkInstance instance)
{
    if (validation_messenger != VK_NULL_HANDLE) {
        vkDestroyDebugUtilsMessengerEXT(instance, validation_messenger, NULL);
        validation_messenger = VK_NULL_HANDLE;
        log_debug("Destroying validation layers...");
    }
}
#else
void vulkan_create_validation_layers(VkInstance instance) { (void)instance; }
void vulkan_destroy_validation_layers(VkInstance instance) { (void)instance; }
#endif /* VK_EXT_debug_utils */

const char *vulkan_result(VkResult code)
{
#define ERRSTR(r) case VK_ ##r: return "VK_"#r
	switch (code) {
		ERRSTR(SUCCESS);
		ERRSTR(NOT_READY);
		ERRSTR(TIMEOUT);
		ERRSTR(EVENT_SET);
		ERRSTR(EVENT_RESET);
		ERRSTR(INCOMPLETE);
		ERRSTR(ERROR_OUT_OF_HOST_MEMORY);
		ERRSTR(ERROR_OUT_OF_DEVICE_MEMORY);
		ERRSTR(ERROR_INITIALIZATION_FAILED);
		ERRSTR(ERROR_DEVICE_LOST);
		ERRSTR(ERROR_MEMORY_MAP_FAILED);
		ERRSTR(ERROR_LAYER_NOT_PRESENT);
		ERRSTR(ERROR_EXTENSION_NOT_PRESENT);
		ERRSTR(ERROR_FEATURE_NOT_PRESENT);
		ERRSTR(ERROR_INCOMPATIBLE_DRIVER);
		ERRSTR(ERROR_TOO_MANY_OBJECTS);
		ERRSTR(ERROR_FORMAT_NOT_SUPPORTED);
		ERRSTR(ERROR_SURFACE_LOST_KHR);
		ERRSTR(ERROR_NATIVE_WINDOW_IN_USE_KHR);
		ERRSTR(SUBOPTIMAL_KHR);
		ERRSTR(ERROR_OUT_OF_DATE_KHR);
		ERRSTR(ERROR_INCOMPATIBLE_DISPLAY_KHR);
		ERRSTR(ERROR_VALIDATION_FAILED_EXT);
		ERRSTR(ERROR_INVALID_SHADER_NV);
		ERRSTR(ERROR_OUT_OF_POOL_MEMORY);
		default: return "<unknown>";
	}
#undef ERRSTR
	return "UNKNOWN ERROR";
}
