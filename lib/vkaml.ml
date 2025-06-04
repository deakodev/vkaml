type window_desc =
  { title : string option
  ; width : int option
  ; height : int option
  }

let make_window_desc ?title ?width ?height () = { title; width; height }

type window_handle = nativeint option

external window_create : ?desc:window_desc -> unit -> window_handle = "ml_window_create"
external window_destroy : window_handle -> unit = "ml_window_destroy"
external window_poll_events : unit -> unit = "ml_window_poll_events"
external window_should_close : window_handle -> bool = "ml_window_should_close"

type context_desc =
  { app_name : string option
  ; app_version : int option
  ; engine_name : string option
  ; engine_version : int option
  ; api_version : int option
  ; enabled_extensions : string list option
  ; enabled_layers : string list option
  }

let make_context_desc
      ?app_name
      ?app_version
      ?engine_name
      ?engine_version
      ?api_version
      ?enabled_extensions
      ?enabled_layers
      ()
  =
  { app_name
  ; app_version
  ; engine_name
  ; engine_version
  ; api_version
  ; enabled_extensions
  ; enabled_layers
  }
;;

type context_handle = nativeint option

external init : ?desc:context_desc -> unit -> context_handle = "ml_vkaml_init"
