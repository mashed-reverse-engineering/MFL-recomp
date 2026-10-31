#!/usr/bin/env python3
"""Asset-parity oracle, original side (plan_librw_integration S2-b).
Walks a full memory dump of the running original game (play_inner.sh `dumpall NAME`: NAME.maps + NAME.<start>.bin) and writes the RenderWare objects found
there as normalised rows (the same rows tests/asset_dump.cpp writes for librw), see ROWS below.  Only RW 3.7 32-bit layouts the game reads by offset
(output/rwsurf/fields_used.csv, librw-mfl/capi/layout_check.cpp) plus the public RW 3.7 headers are used.
Usage: walk.py <dump dir> <NAME> <out.csv>
Rows (comma separated, floats as 8 hex digit IEEE bits so the compare is exact):
  C,ckey,natomics,nframes            clump (ckey = clump signature, see below)
  F,ckey,depth,r(3),u(3),a(3),p(3),mflags   every frame of the clump (depth first, children sorted by matrix bits; compared as a multiset per depth)
  A,ckey,gkey,bsphere(4),frame,flags,interp_flags,pipeline!=0,renderCB!=0   atomic; frame = depth of its frame; interp_flags / pipeline / renderCB are the plugin data at offsets 0x4c / 0x6c / 0x48
  G,gkey,flags,nverts,ntris,nmorph,ntexsets,nmats,bsphere(4),h_verts,h_normals,h_tris,h_uv0,h_uv1,h_colors
  M,gkey,i,rgba,ambient,specular,diffuse,texture name,mask
  T,name,mask,filteraddr,w,h,depth    every texture (of dictionaries and materials)
  TD,ntex,h_names                    texture dictionary
  Worlds are blocks like clumps ("#WORLD wkey" + rows), wkey = nsectors:nplanes:nmats:ntris:nverts:crc32(all sector vertices):
  W,wkey,flags,ntexsets,bbox(6: sup xyz, inf xyz)
  WP,wkey,i,type,value,leftValue,rightValue    plane sector i in depth first order
  WS,wkey,i,nverts,ntris,matbase,tight bbox(6),outer bbox(6),h_verts,h_tris,has_normals,h_uv0,h_prelit   atomic sector i in depth first order
  WM,wkey,i,rgba,ambient,specular,diffuse,texture,mask
ckey = natomics:sum(nverts):sum(ntris):crc32(sorted atomic gkeys)  (no frame count: the game re-parents atomics, e.g. vehicle debris) ; gkey = nverts:ntris:nmats:h_verts
"""
import bisect, glob, os, struct, sys
import numpy as np

class Mem:
    def __init__(self, d, name):
        self.starts, self.arr = [], []
        for l in open(os.path.join(d, name + ".maps")):
            s, e = [int(x, 16) for x in l.split()[0].split("-")]
            fn = os.path.join(d, "%s.%08x.bin" % (name, s))
            if not os.path.exists(fn) or os.path.getsize(fn) < 16: continue
            a = np.memmap(fn, dtype=np.uint8, mode="r")
            self.starts.append(s); self.arr.append(a)
        o = sorted(range(len(self.starts)), key=lambda i: self.starts[i])
        self.starts = [self.starts[i] for i in o]; self.arr = [self.arr[i] for i in o]
    def reg(self, a):
        i = bisect.bisect_right(self.starts, a) - 1
        if i < 0: return None
        o = a - self.starts[i]
        return (self.arr[i], o) if o < len(self.arr[i]) else None
    def b(self, a, n):
        r = self.reg(a)
        if r is None or r[1] + n > len(r[0]): return None
        return bytes(r[0][r[1]:r[1] + n])
    def u32(self, a):
        x = self.b(a, 4)
        return None if x is None else struct.unpack("<I", x)[0]
    def ok(self, a): return a is not None and a >= 0x10000 and self.reg(a) is not None

