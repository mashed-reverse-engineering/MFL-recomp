#!/usr/bin/env python3
"""aliascheck.py: aliases that shadow a name which lib_names.txt (or symbols.txt) gives a DIFFERENT address.
mkobj builds its name->address map with setdefault in the order symbols.txt, aliases.txt, lib_names.txt, so such an alias silently moves the
library forwarder jmp to the wrong function (2026-11-03: `RwFclose 005bfbf0` shadowed lib_names `RwFclose 00560e70`; the standalone exe hung on an
uninitialised lock, all 15 courses failed).  Exit 1 on a shadow or a duplicate that is not in src/aliascheck_known.txt (`name reason`).
`_imp__*` names are import stubs with deliberate duplicates and are skipped."""
import collections, os, sys
G = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
def load(f):
    d = collections.defaultdict(set)
    for l in open(f):
        q = l.split()
        if len(q) >= 2 and not q[0].startswith("#"):
            try: d[q[0]].add(int(q[1], 16))
            except ValueError: pass
    return d
known = set(l.split()[0] for l in open(G + "/src/aliascheck_known.txt") if l.strip() and not l.startswith("#")) if os.path.exists(G + "/src/aliascheck_known.txt") else set()
alias = load(G + "/src/aliases.txt"); lib = load(G + "/output/delink_sdk/lib_names.txt"); sym = load(G + "/src/symbols.txt")
bad = 0
for name, vas in sorted(alias.items()):
    if name.startswith("_imp__") or name in known: continue
    for src, d in (("lib_names", lib), ("symbols", sym)):
        if name in d and d[name] != vas:
            bad += 1; print("SHADOW %-45s aliases %s  %s %s" % (name, " ".join("%08x" % x for x in sorted(vas)), src, " ".join("%08x" % x for x in sorted(d[name]))))
    if len(vas) > 1:
        bad += 1; print("DUP    %-45s aliases %s" % (name, " ".join("%08x" % x for x in sorted(vas))))
print("aliascheck: %d problem(s)" % bad)
sys.exit(1 if bad else 0)
