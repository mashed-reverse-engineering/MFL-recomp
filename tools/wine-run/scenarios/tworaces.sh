# two Quick Battles in one session on different courses: the first is won quickly (DRIVE bit 32 + opponents off), the match ends, the game returns to the Single Player page (race teardown:
# course unload / world destroy / etc), the second race (next course row) loads over it; then freeze.  SCEN_ROW = row of the first race, second race = row + 1.
SCEN_DRIVE=${SCEN_DRIVE-41}
scenario() {
  to_single; unlock_all
  echo "keywait Down $(sel 3) 1 3"; echo "keywait Down $(sel 3) 2 3"
  echo "keywait Return $CUR 4 3"; echo "keywait Return $CUR 5 3"; echo "keywait Return $CUR 6 3"; echo "waitframes 0x$(mapva frames_total) 40"
  for i in $(seq 1 ${SCEN_ROW:-3}); do echo "keychange Down $ROW 2"; done
  echo "keywait Return $CUR 7 3"; echo "waitframes 0x$(mapva frames_total) 40"
  echo "keywait Return $PHASE !0 5"; echo "keywait Return $PHASE !1 6"; echo "waitmem $PHASE 6 90"; echo "shot race1"
  for n in $(seq 1 24); do echo "wait 4"; echo "keyabove Return $PHASE 6"; done; echo "waitmem $CUR 7 60"; echo "wait 3"; echo "shot between"      # the match ends: standings, results, the frontend rebuilds its pages up to the Game Mode page (depth 7)
  echo "poke 0x$(mapva scen_flags) 8"
  echo "keychange Escape $CUR 3"; echo "wait 2"; echo "keychange Down $ROW 2"; echo "wait 1"; echo "shot course2"
  echo "keywait Return $CUR 7 3"; echo "waitframes 0x$(mapva frames_total) 40"
  echo "keywait Return $PHASE !0 5"; echo "keywait Return $PHASE !1 6"; echo "waitmem $PHASE 6 90"; echo "shot race2"
  race_freeze 200
}
