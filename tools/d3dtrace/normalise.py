#!/usr/bin/env python3
"""normalise.py <trace.raw> [out.d3d[.gz]]: turn the proxy's change stream into ONE FULL RECORD PER DRAW (deterministic text).
Output (stdout when no file; .gz is compressed):  '# ...' header lines (caps, device creation, format checks), then per draw
  R <draw index> TAB key=value TAB key=value ...      keys in canonical order (sort_key), values are '|' separated, a trailing '#<id>' component is the creation-order id
Floats are '~' prefixed %.9g (transforms, material, lights, shader constants, viewport depth, float render/texture-stage/sampler states): diff.py compares them with an ulp tolerance.
Resources are resolved at draw time: tex.N = hash|format|WxH|levels|#id (hash = content hash of all mip levels, 'none' = never locked: render target / unknown),
vs/ps = bytecode hash|#id, decl = elements|#id, ib/src = ids and strides.  Events between draws (CLEAR, RESET, PRESENT) are attached to the next record as pre.* keys.
A summary of the draws per frame goes to the header end ('# frames ...')."""
import gzip, io, re, struct, sys

FLOAT_RS = {36, 37, 38, 128, 154, 155, 158, 159, 160, 166, 170, 175, 178, 179, 180, 181, 182, 183, 195}
FLOAT_TSS = {7, 8, 9, 10, 22, 23}
FLOAT_SS = {8}
GROUPS = ["call", "frame", "phase", "racef", "prim", "fvf", "decl", "vs", "ps", "vp", "rt", "zs", "src", "freq", "ib", "vh", "ih", "tex", "rs", "tss", "ss", "xf", "mat", "light", "len", "vsc", "vsi", "vsb", "psc", "clip", "scissor", "pre"]


def sort_key(k):
    p = k.split(".")
    return (GROUPS.index(p[0]) if p[0] in GROUPS else 99, [(0, int(x), "") if x.isdigit() else (1, 0, x) for x in p[1:]])


def f(x):
    return "~%.9g" % float(x)


def fbits(h):
    return f(struct.unpack("<f", struct.pack("<I", int(h, 16)))[0])


