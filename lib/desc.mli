type window_size =
  { width : int
  ; height : int
  }

type string_list_with_count =
  { count : int
  ; names : string list
  }

type t =
  { window_title : string
  ; window_size : window_size
  ; app_name : string
  ; api_version : int * int * int
  ; enable_validation : bool
  ; validation_layers : string_list_with_count
  ; instance_extensions : string_list_with_count
  ; enable_instance_flag : bool
  }

type key =
  | Window_title of string
  | Window_size of int * int
  | App_name of string
  | Api_version of int * int * int
  | Enable_validation of bool
  | Validation_layers of string list
  | Instance_extensions of string list
  | Enable_instance_flag of bool

val default : t
val with_ : key -> t -> t
