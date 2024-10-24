#ifndef _AMW_memory_h_
#define _AMW_memory_h_

#include "../common.h"

/** A block of memory */
typedef struct Slice Slice;

struct Slice {
    Slice    *next;
    size_t    count;
    size_t    capacity;
    uintptr_t data[];
};

#define MEMORY_DEFAULT_SLICE_CAPACITY (8 * 1024)

/** An arena is a pool allocator used for allocating collections
 *  of data that share the same lifetime. It allows for multiple
 *  allocations to be freed all at once. */
typedef struct Arena {
    Slice *begin;
    Slice *end;
} Arena;

#define MEMORY_ARENA_BACKEND_LIBC_MALLOC 0
/* TODO possible backends:
 * - Linux mmap 
 * - Win32 virtualalloc
 * - WebAssembly heapbase */

#ifndef MEMORY_ARENA_BACKEND
    #define MEMORY_ARENA_BACKEND MEMORY_ARENA_BACKEND_LIBC_MALLOC
#endif

extern Slice *new_slice(size_t capacity);
extern void   free_slice(Slice *slice);

extern void *arena_alloc(Arena *a, size_t bytes);
extern void *arena_realloc(Arena *a, void *old_ptr, size_t old_size, size_t new_size);
extern void *arena_memdup(Arena *a, void *data, size_t bytes);
extern char *arena_strdup(Arena *a, const char *str);
extern char *arena_sprintf(Arena *a, const char *fmt, ...) PRINTF_FORMAT(2);

extern void arena_reset(Arena *a);
extern void arena_free(Arena *a);

#endif /* _AMW_memory_h_ */
