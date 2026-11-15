# training_tt: Single Player > Training, lesson SCEN_ROW, played to the end (lane TT, W10; gate step `sa_oracles.sh training`, verdict wine-run/tt_check.py).
# Like training.sh, but Return is pressed only while a lesson dialog is up (g_car_69d2d8 != 0: Return in the race would pause it)
# and the harness drives (SCEN_DRIVE, default 287 = accelerate + steer + weapons + opponents idle + throttle in countdown + bit 256).
# Without bit 256 (harness freeze.c, no freeze on a phase change) the harness freezes the game as soon as the race phase leaves 6 after
# race frame 1 unless scen_flags & 96, and a lesson goes 6 (dialog) -> 5 (countdown) after its first dialog: run x0 (DRIVE 15) stalled
# in phase 5 for 150 s, so did every old training_r* run.  Bit 32 instead forces points / wrecks opponents.
# Probe (out/probe.txt, every 0.25 s): 6 columns "t lesson step dialog text_id race_phase" and 3 columns "t menu_depth career_row".
# End: dumps gs_end (GameState: lesson done = progress[row*18+17] == 2; scenario.sh compares gs_*.bin ctrl vs ours) and row (g_career_row).
# Q1 (e0/e3, DRIVE 63): bit 32 ends the round at race frame ~40 (phase 7 standings) before any lesson step: useless. Default 287 needs bit 256.
SCEN_DRIVE=${SCEN_DRIVE-287}
scenario() {
  local s=${SCEN_SECS:-120} i
  to_single; unlock_all
  echo "key Return"; echo "wait 2"
  echo "keywait Return $CUR 5 3"; echo "waitframes 0x$(mapva frames_total) 40"
  for i in $(seq 1 ${SCEN_ROW:-0}); do echo "keychange Down $ROW 2"; done
  echo "shot lesson_page"
  echo "probe $((s + 200)) 0x8c8078 0x8c86c8 0x69d2d8 0x69d2dc $PHASE"
  echo "probe $((s + 200)) $CUR $ROW"
  echo "key Return"; echo "wait 2"
  race_go 150; echo "shot race_start"
  # Return also on the round end screens (phase > 6, "Current Standings / Continue"): f0 (DRIVE 287) won its point at race second ~4 and waited there 180 s
  for i in $(seq 1 $s); do echo "keyunless Return 0x69d2d8 0"; echo "keyabove Return $PHASE 6"; echo "wait 1"; [ $((i % 30)) = 0 ] && echo "shot t$i"; done
  echo "dump row $ROW 4"; echo "dump gs_end 0x7eebc0 0xb64"
}
