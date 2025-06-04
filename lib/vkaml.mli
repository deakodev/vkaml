(** Optional window configuration *)
type window_desc =
  { title : string option
  ; width : int option
  ; height : int option
  }

(** Optional Vulkan context configuration *)
type context_desc =
  { app_name : string option
  ; app_version : int option
  ; engine_name : string option
  ; engine_version : int option
  ; api_version : int option
  ; enabled_extensions : string list option
  ; enabled_layers : string list option
  }

type window_handle = nativeint option
type context_handle = nativeint option

(* --- Window API --- *)
val make_window_desc : ?title:string -> ?width:int -> ?height:int -> unit -> window_desc
val window_create : ?desc:window_desc -> unit -> window_handle
val window_poll_events : unit -> unit
val window_should_close : window_handle -> bool
val window_destroy : window_handle -> unit

(* --- Vulkan Init API --- *)
val make_context_desc
  :  ?app_name:string
  -> ?app_version:int
  -> ?engine_name:string
  -> ?engine_version:int
  -> ?api_version:int
  -> ?enabled_extensions:string list
  -> ?enabled_layers:string list
  -> unit
  -> context_desc

val init : ?desc:context_desc -> unit -> context_handle
