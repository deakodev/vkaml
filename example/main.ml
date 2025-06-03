let () =
  Echo.set_out (File "debug.log");
  let window_handle =
    match Vkaml.window_create ~width:1200 ~height:800 ~title:"Vkaml" with
    | None ->
      Echo.error "Failed to create window";
      exit 1
    | Some w ->
      print_endline "Window created successfully";
      w
  in
  let vkaml = Vkaml.init () in
  Echo.info
    "Vulkan initialized successfully: %s"
    (Nativeint.to_string (Option.value ~default:0n vkaml));
  while not (Vkaml.window_should_close (Some window_handle)) do
    Vkaml.window_poll_events ();
    Unix.sleepf 0.1;
    Echo.info "polling..."
  done
;;
