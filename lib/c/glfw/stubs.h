#ifndef GLFW_STUBS_H
#define GLFW_STUBS_H

#include <GLFW/glfw3.h>

CAML_WRAP_HANDLE_OPT(GLFWwindow, GLFWwindow*)

CAMLprim value ml_window_create(value width, value height, value title)
{
    CAMLparam3(width, height, title);

    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    GLFWwindow* glfw_window = glfwCreateWindow(Int_val(width), Int_val(height), String_val(title), NULL, NULL);
    if (!glfw_window)
    {
        CAMLreturn(Val_none);
    }

    CAMLreturn(Val_GLFWwindow(glfw_window));
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
