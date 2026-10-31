#!/usr/bin/env python3
"""undup.py <objs_local.txt>: the source-built library objects ("lib:<lib>:src" lines) may define functions our recompiled C also defines (14 png / inflate functions): the C copy wins at the
link anyway (first definition), so the library copy gets a different name (first character `_` -> `$`, same length, in place in the copy under <work>/o) and no LNK4006 is left."""
import struct, sys
def syms(d):
    so, n = struct.unpack_from("<II", d, 8); st = so + n * 18; i = 0
    while i < n:
        q = so + i * 18; raw = bytes(d[q:q + 8])
        if raw[:4] == b"\0\0\0\0": o = st + struct.unpack_from("<I", raw, 4)[0]; nm = d[o:d.index(b"\0", o)].decode("latin1")
        else: o = q; nm = raw.rstrip(b"\0").decode("latin1")
        val, sec, typ, cls, aux = struct.unpack_from("<IhHBB", d, q + 8)
        yield nm, sec, cls, o
        i += 1 + aux
mine, lib = set(), []
for l in open(sys.argv[1]):
    src, obj = l.split()
    if src.startswith("lib:") and src.endswith(":src"): lib.append(obj); continue
    for nm, sec, cls, o in syms(open(obj, "rb").read()):
        if sec > 0 and cls == 2: mine.add(nm)
n = 0
for obj in lib:
    d = bytearray(open(obj, "rb").read())
    for nm, sec, cls, o in syms(d):
        if sec > 0 and cls == 2 and nm in mine and nm[0] == "_": d[o] = ord("$"); n += 1
    open(obj, "wb").write(d)
print("undup: %d library definitions renamed (defined by our C as well)" % n)
