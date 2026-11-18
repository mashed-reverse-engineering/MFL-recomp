# mines G1 (W14 MINE, output/mines_crash_2026-11-16.md section 6): Arcade > Quick Battle on course row SCEN_ROW (default 0), opponents idle (scen_flags 8).
# In race phase 6 the harness (freeze.c mine_step) runs mine_test MINE_MODE (1: drop + equip mines 0xC per holder in one game-thread call, 2: G0, drop all,
# equip after the release) for players 0 .. MINE_HOLDERS-1 (default 4), mine_cap 10 cycles after the first equip.  A gdb hardware write watch on the fifth equip
# record (0x6a8b58 first mine pointer, 0x6a8b64 owner, 0x6a8b68 state) and on record 0's state 0x6a8b08 (positive control: every cycle writes it) logs each
# write with its pc to out/gdbwatch.txt; mine_log (harness) counts the weapon calls that changed 0x6a8b58..0x6a8b6b.  End event: mine_log[1] = 1.
# A crash after the overrun is expected on some machines and is noted (wine.log), not required.  Reader: agents/mine_w14/mines_read.py <out dir> <map>.
SCEN_DRIVE=${SCEN_DRIVE-8}
export SCEN_IMGTHRESH=off
scenario() {
  local ML=0x$(mapva mine_log)
  to_single; unlock_all
  echo "keywait Down $(sel 3) 1 3"; echo "keywait Down $(sel 3) 2 3"
  echo "keywait Return $CUR 4 3"; echo "keywait Return $CUR 5 3"; echo "keywait Return $CUR 6 3"; echo "waitframes 0x$(mapva frames_total) 40"
  for i in $(seq 1 ${SCEN_ROW:-0}); do echo "keychange Down $ROW 2"; done
  echo "keywait Return $CUR 7 3"; echo "waitframes 0x$(mapva frames_total) 40"
  echo "keywait Return $PHASE !0 5"; echo "keywait Return $PHASE !1 6"
  echo "waitevent $PHASE 6 90 race"; echo "shot race_start"
  echo "dump rec_start 0x6a8af8 0x78"; echo "dump status 0x6aac18 0x10"
  echo "gdbwatch 0x6a8b08 0x6a8b58 0x6a8b64 0x6a8b68"
  echo "poke 0x$(mapva mine_holders) ${MINE_HOLDERS:-4}"; echo "poke 0x$(mapva mine_test) ${MINE_MODE:-1}"
  echo "waitevent $(printf 0x%x $((ML + 4))) 1 120 end"
  echo "dump mine_log $ML 0x40"; echo "dump rec_end 0x6a8af8 0x78"; echo "dump slots 0x8bed20 0x2d0"
  echo "shot mines_done"
}
