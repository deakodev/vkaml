#include "vulkan_helpers.h"

#include "vkaml_backend.h"

#include <echo.h>

#include <stdint.h>

#include <vulkan/vulkan.h>

uint32_t vulkan_available_instance_extension_names(const char** out_names)
{
    uint32_t count = 0;
    if (vkEnumerateInstanceExtensionProperties(NULL, &count, NULL) != VK_SUCCESS)
    {
        return 0;
    }

    if (count > VKAML_MAX_INSTANCE_EXTENSIONS)
    {
        echo_error("Too many Vulkan instance extensions available: %u (max: %d)", count, VKAML_MAX_INSTANCE_EXTENSIONS);
        return 0;
    }

    VkExtensionProperties props[count];
    if (vkEnumerateInstanceExtensionProperties(NULL, &count, props) != VK_SUCCESS)
    {
        return 0;
    }

    for (uint32_t i = 0; i < count; ++i)
    {
        out_names[i] = props[i].extensionName;
    }

    return count;
}