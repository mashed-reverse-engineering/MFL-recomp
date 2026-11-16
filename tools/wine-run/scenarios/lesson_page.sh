# (TS2 W12, race-free) Single Player > Training, lesson SCEN_ROW selected on the lesson page (Down presses), shots of the page and of its text; ends there (no race start).
# Use through scenset.sh (SCEN_END=now dumps GameState); the end event is the lesson page with the row changed (keychange on the row cell).
scenario() {
  to_single; unlock_all
  echo "key Return"; echo "wait 2"
  echo "keywait Return $CUR 5 3"; echo "waitframes 0x$(mapva frames_total) 40"
  for i in $(seq 1 ${SCEN_ROW:-0}); do echo "keychange Down $ROW 2"; done
  echo "waitframes 0x$(mapva frames_total) 40"
  echo "dump lesson_row $ROW 8"      # the selected row (gs_end does not carry it; the shots differ by only ~12 < 30): a wrong row in ours is a FAIL
  echo "shot lesson_page"
}
