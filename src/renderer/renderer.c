#include "common.h"
#include "renderer/rana.h"
#include "renderer.h"

RanaRenderer RANA = {0};

static const struct { u32 id; bool (*connect)(void); } supported_backends[] = {
#ifdef LAKE_NATIVE_VULKAN
    { RANA_BACKEND_VULKAN, VulkanOpenDriver },
#endif
};

static const char *backend_string(u32 id)
{
    switch (id) {
        case RANA_BACKEND_VULKAN: return "vulkan";
        case RANA_BACKEND_NULL: return "null";
        default: break;
    }
    return "(none)";
}

static bool select_backend(u32 id) 
{
    const size_t count = ArraySize(supported_backends);

    if (id != RANA_ANY_BACKEND &&
        id != RANA_BACKEND_VULKAN &&
        id != RANA_BACKEND_NULL)
    {
        LogError("RANA: invalid backend ID '%X : %s'", id, backend_string(id));
        return false;
    }

    /* Allow a NULL backend only if explicitly requested. */
    if (id == RANA_BACKEND_NULL) {
        /*
        if (!(RanaNullConnect())) {
            LogError("RANA: Failed to initialize null mode.");
            return false;
        }
        return true; 
        */
        LogError("RANA: Null mode not supported right now.");
        return false;
    } else if (count == 0) {
        LogError("RANA: This binary doesn't support any valid rendering APIs. Null mode must be called explicitly.");
        return false;
    }

    if (id == RANA_ANY_BACKEND) {
        if (count == 1)
            return supported_backends[0].connect();
        
        for (size_t i = 0; i < count; i++) {
            if (supported_backends[i].connect())
                return true;
        }
        LogError("RANA: Failed to detect any supported rendering backends.");
    } else {
        for (size_t i = 0; i < count; i++) {
            if (supported_backends[i].id == id)
                return supported_backends[i].connect();
        }
        LogError("RANA: The requested rendering backend is not supported");
    }
    return false;
}

bool RanaBackendIsSupported(u32 backend_id)
{
    const size_t count = ArraySize(supported_backends);
    size_t i;

    if (backend_id == RANA_BACKEND_NULL)
        return true;
    for (i = 0; i < count; i++) {
        if (backend_id == supported_backends[i].id)
            return true;
    }
    return false;
}

u32 RanaCurrentBackendID(void) {
    if (RANA.initialized)
        return RANA.api.id;
    return RANA_ANY_BACKEND;
}

static void terminate(void)
{
    LogVerbose("RANA: terminating...");

    for (RanaContext *context = RANA.context_list_head; context; context = context->next) {
        RanaDestroyContext(context);
    }

    if (RANA.api.terminate)
        RANA.api.terminate();

    Zero(RANA);
}

i32 RanaInit(u32 backend_id)
{
    LogVerbose("RANA: initializing...");

    if (backend_id <= 0)
        backend_id = HADAL_ANY_BACKEND;

    if (!select_backend(backend_id))
        return LAKE_ERROR_STUB;

    RANA.api.init();

    RANA.initialized = true;
    return LAKE_SUCCESS;
}

void RanaTerminate(void)
{
    if (RANA.initialized)
        terminate();
}

bool _RanaDebugVerifyAPI(const RanaAPI *api)
{
    bool out = true;
#if defined(LAKE_DEBUG) && !defined(LAKE_NDEBUG)
    Assert(api != NULL);
    const char *backend = backend_string(api->id);

#define RANA_APICHECK(fn) \
        if (api->fn == NULL) { LogDebug("RANA: missing call in internal api - '%s_%s'", backend, #fn); out = false; }

    RANA_APICHECK(init)
    RANA_APICHECK(terminate)

#undef RANA_APICHECK
#endif /* LAKE_DEBUG */
    return out;
}
