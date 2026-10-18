# Options > Load / Save: Load Game (Yes, Continue), Save Game (Yes, Continue), Autosave page (Left / Right), Sound > Insults toggle
scenario() {
  to_single; to_main
  keys 0.6 Down Down; echo "key Return"; echo "wait 2"
  keys 0.6 Down Down; echo "key Return"; echo "wait 2"; echo "shot ls_page"
  echo "key Return"; echo "wait 1.5"; echo "shot ls_load_box"; echo "key Return"; echo "wait 2.5"; echo "shot ls_load_ok"; echo "key Return"; echo "wait 2"
  keys 0.6 Down; echo "key Return"; echo "wait 1.5"; echo "shot ls_save_box"; echo "key Return"; echo "wait 2.5"; echo "shot ls_save_ok"; echo "key Return"; echo "wait 2"
  keys 0.6 Down; echo "key Return"; echo "wait 1.5"; echo "shot ls_autosave"; keys 0.6 Left Right Return; echo "shot ls_autosave2"
  keys 1 Escape Escape; echo "key Return"; echo "wait 2"; keys 0.5 Down Down Down Right Left Right; echo "shot insults"; keys 0.8 Return Escape
}
