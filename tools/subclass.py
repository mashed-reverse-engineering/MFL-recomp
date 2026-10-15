#!/usr/bin/env python3
"""subclass.py <automatch.csv> [out.csv]: splits the STRUCTURE class (different instruction sequence) of automatch.csv into routing classes from the first differing block of the asm diff:
PROLOGUE (frame size / push order), INDEL1 (one instruction more or less), SMALL (at most 3 instructions differ), CALLCONV (add esp differs), LARGE.  Output: chunk,function,va,class,size.
Cheap tools / agents go to PROLOGUE, INDEL1 and SMALL first (smallest functions first)."""
import re, sys, os
HERE = os.path.dirname(os.path.abspath(__file__))
fs = {}
for l in open(os.path.join(HERE, "..", "output", "fnsizes.txt")):
    a, n, s = l.split(); fs[n] = int(s)
out = []
for l in open(sys.argv[1]):
    r = l.rstrip("\n").split(",", 5)
    if len(r) < 6 or r[3] != "STRUCTURE": continue
    m = re.match(r"ours\[(\d+):(\d+)\]=(.*?) \| orig\[(\d+):(\d+)\]=(.*)", r[5])
    if not m: k = "NODETAIL"
    else:
        la, lo = int(m.group(2)) - int(m.group(1)), int(m.group(5)) - int(m.group(4))
        both = m.group(3) + m.group(6)
        if "sub esp" in both and int(m.group(1)) <= 1: k = "PROLOGUE"
        elif abs(la - lo) == 1 and min(la, lo) == 0: k = "INDEL1"
        elif abs(la - lo) <= 2 and la + lo <= 6: k = "SMALL"
        elif "add esp" in both: k = "CALLCONV"
        else: k = "LARGE"
    out.append((r[0], r[1], r[2], k, fs.get(r[1], 0)))
out.sort(key=lambda x: (x[3], x[4]))
o = open(sys.argv[2], "w") if len(sys.argv) > 2 else sys.stdout
for x in out: o.write("%s,%s,%s,%s,%d\n" % x)
import collections
print(dict(collections.Counter(x[3] for x in out)), file=sys.stderr)
