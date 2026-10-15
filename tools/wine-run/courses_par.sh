#!/bin/bash
# courses_par.sh <outdir> [P=2] [first=0] [last=14] [frames=90]: courses.sh split into P interleaved-free ranges, run side by side (separate subdirs <outdir>/p<i>), same env as courses.sh.
# Guards: refuses to start when the load average is above LOADMAX (default 6); P is capped at 3 (about 2 cores and 1-2 GB per game container); a whole-run watchdog (WALL, default 3000 s) kills every child by PID.
T=$(dirname "$(readlink -f "$0")"); O=$1; P=${2:-2}; A=${3:-0}; B=${4:-14}; FR=${5:-90}
[ $P -gt 3 ] && P=3; export PAR=${PAR:-$((4 / P))}; [ $PAR -lt 1 ] && PAR=1      # P x PAR <= 4 game processes (8 cores, ~2 cores and 1-2 GB each)
L=$(cut -d' ' -f1 /proc/loadavg | cut -d. -f1); [ $L -gt ${LOADMAX:-6} ] && { echo "load $L too high, not starting"; exit 1; }
mkdir -p "$O"; N=$((B - A + 1)); S=$(( (N + P - 1) / P )); PIDS=()
for i in $(seq 0 $((P - 1))); do
  a=$((A + i * S)); b=$((a + S - 1)); [ $b -gt $B ] && b=$B; [ $a -gt $B ] && break
  $T/courses.sh "$O/p$i" $a $b $FR > "$O/p$i.log" 2>&1 & PIDS+=($!)
done
( sleep ${WALL:-3000}; for p in "${PIDS[@]}"; do kill -TERM $p 2>/dev/null; done; echo "watchdog fired" >> "$O/watchdog.txt" ) & W=$!
wait "${PIDS[@]}"; kill $W 2>/dev/null
grep -h "^course" "$O"/p*.log | sort -t' ' -k2 -n
