#ifndef VKAML_STUBS_H
#define VKAML_STUBS_H

#include <echo.h>
#include <caml/alloc.h>
#include <caml/mlvalues.h>

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

CAMLprim value ml_window_create(value ml_desc);
CAMLprim value ml_window_destroy(value window);
CAMLprim value ml_window_poll_events(value unit);
CAMLprim value ml_window_should_close(value window);

CAMLprim value ml_vkaml_init(value ml_desc);

#endif // VKAML_STUBS_H