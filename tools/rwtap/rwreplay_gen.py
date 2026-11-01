#!/usr/bin/env python3
"""rwreplay_gen.py <rwtap.log> <trace.d3d[.gz]> [--atoms RANGES.txt] > replay.txt: turns a recorded rwtap.log into the command script of librw-mfl/tests/rwreplay.cpp.
Replayed: RwCamera* (clear / view window / clip planes / show), RwRenderStateSet (raster pointers -> rasters sized from tex.0 of the first D3D draw that uses them),
RwD3D9Set{RenderState,TextureStageState,SamplerState} (game-direct D3D calls), RwIm2D* (vertex / index blobs).  Everything else (RwD3D9SetTransform, streams, shaders...) is
what librw does itself and is not replayed.  Only one camera is replayed (title scene: the first one that gets cleared).
--atoms RANGES.txt (course mode, rwtap 2 logs with G / M / A / X records): the main camera = the one cleared once per frame; its atomics (A..X, everything the default render callback
  did in between is NOT replayed) are rebuilt from the dumped RpGeometry / RpMaterial (mat / geo / atom / render commands), the camera view comes from RwCameraBeginUpdate,
  the D3D light set (SetLight / EnableLight / AMBIENT) is turned into world lights; Im2D and the other cameras are not replayed.  RANGES.txt gets the draw index ranges
  `first last_exclusive` of the replayed atomics in the original trace (rwfilter.py cuts them out for the diff)."""
import collections, gzip, re, struct, sys

log, trace = sys.argv[1], sys.argv[2]
ranges_out = sys.argv[sys.argv.index("--atoms") + 1] if "--atoms" in sys.argv else None
tex = {}                                   # D3D draw index -> (w, h, fmt) of tex.0
for ln in (gzip.open if trace.endswith(".gz") else open)(trace, "rt"):
    if ln.startswith("R "):
        h, *t = ln.rstrip("\n").split("\t"); kv = dict(x.split("=", 1) for x in t)
        m = re.match(r"[0-9a-f]+\|(\d+)\|(\d+)x(\d+)\|(\d+)", kv.get("tex.0", ""))
        if m: tex[int(h[2:])] = (int(m[2]), int(m[3]), int(m[1]), int(m[4]))
rid, out = {}, []                          # raster ptr -> id
def calls():
    for ln in open(log):
        c = ln[0]
        if c in "GMAX":
            if ranges_out: yield {"name": c, "raw": ln}
            continue
        if c != "C": continue
        w = ln.split(); d = {"name": w[2], "d": int(w[3][2:]), "a": [int(x, 16) for x in w[5][2:].split(",") if x], "seq": int(w[1])}
        d["p"] = {x.split("=")[0]: bytes.fromhex(x.split("=")[1]) for x in w[6:] if "=" in x and not x.endswith("=!")}
        yield d
f32 = lambda v: struct.unpack("<f", struct.pack("<I", v))[0]
clears = collections.Counter(c["a"][0] for c in calls() if c["name"] == "RwCameraClear")
frames = max(1, sum(1 for c in calls() if c["name"] == "RwCameraShowRaster"))
if ranges_out:   # the camera cleared closest to once per frame
    cam = min(clears, key=lambda k: abs(clears[k] - frames))
else:
    cam = next(c["a"][0] for c in calls() if c["name"] == "RwCameraClear")
ranges, rng = [], None
curcam, pend = None, None
lights, enabled, ambient, last_lights = {}, {}, 0, None
matinfo = {}                               # material ptr -> (name, w, h, fmt) from the first draw that uses it (atomics of the main camera)
recs = list(calls())
if ranges_out:   # pass 1: texture format of every material from the draw it is used in
    pend = None
    for c in recs:
        if c["name"] == "A":
            w = c["raw"].split(); kv = dict(x.split("=", 1) for x in w[1:] if "=" in x and not x.startswith("ltm") and not x.startswith("inst"))
            inst = next(x for x in w if x.startswith("inst="))[5:].split(":")
            n = int(inst[1], 16); b = bytes.fromhex(inst[2]) if len(inst) > 2 and inst[2] != "!" else b""
            d0 = int(w[2][2:])
            for i in range(n):
                mp = struct.unpack_from("<I", b, 36 * i + 8)[0] if len(b) >= 36 * (i + 1) else 0
                if mp and mp not in matinfo and d0 + i in tex: matinfo[mp] = tex[d0 + i]
def emit_lights(amb):   # amb: the ambient light sum of the lighting callback (3 floats, hex)
    global last_lights
    ls = [lights[i] for i in sorted(lights) if enabled.get(i) and i in lights]
    key = (amb, tuple(ls))
    if key != last_lights:
        last_lights = key
        out.append(f"lights {amb} {len(ls)} " + " ".join(x.hex() for x in ls))
