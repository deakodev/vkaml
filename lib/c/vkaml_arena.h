#ifndef VKAML_ARENA_H
#define VKAML_ARENA_H

#include <stddef.h>
#include <stdint.h>

typedef struct vkaml_arena {
    uintptr_t nextAllocation;
    size_t capacity;
    char* memory;
} vkaml_arena;

vkaml_arena* vkaml_arena_create(size_t capacity);

void vkaml_arena_destroy(vkaml_arena* arena);

#endif // VKAML_ARENA_H