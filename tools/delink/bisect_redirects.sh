#!/bin/bash
# Bisect the functions linked in place (mkobj --redirect-only) for a crash on one course.  Assumes one culprit.
# Usage: bisect_redirects.sh <work dir> <names file> [course=0]
T=$(dirname "$(readlink -f "$0")"); W=$(readlink -f "$1"); C=${3:-0}; mkdir -p $W; cp "$2" $W/cand.txt; echo 0 > $W/n
run() {   # $1 = names file -> prints ok / crash
  n=$(( $(cat $W/n) + 1 )); echo $n > $W/n; d=$W/t$n     # counter in a file: run is called in $( )
  MKOBJ_ARGS="--redirect-only $1" timeout 900 $T/link_ours.sh $d 0x400000 > $d.log 2>&1
  until [ $(cut -d' ' -f1 /proc/loadavg | cut -d. -f1) -lt 6 ]; do sleep 5; done
  D=$(objdump -h $d/ours.exe | python3 -c "import sys; print('%x' % next(int(l.split()[3], 16) - 0x5fb000 for l in sys.stdin if len(l.split()) > 3 and l.split()[1] == '.data'))")
  r=$($T/race_relinked.sh $d/game $d/r $D $C); echo "test $n ($(wc -l < $1) functions): $r" | tee -a $W/log.txt >&2
  case "$r" in *": ok") echo ok;; *) echo crash;; esac
}
while [ $(wc -l < $W/cand.txt) -gt 1 ]; do
  h=$(( $(wc -l < $W/cand.txt) / 2 )); head -n $h $W/cand.txt > $W/a.txt; tail -n +$((h+1)) $W/cand.txt > $W/b.txt
  if [ "$(run $W/a.txt)" = crash ]; then cp $W/a.txt $W/cand.txt
  elif [ "$(run $W/b.txt)" = crash ]; then cp $W/b.txt $W/cand.txt
  else echo "neither half crashes alone (interaction): $(wc -l < $W/cand.txt) left" >&2; break; fi
done
echo "culprit candidates:"; cat $W/cand.txt
