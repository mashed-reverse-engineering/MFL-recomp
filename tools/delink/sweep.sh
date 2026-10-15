#!/bin/bash
# Crash-only course sweep of a relinked exe: race_relinked.sh per course, P games side by side (own game copy and own Wine prefix volume wine_pfx_7<slot>
# each: parallel games must never share a prefix), load guard before every game start (LOADMAX, default 6).
# Usage: sweep.sh <exe> <delta hex> <out dir> [courses="0 3"] [P=2]
T=$(dirname "$(readlink -f "$0")"); EXE=$(readlink -f "$1"); D=$2; O=$(readlink -f "$3"); CS=${4:-0 3}; P=${5:-2}; [ $P -gt 3 ] && P=3
INST=$T/../../../MashedGame/FullyLoaded/installed; mkdir -p $O
for i in $(seq 1 $P); do $T/../wine-run/mkpfx.sh 7$i; python3 $T/../wine-run/prepare.py $INST $O/game$i >/dev/null; rm -f $O/game$i/App_Executables/MFL.exe; cp $EXE $O/game$i/App_Executables/MFL.exe; done
set -- $CS; i=0
while [ $# -gt 0 ]; do
  L=$(cut -d' ' -f1 /proc/loadavg | cut -d. -f1); [ $L -gt ${LOADMAX:-6} ] && { echo "load $L, waiting"; sleep 20; continue; }
  PIDS=()
  for s in $(seq 1 $P); do [ $# -gt 0 ] || break; L=$(cut -d' ' -f1 /proc/loadavg | cut -d. -f1); [ $L -gt ${LOADMAX:-6} ] && break    # checked before every game start
    PFX=wine_pfx_7$s $T/race_relinked.sh $O/game$s $O/c$1 $D $1 > $O/c$1.txt & PIDS+=($!); shift; done
  wait "${PIDS[@]}"
done
cat $O/c*.txt | sort -t' ' -k2 -n
