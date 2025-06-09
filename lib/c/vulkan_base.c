#include "vulkan_base.h"

#include "vkaml_backend.h"

#include <echo.h>

#include <assert.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>

#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>

static VKAPI_ATTR VkBool32 VKAPI_CALL debug_message_callback(VkDebugUtilsMessageSeverityFlagBitsEXT severity,
VkDebugUtilsMessageTypeFlagsEXT type,
const VkDebugUtilsMessengerCallbackDataEXT* callback_data,
void* user_data)
{
    const char* type_string;
    switch (type)
    {
    case VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT: type_string = "VALIDATION"; break;
    case VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT: type_string = "PERFORMANCE"; break;
    case VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT:
    default: return VK_FALSE;
    }

    void (*echo_fn)(const char* fmt, ...);
    switch (severity)
    {
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT: echo_fn = echo_error; break;
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT: echo_fn = echo_warn; break;
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT: echo_fn = echo_info; break;
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
    default: return VK_FALSE;
    }

    echo_fn("[VK %s]:\n%s", type_string, callback_data->pMessage);

    return VK_FALSE;
}

VkInstance vulkan_instance_init(Vkaml_backend_desc* desc)
{
    VkApplicationInfo app_info  = { .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO };
    app_info.pApplicationName   = desc->app_name;
    app_info.applicationVersion = VKAML_MAKE_VERSION(1, 0, 0);
    app_info.pEngineName        = "Vkaml Renderer";
    app_info.engineVersion      = VKAML_MAKE_VERSION(1, 0, 0);
    app_info.apiVersion         = desc->api_version;

    VkDebugUtilsMessengerCreateInfoEXT debug_create_info = { .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
        .pNext = NULL,
        .messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT,
        .messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
        .pfnUserCallback = debug_message_callback,
        .pUserData       = NULL };

    VkInstanceCreateInfo create_info = { .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO };
    create_info.pNext                = (VkDebugUtilsMessengerCreateInfoEXT*)&debug_create_info;
    create_info.flags |= desc->enable_instance_flag ? VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR : 0;
    create_info.pApplicationInfo        = &app_info;
    create_info.enabledLayerCount       = desc->enable_validation ? desc->validation_layer_count : 0;
    create_info.ppEnabledLayerNames     = desc->enable_validation ? desc->validation_layer_names : NULL;
    create_info.enabledExtensionCount   = desc->instance_extension_count;
    create_info.ppEnabledExtensionNames = desc->instance_extension_names;

    if (desc->enable_validation)
    {
        echo_trace("Enabled validation layers:");
        for (uint32_t i = 0; i < desc->validation_layer_count; ++i)
        {
            printf("%s\n", desc->validation_layer_names[i]);
        }
    }

    echo_trace("Enabled instance extensions:");
    for (uint32_t i = 0; i < desc->instance_extension_count; ++i)
    {
        printf("%s\n", desc->instance_extension_names[i]);
    }

    VkInstance instance;
    VK_CHECK(vkCreateInstance(&create_info, NULL, &instance));

    return instance;
}

VkDebugUtilsMessengerEXT vulkan_debugger_init(VkInstance instance)
{
    VkDebugUtilsMessengerCreateInfoEXT debug_create_info = { .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
        .pNext = NULL,
        .messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT,
        .messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
        .pfnUserCallback = debug_message_callback,
        .pUserData       = NULL };

    PFN_vkCreateDebugUtilsMessengerEXT vkCreateDebugUtilsMessengerEXT =
    (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");

    if (vkCreateDebugUtilsMessengerEXT != NULL)
    {
        VkDebugUtilsMessengerEXT debug_messenger;
        VkResult result = vkCreateDebugUtilsMessengerEXT(instance, &debug_create_info, NULL, &debug_messenger);
        if (result != VK_SUCCESS)
        {
            echo_error("Failed to create debug messenger: %d", result);
        }

        return debug_messenger;
    }

    return VK_NULL_HANDLE;
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