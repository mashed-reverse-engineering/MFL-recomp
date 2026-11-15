#!/bin/bash
# origrel.sh <work dir> [<reference zero_original_ranges.csv> | zero]: W9 T-ORIGREL (plan W9 section 1 point 2, freeze step).
# Links the RELEASE exe (link_release.sh with TRAP=1; LIB_CRT=libcmt and the LIB_RW default come from link_release.sh) into <work dir>,
# runs origbytes.py on that link (--out <work dir>) and compares its zero_original_ranges.csv with the reference
# (default: the newest output/gate/*_standalone/zero_original_ranges.csv).  `zero`: the release link must have no original code range at all.
# Exit 0 = equal (or zero), 1 = different / link or origbytes failed.  Needs the gate lock free (a link).
set -e
T=$(dirname "$(readlink -f "$0")"); G=$T/../..; W=$1
[ -n "$W" ] || { sed -n 2,6p "$0"; exit 2; }
REF=${2:-$(ls -d $G/output/gate/*_standalone 2>/dev/null | sort | tail -1)/zero_original_ranges.csv}
mkdir -p "$(dirname "$W")"
TRAP=1 $T/link_release.sh "$W" > "$W.link.log" 2>&1 || { tail -8 "$W.link.log"; echo "origrel: release link failed ($W.link.log)"; exit 1; }
python3 $T/origbytes.py "$W" --out "$W" > "$W/origbytes.log" 2>&1 || { tail -5 "$W/origbytes.log"; echo "origrel: origbytes failed"; exit 1; }
python3 -I - "$W/zero_original_ranges.csv" "$REF" <<'PY'
import sys
cur = [l.strip() for l in open(sys.argv[1]) if l.strip()][1:]
print("release link: %d original code ranges, %d B" % (len(cur), sum(int(l.split(",")[3]) for l in cur)))
for l in cur: print("  " + l)
if sys.argv[2] == "zero":
    sys.exit("origrel: FAIL, release link still has original code" if cur else print("origrel: release link has zero original code") or 0)
ref = [l.strip() for l in open(sys.argv[2]) if l.strip()][1:]
if cur == ref: print("origrel: release ranges = reference %s (%d ranges)" % (sys.argv[2], len(ref))); sys.exit(0)
for l in sorted(set(cur) - set(ref)): print("  only release:   " + l)
for l in sorted(set(ref) - set(cur)): print("  only reference: " + l)
sys.exit("origrel: FAIL, release ranges differ from %s" % sys.argv[2])
PY
