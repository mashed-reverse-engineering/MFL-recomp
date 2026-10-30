#!/usr/bin/env python3
"""T-E6: raw code addresses in OUR recompiled objects become relocations.  Some recompiled C names a function by its original address (`push 0x4a2b30`, `mov eax, 0x467e60; call eax`): the
linker cannot move such a target.  tools/delink/code_literals.txt lists `source.c  hex-bytes-before  address-hex  what`; every occurrence of `<bytes before><address, 4 bytes LE>` in a code section of
that source's object copy gets a DIR32 relocation against the undefined symbol _func_0x<address> (mkobj.py defines func_0x<hex> at that address, like the decompiler names).  The stored word becomes 0 (addend), so the
link is identical in the pinned layout.   Usage: litreloc.py <objs_local.txt>   (patches the listed object COPIES in place; the C sources stay untouched)"""
import os, struct, sys
G = os.path.normpath(os.path.join(os.path.dirname(os.path.abspath(__file__)), "..", ".."))
want = {}
for l in open(os.path.join(os.path.dirname(os.path.abspath(__file__)), "code_literals.txt")):
    p = l.split()
    if p and l[0] != "#": want.setdefault(p[0], []).append((bytes.fromhex(p[1]) + struct.pack("<I", int(p[2], 16)), int(p[2], 16)))
n = 0
for l in open(sys.argv[1]):
    src, path = l.split()
    if os.path.basename(src) not in want: continue
    d = open(path, "rb").read()
    ns, symptr, nsyms = struct.unpack_from("<H", d, 2)[0], *struct.unpack_from("<II", d, 8)
    secs = []
    for i in range(ns):
        o = 20 + 40 * i; nm, vs, va, rs, rp, relp, lnp, nr, nl, fl = struct.unpack_from("<8sIIIIIIHHI", d, o)
        rels = [d[relp + 10 * k:relp + 10 * k + 10] for k in range(nr)] if nr != 0xFFFF else None
        assert rels is not None, "relocation overflow"
        secs.append([nm, vs, va, rs, bytearray(d[rp:rp + rs]) if rp else None, rels, fl])
    symb = bytearray(d[symptr:symptr + 18 * nsyms]); strt = bytearray(d[symptr + 18 * nsyms:])
    newsym = {}
    for pat, a in want[os.path.basename(src)]:
        for s in secs:
            if not s[6] & 0x20 or s[4] is None: continue
            covered = set()
            for r in s[5]: covered.update(range(struct.unpack_from("<I", r)[0] - 3, struct.unpack_from("<I", r)[0] + 4))
            i = s[4].find(pat)
            while i >= 0:
                off = i + len(pat) - 4
                if off not in covered:
                    if a not in newsym:
                        nm = ("_func_0x%08x" % a).encode(); o = len(strt); strt += nm + b"\0"
                        newsym[a] = len(symb) // 18; symb += b"\0\0\0\0" + struct.pack("<I", o) + struct.pack("<IhHBB", 0, 0, 0x20, 2, 0)
                    s[5].append(struct.pack("<IIH", off, newsym[a], 6)); n += 1; s[4][off:off + 4] = b"\0\0\0\0"      # DIR32 adds the stored word: addend 0
                i = s[4].find(pat, i + 1)
    if not newsym: continue
    body, shdr, p = bytearray(), bytearray(), 20 + 40 * ns
    for s in secs:
        raw = bytes(s[4]) if s[4] is not None else b""
        rp = p if raw else 0; p += len(raw); body += raw
        rl = b"".join(s[5]); rptr = p if s[5] else 0; p += len(rl); body += rl
        shdr += struct.pack("<8sIIIIIIHHI", s[0], s[1], s[2], s[3], rp, rptr, 0, len(s[5]), 0, s[6])
    struct.pack_into("<I", strt, 0, len(strt))
    open(path, "wb").write(struct.pack("<HHIIIHH", 0x14C, ns, struct.unpack_from("<I", d, 4)[0], p, len(symb) // 18, 0, struct.unpack_from("<H", d, 18)[0]) + shdr + body + symb + strt)
print("litreloc: %d relocations" % n)
