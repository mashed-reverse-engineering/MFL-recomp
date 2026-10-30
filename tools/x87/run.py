#!/usr/bin/env python3
"""run.py [--work DIR] [--flags "/O2 /G5"] [--min N --max N] [--maxsize BYTES] [VA ...]: lift the Rwp functions with MIN..MAX x87 instructions (default 10..49), compile each lifted C with cl and compare with the original.
Writes DIR/<name>.c and DIR/results.csv (name, va, size, x87, status, differing bytes / reason)."""
import csv, json, os, re, sys
HERE = os.path.dirname(os.path.abspath(__file__)); sys.path.insert(0, os.path.join(HERE, "..")); sys.path.insert(0, HERE)
import cmpfn, lift as L

def candidates(lo=10, hi=49):
    out = []
    for l in open(os.path.join(HERE, "..", "..", "output", "rwp_names.csv")):
        if l.startswith("#"): continue
        a, n, c = l.rstrip().split(",", 2); va = int(a, 16); sz = cmpfn.FNSZ.get(va)
        if not sz: continue
        from disx import dis
        x = sum(1 for i in dis(va, sz) if re.match(r"f[a-z]", i[1]))
        if lo <= x <= hi: out.append((va, n, sz, x))
    return sorted(out, key=lambda r: r[2])

def main():
    a = sys.argv[1:]
    def opt(k, d):
        if k in a: i = a.index(k); v = a[i + 1]; del a[i:i + 2]; return v
        return d
    work = opt("--work", "x87work"); flags = opt("--flags", "/O2 /G5").split(); lo = int(opt("--min", "10")); hi = int(opt("--max", "49")); maxsize = int(opt("--maxsize", "100000"))
    os.makedirs(work, exist_ok=True)
    cands = [c for c in candidates(lo, hi) if c[2] <= maxsize]
    if a: cands = [c for c in cands if "%08x" % c[0] in [x.lower().zfill(8) for x in a]]
    rows = []
    for va, rn, sz, x in cands:
        name = L.symbols().get(va, "FUN_%08x" % va)
        try:
            L.PAREN = 0; src = L.lift(va)
            for mode in (1, 2):      # T-X89: flat / parenthesised (RWP-X14) / reversed flat sum chains; the first one that matches is kept
                L.PAREN = mode; src2 = L.lift(va); L.PAREN = 0
                if src2 != src:
                    fp2 = os.path.join(work, name + ".p%d.c" % mode); open(fp2, "w").write(src2)
                    ok2, det2 = cmpfn.check(fp2, [(name, va)], flags)[name]
                    if ok2: src = src2; break
        except L.Unsupported as e: rows.append((name, "%08x" % va, sz, x, "UNSUPPORTED", str(e))); print(rows[-1], flush=True); continue
        except Exception as e: rows.append((name, "%08x" % va, sz, x, "CRASH", repr(e))); print(rows[-1], flush=True); continue
        fp = os.path.join(work, name + ".c"); open(fp, "w").write(src)
        ok, det = cmpfn.check(fp, [(name, va)], flags)[name]
        st = "MATCH" if ok else ("COMPILE" if det.startswith("compile error") else "DIFF")
        rows.append((name, "%08x" % va, sz, x, st, det[:120].replace("\n", " "))); print(rows[-1], flush=True)
    with open(os.path.join(work, "results.csv"), "w", newline="") as f: csv.writer(f).writerows(rows)
    with open(os.path.join(work, "names.txt"), "w") as f:      # for bytematch_permute (X87=1 --src DIR): the functions that lifted and compiled but differ
        for r in rows:
            if r[4] == "DIFF": f.write("%s %s\n" % (r[1], r[0]))

if __name__ == "__main__": main()
