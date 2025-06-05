[@@@ocaml.warning "-69-37"]

module Window_desc = struct
  type t =
    { title : string
    ; width : int
    ; height : int
    }

  let default = { title = "Vkaml Window"; width = 1200; height = 800 }
  let with_title title desc = { desc with title }
  let with_size width height desc = { desc with width; height }
end

module App_desc = struct
  type api_version =
    | VK_API_VERSION_1_0
    | VK_API_VERSION_1_1
    | VK_API_VERSION_1_2
    | VK_API_VERSION_1_3

  type t =
    { name : string
    ; api_version : api_version
    }

  let default = { name = "Vkaml App"; api_version = VK_API_VERSION_1_0 }
  let with_name name desc = { desc with name }
  let with_api api_version desc = { desc with api_version }
end
