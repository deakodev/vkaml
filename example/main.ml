let backend_setup () =
  let open Vkaml.Desc in
  let desc =
    default
    |> with_ (Window_title "Vkaml Window")
    |> with_ (Window_size (1200, 800))
    |> with_ (App_name "Vkaml Example")
    |> with_ (Api_version (1, 0, 0))
    |> with_ (Validation_layers true)
    |> with_ (Instance_extensions [])
  in
  Vkaml.init ~desc
;;

let () =
  Echo.set_out (File "debug.log");
  List.iter print_endline (Vkaml.Stubs.available_on_platform ());
  let backend_handle = backend_setup () in
  let window_handle = Vkaml.window_handle backend_handle in
  while not (Vkaml.window_should_close window_handle) do
    Vkaml.window_poll_events ();
    Unix.sleepf 0.1;
    Echo.info "polling..."
  done
;;
