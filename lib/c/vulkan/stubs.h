#ifndef VULKAN_STUBS_H
#define VULKAN_STUBS_H

#include "structs.h"

#include <vulkan/vulkan.h>

CAML_WRAP_HANDLE_OPT(vkaml_context, vkaml_context*)

static VkInstance vkaml_instance_init(void)
{
    VkApplicationInfo appInfo = { .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO };
    appInfo.pApplicationName  = "deako_app";
    appInfo.pEngineName       = "deako_engine";
    appInfo.apiVersion        = VK_API_VERSION_1_0;

    VkInstanceCreateInfo createInfo = { .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO };
    createInfo.pApplicationInfo     = &appInfo;

    VkInstance instance;
    VK_CHECK(vkCreateInstance(&createInfo, NULL, &instance));

    return instance;
}


CAMLprim value ml_vkaml_init(value unit)
{
    CAMLparam1(unit);

    vkaml_arena* arena = vkaml_arena_create(1024 * 1024);
    if (!arena)
    {
        caml_failwith("vkaml_arena_create failed");
    }

    vkaml_context* context = vkaml_context_allocate(arena);
    if (context == NULL)
    {
        caml_failwith("vkaml_context_allocate failed");
    }

    *context = (vkaml_context){
        .internal_arena = *arena,
        .instance       = (vkaml_instance){ .instance = vkaml_instance_init() },
    };

    CAMLreturn(Val_vkaml_context(context));
}

#endif // VULKAN_STUBS_H