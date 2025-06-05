#include "vkaml_arena.h"

#include <stddef.h>
#include <stdint.h>
#include <malloc.h>

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
