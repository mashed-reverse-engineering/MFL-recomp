# Arcade Modes > Time Trial (second entry of the Arcade page; needs all events unlocked), course row SCEN_ROW, then race / ghost recording for SCEN_SECS seconds, optional quit
SCEN_DRIVE=${SCEN_DRIVE-7}
scenario() {
  to_single; unlock_all
  echo "keywait Down $(sel 3) 1 3"; echo "keywait Down $(sel 3) 2 3"
  echo "keywait Return $CUR 4 3"; echo "wait 1.5"; echo "shot arcade_page"
  echo "key Down"; echo "wait 0.8"; echo "shot tt_sel"
  echo "keywait Return $CUR 5 3"; echo "wait 1.5"; echo "shot d5"
  echo "key Return"; echo "wait 2"; echo "shot d6"
  for i in $(seq 1 ${SCEN_ROW:-0}); do echo "keychange Down $ROW 2"; done
  echo "key Return"; echo "wait 2"; echo "shot d7"
  echo "key Return"; echo "wait 2"; echo "shot d8"
  race_go 150; echo "shot race_start"
  echo "wait ${SCEN_SECS:-40}"; echo "shot race_mid"
}
