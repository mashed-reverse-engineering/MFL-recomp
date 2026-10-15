#!/bin/bash
# Prints a play.sh script that walks from the title screen to the main menu and then presses the keys of KEYS one by one (0.8 s apart), taking a screenshot k<i> after each.
# Usage: KEYS="Down Down Return Return Escape" [PRE="poke 0x.. 1;..."] menu_path.sh <harness map file>
# Keys: poke=0xADDR=VALUE writes a 32 bit value; Return = select, Escape = back, Up/Down/Left/Right; "wait3" waits 3 s.  Never press Return on "Exit To Windows" (last main menu entry).
MAP=$1; mapva() { grep "^$1 " $MAP | cut -d' ' -f2; }
CUR=0x69cbd0; MSG=0x69cfd0
echo "poke 0x$(mapva fast_menu_scale) 6"
echo "waitmem $CUR 1 20"; echo "waitmem 0x69d1e4 0 5"
echo "keywait Return $MSG 2 4"
echo "pressuntil Return $CUR 3 20 0.2"
echo "keychange Escape $CUR 3"; echo "wait 1"
for p in $(echo "$PRE" | tr ';' '\n' | sed 's/ /_/g'); do echo "${p//_/ }"; done
echo "shot k0"
i=1
for k in $KEYS; do
  case $k in poke=*) IFS== read -r _ a v <<< "$k"; echo "poke $a $v"; echo "wait 0.5";; wait*) echo "wait ${k#wait}";; *) echo "key $k"; echo "wait 0.8";; esac
  echo "shot k$i"; i=$((i+1))
done
