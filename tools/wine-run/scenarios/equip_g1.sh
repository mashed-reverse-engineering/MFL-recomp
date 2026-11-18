# equip G1 (W14 MINE batch eqf, output/weapons_equip_sweep_2026-11-17.md): mines_g1 with the weapon EQ_WEAPON (drum | flashbang | shotgun | mines, default drum).
# Arcade > Quick Battle on course row SCEN_ROW (default 0), opponents idle (scen_flags 8).  In race phase 6 the harness (freeze.c mine_step, mine_weapon =
# the weapon id) runs mine_test MINE_MODE (1: drop + equip per holder in one game-thread call, 2: G0) for players 0 .. MINE_HOLDERS-1 (default 4),
# mine_cap 10 cycles after the first equip.  gdb hardware write watch: record 0's state (positive control) and words of the fifth record (one past the
# 4 records); mine_log counts the harness calls that changed the 5 watched dwords.  End event: mine_log[1] = 1.  Shotgun: an overrun sets
# g_player_status[2] = 3, the cycles stop, the end event is then a MISS (expected on the side that overruns).  Reader: agents/mine_w14/equip_read.py.
SCEN_DRIVE=${SCEN_DRIVE-8}
export SCEN_IMGTHRESH=off
scenario() {
  local ML=0x$(mapva mine_log) ID W
  case ${EQ_WEAPON:-drum} in
    drum)      ID=0x0a; W="0x6a5c70 0x6a5d10 0x6a5d1c 0x6a5d20";;
    flashbang) ID=0x12; W="0x6a5d84 0x6a5dd0 0x6a5dd4";;
    shotgun)   ID=0x11; W="0x6aab98 0x6aac28 0x6aac30";;
    *)         ID=0x0c; W="0x6a8b08 0x6a8b58 0x6a8b64 0x6a8b68";;
  esac
  to_single; unlock_all
  echo "keywait Down $(sel 3) 1 3"; echo "keywait Down $(sel 3) 2 3"
  echo "keywait Return $CUR 4 3"; echo "keywait Return $CUR 5 3"; echo "keywait Return $CUR 6 3"; echo "waitframes 0x$(mapva frames_total) 40"
  for i in $(seq 1 ${SCEN_ROW:-0}); do echo "keychange Down $ROW 2"; done
  echo "keywait Return $CUR 7 3"; echo "waitframes 0x$(mapva frames_total) 40"
  echo "keywait Return $PHASE !0 5"; echo "keywait Return $PHASE !1 6"
  echo "waitevent $PHASE 6 90 race"; echo "shot race_start"
  echo "dump status 0x6aac18 0x10"
  echo "gdbwatch $W"
  echo "poke 0x$(mapva mine_weapon) $((ID))"
  echo "poke 0x$(mapva mine_holders) ${MINE_HOLDERS:-4}"; echo "poke 0x$(mapva mine_test) ${MINE_MODE:-1}"
  echo "waitevent $(printf 0x%x $((ML + 4))) 1 120 end"
  echo "dump mine_log $ML 0x40"; echo "dump status_end 0x6aac18 0x10"; echo "dump slots 0x8bed20 0x2d0"
  echo "shot equip_done"
}
