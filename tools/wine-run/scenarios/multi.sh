# Multi Player > Standard Play (SCEN_TEAM=1: Team Play) with two human players (DRIVE bit 128: harness controller_present / player_color_id, freeze.c): colour, ability, course, mode pages, race.
SCEN_DRIVE=${SCEN_DRIVE-$((128+31))}
scenario() {
  to_single
  echo "keychange Escape $CUR 3"; echo "wait 1"; echo "shot menu_single"
  echo "key Down"; echo "wait 0.8"; echo "key Return"; echo "wait 1.5"; echo "shot menu_multi"
  [ -n "$SCEN_TEAM" ] && { echo "key Down"; echo "wait 0.6"; }
  for n in colour ability course mode; do echo "key Return"; echo "wait 2"; echo "shot menu_$n"; done
  echo "probe 80 $CUR $MSG $PHASE"; echo "poke $MSG 3"; echo "wait 2"; echo "shot poked"
  echo "pressuntil Return $PHASE !0 60 1.8"      # ability / course / mode pages and their message boxes: Return every 1.8 s until the loading screen
  echo "shot press_start"; echo "pressuntil Return $PHASE 6 120 0.8"; echo "shot race_start"
  race_freeze ${SCEN_FRAMES:-300}
}
