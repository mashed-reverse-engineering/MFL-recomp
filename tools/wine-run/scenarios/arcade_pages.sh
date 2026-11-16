# (TS2 W12, race-free) Arcade Modes > Quick Battle: challenge page, course page (SCEN_ROW Downs, SCEN_RIGHT Rights), the next page (depth 7), then back out one page; no race start.
scenario() {
  to_single; unlock_all
  echo "keywait Down $(sel 3) 1 3"; echo "keywait Down $(sel 3) 2 3"
  echo "keywait Return $CUR 4 3"; echo "shot page4"; echo "keywait Return $CUR 5 3"; echo "shot page5"; echo "keywait Return $CUR 6 3"; echo "waitframes 0x$(mapva frames_total) 40"
  for i in $(seq 1 ${SCEN_ROW:-0}); do echo "keychange Down $ROW 2"; done
  for i in $(seq 1 ${SCEN_RIGHT:-0}); do echo "key Right"; echo "wait 1"; done
  echo "shot course_page"
  echo "keywait Return $CUR 7 3"; echo "waitframes 0x$(mapva frames_total) 40"; echo "shot page7"
  echo "keychange Escape $CUR 3"; echo "waitframes 0x$(mapva frames_total) 40"; echo "shot back6"
}
