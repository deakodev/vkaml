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

static VkDebugUtilsMessengerCreateInfoEXT debug_create_info()
{
    VkDebugUtilsMessengerCreateInfoEXT create_info = { .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT };
    create_info.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT |
    VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT;
    create_info.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
    VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    create_info.pfnUserCallback = debug_message_callback;
    create_info.pUserData       = NULL;
    return create_info;
}

static VkPhysicalDevice physical_device_select(Vkaml_backend* vkaml)
{
    assert(vkaml != NULL);
    assert(vkaml->base.surface != VK_NULL_HANDLE);
    assert(vkaml->physical_devices.data != NULL);
    assert(vkaml->physical_devices.length > 0);

    VkSurfaceKHR surface                         = vkaml->base.surface;
    Vkaml_physical_device_array physical_devices = vkaml->physical_devices;

    VkPhysicalDevice top_candidate            = VK_NULL_HANDLE;
    Vkaml_physical_device* top_candidate_data = NULL;

    for (uint32_t i = 0; i < physical_devices.length; ++i)
    {
        Vkaml_physical_device* candidate_data = &physical_devices.data[i];
        VkPhysicalDevice candidate            = candidate_data->device;


        { // Gather candidate specs
            vkGetPhysicalDeviceProperties(candidate, &candidate_data->properties);
            vkGetPhysicalDeviceFeatures(candidate, &candidate_data->features);
            vkGetPhysicalDeviceSurfaceCapabilitiesKHR(candidate, surface, &candidate_data->surface_capabilities);

            uint32_t format_count;
            vkGetPhysicalDeviceSurfaceFormatsKHR(candidate, surface, &format_count, NULL);
            if (format_count != 0)
            {
                candidate_data->surface_formats = Vkaml_surface_formats_array_alloc(&vkaml->internal_arena, format_count);
                vkGetPhysicalDeviceSurfaceFormatsKHR(candidate, surface, &format_count, candidate_data->surface_formats.data);
                candidate_data->surface_formats.length = format_count;
            }

            uint32_t present_mode_count;
            vkGetPhysicalDeviceSurfacePresentModesKHR(candidate, surface, &present_mode_count, NULL);
            if (present_mode_count != 0)
            {
                candidate_data->present_modes = Vkaml_present_modes_array_alloc(&vkaml->internal_arena, present_mode_count);
                vkGetPhysicalDeviceSurfacePresentModesKHR(
                candidate, surface, &present_mode_count, candidate_data->present_modes.data);
                candidate_data->present_modes.length = present_mode_count;
            }

            uint32_t queue_family_count = 0;
            vkGetPhysicalDeviceQueueFamilyProperties(candidate, &queue_family_count, NULL);

            candidate_data->queue_families = Vkaml_queue_families_array_alloc(&vkaml->internal_arena, queue_family_count);
            vkGetPhysicalDeviceQueueFamilyProperties(candidate, &queue_family_count, candidate_data->queue_families.data);
            candidate_data->queue_families.length = queue_family_count;

            int32_t graphics_family_index = -1;
            int32_t present_family_index  = -1;
            for (uint32_t i = 0; i < candidate_data->queue_families.length; ++i)
            {
                VkQueueFamilyProperties* family = &candidate_data->queue_families.data[i];
                if (family->queueFlags & VK_QUEUE_GRAPHICS_BIT)
                {
                    graphics_family_index = i;
                }

                VkBool32 present_support = false;
                vkGetPhysicalDeviceSurfaceSupportKHR(candidate, i, surface, &present_support);
                if (present_support)
                {
                    present_family_index = i;
                }

                if (graphics_family_index != -1 && present_family_index != -1)
                {
                    break; // Found both graphics and present families
                }
            }

            candidate_data->graphics_family_index = graphics_family_index;
            candidate_data->present_family_index  = present_family_index;
        }

        { // Evaluate candidate specs
            if (candidate_data->graphics_family_index == -1 || candidate_data->present_family_index == -1)
            {
                continue; // Incomplete queue support
            }

            if (candidate_data->surface_formats.length == 0 || candidate_data->present_modes.length == 0)
            {
                continue; // Incomplete presentation support
            }

            // First viable candidate
            if (top_candidate == VK_NULL_HANDLE)
            {
                top_candidate      = candidate;
                top_candidate_data = candidate_data;
                continue;
            }

            // Prefer discrete GPU over integrated/virtual
            if (candidate_data->properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU &&
            top_candidate_data->properties.deviceType != VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
            {
                top_candidate      = candidate;
                top_candidate_data = candidate_data;
                continue;
            }

            // If both are discrete GPUs, prefer one with higher max image dimension
            if (candidate_data->properties.deviceType == top_candidate_data->properties.deviceType &&
            candidate_data->properties.limits.maxImageDimension2D > top_candidate_data->properties.limits.maxImageDimension2D)
            {
                top_candidate      = candidate;
                top_candidate_data = candidate_data;
                continue;
            }
        }
    }

    assert(top_candidate != VK_NULL_HANDLE);
    assert(top_candidate_data != NULL);

    vkaml->base.physical_device = top_candidate;

    VkPhysicalDeviceProperties* p = &top_candidate_data->properties;

    echo_trace("Selected physical device:");
    echo_trace("    Name      : %s", p->deviceName);
    echo_trace("    Vendor ID : 0x%04X", p->vendorID);
    echo_trace("    Device ID : 0x%04X", p->deviceID);
    echo_trace("    Type      : %s",
    p->deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU   ? "Discrete GPU" :
    p->deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU ? "Integrated GPU" :
    p->deviceType == VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU    ? "Virtual GPU" :
    p->deviceType == VK_PHYSICAL_DEVICE_TYPE_CPU            ? "CPU" :
                                                              "Other");
    echo_trace(
    "    API Ver   : %d.%d.%d", VK_VERSION_MAJOR(p->apiVersion), VK_VERSION_MINOR(p->apiVersion), VK_VERSION_PATCH(p->apiVersion));
    echo_trace("    Max 2D Image Size: %d x %d", p->limits.maxImageDimension2D, p->limits.maxImageDimension2D);
}

/// SETUP FUNCTIONS

void vulkan_instance_setup(Vkaml_backend* vkaml, Vkaml_backend_desc* desc)
{
    VkApplicationInfo app_info  = { .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO };
    app_info.pApplicationName   = desc->app_name;
    app_info.applicationVersion = VKAML_MAKE_VERSION(1, 0, 0);
    app_info.pEngineName        = "Vkaml Renderer";
    app_info.engineVersion      = VKAML_MAKE_VERSION(1, 0, 0);
    app_info.apiVersion         = desc->api_version;

    VkDebugUtilsMessengerCreateInfoEXT debug_info;
    if (desc->enable_validation)
    {
        debug_info = debug_create_info();
    }

    VkInstanceCreateInfo create_info    = { .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO };
    create_info.pNext                   = &debug_info;
    create_info.pApplicationInfo        = &app_info;
    create_info.enabledLayerCount       = desc->enable_validation ? desc->validation_layer_count : 0;
    create_info.ppEnabledLayerNames     = desc->enable_validation ? desc->validation_layer_names : NULL;
    create_info.enabledExtensionCount   = desc->instance_extension_count;
    create_info.ppEnabledExtensionNames = desc->instance_extension_names;
    create_info.flags |= desc->enable_instance_flag ? VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR : 0;

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

    vkaml->base.instance = instance;
}

void vulkan_debugger_setup(Vkaml_backend* vkaml)
{
    assert(vkaml != NULL);
    assert(vkaml->base.instance != VK_NULL_HANDLE);

    VkInstance instance = vkaml->base.instance;
    VkDebugUtilsMessengerEXT debug_messenger;

    VkDebugUtilsMessengerCreateInfoEXT create_info = debug_create_info();

    PFN_vkCreateDebugUtilsMessengerEXT vkCreateDebugUtilsMessengerEXT =
    (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");

    if (vkCreateDebugUtilsMessengerEXT != NULL)
    {
        VK_CHECK(vkCreateDebugUtilsMessengerEXT(instance, &create_info, NULL, &debug_messenger));
    }

    vkaml->base.debug_messenger = debug_messenger;
}

void vulkan_surface_setup(Vkaml_backend* vkaml)
{
    assert(vkaml != NULL);
    assert(vkaml->base.instance != VK_NULL_HANDLE);
    assert(vkaml->window.window != NULL);

    VkSurfaceKHR surface;
    VK_CHECK(glfwCreateWindowSurface(vkaml->base.instance, vkaml->window.window, NULL, &surface));

    vkaml->base.surface = surface;
}

void vulkan_physical_device_setup(Vkaml_backend* vkaml)
{
    assert(vkaml != NULL);
    assert(vkaml->base.instance != VK_NULL_HANDLE);
    assert(vkaml->base.surface != VK_NULL_HANDLE);

    VkInstance instance  = vkaml->base.instance;
    VkSurfaceKHR surface = vkaml->base.surface;

    uint32_t physical_device_count = 0;
    VK_CHECK(vkEnumeratePhysicalDevices(instance, &physical_device_count, NULL));

    VkPhysicalDevice raw_physical_devices[VKAML_MAX_PHYSICAL_DEVICES];
    assert(physical_device_count <= VKAML_MAX_PHYSICAL_DEVICES);

    VK_CHECK(vkEnumeratePhysicalDevices(instance, &physical_device_count, raw_physical_devices));

    vkaml->physical_devices        = Vkaml_physical_device_array_alloc(&vkaml->internal_arena, physical_device_count);
    vkaml->physical_devices.length = physical_device_count;

    for (uint32_t i = 0; i < vkaml->physical_devices.length; ++i)
    {
        vkaml->physical_devices.data[i].device = raw_physical_devices[i];
    }

    physical_device_select(vkaml);
}

// void vulkan_device_init(VkInstance instance)
// {
//     uint32_t physical_device_count = 0;
//     vkEnumeratePhysicalDevices(instance, &physical_device_count, NULL);

//     VkPhysicalDevice* physical_devices = malloc(physical_device_count * sizeof(VkPhysicalDevice));
//     VK_CHECK(vkEnumeratePhysicalDevices(instance, &physical_device_count, physical_devices));

//     // Determine the best physical device later
//     VkPhysicalDevice physical_device = physical_devices[0]; // For simplicity right now

//     uint32_t queue_family_count = 0;
//     vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queue_family_count, NULL);

//     std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
//     vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilies.data());

//     int i = 0;
//     for (const auto& queueFamily : queueFamilies) // find queue families that are supported
//     {
//         if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
//             vb.graphicsFamily = i;

//         VkBool32 presentSupport = false;
//         vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, vb.surface, &presentSupport);

//         if (presentSupport)
//             vb.presentFamily = i;
//         if (vb.graphicsFamily.has_value() && vb.presentFamily.has_value())
//             break;

//         i++;
//     }
// }

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