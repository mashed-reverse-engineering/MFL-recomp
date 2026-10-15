#!/usr/bin/env python3
"""rw_overlay_prep.py <out dir>: prepares LIBOBJ_DIR for build_overlay.py: extracts the SDK library members listed in output/rw_overlay_members.txt (members that contain at least one byte matched function)
as .obj files and writes skip.txt = every function defined by those objects except the verified ones in output/rw_overlay_surv.txt (those stay original).  Usage: LIBOBJ_DIR=<out dir> build_overlay.py ..."""
import os, re, struct, sys
HERE = os.path.dirname(os.path.abspath(__file__))
src = open(os.path.join(HERE, "libmatch_lib.py")).read().replace("\nmain()\n", "\n")
ns = {"__file__": os.path.join(HERE, "libmatch_lib.py"), "__name__": "libmatch_lib"}
exec(compile(src, "libmatch_lib", "exec"), ns)
out = sys.argv[1]; os.makedirs(out, exist_ok=True)
want = {}
for f_ in ("rw_overlay_members.txt", "crt_overlay_members.txt"):
    for l in open(os.path.join(HERE, "..", "output", f_)):
        p = l.split(None, 1)
        if len(p) == 2: want[p[1].strip()] = p[0]
LIBDIRS = [os.path.join(HERE, "..", "toolchain", "rwsdk", "lib", "d3d9", "release"),
           os.path.join(HERE, "..", "toolchain", "vc7", "Vc7", "lib")]
n = 0; defined = set()
libs_ = [(os.path.join(d_, lib), lib) for d_ in LIBDIRS for lib in sorted(os.listdir(d_)) if lib in ("libcmt.lib",) or (d_ == LIBDIRS[0] and lib.endswith(".lib"))]
for libpath, lib in libs_:
    for mem, body in ns["members"](libpath):
        key = lib + ":" + mem
        if key in want:
            fn = os.path.join(out, want[key]); open(fn, "wb").write(body); n += 1
            d = body; machine, nsec, ts, symoff, nsym, opt, ch = struct.unpack_from("<HHIIIHH", d, 0); strtab = symoff + nsym * 18; i = 0
            while i < nsym:
                o = symoff + i * 18
                raw = d[o:o + 8]
                nm = d[strtab + struct.unpack_from("<I", raw, 4)[0]:].split(b"\0")[0].decode() if raw[:4] == b"\0\0\0\0" else raw.rstrip(b"\0").decode()
                value, secnum, typ, cls, aux = struct.unpack_from("<IhHBB", d, o + 8)
                if secnum > 0 and (typ >> 4) == 2 and cls == 2: defined.add(nm[1:] if nm.startswith("_") else nm)
                i += 1 + aux
surv = set()
for f_ in ("rw_overlay_surv.txt", "crt_overlay_surv.txt"):
    pth = os.path.join(HERE, "..", "output", f_)
    if os.path.exists(pth): surv |= set(open(pth).read().split())
known = set()      # only names that exist in the original exe can be patched at all: the skip list must stay short (argument length limit)
for l_ in open(os.path.join(HERE, "..", "src", "symbols.txt")):
    q_ = l_.split()
    if len(q_) == 2: known.add(q_[0]); known.add(q_[0].lstrip("_"))
for fn_ in (os.path.join(HERE, "..", "output", "rw_libsyms.csv"), os.path.join(HERE, "..", "output", "crt_libsyms.csv")):
    if os.path.exists(fn_):
        for l_ in open(fn_):
            q_ = l_.split(",")[0]; known.add(q_); known.add(q_.lstrip("_"))
skip = sorted(x for x in defined if x not in surv and ("_" + x) not in surv and (x in known or "_" + x in known))
open(os.path.join(out, "skip.txt"), "w").write(",".join(skip))
print(n, "members,", len(defined), "functions defined,", len(surv), "verified,", len(skip), "skipped ->", out)
