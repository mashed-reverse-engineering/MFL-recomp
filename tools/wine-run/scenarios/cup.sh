# Single Player > Challenge Cup: colour select, challenge select (row SCEN_ROW = Down presses with all events unlocked, SCEN_RIGHT x Right = another cup/event column), start, play.
# SCEN_AFTER = none (freeze at SCEN_FRAMES) | quit | restart | win (end screens, use SCEN_DRIVE=33 / 65)
SCEN_DRIVE=${SCEN_DRIVE-31}
scenario() {
  to_single; unlock_all
  echo "keywait Down $(sel 3) 1 3"
  echo "keywait Return $CUR 4 3"; echo "keywait Return $CUR 5 3"; echo "waitframes 0x$(mapva frames_total) 40"
  for i in $(seq 1 ${SCEN_ROW:-0}); do echo "keychange Down $ROW 2"; done
  for i in $(seq 1 ${SCEN_RIGHT:-0}); do echo "key Right"; echo "wait 1"; done
  echo "shot challenge_page"
  echo "key Return"; echo "wait 2"; echo "shot intro"
  race_go 150; echo "shot race_start"
  if [ ${SCEN_AFTER:-none} = none ]; then race_freeze ${SCEN_FRAMES:-300}; else echo "wait ${SCEN_SECS:-25}"; echo "shot race_mid"; fi
  case ${SCEN_AFTER:-none} in quit) race_quit;; restart) race_restart;; win) end_screens 10;; esac
}