def fnv(b):
    if b is None: return "x"
    h = 2166136261
    for v in b: h = ((h ^ v) * 16777619) & 0xffffffff
    return "%08x" % h
def fnvn(b):   # numpy variant for large blocks
    if b is None: return "x"
    return fnv(b) if len(b) < 4096 else "%08x" % (zlib_adler(b))
import zlib
def zlib_adler(b): return zlib.crc32(b) & 0xffffffff
def hsh(b): return "x" if b is None else ("0" if len(b) == 0 else "%08x" % (zlib.crc32(b) & 0xffffffff))
def cstr(b): return b.split(b"\0")[0].decode("latin1")
def hx(w): return "%08x" % w

def sphere(m, a):
    x = m.b(a, 16)
    return ["x"] * 4 if x is None else ["%08x" % w for w in struct.unpack("<4I", x)]

def texture(m, t, rows, seen):
    if not m.ok(t): return ("", "")
    b = m.b(t, 0x58)
    if b is None: return ("", "")
    name, mask = cstr(b[0x10:0x30]), cstr(b[0x30:0x50])
    fa = struct.unpack_from("<I", b, 0x50)[0]
    if t not in seen:
        seen.add(t)
        ras = struct.unpack_from("<I", b, 0)[0]
        w = h = d = 0
        if m.ok(ras):
            rb = m.b(ras, 0x18)
            if rb: w, h, d = struct.unpack_from("<III", rb, 0xc)
        rows.append("T,%s,%s,%04x,%d,%d,%d" % (name, mask, fa & 0xffff, w, h, d))
    return (name, mask)

def geometry(m, g, rows, gdone, seen):
    b = m.b(g, 0x64)
    if b is None: return None
    flags, = struct.unpack_from("<I", b, 8)
    nt, nv, nm, ntx = struct.unpack_from("<4i", b, 0x10)
    nmat, = struct.unpack_from("<i", b, 0x24)
    if not (0 <= nv < 1 << 20 and 0 <= nt < 1 << 21 and 0 <= nm < 64 and 0 <= nmat < 4096 and 0 <= ntx <= 8): return None
    mt = struct.unpack_from("<I", b, 0x5c)[0]
    bs, hv, hn = ["x"] * 4, "x", "x"
    if m.ok(mt):
        mb = m.b(mt, 0x1c)
        if mb:
            bs = ["%08x" % w for w in struct.unpack_from("<4I", mb, 4)]
            vp, np_ = struct.unpack_from("<II", mb, 0x14)
            if m.ok(vp) or nv == 0: hv = hsh(m.b(vp, nv * 12) if nv else b"")
            if m.ok(np_): hn = hsh(m.b(np_, nv * 12))
    gk = "%d:%d:%d:%s" % (nv, nt, nmat, hv)
    if g in gdone: return gk
    gdone.add(g)
    tri, col, tc = struct.unpack_from("<III", b, 0x2c)[0], struct.unpack_from("<I", b, 0x30)[0], struct.unpack_from("<I", b, 0x34)[0]
    ht = "0" if nt == 0 else (hsh(m.b(tri, nt * 8)) if m.ok(tri) else "x")
    hc = "0" if nv == 0 else (hsh(m.b(col, nv * 4)) if m.ok(col) else "x")
    huv = []
    for s in range(2):
        p = struct.unpack_from("<I", b, 0x34 + 4 * s)[0]
        huv.append(hsh(m.b(p, nv * 8)) if m.ok(p) and s < ntx else "x")
    rows.append("G,%s,%08x,%d,%d,%d,%d,%d,%s,%s,%s,%s,%s,%s,%s" % (gk, flags, nv, nt, nm, ntx, nmat, ",".join(bs), hv, hn, ht, huv[0], huv[1], hc))
    arr = struct.unpack_from("<I", b, 0x20)[0]
    for i in range(min(nmat, 4096)):
        mp = m.u32(arr + 4 * i) if m.ok(arr) else None
        if not m.ok(mp): rows.append("M,%s,%d,x,x,x,x,," % (gk, i)); continue
        mb = m.b(mp, 0x1c)
        tx, rgba = struct.unpack_from("<II", mb, 0)
        amb, spe, dif = struct.unpack_from("<3I", mb, 0xc)
        tn, tm = texture(m, tx, rows, seen)
        rows.append("M,%s,%d,%08x,%08x,%08x,%08x,%s,%s" % (gk, i, rgba, amb, spe, dif, tn, tm))
    return gk

