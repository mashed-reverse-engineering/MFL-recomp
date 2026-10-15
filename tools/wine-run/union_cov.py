#!/usr/bin/env python3
"""union_cov.py <sweep dir>...: replaced functions that no scenario of any of the given courses.sh sweep directories executed (prints them; summary on stderr)."""
import glob, os, struct, sys
ran, allf = set(), set()
for O in sys.argv[1:]:
    for mp in glob.glob(O + "/c*/ovl.exe.map"):
        cb = os.path.join(os.path.dirname(mp), "out_ovl_1", "counters.bin")
        if not os.path.exists(cb): continue
        base, names = None, []
        for l in open(mp):
            p = l.split()
            if p[0].startswith("count:"): names.append((p[0][6:], int(p[1], 16)))
            elif p[0] == "count_table": base = int(p[1], 16)
        d = open(cb, "rb").read()
        for n, va in names:
            allf.add(n)
            if va - base + 4 <= len(d) and struct.unpack_from("<I", d, va - base)[0] > 0: ran.add(n)
for n in sorted(allf - ran): print(n)
print("replaced %d, executed %d, never executed %d" % (len(allf), len(ran), len(allf - ran)), file=sys.stderr)
