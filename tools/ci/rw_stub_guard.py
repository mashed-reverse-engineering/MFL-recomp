#!/usr/bin/env python3
"""rw_stub_guard.py: FAIL if an exe imports a stubbed mflrw.dll function.
usage: rw_stub_guard.py <capi/stubs_gen.cpp> <exe> | --selftest
Static only: a stub reached inside the DLL or through a function pointer is not seen (say so in the release notes).
Also holds the plain-python PE reader that pe_check.py imports (no objdump)."""
import os, re, struct, sys


def parse_pe(data):
    """-> dict(machine, magic, imports {dll_lower: [names]}, exports [names], res_types [ids])."""
    if data[:2] != b"MZ":
        raise ValueError("no MZ")
    pe = struct.unpack_from("<I", data, 0x3C)[0]
    if data[pe:pe + 4] != b"PE\0\0":
        raise ValueError("no PE signature")
    machine, nsec, _, _, _, osz, _ = struct.unpack_from("<HHIIIHH", data, pe + 4)
    opt = pe + 24
    magic = struct.unpack_from("<H", data, opt)[0]
    out = dict(machine=machine, magic=magic, imports={}, exports=[], res_types=[], manifests=[])
    if magic != 0x10B:  # PE32 only past here
        return out
    dd = [struct.unpack_from("<II", data, opt + 96 + 8 * i) for i in range(16)]
    secs = []
    for i in range(nsec):
        _, vs, va, rs, rp = struct.unpack_from("<8sIIII", data, opt + osz + 40 * i)
        secs.append((va, max(vs, rs), rp))

    def off(rva):
        for va, sz, rp in secs:
            if va <= rva < va + sz:
                return rp + rva - va
        raise ValueError("rva 0x%x outside sections" % rva)

    def cstr(rva):
        o = off(rva)
        return data[o:data.index(b"\0", o)].decode("latin1")

    if dd[1][0]:
        o = off(dd[1][0])
        while True:
            oft, _, _, name, ft = struct.unpack_from("<IIIII", data, o)
            if not name:
                break
            t = off(oft or ft)
            names = []
            while True:
                v = struct.unpack_from("<I", data, t)[0]
                if not v:
                    break
                names.append("#%d" % (v & 0xFFFF) if v & 0x80000000 else cstr(v + 2))
                t += 4
            out["imports"].setdefault(cstr(name).lower(), []).extend(names)
            o += 20
    if dd[0][0]:
        o = off(dd[0][0])
        nn, an = struct.unpack_from("<I", data, o + 24)[0], struct.unpack_from("<I", data, o + 32)[0]
        out["exports"] = [cstr(struct.unpack_from("<I", data, off(an) + 4 * i)[0]) for i in range(nn)]
    if dd[2][0]:
        o = off(dd[2][0])

        def leaves(v):  # resource tree below an entry -> data blobs (v: offset field; high bit = subdirectory, relative to the tree root)
            if not v & 0x80000000:
                r, sz = struct.unpack_from("<II", data, o + v)
                return [data[off(r):off(r) + sz]]
            d = o + (v & 0x7FFFFFFF)
            return [b for i in range(sum(struct.unpack_from("<HH", data, d + 12))) for b in leaves(struct.unpack_from("<I", data, d + 20 + 8 * i)[0])]
        named, ids = struct.unpack_from("<HH", data, o + 12)
        for i in range(named + ids):
            k, v = struct.unpack_from("<II", data, o + 16 + 8 * i)
            out["res_types"].append("name" if k & 0x80000000 else k)
            if k == 24:
                out["manifests"] = leaves(v)
    return out


