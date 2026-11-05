#!/bin/bash
# cc.sh <tag> <course> [frames]: determinism baseline = two control runs (<tag>_1, <tag>_2) with one.sh + statecmp of the pair.
# Result text: $SH_WORK/cc_<tag>.txt.  Env as one.sh, plus MASK=<mask file> (default none, so every nondeterministic word shows).
T=$(dirname "$(readlink -f "$0")"); R=$(readlink -f "$T/../.."); W=${SH_WORK:-$R/output/statehash}
TAG=${1:?tag}; C=${2:?course}; FR=${3:-120}
for r in 1 2; do
  "$T/one.sh" ${TAG}_$r $C $FR > /dev/null || { echo "run $r failed"; exit 1; }
done
python3 "$T/statecmp.py" "$W/runs/${TAG}_1/statehash.bin" "$W/runs/${TAG}_2/statehash.bin" --ptr "$T/data/state_ptrwords.txt" ${MASK:+--mask $MASK} --show 3000 > "$W/cc_$TAG.txt"
head -2 "$W/cc_$TAG.txt"; tail -1 "$W/cc_$TAG.txt"
