#!/usr/bin/env python3
"""Which replaced functions did the race run?  Usage: coverage.py <overlay exe .map> <counters.bin> [--missing]
The overlay is built with `--count` (OVL_ARGS=--count regress.sh ...): every replaced function's entry goes through a thunk that increments a counter.
Prints function, call count (sorted); --missing lists only the functions that never ran (their correctness is not tested by this race)."""
import re, struct, sys
mp, binf = sys.argv[1], sys.argv[2]
names = []
base = None
for l in open(mp):
    p = l.split()
    if p[0].startswith("count:"):
        names.append((p[0][6:], int(p[1], 16)))
    elif p[0] == "count_table":
        base = int(p[1], 16)
d = open(binf, "rb").read()
rows = []
for nm, va in names:
    o = va - base
    rows.append((struct.unpack_from("<I", d, o)[0] if o + 4 <= len(d) else -1, nm))
ran = [r for r in rows if r[0] > 0]
if "--missing" in sys.argv:
    for c, n in sorted(rows, key=lambda x: x[1]):
        if c == 0: print(n)
else:
    for c, n in sorted(rows, reverse=True): print("%9d %s" % (c, n))
print("replaced %d, executed %d, never executed %d" % (len(rows), len(ran), len(rows) - len(ran)), file=sys.stderr)
