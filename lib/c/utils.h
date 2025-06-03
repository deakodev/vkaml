#ifndef UTILS_H
#define UTILS_H

#include "echo.h"

#include <stdint.h>
#include <stdbool.h>
#include <assert.h>

#include <caml/custom.h>

#include <vulkan/vulkan.h>

#define CAML_WRAP_HANDLE_OPT(type, ptr)                         \
    static inline value Val_##type(ptr p)                       \
    {                                                           \
        if (p == NULL)                                          \
            return Val_none;                                    \
        return caml_alloc_some(caml_copy_nativeint((intnat)p)); \
    }                                                           \
    static inline ptr type##_val(value v)                       \
    {                                                           \
        return (ptr)Nativeint_val(Field(v, 0));                 \
    }

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

const char* vk_error_string(VkResult error)
{
    switch (error)
    {
    case VK_SUCCESS: return "VK_SUCCESS";
    case VK_NOT_READY: return "VK_NOT_READY";
    case VK_TIMEOUT: return "VK_TIMEOUT";
    case VK_EVENT_SET: return "VK_EVENT_SET";
    case VK_EVENT_RESET: return "VK_EVENT_RESET";
    case VK_INCOMPLETE: return "VK_INCOMPLETE";
    case VK_ERROR_OUT_OF_HOST_MEMORY: return "VK_ERROR_OUT_OF_HOST_MEMORY";
    case VK_ERROR_OUT_OF_DEVICE_MEMORY: return "VK_ERROR_OUT_OF_DEVICE_MEMORY";
    case VK_ERROR_INITIALIZATION_FAILED: return "VK_ERROR_INITIALIZATION_FAILED";
    case VK_ERROR_DEVICE_LOST: return "VK_ERROR_DEVICE_LOST";
    case VK_ERROR_MEMORY_MAP_FAILED: return "VK_ERROR_MEMORY_MAP_FAILED";
    case VK_ERROR_LAYER_NOT_PRESENT: return "VK_ERROR_LAYER_NOT_PRESENT";
    case VK_ERROR_EXTENSION_NOT_PRESENT: return "VK_ERROR_EXTENSION_NOT_PRESENT";
    case VK_ERROR_FEATURE_NOT_PRESENT: return "VK_ERROR_FEATURE_NOT_PRESENT";
    case VK_ERROR_INCOMPATIBLE_DRIVER: return "VK_ERROR_INCOMPATIBLE_DRIVER";
    case VK_ERROR_TOO_MANY_OBJECTS: return "VK_ERROR_TOO_MANY_OBJECTS";
    case VK_ERROR_FORMAT_NOT_SUPPORTED: return "VK_ERROR_FORMAT_NOT_SUPPORTED";
    case VK_ERROR_FRAGMENTED_POOL: return "VK_ERROR_FRAGMENTED_POOL";
    default: return "VK_ERROR_UNKNOWN";
    }
}

#endif // UTILS_H