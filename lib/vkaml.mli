val window_create : width:int -> height:int -> title:string -> nativeint option
val window_destroy : nativeint option -> unit
val window_poll_events : unit -> unit
val window_should_close : nativeint option -> bool
val init : unit -> nativeint option