def normalise(lines):
    S = {}
    tex = {}      # id -> dict(w, h, lv, fmt, hash)
    vs, ps, decl = {}, {}, {}
    hdr, recs, pre, frames, chk, dyn = [], [], [], {}, {}, {}
    frame = phase = racef = 0
    draw_frame = 0
    for ln in lines:
        p = ln.split()
        if not p:
            continue
        c = p[0]
        if c == "RS":
            i = int(p[1]); S["rs.%d" % i] = fbits(p[2]) if i in FLOAT_RS else p[2]
        elif c == "TSS":
            s, t = int(p[1]), int(p[2]); S["tss.%d.%d" % (s, t)] = fbits(p[3]) if t in FLOAT_TSS else p[3]
        elif c == "SS":
            s, t = int(p[1]), int(p[2]); S["ss.%d.%d" % (s, t)] = fbits(p[3]) if t in FLOAT_SS else p[3]
        elif c == "TEX":
            S["tex.%s" % p[1]] = int(p[2])
        elif c == "XF":
            S["xf.%s" % p[1]] = " ".join(f(x) for x in p[2:])
        elif c == "MAT":
            S["mat"] = " ".join(f(x) for x in p[1:])
        elif c == "LIGHT":
            S["light.%s" % p[1]] = p[2] + " " + " ".join(f(x) for x in p[3:])
        elif c == "LEN":
            S["len.%s" % p[1]] = p[2]
        elif c == "VP":
            S["vp"] = " ".join(p[1:5]) + " " + f(p[5]) + " " + f(p[6])
        elif c == "FVF":
            S["fvf"] = p[1]
        elif c == "DECL":
            S["decl"] = int(p[1])
        elif c == "SRC":
            S["src.%s" % p[1]] = "|".join(p[2:5])   # vb id, offset, stride
        elif c == "FREQ":
            S["freq.%s" % p[1]] = p[2]
        elif c == "IB":
            S["ib"] = int(p[1])
        elif c == "VS":
            S["vs"] = int(p[1])
        elif c == "PS":
            S["ps"] = int(p[1])
        elif c == "VSC":
            S["vsc.%s" % p[1]] = " ".join(f(x) for x in p[2:])
        elif c == "PSC":
            S["psc.%s" % p[1]] = " ".join(f(x) for x in p[2:])
        elif c == "VSI":
            S["vsi.%s" % p[1]] = " ".join(p[2:])
        elif c == "VSB":
            S["vsb.%s" % p[1]] = p[2]
        elif c == "CLIP":
            S["clip.%s" % p[1]] = " ".join(f(x) for x in p[2:])
        elif c == "SCISSOR":
            S["scissor"] = " ".join(p[1:])
        elif c == "RT":
            S["rt"] = int(p[1])
        elif c == "ZS":
            S["zs"] = int(p[1])
        elif c == "STATE":
            S.clear()
        elif c == "RESET":
            S.clear(); pre.append("reset")
        elif c == "CLEAR":
            pre.append("clear:" + ":".join(p[1:3]) + ":" + p[3] + ":" + f(p[4]) + ":" + p[5])
        elif c == "TEXDEF":
            tex[int(p[1])] = dict(w=int(p[2]), h=int(p[3]), lv=int(p[4]), fmt=p[5].split("=")[1], hash="none", us=p[6].split("=")[1])
        elif c == "TEXH":
            tex.setdefault(int(p[1]), dict(w=0, h=0, lv=0, fmt="?", hash="none", us="?"))["hash"] = p[2]
        elif c in ("VBDEF", "IBDEF"):
            dyn[(c, int(p[1]))] = bool(int(p[3].split("=")[1], 16) & 0x200)      # D3DUSAGE_DYNAMIC buffers are a ring picked by history: their id is not part of the record (vh/ih hash the data)
        elif c == "VSDEF":
            vs[int(p[1])] = p[2] + "|" + p[4].split("=")[1]
        elif c == "PSDEF":
            ps[int(p[1])] = p[2] + "|" + p[4].split("=")[1]
        elif c == "DECLDEF":
            decl[int(p[1])] = ",".join(p[2:])
        elif c == "PRESENT":
            frame = int(p[1]) + 1
        elif c == "D":
            r = {"call": p[2], "frame": str(frame), "phase": str(phase), "racef": str(racef)}
            a = p[3:]
            if p[2] == "DP":
                r["prim"] = "|".join(a[:3]); r["vh"] = a[3].split("=")[1]
            elif p[2] == "DIP":
                r["prim"] = "|".join(a[:6]); r["vh"] = a[6].split("=")[1]; r["ih"] = a[7].split("=")[1]
            elif p[2] == "DPUP":
                r["prim"] = "|".join(a[:3]); r["vh"] = a[3]
            else:
                r["prim"] = "|".join(a[:6]); r["ih"] = a[6]; r["vh"] = a[7]
            for k, v in S.items():
                g = k.split(".")[0]
                if g == "tex":
                    t = tex.get(v) if v >= 0 else None
                    r[k] = "none" if v < 0 else ("%s|%s|%dx%d|%d|#%d" % (t["hash"], t["fmt"], t["w"], t["h"], t["lv"], v) if t else "unk|#%d" % v)
                elif g == "vs":
                    r[k] = vs.get(v, "?") + "|#%d" % v if v >= 0 else "none"
                elif g == "ps":
                    r[k] = ps.get(v, "?") + "|#%d" % v if v >= 0 else "none"
                elif g == "decl":
                    r[k] = decl.get(v, "?") + "|#%d" % v if v >= 0 else "none"
                elif g == "ib":
                    r[k] = ("#dyn" if dyn.get(("IBDEF", v)) else "#%d" % v) if v >= 0 else "none"
                elif g == "src":
                    a2 = v.split("|"); r[k] = "|".join(a2[1:]) + ("|#dyn" if dyn.get(("VBDEF", int(a2[0]))) else "|#" + a2[0])
                else:
                    r[k] = v
            if pre:
                r["pre.events"] = ",".join(pre); pre = []
            recs.append((int(p[1]), r))
            frames[frame] = frames.get(frame, 0) + 1
        elif c == "SCENE":
            phase, racef = int(p[1]), int(p[2])
        elif c == "CHECKFMT":
            chk[ln.strip()] = chk.get(ln.strip(), 0) + 1       # the game repeats its format probes: one line each, with a count
        elif c in ("TRACE", "CAPS", "CREATEDEVICE", "WARN"):
            hdr.append("# " + ln.rstrip())
    hdr += ["# %s x%d" % (k, v) for k, v in sorted(chk.items())]
    return hdr, recs, frames


def main():
    src = sys.argv[1]; dst = sys.argv[2] if len(sys.argv) > 2 else None
    op = gzip.open if src.endswith(".gz") else open
    with op(src, "rt", errors="replace") as fh:
        hdr, recs, frames = normalise(fh)
    out = (io.TextIOWrapper(gzip.GzipFile(dst, "wb", mtime=0)) if dst and dst.endswith(".gz") else open(dst, "w") if dst else sys.stdout)
    for h in hdr:
        out.write(h + "\n")
    out.write("# frames %s\n" % " ".join("%d:%d" % (k, v) for k, v in sorted(frames.items())))
    for i, r in recs:
        out.write("R %d\t%s\n" % (i, "\t".join("%s=%s" % (k, r[k]) for k in sorted(r, key=sort_key))))
    if dst:
        out.close()
        print("%s: %d draws, %d frames" % (dst, len(recs), len(frames)), file=sys.stderr)


if __name__ == "__main__":
    main()
