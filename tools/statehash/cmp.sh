#!/bin/bash
# cmp.sh <tagA> <tagB> <out name> [statecmp options]: statecmp of two finished runs ($SH_WORK/runs/<tag>/statehash.bin) -> $SH_WORK/<out name>.txt
# + grouped summary <out name>.groups.txt.  No Wine needed.  Typical options: --ptr data/state_ptrwords.txt --mask data/state_mask.txt
# --map-b <link dir>/ours.map --dead-layers Lua,qhull,zlib/PNG/TIFF,CRT,DirectShow --frames 222
T=$(dirname "$(readlink -f "$0")"); R=$(readlink -f "$T/../.."); W=${SH_WORK:-$R/output/statehash}
A=${1:?tagA}; B=${2:?tagB}; O=${3:?out name}; shift 3
mkdir -p "$W"
python3 "$T/statecmp.py" "$W/runs/$A/statehash.bin" "$W/runs/$B/statehash.bin" --show 3000 "$@" > "$W/$O.txt"
head -2 "$W/$O.txt"; tail -1 "$W/$O.txt"
python3 "$T/groups.py" "$W/$O.txt" > "$W/$O.groups.txt"
