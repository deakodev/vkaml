open Core
module Desc = Desc
module Stubs = Stubs

type window_handle = Stubs.window_handle
type vkaml_handle = Stubs.vkaml_handle

let required_by_platform () =
  match Sys.os_type with
  | "Unix" -> [ "VK_KHR_surface"; "VK_KHR_xcb_surface" ]
  | _ -> failwith "Unsupported OS type for Vulkan instance extensions"
;;

let check_supported ~available extensions =
  let is_supported ext = List.exists available ~f:(String.equal ext) in
  List.filter_map extensions ~f:(fun ext ->
    if is_supported ext
    then Some ext
    else (
      Echo.fatal "Unsupported Vulkan extension: %s" ext;
      None))
;;

let resolve_instance_extensions requested =
  let available = Stubs.available_on_platform () in
  let all = List.concat [ requested; required_by_platform () ] in
  let supported = check_supported ~available all in
  if List.length supported = List.length all then supported else exit 1
;;

let init ~desc =
  let open Desc in
  let { instance_extensions; _ } = desc in
  let backend_desc =
    let backend_instance_extensions =
      resolve_instance_extensions instance_extensions.names
    in
    desc |> with_ (Instance_extensions backend_instance_extensions)
  in
  Stubs.init ~desc:backend_desc
;;

let window_handle backend_handle = Stubs.window_handle backend_handle
let window_destroy backend_handle = Stubs.window_destroy backend_handle
let window_poll_events () = Stubs.window_poll_events ()
let window_should_close backend_handle = Stubs.window_should_close backend_handle
