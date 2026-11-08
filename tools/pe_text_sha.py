#!/usr/bin/env python3
"""pe_text_sha.py <pe> [...]: sha256 of the .text section of each PE (exe/dll), for "code unchanged" checks (e.g. asserts-only edits of librw-mfl).
The HIGHLOW base-relocation words inside .text are rebased to ImageBase 0 first: mingw ld's auto image base hashes the output path, so the same
DLL built into two directories differs only there. Prints `<sha> <text size> <relocs> <path>`."""
import hashlib, struct, sys

def text_sha(p):
    d = open(p, "rb").read(); pe = struct.unpack_from("<I", d, 0x3c)[0]
    n, opt = struct.unpack_from("<H", d, pe + 6)[0], struct.unpack_from("<H", d, pe + 20)[0]
    base = struct.unpack_from("<I", d, pe + 24 + 28)[0]
    rel_rva, rel_sz = struct.unpack_from("<II", d, pe + 24 + 96 + 5 * 8)      # data directory 5 = base relocations
    secs = []
    for i in range(n):
        o = pe + 24 + opt + 40 * i
        vs, va, rs, rp = struct.unpack_from("<IIII", d, o + 8); secs.append((d[o:o + 8].rstrip(b"\0"), va, min(vs, rs), rp))
    def off(rva): return next(rp + rva - va for _, va, sz, rp in secs if va <= rva < va + sz)
    _, tva, tsz, trp = next(s for s in secs if s[0] == b".text")
    t = bytearray(d[trp:trp + tsz]); nrel = 0
    o, end = (off(rel_rva), off(rel_rva) + rel_sz) if rel_rva else (0, 0)      # no .reloc (fixed-base exe, e.g. ours.exe): nothing to rebase
    while o < end:
        page, bs = struct.unpack_from("<II", d, o)
        if bs < 8: break
        for k in range(8, bs, 2):
            e = struct.unpack_from("<H", d, o + k)[0]
            r = page + (e & 0xfff)
            if e >> 12 == 3 and tva <= r < tva + tsz - 3:
                struct.pack_into("<I", t, r - tva, (struct.unpack_from("<I", t, r - tva)[0] - base) & 0xffffffff); nrel += 1
        o += bs
    for i in range(tsz - 4):             # call / jmp rel32 to absolute 0 (libstdc++'s weak undefined _ITM_* symbols): the rel32 depends on the base
        if t[i] in (0xE8, 0xE9) and (base + tva + i + 5 + struct.unpack_from("<i", t, i + 1)[0]) & 0xffffffff == 0: struct.pack_into("<i", t, i + 1, 0)
    return hashlib.sha256(bytes(t)).hexdigest(), tsz, nrel, bytes(t), tva

if __name__ == "__main__":
    if sys.argv[1] == "--diff":          # --diff a b: the first differing rebased .text RVAs
        a, b = text_sha(sys.argv[2]), text_sha(sys.argv[3])
        dif = [i for i in range(min(a[1], b[1])) if a[3][i] != b[3][i]]
        print(len(dif), "bytes differ, first RVAs", ["%x" % (a[4] + i) for i in dif[:12]]); sys.exit(1 if dif else 0)
    for p in sys.argv[1:]: print(*text_sha(p)[:3], p)
