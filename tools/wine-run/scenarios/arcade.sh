# Arcade Modes > Quick Battle on course row SCEN_ROW (Down presses on the challenge select page, all courses unlocked), SCEN_RIGHT x Right (course variant: snow / night / reversed ...), driving SCEN_DRIVE
# (default 31), SCEN_SECS seconds of race (default 25), then SCEN_AFTER=quit|restart|none|win(end screens; use SCEN_DRIVE=33)
SCEN_DRIVE=${SCEN_DRIVE-23}
scenario() {
  to_single; unlock_all
  echo "keywait Down $(sel 3) 1 3"; echo "keywait Down $(sel 3) 2 3"
  echo "keywait Return $CUR 4 3"; echo "keywait Return $CUR 5 3"; echo "keywait Return $CUR 6 3"; echo "waitframes 0x$(mapva frames_total) 40"
  for i in $(seq 1 ${SCEN_ROW:-0}); do echo "keychange Down $ROW 2"; done
  for i in $(seq 1 ${SCEN_RIGHT:-0}); do echo "key Right"; echo "wait 1"; done
  echo "shot course_page"
  echo "keywait Return $CUR 7 3"; echo "waitframes 0x$(mapva frames_total) 40"
  echo "keywait Return $PHASE !0 5"; echo "keywait Return $PHASE !1 6"
  echo "waitmem $PHASE 6 90"; echo "shot race_start"; [ -n "$SCEN_PROBE" ] && echo "probe $SCEN_PROBE $PHASE $CUR $MSG"
  if [ ${SCEN_AFTER:-none} = none ]; then race_freeze ${SCEN_FRAMES:-300}; else echo "wait ${SCEN_SECS:-25}"; echo "shot race_mid"; fi
  case ${SCEN_AFTER:-none} in quit) race_quit;; quitgame) race_pause 4 1; echo "wait 6";; restart) race_restart;; win) end_screens 8;; winmenu) echo "pressuntil Return $CUR 6 150 1.0"; echo "wait 1.5"; echo "shot results_done"; echo "key Escape"; echo "wait 4"; echo "shot main_menu_after"; echo "probe 6 $PHASE $CUR $MSG"; exit_windows;; esac
}
