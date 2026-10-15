#!/bin/bash
# Prints a play.sh script that walks the main menu: for every entry (ENTRIES, default 6) it enters the page, takes a screenshot, optionally presses the
# keys of KEYS ("Down Return ..."), takes another one and goes back (BACK, default Escape).  Usage: menus_script.sh <harness map file>
# Needs the exe built with --harness (fast_menu_scale).  Output: shots m<i>_page, m<i>_after, depth values in probe.txt.
MAP=$1; mapva() { grep "^$1 " $MAP | cut -d' ' -f2; }
CUR=0x69cbd0; MSG=0x69cfd0
echo "poke 0x$(mapva fast_menu_scale) 6"
echo "probe 300 $CUR $MSG 0x69d2e0 0x69d328 0x69d368 0x69d3a8"
echo "waitmem $CUR 1 20"; echo "waitmem 0x69d1e4 0 5"
echo "keywait Return $MSG 2 4"
echo "pressuntil Return $CUR 3 20 0.2"
echo "keychange ${BACK:-Escape} $CUR 3"          # back to the main menu (depth 2)
echo "wait 1"; echo "shot main_menu"
for i in $(seq 0 $(( ${ENTRIES:-6} - 1 ))); do
  echo "keywait Return $CUR 3 3"; echo "wait 1.5"; echo "shot m${i}_page"
  for k in $KEYS; do echo "key $k"; echo "wait 0.7"; done
  [ -n "$KEYS" ] && echo "shot m${i}_after"
  echo "keychange ${BACK:-Escape} $CUR 3"; echo "wait 1"
  echo "key Down"; echo "wait 0.5"
done
