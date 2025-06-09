#include "vkaml_stubs.h"

#include "vkaml_backend.h"
#include "vulkan_helpers.h"

#include <echo.h>
#include <caml/alloc.h>
#include <caml/memory.h>
#include <caml/mlvalues.h>
#include <caml/fail.h>

#include <GLFW/glfw3.h>

static bool _vkaml_initialized = false;

#define VKAML_REQUIRE_INIT()                    \
    if (!_vkaml_initialized)                    \
    {                                           \
        caml_failwith("Vkaml not initialized"); \
    }

#define ML_WRAP_PTR_OPT(type)                                   \
    static inline value Val_##type(type* p)                     \
    {                                                           \
        if (p == NULL)                                          \
        {                                                       \
            echo_error("Null pointer passed to Val_%s", #type); \
            return Val_none;                                    \
        }                                                       \
        return caml_alloc_some(caml_copy_nativeint((intnat)p)); \
    }                                                           \
    static inline type* type##_val(value v)                     \
    {                                                           \
        return (type*)Nativeint_val(Field(v, 0));               \
    }

ML_WRAP_PTR_OPT(GLFWwindow)
ML_WRAP_PTR_OPT(Vkaml_window)
ML_WRAP_PTR_OPT(Vkaml_backend)

CAMLprim value ml_init(value ml_desc)
{
    CAMLparam1(ml_desc);
    CAMLlocal1(ml_list_head);

    value ml_window_title         = Field(ml_desc, 0);
    value ml_window_size          = Field(ml_desc, 1);
    value ml_app_name             = Field(ml_desc, 2);
    value ml_api_version          = Field(ml_desc, 3);
    value ml_enable_validation    = Field(ml_desc, 4);
    value ml_validation_layers    = Field(ml_desc, 5);
    value ml_instance_extensions  = Field(ml_desc, 6);
    value ml_enable_instance_flag = Field(ml_desc, 7);

    value ml_window_width  = Field(ml_window_size, 0);
    value ml_window_height = Field(ml_window_size, 1);

    value ml_api_major = Field(ml_api_version, 0);
    value ml_api_minor = Field(ml_api_version, 1);
    value ml_api_patch = Field(ml_api_version, 2);

    value ml_validation_layers_count = Field(ml_validation_layers, 0);
    value ml_validation_layer_names  = Field(ml_validation_layers, 1);

    value ml_instance_extension_count = Field(ml_instance_extensions, 0);
    value ml_instance_extension_names = Field(ml_instance_extensions, 1);

    Vkaml_backend_desc desc = { 0 };
    desc.window_title       = caml_stat_strdup(String_val(ml_window_title));
    desc.window_width       = Int_val(ml_window_width);
    desc.window_height      = Int_val(ml_window_height);
    desc.app_name           = caml_stat_strdup(String_val(ml_app_name));
    desc.api_version        = VKAML_MAKE_VERSION(Int_val(ml_api_major), Int_val(ml_api_minor), Int_val(ml_api_patch));
    desc.enable_validation  = Bool_val(ml_enable_validation);
    desc.validation_layer_count   = Int_val(ml_validation_layers_count);
    desc.validation_layer_names   = caml_stat_alloc(ml_validation_layers_count * sizeof(const char*));
    desc.instance_extension_count = Int_val(ml_instance_extension_count);
    desc.instance_extension_names = caml_stat_alloc(ml_instance_extension_count * sizeof(const char*));
    desc.enable_instance_flag     = Bool_val(ml_enable_instance_flag);

    for (uint32_t i = 0; i < desc.validation_layer_count; ++i)
    {
        ml_list_head                   = Field(ml_validation_layer_names, 0);
        desc.validation_layer_names[i] = caml_stat_strdup(String_val(ml_list_head));
        ml_validation_layer_names      = Field(ml_validation_layer_names, 1);
    }

    for (uint32_t i = 0; i < desc.instance_extension_count; ++i)
    {
        ml_list_head                     = Field(ml_instance_extension_names, 0);
        desc.instance_extension_names[i] = caml_stat_strdup(String_val(ml_list_head));
        ml_instance_extension_names      = Field(ml_instance_extension_names, 1);
    }

    Vkaml_backend* vkaml = vkaml_init(&desc);
    if (vkaml == NULL)
    {
        echo_error("vkaml_init failed");
        caml_stat_free((void*)desc.window_title);
        caml_stat_free((void*)desc.app_name);

        for (uint32_t i = 0; i < desc.validation_layer_count; ++i)
        {
            caml_stat_free((void*)desc.validation_layer_names[i]);
        }
        caml_stat_free((void*)desc.validation_layer_names);

        for (uint32_t i = 0; i < desc.instance_extension_count; ++i)
        {
            caml_stat_free((void*)desc.instance_extension_names[i]);
        }
        caml_stat_free((void*)desc.instance_extension_names);

        CAMLreturn(Val_none);
    }

    caml_stat_free((void*)desc.window_title);
    caml_stat_free((void*)desc.app_name);

    for (uint32_t i = 0; i < desc.validation_layer_count; ++i)
    {
        caml_stat_free((void*)desc.validation_layer_names[i]);
    }
    caml_stat_free((void*)desc.validation_layer_names);

    for (uint32_t i = 0; i < desc.instance_extension_count; ++i)
    {
        caml_stat_free((void*)desc.instance_extension_names[i]);
    }
    caml_stat_free((void*)desc.instance_extension_names);

    _vkaml_initialized = true;

    CAMLreturn(Val_Vkaml_backend(vkaml));
}

CAMLprim value ml_cleanup(value ml_backend_handle)
{
    CAMLparam1(ml_backend_handle);

    VKAML_REQUIRE_INIT();

    Vkaml_backend* backend = Vkaml_backend_val(ml_backend_handle);
    if (backend == NULL)
    {
        echo_error("Null backend pointer passed to ml_vkaml_cleanup");
        CAMLreturn(Val_unit);
    }

    GLFWwindow* window = backend->window.window;
    if (window)
    {
        glfwDestroyWindow(window);
        glfwTerminate();
    }

    vkaml_cleanup(backend);

    _vkaml_initialized = false;

    CAMLreturn(Val_unit);
}

CAMLprim value ml_window_handle(value ml_backend_handle)
{
    CAMLparam1(ml_backend_handle);

    VKAML_REQUIRE_INIT();

    Vkaml_backend* backend = Vkaml_backend_val(ml_backend_handle);
    if (backend == NULL)
    {
        echo_error("Null backend pointer passed to ml_vkaml_window_handle");
        CAMLreturn(Val_none);
    }

    CAMLreturn(Val_Vkaml_window(&backend->window));
}

CAMLprim value ml_window_poll_events(value unit)
{
    CAMLparam1(unit);

    VKAML_REQUIRE_INIT();
    glfwPollEvents();

    CAMLreturn(Val_unit);
}

CAMLprim value ml_window_should_close(value ml_window)
{
    CAMLparam1(ml_window);

    VKAML_REQUIRE_INIT();

    Vkaml_window* window = Vkaml_window_val(ml_window);
    if (window == NULL)
    {
        echo_error("Null Vkaml_window pointer passed to ml_window_should_close");
        CAMLreturn(Val_bool(false));
    }

    int should_close = glfwWindowShouldClose(window->window);
    CAMLreturn(Val_bool(should_close));
}

CAMLprim value ml_available_validation_layers(value ml_unit)
{
    CAMLparam1(ml_unit);
    CAMLlocal3(ml_layer_names, ml_cons, ml_name);

    const char* layer_names[VKAML_MAX_VALIDATION_LAYERS];
    uint32_t layer_count = vulkan_available_validation_layers(layer_names);
    if (layer_count == 0 || layer_names == NULL)
    {
        echo_error("No Vulkan validation layers found with ml_vkaml_supported_extension_names.");
        CAMLreturn(Val_emptylist);
    }

    ml_layer_names = Val_emptylist;
    for (int i = layer_count - 1; i >= 0; --i)
    {
        ml_name = caml_copy_string(layer_names[i]);
        ml_cons = caml_alloc(2, 0);
        Store_field(ml_cons, 0, ml_name);
        Store_field(ml_cons, 1, ml_layer_names);
        ml_layer_names = ml_cons;
    }

    CAMLreturn(ml_layer_names);
}


CAMLprim value ml_available_instance_extensions(value ml_unit)
{
    CAMLparam1(ml_unit);
    CAMLlocal3(ml_extension_names, ml_cons, ml_name);

    const char* extension_names[VKAML_MAX_INSTANCE_EXTENSIONS];
    uint32_t extension_count = vulkan_available_instance_extensions(extension_names);
    if (extension_count == 0 || extension_names == NULL)
    {
        echo_error("No Vulkan instance extensions found with ml_vkaml_supported_extension_names.");
        CAMLreturn(Val_emptylist);
    }

    ml_extension_names = Val_emptylist;
    for (int i = extension_count - 1; i >= 0; --i)
    {
        ml_name = caml_copy_string(extension_names[i]);
        ml_cons = caml_alloc(2, 0);
        Store_field(ml_cons, 0, ml_name);
        Store_field(ml_cons, 1, ml_extension_names);
        ml_extension_names = ml_cons;
    }

    CAMLreturn(ml_extension_names);
}
