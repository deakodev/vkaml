module Window_desc = Desc.Window_desc
module App_desc = Desc.App_desc

type window_handle = nativeint option

external window_create : desc:Window_desc.t -> window_handle = "ml_window_create"
external window_destroy : window_handle -> unit = "ml_window_destroy"
external window_poll_events : unit -> unit = "ml_window_poll_events"
external window_should_close : window_handle -> bool = "ml_window_should_close"

type vkaml_handle = nativeint

external init : desc:App_desc.t -> vkaml_handle = "ml_vkaml_init"
