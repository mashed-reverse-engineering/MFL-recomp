#!/usr/bin/env python3
"""tocsv.py <trace.d3d[.gz]> <out prefix>: per-draw hash CSV (<prefix>.draws.csv: draw,frame,phase,racef,call,prim,hash,mhash; mhash = hash without the MASK keys) and per-frame hash CSV
(<prefix>.frames.csv: frame,draws,hash).  hash = md5[:16] of the record without draw index / frame / racef (effective state of the draw), the frame hash
chains the draw hashes.  Floats are the exact %.9g text, so two recordings are identical iff the files are (diff.py does the ulp-tolerant compare)."""
import gzip, hashlib, sys
MASK = ("xf.2", "xf.16", "xf.17", "xf.18", "xf.19", "xf.20", "xf.21", "xf.22", "xf.23", "xf.256", "vh", "ih", "rt")      # camera / car / geometry values driven by the (non reproducible) game clock and physics
src, pre = sys.argv[1], sys.argv[2]
op = gzip.open if src.endswith(".gz") else open
fr = {}; d = open(pre + ".draws.csv", "w"); d.write("draw,frame,phase,racef,call,prim,hash,mhash\n")
for ln in op(src, "rt"):
    if not ln.startswith("R\t") and not ln.startswith("R "): continue
    t = ln.rstrip("\n").split("\t"); idx = t[0].split()[1]
    kv = dict(x.split("=", 1) for x in t[1:] if "=" in x)
    h = hashlib.md5("\t".join(x for x in t[1:] if x.split("=", 1)[0] not in ("frame", "racef")).encode()).hexdigest()[:16]
    def dyn(x):      # start index into a dynamic (ring) index buffer depends on history, not on the draw
        if x.startswith("prim=") and "#dyn" in kv.get("ib", "") and x.count("|") > 4: p = x.split("|"); p[4] = "dyn"; return "|".join(p)
        return x
    mh = hashlib.md5("\t".join(dyn(x) for x in t[1:] if x.split("=", 1)[0] not in ("frame", "racef") and x.split("=", 1)[0] not in MASK).encode()).hexdigest()[:16]
    d.write("%s,%s,%s,%s,%s,%s,%s,%s\n" % (idx, kv.get("frame"), kv.get("phase"), kv.get("racef"), kv.get("call"), kv.get("prim", "").replace("|", ":"), h, mh))
    fr.setdefault(kv.get("frame"), []).append(h)
with open(pre + ".frames.csv", "w") as f:
    f.write("frame,draws,hash\n")
    for k, v in fr.items(): f.write("%s,%d,%s\n" % (k, len(v), hashlib.md5("".join(v).encode()).hexdigest()[:16]))
