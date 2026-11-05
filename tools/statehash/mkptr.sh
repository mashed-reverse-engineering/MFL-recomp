#!/bin/bash
# mkptr.sh <runA> <runB>: regenerate data/state_ptrwords.txt (words that only ever hold pointers) from two control runs of the same course
# ($SH_WORK/runs/<tag>/statehash.bin), then re-run the pair through statecmp as a check.  Original: runs c0_ctrl1 / c0_ctrl2 (course 0).
T=$(dirname "$(readlink -f "$0")"); R=$(readlink -f "$T/../.."); W=${SH_WORK:-$R/output/statehash}
A=${1:?runA}; B=${2:?runB}
python3 "$T/statecmp.py" --mkptr "$W/runs/$A/statehash.bin" "$W/runs/$B/statehash.bin" > "$T/data/state_ptrwords.txt"
wc -l "$T/data/state_ptrwords.txt"
python3 "$T/statecmp.py" "$W/runs/$A/statehash.bin" "$W/runs/$B/statehash.bin" --ptr "$T/data/state_ptrwords.txt" --show 3000 | tail -1
