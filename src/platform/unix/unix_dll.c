#include "../system.h"

#include <dlfcn.h>

void *SysLoadDLL(const char *libname)
{
    Assert(libname);
    void *handle = dlopen(libname, RTLD_NOW | RTLD_LOCAL);
    if (!handle)
        LogError("SysLoadDLL: dlopen '%s' failed: %s", libname, dlerror());
    return handle;
}

void SysCloseDLL(void *handle)
{
    Assert(handle);
    dlclose(handle);
}

void *SysGetProcAddress(void *handle, const char *procname)
{
    Assert(handle && procname);
    const char *error;
    void *addr = dlsym(handle, procname);
    if ((error = dlerror()) != NULL) 
        LogError("SysGetProcAddress: dlsym '%s' failed: %s", procname, error);
    return addr;
}
