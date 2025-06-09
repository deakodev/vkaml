type window_handle = nativeint option
type vkaml_handle = nativeint

external init : desc:Desc.t -> vkaml_handle = "ml_init"
external window_destroy : window_handle -> unit = "ml_window_destroy"
external window_poll_events : unit -> unit = "ml_window_poll_events"
external window_should_close : window_handle -> bool = "ml_window_should_close"
external available_on_platform : unit -> string list = "ml_available_extension_names"
external window_handle : vkaml_handle -> window_handle = "ml_window_handle"
