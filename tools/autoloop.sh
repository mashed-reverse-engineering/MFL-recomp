#!/bin/bash
# autoloop.sh <work dir> [rounds=3]: autocompile -> automatch -> variants -> collect global type overrides (names/MFL_global_types.txt), repeated until no new override appears.
# Every round uses its own directory <work dir>/r<N> (nothing is reused, no stale files).  Prints one line per round.
T=$(dirname "$(readlink -f "$0")"); W=$(readlink -f "$1"); N=${2:-3}
export VC7_CONTAINER=${VC7_CONTAINER:-ac}      # one long lived compiler container: `docker run` per chunk check cost 2-15 s each
for r in $(seq 1 $N); do
  D=$W/r$r; mkdir -p $D/ac; cp $T/../src/auto/ghidra_types.h $T/../src/auto/prelude.h $D/ac/
  python3 $T/autocompile.py $D/ac $T/../output/decls.h 401000 5d7000 40 > $D/autocompile.log 2>&1
  python3 $T/automatch.py $D/ac $D/automatch.csv 6 > $D/automatch.log 2>&1
  python3 $T/variants.py $D/ac $D/automatch.csv $D/var 6 > $D/variants.log 2>&1
  M=$(grep -c ",MATCH," $D/automatch.csv); V=$(grep -c . $D/var/found.txt 2>/dev/null)
  NEWG=$(python3 $T/variants_globals.py $D/var/found.txt | head -1)
  echo "round $r $(date +%H:%M): $(tail -1 $D/autocompile.log | cut -c1-60) | MATCH $M | variants found $V | overrides: $NEWG"
  case "$NEWG" in "0 new"*) break;; esac
done
