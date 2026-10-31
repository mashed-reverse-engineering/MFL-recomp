#!/usr/bin/env python3
"""Compare oracle rows (walk.py) with librw rows (tests/asset_dump).  Usage: compare.py <librw.csv> <oracle.csv>...
Global rows (G M T TD) are keyed by their leading identity columns: identical row = matched; otherwise the closest librw row with the same key is compared field by field.
Clump / world blocks ("#CLUMP key" / "#WORLD key" + rows) are paired with the librw block of the same key that differs least; inside a block the rows of one
(family, key) are a multiset: identical rows cancel, the rest are paired by smallest field difference ('fielddiff') or have no counterpart.
env MASK=0 switches the masking of runtime / backend fields off (see mask())."""
import sys, collections, os
FIELDS = {
 "C": ["natoms", "nframes"],
 "F": ["r.x", "r.y", "r.z", "u.x", "u.y", "u.z", "a.x", "a.y", "a.z", "p.x", "p.y", "p.z", "mflags"],
 "A": ["bs.x", "bs.y", "bs.z", "bs.r", "frame", "flags", "interp_flags", "pipeline", "renderCB"],
 "G": ["flags", "nverts", "ntris", "nmorph", "ntexsets", "nmats", "bs.x", "bs.y", "bs.z", "bs.r", "h_verts", "h_normals", "h_tris", "h_uv0", "h_uv1", "h_colors"],
 "M": ["rgba", "ambient", "specular", "diffuse", "tex", "mask"], "T": ["filteraddr", "w", "h", "depth"], "TD": ["h_names"],
 "W": ["flags", "ntexsets", "sup.x", "sup.y", "sup.z", "inf.x", "inf.y", "inf.z"], "WP": ["type", "value", "leftValue", "rightValue"],
 "WS": ["nverts", "ntris", "matbase", "tight.sup.x", "tight.sup.y", "tight.sup.z", "tight.inf.x", "tight.inf.y", "tight.inf.z", "outer.sup.x", "outer.sup.y", "outer.sup.z", "outer.inf.x", "outer.inf.y", "outer.inf.z", "h_verts", "h_tris", "has_normals", "h_uv0", "h_prelit"],
 "WM": ["rgba", "ambient", "specular", "diffuse", "tex", "mask"]}
KEYN = {"C": 1, "F": 2, "A": 2, "G": 1, "M": 2, "T": 2, "TD": 1, "W": 1, "WP": 2, "WS": 2, "WM": 2}   # columns after the family that identify the row (F: clump key, depth; A: clump key, gkey)
BLOCKFAM = ("C", "F", "A", "W", "WP", "WS", "WM")
MASK = os.environ.get("MASK", "1") == "1"
def mask(r):
    """blank fields the game (or the null raster driver) changes after loading: root frame placement, atomic render flags / pipeline / render callback,
    geometry flag bits 0x60 (game sets lighting / modulate), world flag 0x40, raster depth (D3D9 raster is 32 bit)."""
    if not MASK: return r
    p = r.split(",")
    if p[0] == "F" and p[2] == "0": p[3:] = ["*"] * 13
    elif p[0] == "A": p[8] = p[10] = p[11] = "*"
    elif p[0] == "G": p[2] = "%08x" % (int(p[2], 16) & ~0x60)
    elif p[0] == "T": p[6] = "*"
    elif p[0] == "W": p[2] = "%08x" % (int(p[2], 16) & ~0x40)
    return ",".join(p)
def load(fn):
    g, blocks, cur = set(), [], None
    for l in open(fn):
        l = l.rstrip("\r\n")
        if not l.strip(): continue
        if l.startswith("#CLUMP ") or l.startswith("#WORLD "): cur = (l.split()[0][1] + ":" + l.split()[1], []); blocks.append(cur)
        elif l.split(",")[0] in BLOCKFAM and cur is not None: cur[1].append(mask(l))
        else: g.add(mask(l)); cur = None
    return g, blocks
def split(r):
    p = r.split(","); f = p[0]; k = KEYN[f]
    return f, tuple(p[1:1 + k]), p[1 + k:]
def main(lib, oras):
    LG, LB = load(lib); idx = collections.defaultdict(list); bidx = collections.defaultdict(list)
    for r in LG:
        f, k, v = split(r); idx[(f, k)].append(v)
    for ck, rs in LB: bidx[ck].append(rs)
    stat = collections.defaultdict(lambda: [0, 0, 0, 0])   # family: oracle rows, matched, field diff rows, no counterpart
    fdiff = collections.defaultdict(int); fcmp = collections.defaultdict(int); samples = {}
    def eq_fields(f, n=1):
        for nm in FIELDS[f]: fcmp[(f, nm)] += n
    def diff_row(f, k, best, v, s):
        s[2] += 1
        names = FIELDS[f]
        for i, (a, b) in enumerate(zip(best, v)):
            n = names[i] if i < len(names) else str(i)
            fcmp[(f, n)] += 1
            if a != b: fdiff[(f, n)] += 1; samples.setdefault((f, n), []).append("librw %s oracle %s (%s)" % (a, b, ",".join(k)))
    for fn in oras:
        og, ob = load(fn)
        for r in og:
            f, k, v = split(r); s = stat[f]; s[0] += 1
            if r in LG: s[1] += 1; eq_fields(f); continue
            c = idx.get((f, k))
            if not c: s[3] += 1; samples.setdefault((f, "no counterpart"), []).append(r); continue
            diff_row(f, k, min(c, key=lambda x: sum(a != b for a, b in zip(x, v))), v, s)
        for ck, rs in ob:
            cands = bidx.get(ck)
            if not cands:
                for r in rs: stat[r.split(",")[0]][0] += 1; stat[r.split(",")[0]][3] += 1
                samples.setdefault((ck.split(":")[0], "no counterpart"), []).append(ck); continue
            best = min(cands, key=lambda c: len(collections.Counter(rs) - collections.Counter(c)) + len(collections.Counter(c) - collections.Counter(rs)))
            # multiset per (family, key)
            og_, bg_ = collections.defaultdict(collections.Counter), collections.defaultdict(collections.Counter)
            for r in rs: f, k, v = split(r); og_[(f, k)][tuple(v)] += 1
            for r in best: f, k, v = split(r); bg_[(f, k)][tuple(v)] += 1
            for (f, k), cnt in og_.items():
                s = stat[f]; common = cnt & bg_.get((f, k), collections.Counter())
                n_eq = sum(common.values()); s[0] += sum(cnt.values()); s[1] += n_eq; eq_fields(f, n_eq)
                left_o = list((cnt - common).elements()); left_b = list((bg_.get((f, k), collections.Counter()) - common).elements())
                for v in left_o:
                    if not left_b: s[3] += 1; continue
                    j = min(range(len(left_b)), key=lambda x: sum(a != b for a, b in zip(left_b[x], v)))
                    diff_row(f, k, left_b.pop(j), v, s)
    return stat, fdiff, fcmp, samples
if __name__ == "__main__":
    st, fd, fc, sm = main(sys.argv[1], sys.argv[2:])
    for f, s in sorted(st.items()): print("%-3s oracle %6d matched %6d fielddiff %6d nocounterpart %6d" % ((f,) + tuple(s)))
    for k, v in sorted(fd.items()): print("DIFF", k, v, "of", fc[k], sm[k][:3])
    for k, v in sorted(sm.items()):
        if k[1] == "no counterpart": print("NOCP", k[0], len(v), v[:3])
