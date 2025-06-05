#ifndef VKAML_TYPES_H
#define VKAML_TYPES_H

#include "vkaml_arena.h"

#include <vulkan/vulkan.h>

typedef struct vkaml_window_desc {
    const char* title;
    int width;
    int height;
} vkaml_window_desc;

typedef enum vkaml_api_version {
    VKAML_API_VERSION_1_0,
    VKAML_API_VERSION_1_1,
    VKAML_API_VERSION_1_2,
    VKAML_API_VERSION_1_3,
} vkaml_api_version;

typedef struct {
    const char* app_name;
    uint32_t api_version;
} vkaml_desc;

typedef struct vkaml_instance {
    VkInstance instance;
} vkaml_instance;

typedef struct vkaml_backend {
    vkaml_arena internal_arena;
    vkaml_instance instance;
} vkaml_backend;

#endif // VKAML_TYPES_H