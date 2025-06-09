open Core
module Desc = Desc
module Stubs = Stubs

type window_handle = Stubs.window_handle
type vkaml_handle = Stubs.vkaml_handle

let required_instance_extensions_by_platform () =
  match Sys.os_type with
  | "Unix" -> [ "VK_KHR_surface"; "VK_KHR_xcb_surface" ]
  | _ -> failwith "Unsupported OS type for Vulkan instance extensions"
;;

let check_support ~available extensions =
  let is_supported ext = List.exists available ~f:(String.equal ext) in
  List.filter_map extensions ~f:(fun ext ->
    if is_supported ext
    then Some ext
    else (
      Echo.fatal "Unsupported Vulkan extension: %s" ext;
      None))
;;

let resolve_instance_extensions requested =
  let available = Stubs.query_available_instance_extensions () in
  let all = List.concat [ requested; required_instance_extensions_by_platform () ] in
  let supported = check_support ~available all in
  if List.length supported = List.length all
  then List.dedup_and_sort ~compare:String.compare supported
  else failwith "Instance extensions not supported"
;;

let resolve_validation_layers requested enabled =
  let available = Stubs.query_available_validation_layers () in
  let all = if enabled then "VK_LAYER_KHRONOS_validation" :: requested else requested in
  let supported = check_support ~available all in
  if List.length supported = List.length all
  then List.dedup_and_sort ~compare:String.compare supported
  else failwith "Validation layers not supported"
;;

let init ~desc =
  let open Desc in
  let { validation_layers; instance_extensions; _ } = desc in
  let backend_desc =
    let resolved_validation_layers_names =
      resolve_validation_layers validation_layers.names desc.enable_validation
    in
    let resolved_instance_extension_names =
      resolve_instance_extensions instance_extensions.names
    in
    desc
    |> with_ (Validation_layers resolved_validation_layers_names)
    |> with_ (Instance_extensions resolved_instance_extension_names)
  in
  Stubs.init ~desc:backend_desc
;;

let cleanup backend_handle = Stubs.cleanup backend_handle
let window_handle backend_handle = Stubs.window_handle backend_handle
let window_poll_events () = Stubs.window_poll_events ()
let window_should_close backend_handle = Stubs.window_should_close backend_handle
