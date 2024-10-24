#ifndef _AMW_system_h_
#define _AMW_system_h_

#include "../common.h"

/** Exits the game process with a return code. */
NORETURN extern void SysExit(i32 exitcode);

/** Overwrites the return exit code of the game process 
 *  this code will be returned when SysExit() is called. */
extern void SysExitcode(i32 exitcode);

/** Puts current thread to sleep. */
extern void SysSleep(i32 ns);

/** Loads a shared library into memory, returns an opaque handle. */
extern void *SysLoadDLL(const char *libname);

/** Closes the loaded shared library. */
extern void SysCloseDLL(void *handle);

/** Returns a memory address for a DLL exported function. */
extern void *SysGetProcAddress(void *handle, const char *procname);

/** Get the current value of the high resolution counter. */
extern u64 SysTimerCounter(void);

/** Get the count per second of the high resolution counter. */
extern u64 SysTimerFrequency(void);

/** Reads information about the cpu, saves them to the arguments. */
extern void SysCpuCount(i32 *threads, i32 *cores, i32 *packages);

#endif /* _AMW_system_h_ */
