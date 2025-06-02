open Core
module C = Configurator.V1

let print_flags typ flags =
  let body = List.map flags ~f:(sprintf "%s") |> String.concat ~sep:" " in
  let message = sprintf "%s: %s" typ body in
  printf "%s\n" message
;;

let query_package pc package =
  match C.Pkg_config.query pc ~package with
  | Some deps ->
    printf "Found %s via pkg-config\n" package;
    deps
  | None -> C.die "pkg-config could not find package: %s" package
;;

let () =
  C.main ~name:"vulkaml-discover" (fun c ->
    print_endline "Discovering Vulkaml dependencies...";
    let pkg_config =
      match C.Pkg_config.get c with
      | Some pc -> pc
      | None -> C.die "\npkg-config not found. Please install it to proceed.\n"
    in
    let vk = query_package pkg_config "vulkan" in
    let glfw = query_package pkg_config "glfw3" in
    let flags = vk.cflags @ glfw.cflags @ [ "-fPIC" ] in
    let lib_flags = vk.libs @ glfw.libs in
    print_flags "Vulkaml flags" flags;
    print_endline "Writing to vulkaml_flags.sexp...";
    C.Flags.write_sexp "vulkaml_flags.sexp" flags;
    print_flags "Vulkaml library flags" lib_flags;
    print_endline "Writing to vulkaml_library_flags.sexp...";
    C.Flags.write_sexp "vulkaml_library_flags.sexp" lib_flags;
    print_endline "Done.\n")
;;
