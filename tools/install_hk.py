#!/usr/bin/env python3
"""install_hk.py <first> <last> [--dry]: additive install of output/tier2_candidates2/an_hk_<n>.c (n = first..last, diffemu-EQUAL candidates) into the live tier-2 set:
copies each file to src/automatch_near/, appends `va name hk_<n>.c` to src/automatch_near_names.txt (va from src/symbols.txt; the function name is the file's single non-extern definition found
via output/tier2_candidates2/names.txt order = file order), backs up the names file next to it (*.bak.install).  Afterwards run `tier_ledger.py --retag`, check_all.sh and gate.sh tier2 @names (see ROADMAP)."""
import os, re, shutil, sys
G = os.path.join(os.path.dirname(os.path.abspath(__file__)), "..")
C2 = os.path.join(G, "output", "tier2_candidates2"); NEAR = os.path.join(G, "src", "automatch_near"); NAMES = os.path.join(G, "src", "automatch_near_names.txt")
a, b = int(sys.argv[1]), int(sys.argv[2]); dry = "--dry" in sys.argv
sym = {}
for l in open(os.path.join(G, "src", "symbols.txt")):
    p = l.split()
    if len(p) == 2: sym.setdefault(p[0], p[1])
have = open(NAMES).read().split("\n"); haveset = {l.split()[1] for l in have if len(l.split()) >= 2}
out = []
names = [l.strip() for l in open(os.path.join(C2, 'names.txt')) if l.strip()]
for n in range(a, b + 1):
    fn = "an_hk_%03d.c" % n; src = os.path.join(C2, fn)
    if not os.path.exists(src): print("missing", fn); continue
    defs = [names[n - 1]]      # names.txt holds one function per file, in file order
    for name in defs:
        if name not in sym: print("no address for", name); continue
        if name in haveset: print("already installed:", name); continue
        out.append("%s %s hk_%03d.c" % (sym[name], name, n))
    if not dry: shutil.copy(src, os.path.join(NEAR, fn))
print("\n".join(out))
if not dry and out:
    shutil.copy(NAMES, NAMES + ".bak.install")
    txt = open(NAMES).read().rstrip("\n") + "\n" + "\n".join(out) + "\n"
    open(NAMES, "w").write(txt)
