# Single Player > Training, lesson SCEN_ROW (Down presses on the lesson page); the tutorial boxes are confirmed with Return every 5 s for SCEN_SECS
SCEN_DRIVE=${SCEN_DRIVE-23}
scenario() {
  to_single; unlock_all
  echo "key Return"; echo "wait 2"
  echo "keywait Return $CUR 5 3"; echo "waitframes 0x$(mapva frames_total) 40"
  for i in $(seq 1 ${SCEN_ROW:-0}); do echo "keychange Down $ROW 2"; done
  echo "shot lesson_page"
  echo "key Return"; echo "wait 2"
  race_go 150; echo "shot race_start"
  for i in $(seq 1 $(( ${SCEN_SECS:-60} / 5 ))); do echo "key Return"; echo "wait 5"; [ $((i % 4)) = 0 ] && echo "shot t$i"; done
  case ${SCEN_AFTER:-none} in quit) race_quit;; esac
}
