#ifndef GLFW_STUBS_H
#define GLFW_STUBS_H

#include <GLFW/glfw3.h>

CAML_WRAP_HANDLE_OPT(GLFWwindow, GLFWwindow*)

typedef struct vkaml_window_desc {
    const char* title;
    int width;
    int height;
} vkaml_window_desc;

CAMLprim value ml_window_create(value ml_desc, value unit)
{
    CAMLparam2(ml_desc, unit);

    echo_info("Creating GLFW window");

    vkaml_window_desc desc = { "Vkaml Window", 800, 600 };
    if (Is_some(ml_desc))
    {
        value v_desc = Some_val(ml_desc);

        value v_title_opt  = Field(v_desc, 0);
        value v_width_opt  = Field(v_desc, 1);
        value v_height_opt = Field(v_desc, 2);

        if (Is_some(v_title_opt))
        {
            desc.title = String_val(Some_val(v_title_opt));
        }

        if (Is_some(v_width_opt))
        {
            desc.width = Int_val(Some_val(v_width_opt));
        }

        if (Is_some(v_height_opt))
        {
            desc.height = Int_val(Some_val(v_height_opt));
        }
    }

    echo_info("Creating GLFW window: %s (%d x %d)", desc.title, desc.width, desc.height);

    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    GLFWwindow* window = glfwCreateWindow(desc.width, desc.height, desc.title, NULL, NULL);
    if (!window)
    {
        CAMLreturn(Val_none);
    }

    CAMLreturn(Val_GLFWwindow(window));
}


CAMLprim value ml_window_destroy(value window)
{
    glfwDestroyWindow(GLFWwindow_val(window));
    glfwTerminate();
    return Val_unit;
}

CAMLprim value ml_window_poll_events(value unit)
{
    glfwPollEvents();
    return Val_unit;
}

CAMLprim value ml_window_should_close(value window)
{
    int should_close = glfwWindowShouldClose(GLFWwindow_val(window));
    return Val_bool(should_close);
}

#endif // GLFW_STUBS_H
