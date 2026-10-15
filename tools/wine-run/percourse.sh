#!/bin/bash
# percourse.sh <work dir> <names.txt> <out survivors.txt> [courses...]: for every course a regress run (creates the control dump and the params) and a crash / stall only group search over the
# names (THRESH huge: only crashes and stalls count), culprits found on one course are removed before the next one.  Group size 30, PAR slots (default 4).
T=$(dirname "$(readlink -f "$0")"); W=$(readlink -f "$1"); N=$2; OUT=$3; shift 3
mkdir -p $W; cp $N $W/names.cur
for c in ${@:-1 3 4 5 6 7 8 9 10 11 12 13 14}; do
  D=$W/c$c
  SAMELAYOUT=0 COURSE=$c OVL_ARGS="--automatch --automatch-near --bridges" RUN_TIMEOUT=100 timeout 700 $T/regress.sh $D 90 1 > $W/c$c.regress.log 2>&1
  THRESH=${THRESH:-100000} PAR=${PAR:-4} python3 $T/groupsearch.py $D $W/names.cur $W/surv.c$c 30 > $W/c$c.gs.log 2>&1
  [ -f $W/surv.c$c ] && cp $W/surv.c$c $W/names.cur
  echo "[$(date +%H:%M)] course $c: $(grep -a '^done' $W/c$c.gs.log | cut -c1-60) culprits: $(cat $W/surv.c$c.culprits 2>/dev/null | tr '\n' ' ')" >> $W/summary.txt
done
cp $W/names.cur $OUT
echo "[$(date +%H:%M)] finished: $(wc -l < $OUT) functions remain" >> $W/summary.txt
