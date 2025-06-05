#include "vkaml_stubs.h"

#include "vkaml_backend.h"
#include "vkaml_types.h"

#include <echo.h>
#include <caml/memory.h>
#include <caml/mlvalues.h>

#include <GLFW/glfw3.h>

ML_WRAP_PTR_OPT(GLFWwindow)
ML_WRAP_PTR_OPT(vkaml_backend)

CAMLprim value ml_window_create(value ml_desc)
{
    CAMLparam1(ml_desc);

    value ml_title  = Field(ml_desc, 0);
    value ml_width  = Field(ml_desc, 1);
    value ml_height = Field(ml_desc, 2);

    vkaml_window_desc desc = { 0 };
    desc.title             = String_val(ml_title);
    desc.width             = Int_val(ml_width);
    desc.height            = Int_val(ml_height);

    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    echo_info("Creating GLFW window: %s (%d x %d)", desc.title, desc.width, desc.height);
    GLFWwindow* window = glfwCreateWindow(desc.width, desc.height, desc.title, NULL, NULL);

    CAMLreturn(Val_GLFWwindow(window));
}

CAMLprim value ml_window_destroy(value ml_window)
{
    CAMLparam1(ml_window);
    glfwDestroyWindow(GLFWwindow_val(ml_window));
    glfwTerminate();
    CAMLreturn(Val_unit);
}

CAMLprim value ml_window_poll_events(value unit)
{
    CAMLparam1(unit);
    glfwPollEvents();
    CAMLreturn(Val_unit);
}

CAMLprim value ml_window_should_close(value ml_window)
{
    CAMLparam1(ml_window);
    int should_close = glfwWindowShouldClose(GLFWwindow_val(ml_window));
    CAMLreturn(Val_bool(should_close));
}

CAMLprim value ml_vkaml_init(value ml_desc)
{
    CAMLparam1(ml_desc);

    value ml_app_name    = Field(ml_desc, 0);
    value ml_api_version = Field(ml_desc, 1);

    vkaml_desc desc  = { 0 };
    desc.app_name    = caml_stat_strdup(String_val(ml_app_name));
    desc.api_version = Int_val(ml_api_version);

    vkaml_backend* vkaml = vkaml_init(&desc);

    caml_stat_free((void*)desc.app_name);

    CAMLreturn(Val_vkaml_backend(vkaml));
}