def frames(m, root):
    """depth first list of (frame addr, depth); children ordered by matrix bits so the order does not depend on the list direction."""
    out = []
    def key(f): return m.b(f + 0x10, 0x30) or b""
    def walk(f, d):
        out.append((f, d))
        kids, c, n = [], m.u32(f + 0x98), 0
        while m.ok(c) and n < 4096: kids.append(c); c = m.u32(c + 0x9c); n += 1
        for k in sorted(kids, key=key): walk(k, d + 1)
    walk(root, 0)
    return out

blocks = set()
def clump(m, c, rows, gdone, seen):
    # atomic list: sentinel at c+8; atomics hang by inClumpLink at +0x40
    n, lnk, ats = 0, m.u32(c + 8), []
    while lnk != c + 8 and m.ok(lnk) and n < 4096:
        ats.append(lnk - 0x40); lnk = m.u32(lnk); n += 1
    if lnk != c + 8 or not ats: return False
    for a in ats:    # a real atomic: type 1, back pointer to this clump
        ab = m.b(a, 0x70)
        if ab is None or ab[0] != 1 or ab[1] != 0 or struct.unpack_from("<I", ab, 0x3c)[0] != c: return False
    root = m.u32(c + 4)
    rb = m.b(root, 0xa4) if m.ok(root) else None
    if rb is None or rb[0] != 0 or struct.unpack_from("<I", rb, 0xa0)[0] == 0: return False   # frame: type 0, root pointer set
    fr = frames(m, root) if m.ok(root) else []
    fdep = {f: d for f, d in fr}
    sub = []
    arows = []
    tv = tt = 0
    for a in ats:
        ab = m.b(a, 0x70)
        if ab is None: return False
        geo = struct.unpack_from("<I", ab, 0x18)[0]
        gk = geometry(m, geo, sub, gdone, seen) if m.ok(geo) else None
        if gk is None: gk = "none"
        else:
            tv += int(gk.split(":")[0]); tt += int(gk.split(":")[1])
        fo = struct.unpack_from("<I", ab, 4)[0]
        arows.append(("%s,%s,%d,%02x,%08x,%d,%d" % (gk, ",".join("%08x" % w for w in struct.unpack_from("<4I", ab, 0x1c)), fdep.get(fo, -1), ab[2],
                       struct.unpack_from("<I", ab, 0x4c)[0], struct.unpack_from("<I", ab, 0x6c)[0] != 0, struct.unpack_from("<I", ab, 0x48)[0] != 0), ))
    gks = sorted(a[0].split(",", 1)[0] for a in arows)
    ck = "%d:%d:%d:%s" % (len(ats), tv, tt, hsh(",".join(gks).encode()))
    crow = []
    crow.append("C,%s,%d,%d" % (ck, len(ats), len(fr)))
    for i, (f, d) in enumerate(fr):
        fb = m.b(f + 0x10, 0x30)
        w = struct.unpack("<12I", fb)
        keep = [w[0], w[1], w[2], w[4], w[5], w[6], w[8], w[9], w[10], w[12 - 0] if False else 0]
        # matrix = right(3)+flags, up(3)+pad, at(3)+pad, pos(3)+pad (4 words each)
        vals = ["%08x" % x for x in (w[0], w[1], w[2], w[4], w[5], w[6], w[8], w[9], w[10])]
        pb = m.b(f + 0x10 + 0x30, 12)
        vals += ["%08x" % x for x in struct.unpack("<3I", pb)]
        crow.append("F,%s,%d,%s,%08x" % (ck, d, ",".join(vals), w[3]))
    for (a,) in sorted(arows):
        crow.append("A,%s,%s" % (ck, a))
    rows.extend(sub)
    blocks.add("\n".join(["#CLUMP " + ck] + crow))
    return True

