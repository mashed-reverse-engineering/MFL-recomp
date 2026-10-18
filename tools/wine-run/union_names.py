#!/usr/bin/env python3
"""union_names.py <never list> <sweep dir>...: how many names of the list are executed (--count counters) by any c<N>/out_ovl_1 of the sweep dirs; prints the remaining names to stdout with -v."""
import glob, os, re, struct, sys
never = [l.strip() for l in open(sys.argv[1]) if l.strip()]; ex = set()
for d in sys.argv[2:]:
    for mp in glob.glob(d + "/**/ovl.exe.map", recursive=True):
        w = os.path.dirname(mp)
        for cb in glob.glob(w + "/out_ovl*/counters.bin"):
            base, names = None, []
            for l in open(mp):
                p = l.split()
                if p[0].startswith("count:"): names.append((p[0][6:], int(p[1], 16)))
                elif p[0] == "count_table": base = int(p[1], 16)
            dd = open(cb, "rb").read()
            for n, va in names:
                if struct.unpack_from("<I", dd, va - base)[0] > 0: ex.add(n)
        for ef in glob.glob(w + "/executed.txt"): ex |= {l.strip() for l in open(ef)}
print("%d of %d executed" % (len(set(never) & ex), len(never)))
