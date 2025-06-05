#include "vkaml_backend.h"

#include "vkaml_arena.h"

#include "vulkan_base.h"

#include <echo.h>

vkaml_backend* vkaml_context_allocate(vkaml_arena* arena)
{
    size_t totalSizeBytes   = sizeof(vkaml_backend);
    uintptr_t memoryAddress = (uintptr_t)arena->memory;

    uintptr_t nextAllocOffset = (memoryAddress % 64); // cache aligned
    if (nextAllocOffset + totalSizeBytes > arena->capacity)
    {
        return NULL;
    }
    arena->nextAllocation = nextAllocOffset + totalSizeBytes;
    return (vkaml_backend*)(memoryAddress + nextAllocOffset);
}

vkaml_backend* vkaml_init(vkaml_desc* desc)
{
    vkaml_arena* arena = vkaml_arena_create(sizeof(vkaml_backend) + 1024);
    if (!arena)
    {
        echo_fatal("vkaml_arena_create failed");
        return NULL;
    }

    vkaml_backend* vkaml = vkaml_context_allocate(arena);
    if (vkaml == NULL)
    {
        echo_fatal("vkaml_context_allocate failed");
        vkaml_arena_destroy(arena);
        return NULL;
    }

    *vkaml = (vkaml_backend){
        .internal_arena = *arena,
        .instance       = (vkaml_instance){ .instance = vkaml_instance_init(desc) },
    };

    return vkaml;
}