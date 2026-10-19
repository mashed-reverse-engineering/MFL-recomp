#!/usr/bin/env python3
"""patch_import.py <exe> [new name, default d3dp.dll]: rename the d3d9.dll import of a PE (same length) so that Wine loads <new name> from the application
directory instead of its builtin d3d9 (the wine-run harness hard-codes WINEDLLOVERRIDES, so d3d9=n,b cannot be set per run)."""
import struct, sys
p = sys.argv[1]; new = (sys.argv[2] if len(sys.argv) > 2 else "d3dp.dll").encode()
d = bytearray(open(p, "rb").read()); pe = struct.unpack_from("<I", d, 0x3c)[0]
nsec = struct.unpack_from("<H", d, pe + 6)[0]; osz = struct.unpack_from("<H", d, pe + 20)[0]; opt = pe + 24
imp = struct.unpack_from("<I", d, opt + 96 + 8)[0]; sec = opt + osz
secs = [struct.unpack_from("<IIII", d, sec + 40 * i + 8) for i in range(nsec)]   # vsize, va, rawsize, rawptr
def off(rva):
    for vs, va, rs, rp in secs:
        if va <= rva < va + max(vs, rs): return rva - va + rp
n = 0; o = off(imp)
while struct.unpack_from("<I", d, o + 12)[0]:
    no = off(struct.unpack_from("<I", d, o + 12)[0])
    if d[no:no + 9].lower() == b"d3d9.dll\0": assert len(new) == 8; d[no:no + 8] = new; n += 1
    o += 20
assert n == 1, "d3d9.dll import not found"
open(p, "wb").write(d)
