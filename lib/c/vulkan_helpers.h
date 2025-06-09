#ifndef VULKAN_HELPERS_H
#define VULKAN_HELPERS_H

#include <stdint.h>

uint32_t vulkan_available_validation_layers(const char** out_names);
uint32_t vulkan_available_instance_extensions(const char** out_names);

#endif // VULKAN_HELPERS_H