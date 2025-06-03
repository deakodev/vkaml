#ifndef ARENA_H
#define ARENA_H

#include <stddef.h>
#include <stdint.h>
#include <malloc.h>

typedef struct vkaml_arena {
    uintptr_t nextAllocation;
    size_t capacity;
    char* memory;
} vkaml_arena;

vkaml_arena* vkaml_arena_create(size_t capacity)
{
    char* memory = (char*)malloc(capacity);
    if (!memory)
        return NULL;

    vkaml_arena* arena = (vkaml_arena*)malloc(sizeof(vkaml_arena));
    if (!arena)
    {
        free(memory);
        return NULL;
    }

    *arena = (vkaml_arena){ .nextAllocation = 0, .capacity = capacity, .memory = memory };

    return arena;
}

void vkaml_arena_destroy(vkaml_arena* arena)
{
    if (arena)
    {
        free(arena->memory);
        free(arena);
    }
}

#endif // ARENA_H