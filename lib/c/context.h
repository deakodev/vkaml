#ifndef CONTEXT_H
#define CONTEXT_H

#include "arena.h"
#include "vulkan/structs.h"

typedef struct vkaml_context {
    vkaml_arena internal_arena;
    vkaml_instance instance;
} vkaml_context;

vkaml_context* vkaml_context_allocate(vkaml_arena* arena)
{
    size_t totalSizeBytes   = sizeof(vkaml_context);
    uintptr_t memoryAddress = (uintptr_t)arena->memory;

    uintptr_t nextAllocOffset = (memoryAddress % 64); // cache aligned
    if (nextAllocOffset + totalSizeBytes > arena->capacity)
    {
        return NULL;
    }
    arena->nextAllocation = nextAllocOffset + totalSizeBytes;
    return (vkaml_context*)(memoryAddress + nextAllocOffset);
}

#endif // CONTEXT_H