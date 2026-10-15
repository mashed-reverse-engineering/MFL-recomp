#!/bin/bash
# promote_round.sh <autoloop round dir>: merge the winning variants into the chunks of that round, byte compare again, write src/automatch/ (the promoted functions) and print the counts.
T=$(dirname "$(readlink -f "$0")"); D=$(readlink -f "$1"); M=$D/merged
mkdir -p $M; cp $D/ac/*.h $D/ac/ok_*.c $D/ac/addrs.txt $M/; [ -d $D/var ] && cp $D/var/ok_*.c $M/ 2>/dev/null
python3 $T/automatch.py $M $D/automatch_merged.csv 6 | tail -1 | cut -c1-120
python3 $T/automatch_promote.py $M $D/automatch_merged.csv
