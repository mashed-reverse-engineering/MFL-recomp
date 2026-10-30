"""Resource tree reader for PE files and .res files: entries() -> {(type,name,lang): bytes}. Names/types are int or str."""
import struct

def pe_entries(b):
    pe = struct.unpack_from("<I", b, 0x3c)[0]
    nsec = struct.unpack_from("<H", b, pe + 6)[0]
    opt = pe + 24
    dd = opt + (96 if struct.unpack_from("<H", b, opt)[0] == 0x10b else 112)
    rva, size = struct.unpack_from("<II", b, dd + 16)
    secs = [struct.unpack_from("<xxxxxxxxIIII", b, opt + struct.unpack_from("<H", b, pe + 20)[0] + 40 * i) for i in range(nsec)]
    def off(r):
        for vs, va, rs, rp in secs:
            if va <= r < va + max(vs, rs): return r - va + rp
        raise ValueError("rva %x" % r)
    base = off(rva)
    def name(o):
        p = base + (o & 0x7fffffff); n = struct.unpack_from("<H", b, p)[0]
        return b[p + 2:p + 2 + 2 * n].decode("utf-16le")
    out = {}
    def walk(o, path):
        n = struct.unpack_from("<HH", b, base + o + 12)
        for i in range(sum(n)):
            nid, to = struct.unpack_from("<II", b, base + o + 16 + 8 * i)
            k = name(nid) if nid & 0x80000000 else nid
            if to & 0x80000000: walk(to & 0x7fffffff, path + (k,))
            else:
                d, sz = struct.unpack_from("<II", b, base + to)
                out[path + (k,)] = bytes(b[off(d):off(d) + sz])
    walk(0, ())
    return out

def res_entries(b):
    out, p = {}, 0
    while p < len(b):
        ds, hs = struct.unpack_from("<II", b, p); q = p + 8
        def ident(q):
            if struct.unpack_from("<H", b, q)[0] == 0xffff: return struct.unpack_from("<H", b, q + 2)[0], q + 4
            e = q
            while b[e:e + 2] != b"\0\0": e += 2
            return b[q:e].decode("utf-16le"), (e + 2)
        t, q = ident(q); n, q = ident(q); q = (q + 3) & ~3
        lang = struct.unpack_from("<H", b, q + 6)[0]
        if ds: out[(t, n, lang)] = bytes(b[p + hs:p + hs + ds])
        p = (p + hs + ds + 3) & ~3
    return out

def entries(path):
    b = open(path, "rb").read()
    return res_entries(b) if path.endswith(".res") else pe_entries(b)
