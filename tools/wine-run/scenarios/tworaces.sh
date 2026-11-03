# two Quick Battles in one session on different courses: the first race is quit from the pause menu at once (race teardown: course unload / world destroy / etc),
# the game comes back on Challenge Select, the second race (next course row) loads over it; then freeze.  SCEN_ROW = row of the first race, second race = row + 1.
# Why not win race 1 and run through the end screens (the old version): the round end (phase 7) and match end (11) need Return, but the Race Results page (phase 10)
# leads to Challenge Select where Return restarts the same course, and the games of both builds (control too) died silently in round 2 now and then.
# scen_flags: 8 (opponents idle) so race 1 does not end by itself; 40 from the pause menu until race 2 runs, because the harness freezes the game when the phase leaves 6
# without bit 32/64 (freeze.c frame hook).
SCEN_DRIVE=${SCEN_DRIVE-8}
scenario() {
  to_single; unlock_all
  echo "keywait Down $(sel 3) 1 3"; echo "keywait Down $(sel 3) 2 3"
  echo "keywait Return $CUR 4 3"; echo "keywait Return $CUR 5 3"; echo "keywait Return $CUR 6 3"; echo "waitframes 0x$(mapva frames_total) 40"
  for i in $(seq 1 ${SCEN_ROW:-3}); do echo "keychange Down $ROW 2"; done
  echo "keywait Return $CUR 7 3"; echo "waitframes 0x$(mapva frames_total) 40"
  echo "keywait Return $PHASE !0 5"; echo "keywait Return $PHASE !1 6"; echo "waitmem $PHASE 6 90"; echo "shot race1"
  echo "keywait Escape $CUR 1 3"; echo "poke 0x$(mapva scen_flags) 40"; for i in 1 2 3; do echo "keychange Down $(sel 1) 3"; done; echo "pressuntil Return $CUR 2 30 1.0"      # pause menu: Quit Race, Yes (key checks: presses get lost under load)
  echo "waitmem $CUR 2 60"; echo "waitmem $PHASE 1 30"; echo "wait 2"; echo "shot between"      # Challenge Select (depth 2 after the quit, phase 1)
  echo "keychange Down $ROW 2"; echo "wait 1"; echo "shot course2"
  echo "keywait Return $CUR 3 3"; echo "keywait Return $PHASE !1 6"; echo "waitmem $PHASE 6 90"      # Game Mode page (Play Game), then loading
  echo "poke 0x$(mapva scen_flags) 8"; echo "shot race2"      # back to 8 only in the race: with 8 any phase but 6 freezes the game (also the frozen state race_freeze waits for)
  race_freeze 600
}