def make_pe(imports, exports=(), res=(), machine=0x14C):
    """Tiny PE32 builder for the selftests (one section, VA 0x1000, raw 0x200)."""
    blob = bytearray()
    rva = lambda: 0x1000 + len(blob)

    def put(b):
        r = rva(); blob.extend(b); return r

    def cs(s):
        return put(s.encode() + b"\0")
    dirs = {}
    if imports:
        ds = []
        for d, ns in imports.items():
            hn = [put(b"\0\0" + n.encode() + b"\0") for n in ns]
            ds.append((put(b"".join(struct.pack("<I", h) for h in hn) + bytes(4)), cs(d)))
        t = b"".join(struct.pack("<IIIII", a, 0, 0, n, a) for a, n in ds) + bytes(20)
        dirs[1] = (put(t), len(t))
    if exports:
        nr = [cs(n) for n in exports]
        na = put(b"".join(struct.pack("<I", x) for x in nr))
        fa = put(bytes(4 * len(nr)))
        oa = put(bytes(2 * len(nr)))
        h = struct.pack("<IIHHIIIIIII", 0, 0, 0, 0, cs("x.dll"), 1, len(nr), len(nr), fa, na, oa)
        dirs[0] = (put(h), len(h))
    if res:  # res: [type ids] (body b"x") or {type id: body}; one name id 1, one language per type
        res = res if isinstance(res, dict) else {r: b"x" for r in res}
        base, t = rva(), bytearray(struct.pack("<IIHHHH", 0, 0, 0, 0, 0, len(res)) + bytes(8 * len(res)))
        for i, (typ, body) in enumerate(res.items()):
            s = len(t)
            struct.pack_into("<II", t, 16 + 8 * i, typ, 0x80000000 | s)
            t += struct.pack("<IIHHHHII", 0, 0, 0, 0, 0, 1, 1, 0x80000000 | (s + 24))  # name dir -> language dir
            t += struct.pack("<IIHHHHII", 0, 0, 0, 0, 0, 1, 0x409, s + 48)  # language dir -> data entry
            t += struct.pack("<IIII", base + s + 64, len(body), 0, 0) + body
        dirs[2] = (put(bytes(t)), len(t))
    opt = bytearray(224)
    struct.pack_into("<H", opt, 0, 0x10B)
    for i, (r, s) in dirs.items():
        struct.pack_into("<II", opt, 96 + 8 * i, r, s)
    coff = struct.pack("<HHIIIHH", machine, 1, 0, 0, 0, 224, 0x102)
    sec = struct.pack("<8sIIIIIIHHI", b".t", len(blob), 0x1000, len(blob), 0x200, 0, 0, 0, 0, 0x40000040)
    hdr = bytearray(0x40); hdr[:2] = b"MZ"; struct.pack_into("<I", hdr, 0x3C, 0x40)
    img = bytes(hdr) + b"PE\0\0" + coff + bytes(opt) + sec
    return img + bytes(0x200 - len(img)) + bytes(blob)


def stub_names(text):
    m = re.search(r"names\[(\d+)\]\s*=\s*\{(.*?)\};", text, re.S)
    if not m:
        raise ValueError("names[N] block not found in stubs_gen.cpp")
    names = re.findall(r'"([^"]+)"', m.group(2))
    if len(names) != int(m.group(1)):
        raise ValueError("names[%s] declared, %d parsed" % (m.group(1), len(names)))
    return names


norm = lambda n: re.sub(r"@\d+$", "", n).lstrip("_")  # decorated stdcall / underscore forms


def hits(names, data):
    imp = parse_pe(data)["imports"].get("mflrw.dll", [])
    s = {norm(n) for n in names}
    return sorted(n for n in imp if norm(n) in s)


def selftest():
    names = ["RtFoo", "FUN_004da0e0", "RpBar"]
    text = "static unsigned hits[3]; static const char *const names[3] = {\n\t\"RtFoo\",\n\t\"FUN_004da0e0\",\n\t\"RpBar\",\n};\n"
    assert stub_names(text) == names
    clean = make_pe({"mflrw.dll": ["RwReal", "RwOther"], "kernel32.dll": ["RtFoo"]})  # RtFoo from kernel32 is not ours
    assert parse_pe(clean)["imports"]["mflrw.dll"] == ["RwReal", "RwOther"]
    assert hits(names, clean) == [], "clean exe flagged"
    bad = make_pe({"mflrw.dll": ["RwReal", "FUN_004da0e0"]})  # planted stub name
    assert hits(names, bad) == ["FUN_004da0e0"], "planted stub not found"
    try:
        stub_names(text.replace("[3] =", "[4] ="))
    except ValueError:
        pass
    else:
        raise AssertionError("count mismatch not caught")
    print("rw_stub_guard selftest OK")


def main(a):
    if a[:1] == ["--selftest"]:
        return selftest()
    if len(a) != 2:
        sys.exit(__doc__)
    names = stub_names(open(a[0], encoding="utf-8", errors="replace").read())
    h = hits(names, open(a[1], "rb").read())
    print("rw_stub_guard: %d stub names, %d imported from mflrw.dll" % (len(names), len(h)))
    if h:
        print("FAIL: exe imports stubbed functions: " + " ".join(h))
        sys.exit(1)
    print("PASS (static: stubs reached inside the DLL or via function pointers are not seen)")


if __name__ == "__main__":
    main(sys.argv[1:])
