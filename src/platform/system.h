#ifndef _AMW_system_h_
#define _AMW_system_h_

#include "../common.h"

/** Exits the game process with a return code. */
NORETURN extern void sys_exit(i32 exitcode);

/** Overwrites the return exit code of the game process 
 *  this code will be returned when SysExit() is called. */
extern void sys_exitcode(i32 exitcode);

/** Puts current thread to sleep. */
extern void sys_sleep(i32 ns);

/** Loads a shared library into memory, returns an opaque handle. */
extern void *sys_load_dll(const char *libname);

/** Closes the loaded shared library. */
extern void sys_close_dll(void *handle);

/** Returns a memory address for a DLL exported function. */
extern void *sys_get_proc_address(void *handle, const char *procname);

/** Get the current value of the high resolution counter. */
extern u64 sys_timer_counter(void);

/** Get the count per second of the high resolution counter. */
extern u64 sys_timer_frequency(void);

/** Reads information about the cpu, saves them to the arguments. */
extern void sys_cpu_count(i32 *threads, i32 *cores, i32 *packages);

#endif /* _AMW_system_h_ */
