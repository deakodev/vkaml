module Window_desc : module type of Desc.Window_desc
module App_desc : module type of Desc.App_desc

type window_handle = nativeint option
type vkaml_handle = nativeint

val window_create : desc:Window_desc.t -> window_handle
val window_poll_events : unit -> unit
val window_should_close : window_handle -> bool
val window_destroy : window_handle -> unit
val init : desc:App_desc.t -> vkaml_handle