for c in recs:
    n = c["name"]
    if n in "GMAX":
        w = c["raw"].rstrip("\n").split()
        if n == "M":
            ptr = w[1]; kv = dict(x.split("=", 1) for x in w[2:] if "=" in x)
            t = matinfo.get(int(ptr, 16), (int(kv.get("rw", 0)) or 64, int(kv.get("rh", 0)) or 64, 21, 1))
            out.append(f"mat {ptr} {kv['col']} {kv['sp']} {kv['tex']} {kv.get('fa', '0')} {t[0]} {t[1]} {t[2]} {t[3]}")
        elif n == "G":
            kv = dict(x.split("=", 1) for x in w[2:] if "=" in x)
            out.append("geo " + w[1] + " " + " ".join(kv.get(k, "-") or "-" for k in ("flags", "nt", "nv", "ntc", "nmat", "mats", "tri", "lit", "tc0", "tc1", "sph", "verts", "norm", "mesh")))
        elif n == "A":
            kv = dict(x.split("=", 1) for x in w[1:] if "=" in x)
            if rng is not None: raise SystemExit("nested A")
            rng = [int(w[2][2:]), None, curcam == cam, kv]
        else:   # X
            if rng is not None:
                rng[1] = int(w[2][2:])
                if rng[2]:
                    kv = rng[3]; emit_lights(kv.get('amb', '000000000000000000000000'))
                    out.append(f"atom {kv['atom']} {kv['geo']} {kv['fl']} {kv['ltm']} {kv.get('bs', '-')}"); out.append(f"render {kv['atom']}")
                    ranges.append((rng[0], rng[1]))
                rng = None
        continue
    a, d = c["a"], c["d"]
    if n == "RwCameraBeginUpdate":
        curcam = a[0]; pend = None
        if ranges_out and a[0] == cam:   # RwCamera: viewWindow 0x68, viewOffset 0x78, nearPlane 0x80, farPlane 0x84, fogPlane 0x88; the view matrix is computed inside the call: the D3D transform logged right after it
            cd = c["p"]["p0"]; vw = struct.unpack_from("<ff", cd, 0x68); pend = "%r %r %r %r %r" % (vw[0], vw[1], *struct.unpack_from("<fff", cd, 0x80))
        continue
    if n == "RwD3D9SetTransform" and a[0] == 2 and pend and rng is None:
        out.append("begin %s %s" % (c["p"]["p1"].hex(), pend)); pend = None
    if n == "RwCameraEndUpdate":
        if ranges_out and a[0] == cam: out.append("end")
        curcam = None; pend = None; continue
    if rng is not None:           # inside the render callback: librw does these itself
        if n == "RwD3D9SetLight": lights[a[0]] = c["p"]["p1"]
        elif n == "RwD3D9EnableLight": enabled[a[0]] = a[1]
        elif n == "RwD3D9SetRenderState" and a[0] == 0x8b: ambient = a[1]
        continue
    if n == "RwD3D9RWSetTexture":   # applies the texture's addressing / filter to the sampler of that stage and binds its raster (NULL: unbinds)
        if ranges_out: out.append("settex %d %s" % (a[1], c["p"]["p0"][0x50:0x54][::-1].hex() if "p0" in c["p"] else "-"))
        continue
    if n == "RwD3D9SetLight": lights[a[0]] = c["p"]["p1"]; continue
    if n == "RwD3D9EnableLight": enabled[a[0]] = a[1]; continue
    if n == "RwD3D9SetRenderState" and a[0] == 0x8b: ambient = a[1]
    if n == "RwCameraClear" and a[0] == cam: out.append(f"clear {a[2]}")
    elif n == "RwCameraSetViewWindow" and a[0] == cam:
        x, y = struct.unpack("<ff", c["p"]["p1"][:8]); out.append(f"viewwindow {x!r} {y!r}")
    elif n in ("RwCameraSetNearClipPlane", "RwCameraSetFarClipPlane") and a[0] == cam and len(a) > 1:
        out.append(f"{'near' if 'Near' in n else 'far'} {f32(a[1])!r}")
    elif n == "RwCameraShowRaster" and a[0] == cam: out.append("show")
    elif n == "RwRenderStateSet":
        v = a[1]
        if a[0] == 1 and v:
            if v not in rid:
                rid[v] = len(rid); w, h, fm = tex.get(d, tex.get(d + 1, (64, 64, 21)))[:3]   # the raster is set right before the draw d
                out.append(f"raster {rid[v]} {w} {h} {fm}")
            v = f"R{rid[v]}"
        else: v = f"{v:x}"
        out.append(f"rs {a[0]} {v}")
    elif n == "RwD3D9SetRenderState": out.append(f"d3drs {a[0]} {a[1]:x}")
    elif n in ("RwD3D9SetTextureStageState", "RwD3D9SetSamplerState"): out.append(f"{'tss' if 'Tex' in n else 'ss'} {a[0]} {a[1]} {a[2]:x}")
    elif ranges_out: pass
    elif n == "RwIm2DRenderPrimitive": out.append(f"prim {a[0]} {a[2]} {c['p']['p1'].hex()}")
    elif n == "RwIm2DRenderIndexedPrimitive": out.append(f"iprim {a[0]} {a[2]} {a[4]} {c['p']['p1'].hex()} {c['p']['p3'].hex()}")
    elif n == "RwIm2DRenderTriangle": out.append(f"tri {a[1]} {a[2]} {a[3]} {a[4]} {c['p']['p0'].hex()}")
    elif n == "RwIm2DRenderLine": out.append(f"line {a[1]} {a[2]} {a[3]} {c['p']['p0'].hex()}")
print("\n".join(out))
if ranges_out: open(ranges_out, "w").write("".join(f"{a} {b}\n" for a, b in ranges))
