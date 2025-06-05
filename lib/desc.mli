module Window_desc : sig
  type t

  val default : t
  val with_title : string -> t -> t
  val with_size : int -> int -> t -> t
end

module App_desc : sig
  type api_version =
    | VK_API_VERSION_1_0
    | VK_API_VERSION_1_1
    | VK_API_VERSION_1_2
    | VK_API_VERSION_1_3

  type t

  val default : t
  val with_name : string -> t -> t
  val with_api : api_version -> t -> t
end
