#include "vulkan_base.h"

#include "vkaml_types.h"
#include "vkaml_backend.h"

#include <echo.h>

#include <vulkan/vulkan.h>

static uint32_t vkaml_api(vkaml_api_version api_version)
{
    switch (api_version)
    {
    case VKAML_API_VERSION_1_0: return VK_API_VERSION_1_0;
    case VKAML_API_VERSION_1_1: return VK_API_VERSION_1_1;
    case VKAML_API_VERSION_1_2: return VK_API_VERSION_1_2;
    case VKAML_API_VERSION_1_3: return VK_API_VERSION_1_3;
    default: echo_error("Invalid Vulkan API version"); return VK_API_VERSION_1_0; // Fallback
    }
}

VkInstance vk_instance_init(vkaml_desc* desc)
{
    VkApplicationInfo appInfo  = { .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO };
    appInfo.pApplicationName   = desc->app_name;
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName        = "Vkaml Renderer";
    appInfo.engineVersion      = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion         = vkaml_api(desc->api_version);

    VkInstanceCreateInfo createInfo = { .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO };
    createInfo.pNext                = NULL;
    createInfo.flags                = 0;
    createInfo.pApplicationInfo     = &appInfo;
    createInfo.enabledLayerCount    = 0;
    createInfo.ppEnabledLayerNames  = NULL;


    createInfo.enabledExtensionCount   = 0;
    createInfo.ppEnabledExtensionNames = NULL;

    VkInstance instance;
    VK_CHECK(vkCreateInstance(&createInfo, NULL, &instance));

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