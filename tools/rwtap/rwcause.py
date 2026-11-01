#!/usr/bin/env python3
"""rwcause.py <replay.d3d> <orig.d3d> [--ulp 16] [--top 4] [--ignore RE] [--sig]: differing keys sorted into causes (harness metadata, state the replay cannot know, D3D defaults the original
writes explicitly); whatever no rule explains is listed with its most frequent (replay -> original) value pairs.  Exit 1 if any draw has an unexplained difference.  --sig: also the draws grouped by their set of differing keys.  Both traces are paired by order (rwreplay.sh ATOMS=1 cuts the original to the replayed draws)."""
import argparse, collections, os, re, sys
sys.path.insert(0, os.path.join(os.path.dirname(os.path.abspath(__file__)), "..", "d3dtrace"))
import diff as D
ap = argparse.ArgumentParser(); ap.add_argument("a"); ap.add_argument("b"); ap.add_argument("--ulp", type=int, default=16); ap.add_argument("--top", type=int, default=4)
ap.add_argument("--ignore", action="append", default=[]); ap.add_argument("--sig", action="store_true"); o = ap.parse_args()
_, ra = D.load(o.a); _, rb = D.load(o.b); ign = [re.compile(x) for x in o.ignore]
# --- cause rules: a differing key is explained when it is harness metadata, a value the replay cannot know, or the original writing a D3D default explicitly
RS = {7: "1", 8: "3", 9: "2", 14: "1", 15: "0", 16: "1", 19: "2", 20: "1", 22: "3", 23: "4", 24: "0", 25: "8", 26: "0", 27: "0", 28: "0", 29: "0", 34: "0", 35: "0", 36: "~0", 37: "~1", 38: "~1",
      48: "0", 52: "0", 53: "1", 54: "1", 55: "1", 56: "8", 57: "0", 58: "ffffffff", 59: "ffffffff", 60: "ffffffff", 136: "1", 137: "1", 139: "0", 140: "0", 141: "1", 142: "1", 143: "0",
      145: "1", 146: "2", 147: "0", 148: "0", **{i: "0" for i in range(128, 136)}}
SS = {1: "1", 2: "1", 3: "1", 4: "0", 5: "1", 6: "1", 7: "0", 10: "1"}                       # D3D9 sampler defaults (address wrap, border 0, point filters, no mip filter, anisotropy 1)
def tss_default(stage, t): return {1: "4" if stage == 0 else "1", 2: "2", 3: "1", 4: "2" if stage == 0 else "1", 5: "2", 6: "1", 11: str(stage), 24: "0", 26: "1"}.get(t)
def default_of(k):
    p = k.split(".")
    if p[0] == "rs" and int(p[1]) in RS: return RS[int(p[1])]
    if p[0] == "ss" and int(p[2]) in SS: return SS[int(p[2])]
    if p[0] == "tss": return tss_default(int(p[1]), int(p[2]))
    if p[0] in ("ps", "vs") or (p[0] == "tex" and p[1] != "0"): return "none"                       # unbound
    if p[0] == "src" and p[1] != "0": return "0|0|#-1"                                               # null stream
    if p[0] in ("rt", "zs"): return "*"                                                              # default render target / depth surface (harness)
    return None
def stage_op(r, st, op):   # COLOROP (1) / ALPHAOP (4) of a stage, D3D default when never set
    return r.get("tss.%d.%d" % (st, op)) or {1: "4" if st == 0 else "1", 4: "2" if st == 0 else "1"}[op]
def uses_tfactor(r):
    return any((r.get("tss.%d.%d" % (st, t)) or tss_default(st, t)) == "3" and stage_op(r, st, op) != "1" for st in range(8) for t, op in ((2, 1), (3, 1), (5, 4), (6, 4)))
