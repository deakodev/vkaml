type window_handle = nativeint option
type vkaml_handle = nativeint

val init : desc:Desc.t -> vkaml_handle
val window_destroy : window_handle -> unit
val window_poll_events : unit -> unit
val window_should_close : window_handle -> bool
val available_on_platform : unit -> string list
val window_handle : vkaml_handle -> window_handle
