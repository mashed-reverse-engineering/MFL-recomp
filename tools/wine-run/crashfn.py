#!/usr/bin/env python3
"""Which function crashed?  Usage: crashfn.py <regress dir> [out_dir_name=out_ovl_1]
Reads the 'Unhandled page fault ... at address X' line of <dir>/<out>/wine.log and names the function at X: a recompiled function from
<dir>/ovl.exe.map (section .recomp) or an original one from src/symbols.txt (the nearest symbol below X)."""
import bisect, os, re, sys
W = sys.argv[1]
out = sys.argv[2] if len(sys.argv) > 2 else "out_ovl_1"
log = open(os.path.join(W, out, "wine.log"), errors="replace").read()
m = re.search(r"Unhandled (page fault|exception)[^\n]*?(?:on (\w+) access to ([0-9A-Fa-f]+) )?at address ([0-9A-Fa-f]+)", log)
if not m:
    print("no crash in", out); sys.exit(0)
addr = int(m.group(4), 16)
syms = []
MAPNAME = sys.argv[3] if len(sys.argv) > 3 else "ovl.exe.map"      # b_1.exe.map for the runs of bisect_files.sh
for p in (os.path.join(W, MAPNAME), os.path.join(os.path.dirname(os.path.abspath(__file__)), "..", "..", "src", "symbols.txt")):
    for l in open(p):
        q = l.split()
        if len(q) == 2 and not q[0].startswith("count"):
            try: syms.append((int(q[1], 16), q[0]))
            except ValueError: pass
syms.sort()
i = bisect.bisect_right([s[0] for s in syms], addr) - 1
print("%s at %08x in %s+%#x  (%s%s)" % (m.group(1), addr, syms[i][1], addr - syms[i][0], "write" if m.group(2) == "write" else (m.group(2) or "access"), " to %s" % m.group(3) if m.group(3) else ""))
