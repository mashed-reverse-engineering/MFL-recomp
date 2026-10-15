#!/usr/bin/env python3
"""Collect the `gsign` findings of variants.py (found.txt: <chunk> <function> gsign:<int|uint>:<global>) into names/MFL_global_types.txt ("name type" per line; autocompile.py applies it to the
declarations). Usage: variants_globals.py <found.txt> [...]"""
import os, sys
P = os.path.join(os.path.dirname(os.path.abspath(__file__)), "..", "names", "MFL_global_types.txt")
cur = {}
if os.path.exists(P):
    for l in open(P):
        p = l.split()
        if len(p) == 2 and not l.startswith("#"): cur[p[0]] = p[1]
new = 0
for f in sys.argv[1:]:
    for l in open(f):
        p = l.split()
        if len(p) == 3 and p[2].startswith("gsign:"):
            _, typ, g = p[2].split(":")
            if cur.get(g) != typ: cur[g] = typ; new += 1
with open(P, "w") as o:
    o.write("# global variable type overrides for the autocompile pipeline (found by variants.py rule gsign): name type\n")
    for g, t in sorted(cur.items()): o.write("%s %s\n" % (g, t))
print(new, "new / changed, total", len(cur), "->", P)
