external window_create
  :  width:int
  -> height:int
  -> title:string
  -> nativeint option
  = "ml_window_create"

external window_destroy : nativeint option -> unit = "ml_window_destroy"
external window_poll_events : unit -> unit = "ml_window_poll_events"
external window_should_close : nativeint option -> bool = "ml_window_should_close"
external vulkan_init : unit -> nativeint option = "ml_vulkan_init"
