#!/bin/bash
# Crash-only course sweep of a relinked exe: race_relinked.sh per course, P games side by side (own game copy and own Wine prefix volume wine_pfx_7<slot>
# each: parallel games must never share a prefix).  No load guard of its own: every game start in race_relinked.sh takes one of the global gameslot.sh slots
# (GAME_SLOTS, default 4) and waits there (plan_speedup_2026-11-03 item 8; the old /proc/loadavg > LOADMAX sleep stalled gate sweeps for 20+ min).
# Usage: sweep.sh <exe> <delta hex> <out dir> [courses="0 3"] [P=2]
T=$(dirname "$(readlink -f "$0")"); EXE=$(readlink -f "$1"); D=$2; O=$(readlink -f "$3"); CS=${4:-0 3}; P=${5:-2}; [ $P -gt 3 ] && P=3
INST=$T/../../../MashedGame/FullyLoaded/installed; mkdir -p $O; MAP=${EXE%.exe}.map; [ -f $MAP ] || MAP=
for i in $(seq 1 $P); do $T/../wine-run/mkpfx.sh 7$i; python3 $T/../wine-run/prepare.py $INST $O/game$i $EXE $MAP >/dev/null; [ -f $(dirname $EXE)/game/App_Executables/mflrw.dll ] && cp $(dirname $EXE)/game/App_Executables/mflrw.dll $O/game$i/App_Executables/mflrw.dll; done      # LIB_RW link: the facade DLL next to the exe
set -- $CS; i=0
while [ $# -gt 0 ]; do
  PIDS=()
  for s in $(seq 1 $P); do [ $# -gt 0 ] || break
    PFX=wine_pfx_7$s $T/race_relinked.sh $O/game$s $O/c$1 $D $1 > $O/c$1.txt & PIDS+=($!); shift; done
  wait "${PIDS[@]}"
done
cat $O/c*.txt | sort -t' ' -k2 -n
