#include "common.h"
#include "hadopelagic.h"
#include "platform/hadal.h"

Hadopelagic HADAL = {0};

static const char *backend_string(u32 id)
{
    switch (id) {
        case HADAL_BACKEND_WIN32: return "win32";
        case HADAL_BACKEND_COCOA: return "cocoa";
        case HADAL_BACKEND_IOS: return "ios";
        case HADAL_BACKEND_ANDROID: return "android";
        case HADAL_BACKEND_WAYLAND: return "wayland";
        case HADAL_BACKEND_XCB: return "xcb";
        case HADAL_BACKEND_KMS: return "kms";
        case HADAL_BACKEND_HEADLESS: return "headless";
        default: break;
    }
    return "(none)";
}

static const struct { u32 id; bool (*connect)(void); } supported_backends[] = {
#if defined(AMW_PLATFORM_WINDOWS)
    /* TODO */
#elif defined(AMW_PLATFORM_MACOSX)
    /* TODO */
#elif defined(AMW_PLATFORM_IOS)
    /* TODO */
#elif defined(AMW_PLATFORM_ANDROID)
    /* TODO */
#endif
#ifdef AMW_NATIVE_WAYLAND
    { HADAL_BACKEND_WAYLAND, HadalWayland_connect},
#endif
#ifdef AMW_NATIVE_XCB
    /* TODO */
#endif
#ifdef AMW_NATIVE_KMS
    /* TODO */
#endif
};

static bool select_backend(u32 id) 
{
    const size_t count = array_size(supported_backends);

    if (id != HADAL_ANY_BACKEND &&
        id != HADAL_BACKEND_WIN32 &&
        id != HADAL_BACKEND_COCOA &&
        id != HADAL_BACKEND_IOS &&
        id != HADAL_BACKEND_ANDROID &&
        id != HADAL_BACKEND_WAYLAND &&
        id != HADAL_BACKEND_XCB &&
        id != HADAL_BACKEND_KMS &&
        id != HADAL_BACKEND_HEADLESS)
    {
        log_error("HADAL: invalid backend ID '%X : %s'", id, backend_string(id));
        return false;
    }

    /* only allow headless mode if explicitly requested */
    if (id == HADAL_BACKEND_HEADLESS) {
        if (!HadalHeadless_connect()) {
            log_error("HADAL: failed to initialize headless mode.");
            return false;
        }
        return true; 
    } else if (count == 0) {
        log_error("HADAL: this binary supports only headless platform. Headless mode must be called explicitly.");
        return false;
    }

    if (id == HADAL_ANY_BACKEND) {
        if (count == 1)
            return supported_backends[0].connect();
        
        for (size_t i = 0; i < count; i++) {
            if (supported_backends[i].connect())
                return true;
        }
        log_error("HADAL: failed to detect any supported backends.");
    } else {
        for (size_t i = 0; i < count; i++) {
            if (supported_backends[i].id == id)
                return supported_backends[i].connect();
        }
        log_error("HADAL: the requested backend is not supported");
    }
    return false;
}

u32 hadal_current_backend_id(void)
{
    if (HADAL.initialized)
        return HADAL.api.id;
    return HADAL_ANY_BACKEND;
}

bool hadal_backend_is_supported(u32 backend_id)
{
    const size_t count = array_size(supported_backends);
    size_t i;

    if (backend_id == HADAL_BACKEND_HEADLESS)
        return true;
    for (i = 0; i < count; i++) {
        if (backend_id == supported_backends[i].id)
            return true;
    }
    return false;
}

static void terminate(void)
{
    log_verbose("HADAL: terminating...");

    for (Window *window = HADAL.window_list_head; window; window = window->next) {
        hadal_destroy_window(window);
    }

    if (HADAL.api.terminate)
        HADAL.api.terminate();

    // TODO mutex

    zero(HADAL);
}

i32 hadal_init(u32 backend_id)
{
    if (HADAL.initialized)
        return AMW_SUCCESS;

    log_verbose("HADAL: initializing...");

    if (backend_id <= 0)
        backend_id = HADAL_ANY_BACKEND;

    if (!select_backend(backend_id))
        return AMW_ERROR_STUB;

    // TODO mutex

    HADAL.api.init();

    HADAL.initialized = true;
    return AMW_SUCCESS;
}

void hadal_terminate(void)
{
    if (HADAL.initialized)
        terminate();
}

bool _hadal_debug_verify_api(const HadalAPI *api)
{
    bool out = true;

#if defined(AMW_DEBUG) && !defined(AMW_NDEBUG)
    assert_debug(api != NULL);
    const char *backend = backend_string(api->id);

#define HADAL_APICHECK(fn) \
        if (api->fn == NULL) { log_debug("HADAL: missing call in internal api - '%s_%s'", backend, #fn); out = false; }

    HADAL_APICHECK(init)
    HADAL_APICHECK(terminate)
#ifdef AMW_NATIVE_VULKAN
    HADAL_APICHECK(vkPhysicalDevicePresentationSupport)
    HADAL_APICHECK(vkCreateSurface)
#endif /* AMW_NATIVE_VULKAN */
    
#undef HADAL_APICHECK
#else
    (void)api;
#endif /* AMW_DEBUG */
    return out;
}
