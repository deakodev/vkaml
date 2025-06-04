#ifndef VULKAN_STUBS_H
#define VULKAN_STUBS_H

#include "structs.h"

#include <string.h>

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

typedef struct {
    const char* app_name;
    uint32_t app_version;
    const char* engine_name;
    uint32_t engine_version;
    uint32_t api_version;
    const char** enabled_extensions;
    uint32_t enabled_extension_count;
    const char** enabled_layers;
    uint32_t enabled_layer_count;
} vkaml_desc;

uint32_t caml_string_array_len(value arr)
{
    uint32_t len = 0;
    while (arr != Val_emptylist)
    {
        len++;
        arr = Field(arr, 1);
    }
    return len;
}

const char** vkaml_copy_string_array(value arr, uint32_t* out_len)
{
    uint32_t len = 0;
    value orig   = arr;
    while (arr != Val_emptylist)
    {
        len++;
        arr = Field(arr, 1);
    }

    const char** result = malloc(sizeof(char*) * len);
    if (!result)
        return NULL;

    arr = orig;
    for (uint32_t i = 0; arr != Val_emptylist; i++, arr = Field(arr, 1))
    {
        value item = Field(arr, 0); // head of list
        result[i]  = strdup(String_val(item));
    }

    *out_len = len;
    return result;
}


CAMLprim value ml_vkaml_init(value ml_desc, value unit)
{
    CAMLparam2(ml_desc, unit);

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

    vkaml_desc desc = (vkaml_desc){
        .app_name                = "Vkaml App",
        .app_version             = 1,
        .engine_name             = "Vkaml Engine",
        .engine_version          = 1,
        .api_version             = VK_API_VERSION_1_0,
        .enabled_extensions      = NULL,
        .enabled_extension_count = 0,
        .enabled_layers          = NULL,
        .enabled_layer_count     = 0,
    };

    if (Is_some(ml_desc))
    {
        value v_desc = Some_val(ml_desc);

        value v_app_name_opt       = Field(v_desc, 0);
        value v_app_version_opt    = Field(v_desc, 1);
        value v_engine_name_opt    = Field(v_desc, 2);
        value v_engine_version_opt = Field(v_desc, 3);
        value v_api_version_opt    = Field(v_desc, 4);
        value v_enabled_exts_opt   = Field(v_desc, 5);
        value v_enabled_layers_opt = Field(v_desc, 6);

        if (Is_some(v_app_name_opt))
        {
            desc.app_name = strdup(String_val(Some_val(v_app_name_opt)));
        }

        if (Is_some(v_app_version_opt))
        {
            desc.app_version = Int_val(Some_val(v_app_version_opt));
        }

        if (Is_some(v_engine_name_opt))
        {
            desc.engine_name = strdup(String_val(Some_val(v_engine_name_opt)));
        }

        if (Is_some(v_engine_version_opt))
        {
            desc.engine_version = Int_val(Some_val(v_engine_version_opt));
        }

        if (Is_some(v_api_version_opt))
        {
            desc.api_version = Int_val(Some_val(v_api_version_opt));
        }

        if (Is_some(v_enabled_exts_opt))
        {
            value exts                   = Some_val(v_enabled_exts_opt);
            desc.enabled_extension_count = caml_string_array_len(exts);
            desc.enabled_extensions      = vkaml_copy_string_array(exts, &desc.enabled_extension_count);
        }

        if (Is_some(v_enabled_layers_opt))
        {
            value layers             = Some_val(v_enabled_layers_opt);
            desc.enabled_layer_count = caml_string_array_len(layers);
            desc.enabled_layers      = vkaml_copy_string_array(layers, &desc.enabled_layer_count);
        }
    }


    *context = (vkaml_context){
        .internal_arena = *arena,
        .instance       = (vkaml_instance){ .instance = vkaml_instance_init() },
    };

    CAMLreturn(Val_vkaml_context(context));
}

#endif // VULKAN_STUBS_H