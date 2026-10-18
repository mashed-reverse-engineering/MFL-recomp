# main menu > Options: Sound page (Left/Right on the volume sliders), Gamma Correction (Left/Right), Load / Save; then back out
scenario() {
  to_single; to_main
  keys 0.6 Down Down; echo "shot opt_sel"
  echo "key Return"; echo "wait 2"; echo "shot opt_page"
  echo "key Return"; echo "wait 2"; echo "shot sound_page"
  keys 0.5 Right Right Right Down Left Left Down Right Right Down Left Up Up
  echo "shot sound_after"
  echo "key Escape"; echo "wait 1.5"
  echo "key Down"; echo "wait 0.6"; echo "key Return"; echo "wait 2"; echo "shot gamma_page"
  keys 0.5 Right Right Right Left Left Left Left Right
  echo "shot gamma_after"
  echo "key Escape"; echo "wait 1.5"
  keys 0.6 Down; echo "key Return"; echo "wait 2"; echo "shot loadsave_page"
  keys 0.7 Return; echo "shot loadsave2"
  keys 0.7 Down Return; echo "shot loadsave3"
  keys 1 Escape Escape Escape
}