def world(m, w, rows):
    b = m.b(w, 0x70)
    if b is None: return False
    flags, = struct.unpack_from("<I", b, 8)
    marr, nmat = struct.unpack_from("<II", b, 0x10)
    root, ntx = struct.unpack_from("<II", b, 0x1c)
    if not m.ok(root) or not (0 < nmat < 65536) or ntx > 8: return False
    wr, secs, planes = [], [], []
    def walk(n, depth):
        t = m.u32(n)
        if t is None or depth > 64: return False
        if t == 0xffffffff:
            sb = m.b(n, 0x88)
            if sb is None: return False
            secs.append((n, sb)); return True
        if t not in (0, 4, 8): return False
        pb = m.b(n, 0x18)
        if pb is None: return False
        planes.append(pb)
        _, v, l, r, lv, rv = struct.unpack("<IfIIff", pb)
        planes[-1] = (t, struct.unpack_from("<I", pb, 4)[0], struct.unpack_from("<I", pb, 0x10)[0], struct.unpack_from("<I", pb, 0x14)[0])
        return m.ok(l) and m.ok(r) and walk(l, depth + 1) and walk(r, depth + 1)
    if not walk(root, 0) or not secs: return False
    nv = sum(struct.unpack_from("<H", sb, 0x82)[0] for _, sb in secs)
    nt = sum(struct.unpack_from("<H", sb, 0x84)[0] for _, sb in secs)
    wk = "%d:%d:%d:%d:%d:%s" % (len(secs), len(planes), nmat, nt, nv, hsh(b"".join((m.b(struct.unpack_from("<I", sb, 8)[0], struct.unpack_from("<H", sb, 0x82)[0] * 12) or b"") for _, sb in secs)))
    out = ["W,%s,%08x,%d,%s" % (wk, flags, ntx, ",".join("%08x" % x for x in struct.unpack_from("<6I", b, 0x50)))]
    for i, (t, v, lv, rv) in enumerate(planes): out.append("WP,%s,%d,%d,%08x,%08x,%08x" % (wk, i, t, v, lv, rv))
    seen = set()
    for i, (n, sb) in enumerate(secs):
        nvs, nts = struct.unpack_from("<HH", sb, 0x82)[0], struct.unpack_from("<H", sb, 0x84)[0]
        mbase = struct.unpack_from("<H", sb, 0x80)[0]
        outer = struct.unpack_from("<6I", sb, 0x48); tight = struct.unpack_from("<6I", sb, 0x60)
        tri, vp, np_ = struct.unpack_from("<III", sb, 4)[0], struct.unpack_from("<I", sb, 8)[0], struct.unpack_from("<I", sb, 0xc)[0]
        uv0, pre = struct.unpack_from("<I", sb, 0x10)[0], struct.unpack_from("<I", sb, 0x30)[0]
        hh = lambda p, n_: "0" if n_ == 0 else (hsh(m.b(p, n_)) if m.ok(p) else "x")
        out.append("WS,%s,%d,%d,%d,%d,%s,%s,%s,%s,%s,%s,%s" % (wk, i, nvs, nts, mbase, ",".join("%08x" % x for x in tight), ",".join("%08x" % x for x in outer),
                   hh(vp, nvs * 12), hh(tri, nts * 8), "1" if np_ else "0", hh(uv0, nvs * 8), hh(pre, nvs * 4)))
    for i in range(nmat):
        mp = m.u32(marr + 4 * i) if m.ok(marr) else None
        if not m.ok(mp): out.append("WM,%s,%d,x,x,x,x,," % (wk, i)); continue
        mb = m.b(mp, 0x1c); tx, rgba = struct.unpack_from("<II", mb, 0)
        amb, spe, dif = struct.unpack_from("<3I", mb, 0xc)
        tmp = []; tn, tm = texture(m, tx, tmp, seen_tex_global); rows.extend(tmp)
        out.append("WM,%s,%d,%08x,%08x,%08x,%08x,%s,%s" % (wk, i, rgba, amb, spe, dif, tn, tm))
    blocks.add("\n".join(["#WORLD " + wk] + out))
    return True

