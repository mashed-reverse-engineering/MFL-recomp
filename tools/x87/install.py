#!/usr/bin/env python3
"""install.py [--dry] FILE.c NAME VA: install a byte-identical lifted/mutated x87 function (cl 13.00 /O2 /G5) as src/tier1/NAME.c (marker `/* compile: /G5 */`).
Verifies the standalone file with cmpfn.check first; when a tier-2 chunk (src/automatch_near) still defines the function at that address, the definition is cut out of the chunk and its line leaves
automatch_near_names.txt (the same bookkeeping as tools/bytematch_install.py, which only handles functions that live in the chunk text)."""
import os, re, shutil, sys, tempfile
HERE = os.path.dirname(os.path.abspath(__file__)); sys.path.insert(0, os.path.join(HERE, "..")); sys.path.insert(0, HERE)
import cmpfn, bytematch_rewrite as rw
R = os.path.join(HERE, "..", "..", "src")

def main():
    a = sys.argv[1:]; dry = "--dry" in a
    if dry: a.remove("--dry")
    src, name, va = a[0], a[1], int(a[2], 16)
    txt = open(src).read()
    if "compile: /G5" not in txt: txt = "/* compile: /G5 */\n" + txt
    tmp = tempfile.mkdtemp(prefix="x87i_"); fp = os.path.join(tmp, name + ".c"); open(fp, "w").write(txt)
    for h in os.listdir(R):      # T-X90: headers (mfl_tier2.h) for files that include them
        if h.endswith(".h"): shutil.copy(os.path.join(R, h), os.path.join(tmp, h))
    ok, det = cmpfn.check(fp, [(name, va)], ["/O2", "/G5"])[name]
    if not ok: print("SKIP %s: %s" % (name, det[:100])); shutil.rmtree(tmp); return 1
    print("OK   %s %s" % (name, det))
    if dry: shutil.rmtree(tmp); return 0
    open(os.path.join(R, "tier1", name + ".c"), "w").write(txt)
    nf = os.path.join(R, "automatch_near_names.txt"); L = open(nf).readlines()
    for l in L:
        p = l.split()
        if len(p) >= 3 and int(p[0], 16) == va:
            ch = os.path.join(R, "automatch_near", "an_" + p[2]); t = open(ch, errors="surrogateescape").read()
            m = next((m for m in rw.DEF.finditer(t) if m.group(1) == p[1]), None)
            if m:
                s = t.rfind("\n/*", 0, m.start()); s = s + 1 if s >= 0 and t[s:m.start()].count("\n") <= 3 else m.start()
                open(ch, "w", errors="surrogateescape").write(t[:s] + t[m.end():])
            open(nf, "w").write("".join(x for x in L if x is not l)); print("removed tier-2 definition %s from %s" % (p[1], p[2])); break
    shutil.rmtree(tmp); return 0

if __name__ == "__main__": sys.exit(main())
