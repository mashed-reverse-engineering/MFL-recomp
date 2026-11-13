#!/usr/bin/env python3
"""bindzero.py [--selftest] [dir]: no 0 slot in a mflrw.dll bind table (W6 T6).
Scans src/automatch_near/an_hk_612.c and an_hk_612_*.inc (or the same files in [dir]) for `void *const <name>[N] = { ... };` tables; every
element that is a plain 0 must be listed in src/automatch_near/bind_zero_allow.txt as `<table> <index> <reason>`.  A trap row's slot is its
trap stub (HK_TRAP_STUB in an_hk_612.c), not 0: a call through 0 faults without a name.  Exit 1 on an unlisted 0 slot or a stale allow line."""
import glob, os, re, sys
G = os.path.normpath(os.path.join(os.path.dirname(os.path.abspath(__file__)), ".."))
TAB = re.compile(r"void\s*\*\s*const\s+(\w+)\s*\[[^\]]*\]\s*=\s*\{(.*?)\}\s*;", re.S)


def zeros(text):
    """-> [(table, index)] of the plain 0 elements"""
    text = re.sub(r"/\*.*?\*/", "", text, flags=re.S)
    return [(m.group(1), i) for m in TAB.finditer(text) for i, e in enumerate(m.group(2).split(",")) if re.fullmatch(r"\(?\s*(void\s*\*\s*\))?\s*0\s*\)?", e.strip())]


def main(argv):
    if "--selftest" in argv:
        t = "static void *const tA[4] = {(void *)f, 0 /* trap */, &g,\n 0};\nstatic void *const tB[2] = {(void *)0x10, (void *)f0};"
        assert zeros(t) == [("tA", 1), ("tA", 3)], zeros(t)
        print("bindzero selftest ok"); return 0
    d = argv[0] if argv else os.path.join(G, "src", "automatch_near")
    found = []
    for f in sorted(glob.glob(os.path.join(d, "an_hk_612_*.inc")) + glob.glob(os.path.join(d, "an_hk_612.c"))):
        found += [(t, i, os.path.basename(f)) for t, i in zeros(open(f, errors="replace").read())]
    ap = os.path.join(d, "bind_zero_allow.txt")
    allow = {(l.split()[0], int(l.split()[1])) for l in (open(ap) if os.path.exists(ap) else []) if l.strip() and not l.startswith("#")}
    bad = ["0 slot %s[%d] in %s (use the row's trap stub, or list it in bind_zero_allow.txt with a reason)" % x for x in found if x[:2] not in allow]
    bad += ["stale allow line %s %d (no such 0 slot)" % x for x in sorted(allow - {x[:2] for x in found})]
    print("\n".join(bad + ["bindzero: %d zero slots, %d listed, %d problems" % (len(found), len(allow), len(bad))]))
    return 1 if bad else 0


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
