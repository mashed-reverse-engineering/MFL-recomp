#!/usr/bin/env python3
"""patch_import.py <exe> [old, default dinput8.dll] [new, default padsim8.dll]: rename one DLL import of a PE in place (same length), so Wine loads
<new> from the application directory instead of its builtin <old> (the wine-run harness hard-codes WINEDLLOVERRIDES).  As tools/d3dtrace/patch_import.py,
with the names as arguments.  Already patched = no change, exit 0."""
import struct, sys
p = sys.argv[1]; old = (sys.argv[2] if len(sys.argv) > 2 else "dinput8.dll").encode(); new = (sys.argv[3] if len(sys.argv) > 3 else "padsim8.dll").encode()
assert len(old) == len(new), "names must have the same length"
d = bytearray(open(p, "rb").read()); pe = struct.unpack_from("<I", d, 0x3c)[0]
nsec = struct.unpack_from("<H", d, pe + 6)[0]; osz = struct.unpack_from("<H", d, pe + 20)[0]; opt = pe + 24
imp = struct.unpack_from("<I", d, opt + 96 + 8)[0]; sec = opt + osz
secs = [struct.unpack_from("<IIII", d, sec + 40 * i + 8) for i in range(nsec)]   # vsize, va, rawsize, rawptr
def off(rva):
    for vs, va, rs, rp in secs:
        if va <= rva < va + max(vs, rs): return rva - va + rp
n = 0; o = off(imp)
while struct.unpack_from("<I", d, o + 12)[0]:
    no = off(struct.unpack_from("<I", d, o + 12)[0]); name = bytes(d[no:no + len(old) + 1]).lower()
    if name == new.lower() + b"\0": n += 1
    if name == old.lower() + b"\0": d[no:no + len(old)] = new; n += 1
    o += 20
assert n == 1, "%s import not found" % old.decode()
open(p, "wb").write(d)
