# Pause menu ("Transmission Interrupted": Continue / Options / Restart Race / Quit Race / Quit Game), every entry, inside a race of SCEN_MODE = arcade (Quick Battle row 0, default) | cup | training | multi (2 humans).
#   1. Escape > Continue x10 rapid   2. Escape > Options: every entry Left x3 / Right x6, Down, ..., Escape   3. Escape > Restart Race > Yes, race again   4. Escape > Quit Race > Yes (GameState dump gs_quit)
#   SCEN_LAST=quitgame: instead of 3/4 do Escape > Quit Game > Yes from within the race (the process ends).  SCEN_OPTROWS (default 5): number of entries tried on the Options page.
# SCEN_DRIVE 8 (like pause_test): opponents disabled, player idle, so the round cannot end; with driving + weapons (23 / 31) the round ends within ~25 s and the harness FREEZES the game
# at the standings (freeze.c: race_frames > 0 and phase != 6), which looks like a soft lock after the pause cycles.
case ${SCEN_MODE:-arcade} in multi) SCEN_DRIVE=${SCEN_DRIVE-$((128+8))};; *) SCEN_DRIVE=${SCEN_DRIVE-8};; esac
pause_open() { echo "key Escape"; echo "wait 1"; }
scenario() {
  to_single
  case ${SCEN_MODE:-arcade} in
  arcade)
    unlock_all
    echo "keywait Down $(sel 3) 1 3"; echo "keywait Down $(sel 3) 2 3"
    echo "keywait Return $CUR 4 3"; echo "keywait Return $CUR 5 3"; echo "keywait Return $CUR 6 3"; echo "waitframes 0x$(mapva frames_total) 40"
    echo "keywait Return $CUR 7 3"; echo "waitframes 0x$(mapva frames_total) 40"
    echo "keywait Return $PHASE !0 5"; echo "keywait Return $PHASE !1 6";;
  cup)
    unlock_all
    echo "keywait Down $(sel 3) 1 3"
    echo "keywait Return $CUR 4 3"; echo "keywait Return $CUR 5 3"; echo "waitframes 0x$(mapva frames_total) 40"
    echo "key Return"; echo "wait 2"; race_go 150;;
  training)
    unlock_all
    echo "key Return"; echo "wait 2"
    echo "keywait Return $CUR 5 3"; echo "waitframes 0x$(mapva frames_total) 40"
    echo "key Return"; echo "wait 2"; race_go 150;;
  multi)
    echo "keychange Escape $CUR 3"; echo "wait 1"
    echo "key Down"; echo "wait 0.8"; echo "key Return"; echo "wait 1.5"
    for n in colour ability course mode; do echo "key Return"; echo "wait 2"; done
    echo "pressuntil Return $PHASE !0 60 1.8"; echo "pressuntil Return $PHASE 6 120 0.8";;
  esac
  echo "waitmem $PHASE 6 90"; echo "shot race_start"; echo "probe ${SCEN_PROBE:-0} $PHASE $CUR $MSG"; echo "wait 0.5"
  # 1. Continue x10 rapid (Escape opens the menu, Return = Continue)
  for i in $(seq 1 10); do echo "key Escape"; echo "wait ${SCEN_CW:-0.9}"; echo "key Return"; echo "wait ${SCEN_CG:-0.15}"; done      # SCEN_CW: wait for the menu (s), SCEN_CG: running time between Continue and the next Escape.  The arcade round ends (phase 7, harness freezes) ~4 s after the start even with idle players, and only running time counts, so the gap must be short (10 x 0.15 s)
  echo "shot after_continue"; echo "waitmem $PHASE 6 10"
  # 2. Options page: every entry Left x3 / Right x6 (sliders, toggles), Save / Retry buttons just get Return when they are the last entry
  # (the pause Options page has the same 3 entries as the main menu: Sound (4 rows), Gamma Correction, Load / Save)
  pause_open; echo "key Down"; echo "wait 0.6"; echo "key Return"; echo "wait 2"; echo "shot pause_options"
  echo "key Return"; echo "wait 2"; echo "shot pause_sound"
  for r in 1 2 3 4; do keys 0.3 Left Left Left Right Right Right Right Right Right; echo "shot sound_row$r"; echo "key Down"; echo "wait 0.5"; done
  echo "key Escape"; echo "wait 1.5"
  echo "key Down"; echo "wait 0.6"; echo "key Return"; echo "wait 2"; echo "shot pause_gamma"; keys 0.3 Left Left Left Right Right Right Right Right Right; echo "shot gamma_row"
  echo "key Escape"; echo "wait 1.5"
  echo "key Down"; echo "wait 0.6"; echo "key Return"; echo "wait 2"; echo "shot pause_loadsave"
  keys 1 Escape Escape; echo "shot pause_back"
  echo "key Escape"; echo "wait 1.5"; echo "waitmem $PHASE 6 10"; echo "wait 2"
  if [ "${SCEN_LAST:-}" = quitgame ]; then race_pause 4 1; echo "wait 6"; return; fi
  # 3. Restart Race > Yes, wait for the new race
  race_pause 2 1; echo "wait 8"; echo "pressuntil Return $PHASE 6 60 0.7"; echo "shot after_restart"
  # 4. Quit Race > Yes
  race_pause 3 1; echo "wait 3"; echo "shot after_quit"; echo "dump gs_quit 0x7eebc0 0xB64"
}
