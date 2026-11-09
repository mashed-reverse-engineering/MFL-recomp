#!/usr/bin/env python3
"""tier1_join.py [--manifest <csv>] [--write]: engine-switch window 3 lane T step 0.
Every engine-flip manifest row (all routes) whose name has a src/tier1/<name>.c (or automatch_agent/<name>.c) but no fnsizes row at its VA:
that byte-identical C is linked but its original bytes are never dropped (window 2: StrUpr/StrLwr/StrChr/StrRChr).  Each candidate is compiled
with tools/cmpfn.py (/O2 + the file's `/* compile: ... */` flags) against the manifest VA; MATCH -> row `va name size` (size = our code to the ret).
Groups in SKIP (an active KES lane's group) are listed, not checked.  --write merges the MATCH rows into output/fnsizes.txt (sorted), src/aliases.txt (name not yet a symbol/alias) and
tools/delink/fnsizes_unanalysed.txt (rule 12).  Without --write it only prints.  A MATCH whose extent overlaps the next fnsizes row is reported, not written."""
import bisect, csv, glob, os, re, subprocess, sys
from concurrent.futures import ThreadPoolExecutor
G = os.path.normpath(os.path.join(os.path.dirname(os.path.abspath(__file__)), "..", ".."))
sys.path.insert(0, os.path.join(G, "tools"))
import cmpfn
A = sys.argv[1:]
MAN = A[A.index("--manifest") + 1] if "--manifest" in A else max(glob.glob(os.path.join(G, "output", "engine_flip_manifest_*.csv")))
DIRS = [os.path.join(G, "src", d) for d in ("tier1", "automatch_agent")]
SKIP = {"d3d9raster": "lane X KES slice owns the group (window 3): join only by X's decision"}      # groups an active KES lane binds into the DLL
FS = os.path.join(G, "output", "fnsizes.txt"); AL = os.path.join(G, "src", "aliases.txt"); UN = os.path.join(G, "tools", "delink", "fnsizes_unanalysed.txt")


def src_of(name):
    for d in DIRS:
        for n in (name, name.lstrip("_")):
            p = os.path.join(d, n + ".c")
            if os.path.exists(p): return p, n
    return None, None


def flags_of(p):
    m = re.search(r"/\*\s*compile:([^*]*)\*/", open(p, errors="replace").read())
    return ["/O2"] + [f for f in (m.group(1).split() if m else []) if f.startswith("/")]


def main():
    fs = {int(l.split()[0], 16): l.split()[1] for l in open(FS) if len(l.split()) == 3}
    starts = sorted(fs)
    cand = []
    for r in csv.DictReader(open(MAN)):
        va = int(r["va"], 16)
        p, sym = src_of(r["name"])
        if p and va not in fs and r["group"] not in SKIP: cand.append((r, p, sym, va))
        elif p and va not in fs: print("SKIP     %08x %-40s group %s: %s" % (va, sym, r["group"], SKIP[r["group"]]))
    print("%s: %d candidates" % (os.path.relpath(MAN, G), len(cand)))
    os.makedirs(cmpfn.WORKROOT, exist_ok=True)
    subprocess.run([cmpfn.VC7, cmpfn.WORKROOT, "true"], env=dict(os.environ, VC7_SHELL="1"), capture_output=True)      # start the cmpfn container once, before the threads
    def one(c):
        r, p, sym, va = c
        ok, det = cmpfn.check(p, [(sym, va)], flags_of(p))[sym]
        return c, ok, det, len(cmpfn.LAST[sym][0]) if ok else 0
    with ThreadPoolExecutor(6) as ex: res = list(ex.map(one, cand))
    add = []
    for (r, p, sym, va), ok, det, n in res:
        nx = starts[bisect.bisect_right(starts, va)] if bisect.bisect_right(starts, va) < len(starts) else None
        if ok and nx is not None and va + n > nx: ok, det = False, "overlaps fnsizes row %08x %s" % (nx, fs[nx])
        print("%-8s %08x %-40s %-6s %4s B  %s  %s" % ("MATCH" if ok else "DIFFER", va, sym, r["route"], n or r["bytes"], os.path.relpath(p, G), "" if ok else det[:90]))
        if ok: add.append(("%08x" % va, sym, n))
    print("%d MATCH, %d B" % (len(add), sum(n for _, _, n in add)))
    if "--write" not in A or not add: return
    rows = {l.split()[0]: l.rstrip("\n") for l in open(FS) if l.strip()}
    for va, n, sz in add: rows[va] = "%s %s %d" % (va, n, sz)
    open(FS, "w").write("".join(rows[k] + "\n" for k in sorted(rows)))
    have = {l.split()[0] for l in open(AL) if l.strip() and not l.startswith("#")} | {l.split()[0] for l in open(os.path.join(G, "src", "symbols.txt")) if l.strip()}
    with open(AL, "a") as f:
        for va, n, _ in add:
            if n not in have: f.write("%s %s\n" % (n, va))
    un = {l.split()[0] for l in open(UN) if l.strip() and not l.startswith("#")}
    with open(UN, "a") as f:
        for va, n, _ in add:
            if va not in un: f.write("%s %s\n" % (va, n))
    print("written: fnsizes.txt, aliases.txt, fnsizes_unanalysed.txt (%d rows)" % len(add))


if __name__ == "__main__": main()
