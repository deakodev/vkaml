#ifndef VULKAN_BASE_H
#define VULKAN_BASE_H

#include "vkaml_types.h"

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

VkInstance vkaml_instance_init(vkaml_desc* desc);

#endif // VULKAN_BASE_H