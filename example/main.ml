let () =
  Echo.set_out (File "debug.log");
  let window_desc =
    Vkaml.Window_desc.(default |> with_title "Vkaml Window" |> with_size 1200 800)
  in
  let vkaml_desc =
    Vkaml.App_desc.(default |> with_name "Vkaml Example" |> with_api VK_API_VERSION_1_0)
  in
  let window_handle =
    match Vkaml.window_create ~desc:window_desc with
    | None ->
      Echo.error "Failed to create window";
      exit 1
    | Some w ->
      Echo.info "Window created successfully";
      w
  in
  let vkaml_handle = Vkaml.init ~desc:vkaml_desc in
  Echo.info "Vulkan initialized successfully: %s" (Nativeint.to_string vkaml_handle);
  while not (Vkaml.window_should_close (Some window_handle)) do
    Vkaml.window_poll_events ();
    Unix.sleepf 0.1;
    Echo.info "polling..."
  done
;;
