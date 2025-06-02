let () =
  Echo.set_out (File "debug.log");
  let window_handle =
    match Vulkaml.window_create ~width:1200 ~height:800 ~title:"Vulkaml" with
    | None ->
      Echo.error "Failed to create window";
      exit 1
    | Some w ->
      print_endline "Window created successfully";
      w
  in
  let _ = Vulkaml.vulkan_init () in
  while not (Vulkaml.window_should_close (Some window_handle)) do
    Vulkaml.window_poll_events ();
    Unix.sleepf 0.1;
    Echo.info "polling..."
  done
;;
