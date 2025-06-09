#ifndef VKAML_STUBS_H
#define VKAML_STUBS_H

#include <caml/mlvalues.h>

CAMLprim value ml_init(value ml_desc);

CAMLprim value ml_window_handle(value ml_backend_handle);
CAMLprim value ml_window_destroy(value window);
CAMLprim value ml_window_poll_events(value unit);
CAMLprim value ml_window_should_close(value window);

#endif // VKAML_STUBS_H