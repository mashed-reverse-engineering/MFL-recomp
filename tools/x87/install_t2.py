#!/usr/bin/env python3
"""install_t2.py FILE.c NAME VA [--dry]: install a hand-typed readable x87 function as TIER 2 (T-N6/A6): src/automatch_near/an_rwp_<va>.c + a line in src/automatch_near_names.txt
(`va name rwp_<va>.c`), the same bookkeeping as tools/install_hk.py.  Verify with diffemu FIRST (DIFFEMU_FPUCW=0x7f and default, EQUAL).  The registry edit is done under
flock (the ledger lock) by the caller (flock -x <ledger lock> python3 tools/x87/install_t2.py ...)."""
import os, shutil, sys
G = os.path.join(os.path.dirname(os.path.abspath(__file__)), "..", "..")
NEAR = os.path.join(G, "src", "automatch_near"); NAMES = os.path.join(G, "src", "automatch_near_names.txt")
a = [x for x in sys.argv[1:] if x != "--dry"]; src, name, va = a[0], a[1], a[2].lower().zfill(8)
fn = "an_rwp_%s.c" % va
have = [l.split() for l in open(NAMES) if l.strip()]
if any(len(p) >= 2 and p[1] == name for p in have): print("already installed:", name); sys.exit(1)
if "--dry" in sys.argv: print("would install", fn, va, name); sys.exit(0)
shutil.copy(src, os.path.join(NEAR, fn))
shutil.copy(NAMES, NAMES + ".bak.install")
open(NAMES, "a").write("%s %s rwp_%s.c\n" % (va, name, va))
print("installed", fn)
