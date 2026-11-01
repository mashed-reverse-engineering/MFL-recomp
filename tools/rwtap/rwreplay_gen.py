#!/usr/bin/env python3
"""rwreplay_gen.py <rwtap.log> <trace.d3d[.gz]> > replay.txt: turns a recorded rwtap.log into the command script of librw-mfl/tests/rwreplay.cpp.
Replayed: RwCamera* (clear / view window / clip planes / show), RwRenderStateSet (raster pointers -> rasters sized from tex.0 of the first D3D draw that uses them),
RwD3D9Set{RenderState,TextureStageState,SamplerState} (game-direct D3D calls), RwIm2D* (vertex / index blobs).  Everything else (RwD3D9SetTransform, streams, shaders...) is
what librw does itself and is not replayed.  Only the first camera that gets cleared is replayed (title scene: the 640x480 main camera)."""
import gzip, re, struct, sys

log, trace = sys.argv[1], sys.argv[2]
tex = {}                                   # D3D draw index -> (w, h, fmt) of tex.0
for ln in (gzip.open if trace.endswith(".gz") else open)(trace, "rt"):
    if ln.startswith("R "):
        h, *t = ln.rstrip("\n").split("\t"); kv = dict(x.split("=", 1) for x in t)
        m = re.match(r"[0-9a-f]+\|(\d+)\|(\d+)x(\d+)", kv.get("tex.0", ""))
        if m: tex[int(h[2:])] = (int(m[2]), int(m[3]), int(m[1]))
rid, out = {}, []                          # raster ptr -> id
def calls():
    for ln in open(log):
        if not ln.startswith("C "): continue
        w = ln.split(); d = {"name": w[2], "d": int(w[3][2:]), "a": [int(x, 16) for x in w[5][2:].split(",")]}
        d["p"] = {x.split("=")[0]: bytes.fromhex(x.split("=")[1]) for x in w[6:] if "=" in x and not x.endswith("=!")}
        yield d
f32 = lambda v: struct.unpack("<f", struct.pack("<I", v))[0]
cam = next(c["a"][0] for c in calls() if c["name"] == "RwCameraClear")
for c in calls():
    n, a, d = c["name"], c["a"], c["d"]
    if n == "RwCameraClear" and a[0] == cam: out.append(f"clear {a[2]}")
    elif n == "RwCameraSetViewWindow" and a[0] == cam:
        x, y = struct.unpack("<ff", c["p"]["p1"][:8]); out.append(f"viewwindow {x!r} {y!r}")
    elif n in ("RwCameraSetNearClipPlane", "RwCameraSetFarClipPlane") and a[0] == cam:
        out.append(f"{'near' if 'Near' in n else 'far'} {f32(a[1])!r}")
    elif n == "RwCameraShowRaster" and a[0] == cam: out.append("show")
    elif n == "RwRenderStateSet":
        v = a[1]
        if a[0] == 1 and v:
            if v not in rid:
                rid[v] = len(rid); w, h, fm = tex.get(d, tex.get(d + 1, (64, 64, 21)))   # the raster is set right before the draw d
                out.append(f"raster {rid[v]} {w} {h} {fm}")
            v = f"R{rid[v]}"
        else: v = f"{v:x}"
        out.append(f"rs {a[0]} {v}")
    elif n == "RwD3D9SetRenderState": out.append(f"d3drs {a[0]} {a[1]:x}")
    elif n in ("RwD3D9SetTextureStageState", "RwD3D9SetSamplerState"): out.append(f"{'tss' if 'Tex' in n else 'ss'} {a[0]} {a[1]} {a[2]:x}")
    elif n == "RwIm2DRenderPrimitive": out.append(f"prim {a[0]} {a[2]} {c['p']['p1'].hex()}")
    elif n == "RwIm2DRenderIndexedPrimitive": out.append(f"iprim {a[0]} {a[2]} {a[4]} {c['p']['p1'].hex()} {c['p']['p3'].hex()}")
    elif n == "RwIm2DRenderTriangle": out.append(f"tri {a[1]} {a[2]} {a[3]} {a[4]} {c['p']['p0'].hex()}")
    elif n == "RwIm2DRenderLine": out.append(f"line {a[1]} {a[2]} {a[3]} {c['p']['p0'].hex()}")
print("\n".join(out))
