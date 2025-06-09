module Desc : module type of Desc
module Stubs : module type of Stubs

type window_handle = Stubs.window_handle
type vkaml_handle = Stubs.vkaml_handle

val init : desc:Desc.t -> vkaml_handle
val window_handle : vkaml_handle -> window_handle
val window_poll_events : unit -> unit
val window_should_close : window_handle -> bool
val window_destroy : window_handle -> unit
