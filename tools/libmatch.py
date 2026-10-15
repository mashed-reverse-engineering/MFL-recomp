#!/usr/bin/env python3
"""Find the functions of a statically linked library (Lua, zlib, qhull, ...) in MFL.exe by compiling the library source with the
game's compiler and searching the .text bytes (relocation fields wildcarded).  Needs one COMDAT per function (/Gy).
Usage: libmatch.py <workdir with the .c files> <out.csv> [cl flags...]   (include paths: put /I<dir> in the flags, as paths inside the container: /work/...)
Output csv: VA(hex),function name,source file[,note]  -> usable with scripts/RenameFns.java after adding a prefix if wanted."""
import os, re, struct, subprocess, sys, glob
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import cmpfn

def text_of_exe():
    d = open(cmpfn.EXE, "rb").read()
    e = struct.unpack_from("<I", d, 0x3C)[0]
    ns = struct.unpack_from("<H", d, e + 6)[0]
    osz = struct.unpack_from("<H", d, e + 20)[0]
    base = struct.unpack_from("<I", d, e + 24 + 28)[0]
    for i in range(ns):
        o = e + 24 + osz + i * 40
        nm = d[o:o + 8].rstrip(b"\0")
        vs, va, rs, rp = struct.unpack_from("<IIII", d, o + 8)
        if nm == b".text":
            return d[rp:rp + rs], base + va
    raise SystemExit("no .text")

def obj_functions(path):
    """yield (name, bytes, masked offsets) for every function COMDAT section"""
    d = open(path, "rb").read()
    machine, nsec, ts, symoff, nsym, opt, ch = struct.unpack_from("<HHIIIHH", d, 0)
    strtab = symoff + nsym * 18
    def nm_of(raw):
        if raw[:4] == b"\0\0\0\0":
            o = struct.unpack_from("<I", raw, 4)[0]
            return d[strtab + o:d.index(b"\0", strtab + o)].decode()
        return raw.rstrip(b"\0").decode()
    secs = []
    for i in range(nsec):
        o = 20 + opt + i * 40
        name, vs, va, rawsz, rawptr = struct.unpack_from("<8sIIII", d, o)
        relptr = struct.unpack_from("<I", d, o + 24)[0]
        nrel = struct.unpack_from("<H", d, o + 32)[0]
        flags = struct.unpack_from("<I", d, o + 36)[0]
        secs.append((nm_of(name), rawsz, rawptr, flags, relptr, nrel))
    i = 0
    while i < nsym:
        o = symoff + i * 18
        nm = nm_of(d[o:o + 8])
        value, secnum, typ, cls, aux = struct.unpack_from("<IhHBB", d, o + 8)
        if secnum > 0 and (typ >> 4) == 2 and cls in (2, 3):     # function symbol
            n, rawsz, rawptr, flags, relptr, nrel = secs[secnum - 1]
            if flags & 0x20 and rawsz:                        # code section
                masked = set()
                for k in range(nrel):
                    va, symidx, rtype = struct.unpack_from("<IIH", d, relptr + k * 10)
                    if rtype in (6, 0x14, 7):
                        masked.update(range(va - value, va - value + 4))
                code = d[rawptr + value:rawptr + rawsz]
                yield nm.lstrip("_"), code.rstrip(b"\x90\xcc") or code, masked
        i += 1 + aux

def main():
    work, out = sys.argv[1], sys.argv[2]
    flags = sys.argv[3:] or ["/O2", "/Gy"]
    text, base = text_of_exe()
    rows = []
    for c in sorted(glob.glob(os.path.join(work, "**/*.c"), recursive=True)):
        rel = os.path.relpath(c, work)
        subprocess.run([cmpfn.VC7, work, "cl.exe", "/nologo", "/c", "/Gy"] + flags + ["/Fo_o\\", rel], capture_output=True, text=True)
        obj = os.path.join(work, "_o", os.path.basename(c)[:-2] + ".obj")
        if not os.path.exists(obj):
            print("compile failed:", rel); continue
        n = 0
        for name, code, masked in obj_functions(obj):
            if len(code) < 6:
                continue
            pat = b"".join(b"." if i in masked else re.escape(bytes([b])) for i, b in enumerate(code))
            hits = [m.start() for m in re.finditer(pat, text, re.S)]
            if len(hits) == 1:
                rows.append((base + hits[0] + (0), name, rel, ""))
                n += 1
            elif hits:
                rows.append((base + hits[0], name, rel, "%d candidates" % len(hits)))
        print("%s: %d unique matches" % (rel, n))
    with open(out, "w") as f:
        for va, name, rel, note in sorted(rows):
            f.write("%08x,%s,%s,%s\n" % (va, name, rel, note))
    print(len(rows), "rows ->", out)

main()
