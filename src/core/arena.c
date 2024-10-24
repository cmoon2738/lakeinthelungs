#include "memory.h"

#include <stdio.h>

#if MEMORY_ARENA_BACKEND == MEMORY_ARENA_BACKEND_LIBC_MALLOC
Slice *new_slice(size_t capacity)
{
    size_t bytes = sizeof(Slice) + sizeof(uintptr_t) * capacity;
    Slice *slice = (Slice *)malloc(bytes);
    assert_debug(slice);
    slice->next = NULL;
    slice->count = 0;
    slice->capacity = capacity;
    return slice;
}

void free_slice(Slice *slice)
{
    free(slice);
}

#else
#error "Arena memory backend not recognized"
#endif /* MEMORY_ARENA_BACKEND */

void *arena_alloc(Arena *a, size_t bytes)
{
    size_t size = (bytes + sizeof(uintptr_t) - 1) / sizeof(uintptr_t);
    if (a->end == NULL) {
        assert_debug(a->begin == NULL);
        size_t capacity = MEMORY_DEFAULT_SLICE_CAPACITY;
        if (capacity < size) 
            capacity = size;
        a->end = new_slice(capacity);
        a->begin = a->end;
    }

    while (a->end->count + size > a->end->capacity && 
           a->end->next != NULL) 
    {
        a->end = a->end->next;
    }

    if (a->end->count + size > a->end->capacity) {
        assert_debug(a->end->next == NULL);
        size_t capacity = MEMORY_DEFAULT_SLICE_CAPACITY;
        if (capacity < size)
            capacity = size;
        a->end->next = new_slice(capacity);
        a->end = a->end->next;
    }

    void *res = &a->end->data[a->end->count];
    a->end->count += size;
    return res;
}

void *arena_realloc(Arena *a, void *old_ptr, size_t old_size, size_t new_size)
{
    if (new_size <= old_size)
        return old_ptr;

    void *new_ptr = arena_alloc(a, new_size);
    char *new_ptr_char = (char *)new_ptr;
    char *old_ptr_char = (char *)old_ptr;
    for (size_t i = 0; i < old_size; ++i) {
        new_ptr_char[i] = old_ptr_char[i];
    }
    return new_ptr;
}

void *arena_memdup(Arena *a, void *data, size_t bytes)
{
    return memcpy(arena_alloc(a, bytes), data, bytes);
}

char *arena_strdup(Arena *a, const char *str) {
    size_t n = strlen(str);
    char *dup = (char *)arena_alloc(a, n + 1);
    memcpy(dup, str, n);
    dup[n] = '\0';
    return dup;
}

char *arena_sprintf(Arena *a, const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    int32_t n = vsnprintf(NULL, 0, fmt, args);
    va_end(args);

    assert_debug(n >= 0);
    char *res = (char *)arena_alloc(a, n + 1);
    va_start(args, fmt);
    vsnprintf(res, n + 1, fmt, args);
    va_end(args);
    return res;
}

void arena_reset(Arena *a)
{
    for (Slice *slice = a->begin; slice != NULL; slice = slice->next) {
        slice->count = 0;
    }
    a->end = a->begin;
}

void arena_free(Arena *a)
{
    Slice *s = a->begin;
    while (s) {
        Slice *s0 = s;
        s = s->next;
        free_slice(s0);
    }
    a->begin = NULL;
    a->end = NULL;
}
