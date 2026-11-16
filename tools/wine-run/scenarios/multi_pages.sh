# (TS2 W12, race-free) Multi Player (SCEN_TEAM=1: Team Play): colour, ability, course, mode pages as multi.sh, then back out two pages; no race start.
SCEN_DRIVE=${SCEN_DRIVE-$((128+31))}
scenario() {
  to_single
  echo "keychange Escape $CUR 3"; echo "wait 1"
  echo "key Down"; echo "wait 0.8"; echo "key Return"; echo "wait 1.5"; echo "shot menu_multi"
  [ -n "$SCEN_TEAM" ] && { echo "key Down"; echo "wait 0.6"; }
  for n in colour ability course mode; do echo "key Return"; echo "wait 2"; echo "shot menu_$n"; done
  keys 1.5 Escape Escape; echo "shot back2"
}
