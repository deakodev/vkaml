type window_size =
  { width : int
  ; height : int
  }

type instance_extensions =
  { count : int
  ; names : string list
  }

type t =
  { window_title : string
  ; window_size : window_size
  ; app_name : string
  ; api_version : int * int * int
  ; validation_layers : bool
  ; instance_extensions : instance_extensions
  }

type key =
  | Window_title of string
  | Window_size of int * int
  | App_name of string
  | Api_version of int * int * int
  | Validation_layers of bool
  | Instance_extensions of string list

val default : t
val with_ : key -> t -> t
