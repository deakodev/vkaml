#ifndef UTILS_H
#define UTILS_H

#include "echo.h"
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>
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

// Returns a human-readable string for a VkResult
const char* vk_error_string(VkResult error);

// Macro to check Vulkan result
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


#endif // UTILS_H