seen_tex_global = set()
def scan_worlds(m):
    found = []
    for s, a in zip(m.starts, m.arr):
        n = len(a) // 4
        if n < 40: continue
        w = np.frombuffer(a[:n * 4], dtype="<u4")
        cand = np.nonzero(((w[:-0x20] & 0xffff) == 7) & (w[1:-0x1f] == 0) & (w[0x1c // 4 + 0:-0x20 + 0x1c // 4] >= 0x10000) if False else ((w[:-0x20] & 0xffff) == 7) & (w[1:-0x1f] == 0))[0]
        for i in cand: found.append(s + 4 * int(i))
    return found

def scan_clumps(m):
    found = []
    for s, a in zip(m.starts, m.arr):
        n = len(a) // 4
        if n < 16: continue
        w = np.frombuffer(a[:n * 4], dtype="<u4")
        cand = np.nonzero(((w[:-3] & 0xffff) == 2) & (w[1:-2] >= 0x10000) & (w[2:-1] > 0x10000) & (w[3:] > 0x10000))[0]
        for i in cand:
            c = s + 4 * int(i)
            # object.parent is the root frame (type 0 object at that address); atomic list sentinel c+8 non-empty or self-linked
            if m.u32(c + 4) is None: continue
            found.append(c)
    return found

def scan_dicts(m):
    found = []
    for s, a in zip(m.starts, m.arr):
        n = len(a) // 4
        if n < 16: continue
        w = np.frombuffer(a[:n * 4], dtype="<u4")
        cand = np.nonzero(((w[:-3] & 0xffff) == 6) & (w[1:-2] == 0) & (w[2:-1] >= 0x10000) & (w[3:] >= 0x10000))[0]
        for i in cand:
            d = s + 4 * int(i)
            lst, n_, lnk = m.u32(d + 8), 0, m.u32(d + 8)
            if lnk == d + 8: continue   # empty
            names = []
            while lnk != d + 8 and m.ok(lnk) and n_ < 8192:
                t = lnk - 8
                tb = m.b(t, 0x58)
                if tb is None or struct.unpack_from("<I", tb, 4)[0] != d: break
                names.append((cstr(tb[0x10:0x30]), t)); lnk = m.u32(lnk); n_ += 1
            if lnk == d + 8 and names: found.append((d, names))
    return found

def main(d, name, out):
    m = Mem(d, name)
    rows, gdone, seen = [], set(), set()
    nc = 0
    for c in scan_clumps(m):
        if clump(m, c, rows, gdone, seen): nc += 1
    nw = 0
    for wd in scan_worlds(m):
        if world(m, wd, rows): nw += 1
    print("worlds", nw)
    for dct, names in scan_dicts(m):
        for nm, t in names: texture(m, t, rows, seen)
        rows.append("TD,%d,%s" % (len(names), hsh(",".join(sorted(n for n, _ in names)).encode())))
    open(out, "w").write("\n".join(sorted(set(rows)) + sorted(blocks)) + "\n")
    print("clumps %d rows %d unique %d" % (nc, len(rows), len(set(rows))))

if __name__ == "__main__":
    main(*sys.argv[1:4])
