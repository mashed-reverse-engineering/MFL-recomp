#!/usr/bin/env python3
"""diff.py <a.d3d[.gz]> <b.d3d[.gz]> [options]: compare two normalised traces (normalise.py output) draw by draw.
  --ulp N        float tolerance in float32 ulps for '~' values (default 0 = exact; use e.g. 16 for a reimplementation)
  --no-ids       ignore the creation-order '#id' components (use when the object creation order differs between implementations)
  --ignore RE    ignore keys matching the regex (repeatable), e.g. --ignore '^(rs|tss|ss)\\.' or --ignore '^tex\\.\\d+$'
  --only RE      compare only keys matching the regex
  --max N        print at most N differing draws (default 10)
  --mask M.d3d   a third trace (a second recording of A): keys whose values differ between A and M at the same draw index are run-to-run noise and are ignored
  --align KEY    pair records by order within each value of KEY ('frame' pairs the k-th draw of frame f with the k-th draw of frame f)
Prints: header diffs, draw count per frame diffs (first frame where the counts differ), the first differing draws with every differing key and both values,
a histogram of differing keys, and a summary line.  Exit status 0 = identical, 1 = differences."""
import argparse, collections, gzip, re, struct, sys


def load(path):
    op = gzip.open if path.endswith(".gz") else open
    hdr, recs = [], []
    with op(path, "rt") as fh:
        for ln in fh:
            ln = ln.rstrip("\n")
            if ln.startswith("#"):
                hdr.append(ln)
            elif ln.startswith("R "):
                head, *toks = ln.split("\t")
                recs.append((int(head[2:]), dict(t.split("=", 1) for t in toks)))
    return hdr, recs


def ulpdist(a, b):
    def k(x):
        i = struct.unpack("<i", struct.pack("<f", x))[0]
        return i if i >= 0 else -(i & 0x7fffffff)
    return abs(k(a) - k(b))


ID = re.compile(r"#-?\d+")


def eq(va, vb, ulp, noids):
    if noids:
        va, vb = ID.sub("#", va), ID.sub("#", vb)
    if va == vb:
        return True
    if "~" not in va:
        return False
    ta, tb = re.split(r"([ |,])", va), re.split(r"([ |,])", vb)
    if len(ta) != len(tb):
        return False
    for x, y in zip(ta, tb):
        if x == y:
            continue
        if x.startswith("~") and y.startswith("~"):
            try:
                if ulpdist(float(x[1:]), float(y[1:])) <= ulp:
                    continue
            except (ValueError, OverflowError):
                pass
        return False
    return True


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("a"); ap.add_argument("b")
    ap.add_argument("--ulp", type=int, default=0); ap.add_argument("--no-ids", action="store_true")
    ap.add_argument("--ignore", action="append", default=[]); ap.add_argument("--only")
    ap.add_argument("--mask"); ap.add_argument("--max", type=int, default=10); ap.add_argument("--align")
    o = ap.parse_args()
    ha, ra = load(o.a); hb, rb = load(o.b)
    ign = [re.compile(x) for x in o.ignore]; only = re.compile(o.only) if o.only else None
    keep = lambda k: not any(r.search(k) for r in ign) and (only is None or only.search(k))
    bad = 0
    sa, sb = set(ha), set(hb)
    for l in sorted(sa - sb)[:20]: print("header only in A:", l[:200]); bad += 1
    for l in sorted(sb - sa)[:20]: print("header only in B:", l[:200]); bad += 1
    fa = collections.Counter(r.get("frame") for _, r in ra); fb = collections.Counter(r.get("frame") for _, r in rb)
    fd = [(f, fa[f], fb[f]) for f in sorted(set(fa) | set(fb), key=lambda x: int(x)) if fa[f] != fb[f]]
    if fd:
        print("frames with different draw counts: %d, first: frame %s A=%d B=%d" % (len(fd), *fd[0])); bad += 1
    if o.align:
        def grp(rs):
            g = collections.defaultdict(list)
            for i, r in rs: g[r.get(o.align)].append((i, r))
            return g
        ga, gb = grp(ra), grp(rb)
        pairs = []
        for k in sorted(set(ga) | set(gb), key=lambda x: (int(x) if x and x.isdigit() else -1)):
            pairs += list(zip(ga.get(k, []), gb.get(k, [])))
    else:
        pairs = list(zip(ra, rb))
    if len(ra) != len(rb): print("draw count: A=%d B=%d" % (len(ra), len(rb))); bad += 1
    noise = {}
    if o.mask:
        _, rm = load(o.mask)
        for (ia, x), (_, m) in zip(ra, rm):
            nk = {k for k in set(x) | set(m) if x.get(k) != m.get(k)}
            if nk: noise[ia] = nk
        print("noise mask: %d draws of A differ from the mask recording (%d keys)" % (len(noise), sum(map(len, noise.values()))))
    hist = collections.Counter(); ndiff = 0; shown = 0
    for (ia, x), (ib, y) in pairs:
        diffs = []
        for k in sorted(set(x) | set(y)):
            if not keep(k) or k in noise.get(ia, ()): continue
            if k not in x: diffs.append((k, None, y[k]))
            elif k not in y: diffs.append((k, x[k], None))
            elif not eq(x[k], y[k], o.ulp, o.no_ids): diffs.append((k, x[k], y[k]))
        if diffs:
            ndiff += 1; hist.update(k for k, _, _ in diffs)
            if shown < o.max:
                shown += 1
                print("draw A#%d B#%d (frame %s/%s, %s): %d keys differ" % (ia, ib, x.get("frame"), y.get("frame"), x.get("call"), len(diffs)))
                for k, va, vb in diffs[:12]: print("    %-12s A=%s\n    %-12s B=%s" % (k, va, "", vb))
                if len(diffs) > 12: print("    ... %d more" % (len(diffs) - 12))
    if hist: print("differing keys (draws): " + ", ".join("%s %d" % kv for kv in hist.most_common(25)))
    print("compared %d draw pairs (ulp %d%s): %d draws differ, %d header diffs/frame diffs" % (len(pairs), o.ulp, ", ids ignored" if o.no_ids else "", ndiff, bad))
    print("IDENTICAL" if not ndiff and not bad and len(ra) == len(rb) else "DIFFERENT")
    sys.exit(0 if not ndiff and not bad and len(ra) == len(rb) else 1)


main()