def cause(k, a, b, x=None, y=None):
    if k in ("phase", "racef", "pre.events", "frame"): return "harness: game phase / frame events are not replayed"
    if k == "fvf": return "stale SetFVF left by draws that are not replayed (the declaration overrides it)"
    if re.match(r"len\.\d$", k): return "streams 1..7: replay unbinds explicitly, original never bound them"
    if re.match(r"(vsc|psc)\.", k): return "VS/PS constants uploaded by RW although no shader is bound (unused by the fixed function pipeline)"
    if x is not None:
        m = re.match(r"ss\.(\d)\.\d+$", k)
        if m and x.get("tex." + m.group(1), "none").startswith("none") and y.get("tex." + m.group(1), "none").startswith("none"):
            return "sampler state of a stage with no texture bound (unused; left by passes the replay does not run)"
        if k == "rs.60" and not uses_tfactor(x) and not uses_tfactor(y): return "D3DRS_TEXTUREFACTOR left by passes that are not replayed; no texture stage of this draw reads it"
        m = re.match(r"tss\.(\d)\.([2356])$", k)
        if m and stage_op(x, int(m.group(1)), 1 if m.group(2) in "23" else 4) == "1" and stage_op(y, int(m.group(1)), 1 if m.group(2) in "23" else 4) == "1":
            return "argument of a disabled texture stage (unused)"
        if k.startswith("light.") and x.get("rs.137") == "0" and y.get("rs.137") == "0": return "D3D light slot left by atomics of other cameras; lighting is off for this draw"
        if k == "prim":
            pa, pb = a.split("|"), b.split("|")
            if len(pa) == len(pb) and pa[0] == pb[0] and pa[2:] == pb[2:]: return "BaseVertexIndex: RW packs geometries into shared vertex buffers, librw has one buffer per geometry"
        if re.match(r"xf\.(1[6-9]|2[0-3])$", k) and all(x.get("tss.%d.24" % s_, "0") == "0" for s_ in range(8)): return "texture matrices (TEXTURE0..7) set by other passes; texture transforms are off for this draw (masked in the mhash)"
    if k == "rs.136" and a == "<none>": return "D3DRS_CLIPPING is written by the render callbacks (frustum test); none was replayed (title mode)"
    if k in ("vh", "ih", "xf.2", "xf.256") : return "geometry / camera / world values masked in the mhash by tocsv.py"
    d = default_of(k)
    for x, y in ((a, b), (b, a)):
        if x == "<none>" and d is not None and (d == "*" or D.eq(y, d, 16, True) or (d == "0|0|#-1" and y.startswith("0|0|"))): return ("original sets the D3D default explicitly" if x is a else "replay sets the D3D default explicitly")
    if k in ("rt", "zs") : return "render target / depth surface ids (harness)"
    return None
per, sig, n, unex = collections.defaultdict(collections.Counter), collections.Counter(), 0, 0
bycause, detail = collections.defaultdict(collections.Counter), collections.defaultdict(collections.Counter)
for (_, x), (_, y) in zip(ra, rb):
    ks = sorted(k for k in set(x) | set(y) if not any(r.search(k) for r in ign) and not D.eq(x.get(k, "<none>"), y.get(k, "<none>"), o.ulp, True))
    n += 1; sig[tuple(ks)] += 1; bad = False
    for k in ks:
        a, b = x.get(k, "<none>"), y.get(k, "<none>"); cz = cause(k, a, b, x, y)
        if cz: bycause[cz][k] += 1
        else: bycause["UNEXPLAINED"][k] += 1; detail[k][(a[:90], b[:90])] += 1; bad = True
    unex += bad
print("%d pairs (replay %d, orig %d), %d draws differ" % (n, len(ra), len(rb), sum(v for k, v in sig.items() if k)))
print("causes (key: draws):")
for cz, ks in sorted(bycause.items(), key=lambda kv: kv[0] == "UNEXPLAINED"):
    print("  %s: %d keys, e.g. %s" % (cz, len(ks), ", ".join("%s %d" % kv for kv in ks.most_common(6))))
if detail: print("UNEXPLAINED detail:")
for k, c in sorted(detail.items(), key=lambda kv: -sum(kv[1].values())):
    print("%-10s %5d draws, %d distinct pairs" % (k, sum(c.values()), len(c)))
    for (a, b), m in c.most_common(o.top): print("      %5d  replay=%s  orig=%s" % (m, a, b))
print("draws with an unexplained difference: %d of %d" % (unex, n))
if o.sig:
    print("signatures:")
    for k, m in sig.most_common(30): print("%6d  %s" % (m, " ".join(k) or "(equal)"))
sys.exit(1 if unex else 0)
