#!/usr/bin/env python3
"""add_fnsizes.py [FILE=output/fnsizes_added.txt]: merge the rows `va name size` of FILE (functions Ghidra did not split off; the C exists, T-E5) into output/fnsizes.txt (sorted by va,
a row of the same va is replaced) the names into src/aliases.txt and plain rows into output/reginputs.csv (no register input, ret 0: only for such functions).  Rerun after every regeneration of fnsizes.txt (scripts/FnIndex.java)."""
import os, sys
G = os.path.normpath(os.path.join(os.path.dirname(os.path.abspath(__file__)), ".."))
add = [l.split() for l in open(sys.argv[1] if len(sys.argv) > 1 else os.path.join(G, "output", "fnsizes_added.txt")) if l.strip() and not l.startswith("#")]
fp = os.path.join(G, "output", "fnsizes.txt"); rows = {l.split()[0]: l.rstrip("\n") for l in open(fp) if l.strip()}
for va, n, sz in add: rows[va] = "%s %s %s" % (va, n, sz)
open(fp, "w").write("".join(rows[k] + "\n" for k in sorted(rows)))
ap = os.path.join(G, "src", "aliases.txt"); have = {l.split()[0] for l in open(ap) if l.strip() and not l.startswith("#")}
with open(ap, "a") as f:
    for va, n, sz in add:
        if n not in have: f.write("%s %s\n" % (n, va))
# reginputs.csv (tools/reginputs.py only knows the functions of MFL_app_named.c): a plain stack-convention function row, no register input, `ret 0` (checked by hand for every row of the file)
rp = os.path.join(G, "output", "reginputs.csv"); have = {l.split(",")[0] for l in open(rp)}
with open(rp, "a") as f:
    for va, n, sz in add:
        if n not in have: f.write("%s,%s,,0,0,eax|edx,%s,,,1,\n" % (n, va, sz))
print("added", len(add))
