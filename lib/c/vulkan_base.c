#include "vulkan_base.h"

#include "vkaml_backend.h"

#include <echo.h>

#include <assert.h>
#include <stdbool.h>
#include <string.h>

#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>

VkInstance vkaml_instance_init(Vkaml_backend_desc* desc)
{
    VkApplicationInfo app_info  = { .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO };
    app_info.pNext              = NULL;
    app_info.pApplicationName   = desc->app_name;
    app_info.applicationVersion = VKAML_MAKE_VERSION(1, 0, 0);
    app_info.pEngineName        = "Vkaml Renderer";
    app_info.engineVersion      = VKAML_MAKE_VERSION(1, 0, 0);
    app_info.apiVersion         = desc->api_version;

    VkInstanceCreateInfo create_info = { .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO };
    create_info.pNext                = NULL;
    create_info.flags                = 0;
    create_info.pApplicationInfo     = &app_info;
    create_info.enabledLayerCount    = 0;
    create_info.ppEnabledLayerNames  = NULL;

    echo_trace("instance extension count: %d", desc->instance_extension_count);
    for (uint32_t i = 0; i < desc->instance_extension_count; ++i)
    {
        echo_trace("instance extension %d: %s", i, desc->instance_extension_names[i]);
    }

    create_info.enabledExtensionCount   = desc->instance_extension_count;
    create_info.ppEnabledExtensionNames = desc->instance_extension_names;

    VkInstance instance;
    VK_CHECK(vkCreateInstance(&create_info, NULL, &instance));

    return instance;
}

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