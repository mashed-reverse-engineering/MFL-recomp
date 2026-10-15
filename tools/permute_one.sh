#!/bin/bash
# permute_one.sh <name> [seconds=600] [seed=1]: run tools/permute.py (our x86 permuter; decomp-permuter has no x86 backend) on ONE near miss of output/nearmiss_triage.csv with a hard timeout.
# Result goes to output/permute/<name>.best.c (a SEPARATE candidate file, never installed: permuter output can be implausible code, review before promoting) and a line to output/permute/results.txt.
# Exit 0 = byte identical variant found.  Own compiler container (mfl_permute) so it does not disturb other jobs.
T=$(dirname "$(readlink -f "$0")"); N=$1; SECS=${2:-600}; SEED=${3:-1}
ROW=$(awk -F, -v n="$N" '$1==n{print; exit}' $T/../output/nearmiss_triage.csv) || exit 3
[ -z "$ROW" ] && { echo "$N not in output/nearmiss_triage.csv"; exit 3; }
VA=$(echo "$ROW" | cut -d, -f2); CH=$(echo "$ROW" | cut -d, -f7); SD=$(echo "$ROW" | cut -d, -f8)
O=$T/../output/permute; mkdir -p $O
W=${TMPDIR:-/tmp/claude-$(id -u)}/permute_one/$N; mkdir -p $W      # below the container mount
python3 $T/extract_fn.py $SD/$CH $N $W/f.c || exit 3
export VC7_CONTAINER=permute VC7_PFX=wine_pfx_ccp
timeout $SECS python3 -u $T/permute.py $W/f.c $N $VA 100000 40 $SEED > $W/log.txt 2>&1
S=$(grep -E "^(start|round)" $W/log.txt | tail -1 | awk '{print $NF}'); [ -z "$S" ] && S=0
if [ -f $W/f.best.c ]; then cp $W/f.best.c $O/$N.best.c; fi
echo "$N score ${S} (of 1.0) ${SECS}s limit" >> $O/results.txt
echo "$N score $S"; [ "$S" = "1.0" ]
