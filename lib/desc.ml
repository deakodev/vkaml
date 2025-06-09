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

let default =
  { window_title = "Vkaml Window"
  ; window_size = { width = 1200; height = 800 }
  ; app_name = "Vkaml App"
  ; api_version = 1, 0, 0
  ; validation_layers = false
  ; instance_extensions = { count = 0; names = [] }
  }
;;

type key =
  | Window_title of string
  | Window_size of int * int
  | App_name of string
  | Api_version of int * int * int
  | Validation_layers of bool
  | Instance_extensions of string list

let with_ key desc =
  match key with
  | Window_title title -> { desc with window_title = title }
  | Window_size (width, height) -> { desc with window_size = { width; height } }
  | App_name name -> { desc with app_name = name }
  | Api_version (major, minor, patch) -> { desc with api_version = major, minor, patch }
  | Validation_layers is_enabled -> { desc with validation_layers = is_enabled }
  | Instance_extensions extensions ->
    { desc with
      instance_extensions = { count = List.length extensions; names = extensions }
    }
;;
