#!/usr/bin/env python3
"""lib_extract.py <out dir> <lib> [member name substring ...]: write the COFF members of a static library as .obj files (all members when no filter).  Used to overlay SDK library objects (LIBOBJ_DIR for build_overlay.py)."""
import os, re, sys
HERE = os.path.dirname(os.path.abspath(__file__))
src = open(os.path.join(HERE, "libmatch_lib.py")).read().replace("\nmain()\n", "\n")
ns = {"__file__": os.path.join(HERE, "libmatch_lib.py"), "__name__": "libmatch_lib"}
exec(compile(src, "libmatch_lib", "exec"), ns)
out, lib, flt = sys.argv[1], sys.argv[2], sys.argv[3:]
os.makedirs(out, exist_ok=True); n = 0
for mem, body in ns["members"](lib):
    base = re.split(r"[\\/]", mem)[-1]
    if not flt or any(f in base for f in flt):
        open(os.path.join(out, os.path.basename(lib)[:-4] + "_" + base), "wb").write(body); n += 1
print(n, "members written to", out)
