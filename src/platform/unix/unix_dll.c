#include "../system.h"

#include <dlfcn.h>

void *sys_load_dll(const char *libname)
{
    assert_debug(libname);
    void *handle = dlopen(libname, RTLD_NOW | RTLD_LOCAL);
    if (!handle)
        log_error("sys_load_dll: dlopen '%s' failed: %s", libname, dlerror());
    return handle;
}

void sys_close_dll(void *handle)
{
    assert_debug(handle);
    dlclose(handle);
}

void *sys_get_proc_address(void *handle, const char *procname)
{
    assert_debug(handle && procname);
    const char *error;
    void *addr = dlsym(handle, procname);
    if ((error = dlerror()) != NULL) 
        log_error("sys_get_proc_address: dlsym '%s' failed: %s", procname, error);
    return addr;
}
