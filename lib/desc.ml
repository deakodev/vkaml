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

let default =
  { window_title = "Vkaml Window"
  ; window_size = { width = 1200; height = 800 }
  ; app_name = "Vkaml App"
  ; api_version = 1, 0, 0
  ; enable_validation = false
  ; validation_layers = { count = 0; names = [] }
  ; instance_extensions = { count = 0; names = [] }
  ; enable_instance_flag = true
  }
;;

type key =
  | Window_title of string
  | Window_size of int * int
  | App_name of string
  | Api_version of int * int * int
  | Enable_validation of bool
  | Validation_layers of string list
  | Instance_extensions of string list
  | Enable_instance_flag of bool

let with_ key desc =
  match key with
  | Window_title title -> { desc with window_title = title }
  | Window_size (width, height) -> { desc with window_size = { width; height } }
  | App_name name -> { desc with app_name = name }
  | Api_version (major, minor, patch) -> { desc with api_version = major, minor, patch }
  | Enable_validation is_enabled -> { desc with enable_validation = is_enabled }
  | Validation_layers layers ->
    { desc with validation_layers = { count = List.length layers; names = layers } }
  | Instance_extensions exts ->
    { desc with instance_extensions = { count = List.length exts; names = exts } }
  | Enable_instance_flag is_enabled -> { desc with enable_instance_flag = is_enabled }
;;
