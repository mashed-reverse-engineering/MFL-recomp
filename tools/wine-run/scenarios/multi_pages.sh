# (TS2 W12, race-free) Multi Player (SCEN_TEAM=1: Team Play): colour, ability, course, mode pages as multi.sh, then back out two pages; no race start.
# (W13 hx5: page opens / closes = pagekey where the menu depth changes (probe of C3 run 1, ss_multipages / ss_teampages); was key + wait 1.5 / 2)
SCEN_DRIVE=${SCEN_DRIVE-$((128+31))}
scenario() {
  to_single
  echo "keychange Escape $CUR 3"; echo "wait 1"
  echo "key Down"; echo "wait 0.8"; pagekey Return; echo "shot menu_multi"
  [ -n "$SCEN_TEAM" ] && { echo "key Down"; echo "wait 0.6"; }
  for n in colour ability course mode; do      # Team Play: the course and mode Returns do not change the depth (probe ss_teampages): those keep key + wait 2
    if [ -n "$SCEN_TEAM" ] && [ $n = course -o $n = mode ]; then echo "key Return"; echo "wait 2"; else pagekey Return; fi; echo "shot menu_$n"; done
  pagekey Escape; pagekey Escape; echo "shot back2"
}
