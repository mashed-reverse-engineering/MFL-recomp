# walk every main menu entry except the last (Exit To Windows): enter, wait, screenshot, Escape back; options page keys: a few Left/Right/Down presses
scenario() {
  to_single; to_main; echo "shot main_menu"
  for i in 0 1 2 3 4 5 6; do
    echo "key Return"; echo "wait 2"; echo "shot m${i}_page"
    keys 0.6 Down Down Right Left Up
    echo "shot m${i}_after"
    echo "key Escape"; echo "wait 1.2"; echo "shot m${i}_back"
    echo "key Down"; echo "wait 0.6"
  done
}
