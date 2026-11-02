#!/usr/bin/env python3
"""drawcmp.py <ours trace.d3d.gz> <ctrl trace.d3d.gz> [max prim] [max mhash] [max unmatched]: render oracle of gate.sh standalone (T-P18).
The draws of each frame are aligned (difflib) on a key that does not contain the vertex / index ranges (call, fvf, texture 0 content hash, primitive type,
NumVertices, primCount); every aligned pair is compared on
  prim   the full prim field (type|BaseVertexIndex|MinVertexIndex|NumVertices|startIndex|primCount): T-P18 swapped Base / MinVertexIndex
  mhash  the draw record without the MASK keys of tocsv.py (camera / car / geometry values of the game clock) and without `zs`, with the object ids
         (`#N`, creation order of textures / buffers / declarations: differ between the standalone and the control) removed and floats rounded to 5 digits
Draws that align with nothing (extra / missing draws) are counted as unmatched.  Prints the totals and the first differences; exit 1 when a total exceeds its limit."""
import collections, difflib, gzip, hashlib, re, sys
MASK = {"xf.2", "xf.16", "xf.17", "xf.18", "xf.19", "xf.20", "xf.21", "xf.22", "xf.23", "xf.256", "vh", "ih", "rt", "zs", "frame", "racef"}      # tocsv.py MASK + zs (render target / depth surface ids)
ID = re.compile(r"#-?\d+"); FL = re.compile(r"~(-?[0-9.e+-]+)")
def fl(m):
    try: return "~%.5g" % float(m.group(1))
    except ValueError: return m.group(0)
def load(p):
    fr = collections.defaultdict(list)
    for ln in (gzip.open if p.endswith(".gz") else open)(p, "rt"):
        if not ln.startswith("R "): continue
        kv = [x.split("=", 1) for x in ln.rstrip("\n").split("\t")[1:] if "=" in x]
        d = dict(kv); prim = d.get("prim", ""); pp = prim.split("|")
        if "#dyn" in d.get("ib", "") and len(pp) > 5: pp[4] = "dyn"; prim = "|".join(pp)      # ring index buffer: the start depends on history
        key = (d.get("call"), d.get("fvf"), ID.sub("", d.get("tex.0", "")), pp[0], pp[3] if len(pp) > 5 else "", pp[5] if len(pp) > 5 else "")
        mh = hashlib.md5("\t".join("%s=%s" % (k, prim if k == "prim" else FL.sub(fl, ID.sub("#", v))) for k, v in kv if k not in MASK).encode()).hexdigest()[:16]
        fr[int(d.get("frame", -1))].append((key, prim, mh, d.get("call", "")))
    return fr
def main(a, b, lim):
    A, B = load(a), load(b); tot = collections.Counter(); first = {}
    common = sorted(set(A) & set(B))
    for f in common:
        x, y = A[f], B[f]
        for op, i1, i2, j1, j2 in difflib.SequenceMatcher(None, [r[0] for r in x], [r[0] for r in y], autojunk=False).get_opcodes():
            if op != "equal":
                tot["unmatched"] += max(i2 - i1, j2 - j1); first.setdefault("unmatched", "frame %d: ours draws %d..%d %s, ctrl draws %d..%d %s" % (f, i1, i2 - 1, [r[1] for r in x[i1:i1 + 2]], j1, j2 - 1, [r[1] for r in y[j1:j1 + 2]])); continue
            for i, j in zip(range(i1, i2), range(j1, j2)):
                tot["draws"] += 1
                if x[i][1] != y[j][1]: tot["prim"] += 1; first.setdefault("prim", "frame %d draw %d/%d %s: ours %s ctrl %s" % (f, i, j, x[i][3], x[i][1], y[j][1]))
                if x[i][2] != y[j][2]: tot["mhash"] += 1; first.setdefault("mhash", "frame %d draw %d/%d %s" % (f, i, j, x[i][3]))
    print("drawcmp: %d common frames (ours %d, ctrl %d), %d draw pairs: prim differs %d (limit %d), mhash differs %d (limit %d), unmatched draws %d (limit %d)"
          % (len(common), len(A), len(B), tot["draws"], tot["prim"], lim[0], tot["mhash"], lim[1], tot["unmatched"], lim[2]))
    for k in ("prim", "mhash", "unmatched"):
        if k in first: print("  first %s difference: %s" % (k, first[k]))
    bad = not tot["draws"] or tot["prim"] > lim[0] or tot["mhash"] > lim[1] or tot["unmatched"] > lim[2]
    print("drawcmp: " + ("FAIL" if bad else "ok")); return 1 if bad else 0
if __name__ == "__main__":
    if len(sys.argv) < 3: sys.exit(__doc__)
    sys.exit(main(sys.argv[1], sys.argv[2], ([int(x) for x in sys.argv[3:6]] + [0, 0, 0])[:3]))
