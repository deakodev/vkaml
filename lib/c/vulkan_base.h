#ifndef VULKAN_BASE_H
#define VULKAN_BASE_H

#include "vkaml_backend.h"

#include <assert.h>

#include <vulkan/vulkan.h>

const char* vk_error_string(VkResult error);

#define VK_CHECK(f)                                                                                        \
    do                                                                                                     \
    {                                                                                                      \
        VkResult res = (f);                                                                                \
        if (res != VK_SUCCESS)                                                                             \
        {                                                                                                  \
            echo_fatal("VkResult is \"%s\" in %s at line %d\n", vk_error_string(res), __FILE__, __LINE__); \
                                                                                                           \
            assert(res == VK_SUCCESS);                                                                     \
        }                                                                                                  \
    } while (0)

void vulkan_instance_setup(Vkaml_backend* vkaml, Vkaml_backend_desc* desc);
void vulkan_debugger_setup(Vkaml_backend* vkaml);
void vulkan_surface_setup(Vkaml_backend* vkaml);
void vulkan_physical_device_setup(Vkaml_backend* vkaml);

#endif // VULKAN_BASE_H