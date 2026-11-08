#!/usr/bin/env python3
"""ringshift.py <in trace.d3d.gz> <out trace.d3d.gz> <shift> [bad]: V2 fixture (drawcmp dynamic VB ring rule).
Every DP / DIP whose stream is `#dyn` gets StartVertex / BaseVertexIndex + shift and every #dyn stream offset + 4*shift (same vertex bytes, other ring
position: vh unchanged).  `bad` also flips the vh of the first ring draw (content at the shifted position is wrong): that one must still FAIL.
Kept outputs: ref/neg_ringshift.d3d.gz (must PASS), ref/neg_ringbad.d3d.gz (must FAIL), both from ref/neg_ctrl.d3d.gz with shift 40."""
import gzip, sys

def main(src, dst, shift, bad):
    out = gzip.GzipFile(dst, "wb", mtime=0); flipped = False
    for ln in gzip.open(src, "rt"):
        if ln.startswith("R "):
            f = ln.rstrip("\n").split("\t"); kv = [x.split("=", 1) for x in f[1:]]; d = dict(kv)
            if d.get("call") in ("DP", "DIP") and any(k.startswith("src.") and v.endswith("|#dyn") for k, v in kv):
                for x in kv:
                    if x[0] == "prim": pp = x[1].split("|"); pp[1] = str(int(pp[1]) + shift); x[1] = "|".join(pp)
                    elif x[0].startswith("src.") and x[1].endswith("|#dyn"): a = x[1].split("|"); a[0] = str(int(a[0]) + 4 * shift); x[1] = "|".join(a)
                    elif x[0] == "vh" and bad and not flipped: x[1] = "%016x" % (int(x[1], 16) ^ 1); flipped = True
                ln = "\t".join([f[0]] + ["=".join(x) for x in kv]) + "\n"
        out.write(ln.encode())
    out.close()

if __name__ == "__main__":
    if len(sys.argv) < 4: sys.exit(__doc__)
    main(sys.argv[1], sys.argv[2], int(sys.argv[3]), "bad" in sys.argv[4:])
