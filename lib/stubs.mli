type window_handle = nativeint option
type vkaml_handle = nativeint

val init : desc:Desc.t -> vkaml_handle
val cleanup : vkaml_handle -> unit
val window_poll_events : unit -> unit
val window_should_close : window_handle -> bool
val query_available_validation_layers : unit -> string list
val query_available_instance_extensions : unit -> string list
val window_handle : vkaml_handle -> window_handle
