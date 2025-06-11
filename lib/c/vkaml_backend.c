#include "vkaml_backend.h"

#include "vulkan_base.h"

#include <echo.h>

#include <stddef.h>
#include <stdint.h>
#include <malloc.h>

#include <GLFW/glfw3.h>

void vkaml_window_create(Vkaml_backend* vkaml, Vkaml_backend_desc* desc)
{
    assert(vkaml != NULL);

    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    echo_info("Creating GLFW window: %s (%d x %d)", desc->window_title, desc->window_width, desc->window_height);
    GLFWwindow* window = glfwCreateWindow(desc->window_width, desc->window_height, desc->window_title, NULL, NULL);

    vkaml->window.window = window;
}

Vkaml_arena* vkaml_arena_create(size_t capacity)
{
    char* memory = (char*)malloc(capacity);
    if (!memory)
        return NULL;

    Vkaml_arena* arena = (Vkaml_arena*)malloc(sizeof(Vkaml_arena));
    if (!arena)
    {
        free(memory);
        return NULL;
    }

    *arena = (Vkaml_arena){ .next_allocation = 0, .capacity = capacity, .memory = memory };

    return arena;
}

void vkaml_arena_destroy(Vkaml_arena* arena)
{
    if (arena)
    {
        free(arena->memory);
        free(arena);
    }
}

void* vkaml_array_alloc(Vkaml_arena* arena, uint32_t capacity, size_t element_size)
{
    size_t total_size           = capacity * element_size;
    uintptr_t next_alloc_offset = arena->next_allocation + (64 - (arena->next_allocation % 64)); // cache aligned
    if (next_alloc_offset + total_size <= arena->capacity)
    {
        arena->next_allocation = next_alloc_offset + total_size;
        return (void*)((uintptr_t)arena->memory + next_alloc_offset);
    }

    echo_error("vkaml_array_alloc: Not enough memory in arena");
    return NULL;
}

Vkaml_backend* vkaml_backend_alloc(Vkaml_arena* arena)
{
    size_t totalSizeBytes   = sizeof(Vkaml_backend);
    uintptr_t memoryAddress = (uintptr_t)arena->memory;

    uintptr_t nextAllocOffset = (memoryAddress % 64); // cache aligned
    if (nextAllocOffset + totalSizeBytes > arena->capacity)
    {
        return NULL;
    }
    arena->next_allocation = nextAllocOffset + totalSizeBytes;
    return (Vkaml_backend*)(memoryAddress + nextAllocOffset);
}

Vkaml_backend* vkaml_init(Vkaml_backend_desc* desc)
{
    Vkaml_arena* arena = vkaml_arena_create(sizeof(Vkaml_backend) + 1024 * 1024);
    if (arena == NULL)
    {
        echo_fatal("vkaml_arena_create failed");
        return NULL;
    }

    Vkaml_backend* vkaml = vkaml_backend_alloc(arena);
    if (vkaml == NULL)
    {
        echo_fatal("vkaml_backend_alloc failed");
        return NULL;
    }

    vkaml->internal_arena = *arena;

    vkaml_window_create(vkaml, desc);
    vulkan_instance_setup(vkaml, desc);

    if (desc->enable_validation)
    {
        vulkan_debugger_setup(vkaml);
    }

    vulkan_surface_setup(vkaml);
    vulkan_physical_device_setup(vkaml);

    return vkaml;
}

void vkaml_cleanup(Vkaml_backend* vkaml)
{
    if (vkaml->base.surface)
        vkDestroySurfaceKHR(vkaml->base.instance, vkaml->base.surface, NULL);

    if (vkaml->base.debug_messenger)
    {
        PFN_vkDestroyDebugUtilsMessengerEXT vkDestroyDebugUtilsMessengerEXT =
        (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(vkaml->base.instance, "vkDestroyDebugUtilsMessengerEXT");

        if (vkDestroyDebugUtilsMessengerEXT)
        {
            vkDestroyDebugUtilsMessengerEXT(vkaml->base.instance, vkaml->base.debug_messenger, NULL);
        }
    }

    if (vkaml->base.instance)
        vkDestroyInstance(vkaml->base.instance, NULL);
}
