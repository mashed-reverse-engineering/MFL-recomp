#!/usr/bin/env python3
"""rwfilter.py <trace.d3d[.gz]> <ranges.txt> > cut.d3d: keeps the header and the draws `first <= idx < last` of the ranges (rwreplay_gen.py --atoms), renumbered 0.."""
import gzip, sys
rg = [tuple(map(int, l.split())) for l in open(sys.argv[2]) if l.strip()]
keep = set(i for a, b in rg for i in range(a, b)); n = 0
for ln in (gzip.open if sys.argv[1].endswith(".gz") else open)(sys.argv[1], "rt"):
    if ln.startswith("R "):
        h, rest = ln.split("\t", 1)
        if int(h[2:]) not in keep: continue
        sys.stdout.write("R %d\t%s" % (n, rest)); n += 1
    else: sys.stdout.write(ln)
