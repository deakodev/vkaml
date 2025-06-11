#ifndef VKAML_BACKEND_H
#define VKAML_BACKEND_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>


// Platform detection
#if defined(__linux__)
#define VKAML_LINUX
#else
#error "Unsupported platform!"
#endif


// Versioning macros
#define VKAML_MAKE_VERSION_SELECT(_1, _2, _3, MAKE_VERSION_XARG, ...) MAKE_VERSION_XARG
#define VKAML_MAKE_VERSION_1ARG(n) VK_MAKE_VERSION(1, n, 0)
#define VKAML_MAKE_VERSION_3ARG(m, n, p) VK_MAKE_VERSION(m, n, p)

#define VKAML_MAKE_VERSION(...) \
    VKAML_MAKE_VERSION_SELECT(__VA_ARGS__, VKAML_MAKE_VERSION_3ARG, _, VKAML_MAKE_VERSION_1ARG)(__VA_ARGS__)

// Maximum number of Vulkan validation layers
#define VKAML_MAX_VALIDATION_LAYERS 16
// Maximum number of Vulkan instance extensions
#define VKAML_MAX_INSTANCE_EXTENSIONS 64
// Maximum number of Vulkan physical devices
#define VKAML_MAX_PHYSICAL_DEVICES 8

typedef struct Vkaml_arena {
    uintptr_t next_allocation;
    size_t capacity;
    char* memory;
} Vkaml_arena;

Vkaml_arena* vkaml_arena_create(size_t capacity);
void vkaml_arena_destroy(Vkaml_arena* arena);

void* vkaml_array_alloc(Vkaml_arena* arena, uint32_t capacity, size_t element_size);

// Array definition macros
#define VKAML_ARRAY_DEFINE_FUNCTIONS(type, array)                                                                             \
                                                                                                                              \
    static array array##_alloc(Vkaml_arena* arena, uint32_t capacity)                                                         \
    {                                                                                                                         \
        return (array){ .capacity = capacity, .length = 0, .data = (type*)vkaml_array_alloc(arena, capacity, sizeof(type)) }; \
    }

#define VKAML_ARRAY_DEFINE(type, array) \
    typedef struct {                    \
        uint32_t length;                \
        uint32_t capacity;              \
        type* data;                     \
    } array;                            \
                                        \
    VKAML_ARRAY_DEFINE_FUNCTIONS(type, array)

typedef struct Vkaml_backend_desc {

    const char* window_title;
    int window_width;
    int window_height;

    const char* app_name;
    uint32_t api_version;
    bool enable_validation;
    uint32_t validation_layer_count;
    const char** validation_layer_names;
    uint32_t instance_extension_count;
    const char** instance_extension_names;
    bool enable_instance_flag;

} Vkaml_backend_desc;

typedef struct Vkaml_window {
    GLFWwindow* window;
} Vkaml_window;

typedef struct Vkaml_base {
    VkInstance instance;
    VkDebugUtilsMessengerEXT debug_messenger;
    VkSurfaceKHR surface;
    VkPhysicalDevice physical_device;
} Vkaml_base;

VKAML_ARRAY_DEFINE(VkSurfaceFormatKHR, Vkaml_surface_formats_array);
VKAML_ARRAY_DEFINE(VkPresentModeKHR, Vkaml_present_modes_array);
VKAML_ARRAY_DEFINE(VkQueueFamilyProperties, Vkaml_queue_families_array);

typedef struct Vkaml_physical_device {
    VkPhysicalDevice device;
    VkPhysicalDeviceProperties properties;
    VkPhysicalDeviceFeatures features;
    VkSurfaceCapabilitiesKHR surface_capabilities;
    Vkaml_surface_formats_array surface_formats;
    Vkaml_present_modes_array present_modes;
    Vkaml_queue_families_array queue_families;
    int32_t graphics_family_index;
    int32_t present_family_index;
} Vkaml_physical_device;

VKAML_ARRAY_DEFINE(Vkaml_physical_device, Vkaml_physical_device_array);

typedef struct Vkaml_backend {
    Vkaml_arena internal_arena;
    Vkaml_window window;
    Vkaml_base base;

    Vkaml_physical_device_array physical_devices;
} Vkaml_backend;

Vkaml_backend* vkaml_init(Vkaml_backend_desc* desc);
void vkaml_cleanup(Vkaml_backend* vkaml);

Vkaml_backend* vkaml_backend_alloc(Vkaml_arena* arena);

#endif // VKAML_BACKEND_H