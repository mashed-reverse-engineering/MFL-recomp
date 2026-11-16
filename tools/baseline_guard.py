#!/usr/bin/env python3
"""baseline_guard.py [--allow FILE] [--tree DIR] [--rev REV]   |   --selftest
W11 B12: after a gate with GATE_BASELINE_WRITE=1 (or any baseline rewrite), before M commits the baseline: diff every tracked output/*baseline*
file of the tree (default this ghidra/ tree; --tree = a scratch copy laid out the same way) against git REV (default HEAD), key by key.
A line's key is the line without its last field ("datalib:RenderWare:bss 28995" -> "datalib:RenderWare:bss"); '#' lines are printed, never judged.
--allow FILE: the lines the batch declared in its READY report, one per line: "<baseline file name> <key>" (e.g. "zero_original_baseline.txt
data:bss:lib"), or "<baseline file name> *" for a whole file.  Exit 1 when a key outside that list changed, appeared or vanished; 0 otherwise."""
import os, subprocess, sys
G = os.path.normpath(os.path.join(os.path.dirname(os.path.abspath(__file__)), ".."))

def keyed(text):
    d = {}
    for l in text.splitlines():
        if l.strip() and not l.startswith("#"):
            p = l.rsplit(None, 1); d[p[0] if len(p) > 1 else l] = l
    return d

def judge(name, old, new, allow):
    """-> (changed lines for the printout, the ones not allowed)"""
    o, n = keyed(old), keyed(new); ch = []; bad = []
    for k in sorted(set(o) | set(n)):
        if o.get(k) == n.get(k): continue
        ch.append("%s: %s -> %s" % (name, o.get(k, "<none>"), n.get(k, "<none>")))
        if (name, k) not in allow and (name, "*") not in allow: bad.append(ch[-1])
    for l in sorted(set(x for x in new.splitlines() if x.startswith("#")) - set(old.splitlines())): ch.append("%s: comment now %s" % (name, l))
    return ch, bad

def files(rev):
    out = subprocess.run(["git", "-C", G, "ls-files", "--full-name", ":/output/*baseline*"], capture_output=True, text=True, check=True).stdout.split()      # :/ + --full-name: also from a scratch tree inside the repo
    return [p for p in out if not p.endswith(".proposed.txt")]

def selftest():
    old = "# link a\nx:1 5\ny:2 7\n"
    assert judge("b.txt", old, "# link b\nx:1 4\ny:2 7\n", {("b.txt", "x:1")})[1] == []                     # declared line changed: ok
    ch, bad = judge("b.txt", old, "# link b\nx:1 4\ny:2 8\n", {("b.txt", "x:1")})                            # planted extra change: FAIL
    assert len(bad) == 1 and "y:2" in bad[0], bad
    assert len(judge("b.txt", old, old + "z:3 1\n", set())[1]) == 1                                           # new key: FAIL
    assert len(judge("b.txt", old, "# link a\nx:1 5\n", set())[1]) == 1                                       # vanished key: FAIL
    assert judge("b.txt", old, "# link b\nx:1 9\ny:2 9\n", {("b.txt", "*")})[1] == []
    print("baseline_guard selftest: ok (declared change passes, extra / new / vanished key FAIL, * covers a file)")

def main(a):
    if a[:1] == ["--selftest"]: return selftest()
    allow, tree, rev = set(), G, "HEAD"
    while a:
        if a[0] == "--allow": allow = {tuple(l.split(None, 1)) for l in (x.strip() for x in open(a[1])) if l and not l.startswith("#")}; a = a[2:]
        elif a[0] == "--tree": tree = a[1]; a = a[2:]
        elif a[0] == "--rev": rev = a[1]; a = a[2:]
        else: print(__doc__); return 2
    allbad = []
    for p in files(rev):
        old = subprocess.run(["git", "-C", G, "show", "%s:%s" % (rev, p)], capture_output=True, text=True).stdout
        np_ = os.path.join(tree, p); new = open(np_).read() if os.path.exists(np_) else ""
        ch, bad = judge(os.path.basename(p), old, new, allow)
        for l in ch: print(("NOT DECLARED " if l in bad else "declared     ") + l if not l.split(": ", 1)[1].startswith("comment") else "info         " + l)
        allbad += bad
    print("baseline_guard: %d undeclared change(s) against %s%s" % (len(allbad), rev, "" if allbad else " (ok)"))
    return 1 if allbad else 0

if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
