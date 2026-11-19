#!/usr/bin/env python3
"""objtable.py: the image object (mkobj.py's mfl.obj) as a text table and back, so a link can build it with no MFL.exe (SK W16, NOEXE form).
  objtable.py emit <mfl.obj> <table.txt>     table from an object the normal form (mkobj --exe) built
  objtable.py build <table.txt> <out.obj>    the same object, byte for byte, from the table alone (mkobj.py <out.obj> --table <table.txt> calls this)
  objtable.py check <mfl.obj>                emit + build in memory, FAIL unless byte-identical (self-check)
The table holds numbers and names only: section sizes / flags and their fill byte, the byte runs that differ from the fill (entry jmp stubs,
nop padding, the few kept thunks), relocations by symbol index, the symbol table.  It is valid for the HEAD whose link it was emitted from."""
import collections, struct, sys

def emit(obj):
    d = open(obj, "rb").read()
    mach, ns, ts, so, nsy, ohs, ch = struct.unpack_from("<HHIIIHH", d, 0)
    assert ohs == 0, "optional header in an object"
    L = ["objtable 1", "hdr %x %x %x" % (mach, ts, ch)]
    for i in range(ns):
        o = 20 + i * 40
        nm = d[o:o + 8]; assert nm[:1] != b"/", "long section name"
        vs, va, sz, rp, rr, ln, nrel, nln, fl = struct.unpack_from("<IIIIIIHHI", d, o + 8)
        assert vs == va == ln == nln == 0, "section %d: unexpected header field" % i
        b = d[rp:rp + sz] if rp else None
        fill = collections.Counter(b).most_common(1)[0][0] if b else None
        L.append("sec %s %x %x %s %x" % (nm.rstrip(b"\0").decode(), sz, fl, "-" if b is None else "%02x" % fill, nrel))
        if b:
            k = 0
            while k < sz:
                if b[k] == fill: k += 1; continue
                j = k
                while j < sz and b[j] != fill: j += 1
                L += ["r %x %s" % (o, b[o:min(o + 32, j)].hex()) for o in range(k, j, 32)]; k = j      # SK2: <= 32 bytes per line (export BAD_TEXT: 65+ hex digits in a row)
        nr = struct.unpack_from("<I", d, rr)[0] if fl & 0x01000000 else nrel
        for k in range(nr if rr else 0):
            off, si, ty = struct.unpack_from("<IIH", d, rr + 10 * k)
            L.append("x %x %x %x" % (off, si, ty))
    st = so + nsy * 18
    k = 0
    while k < nsy:
        o = so + k * 18; raw = d[o:o + 8]
        nm = d[st + struct.unpack_from("<I", raw, 4)[0]:d.index(b"\0", st + struct.unpack_from("<I", raw, 4)[0])] if raw[:4] == b"\0\0\0\0" else raw.rstrip(b"\0")
        assert nm and b" " not in nm and nm.rstrip(b"\0") == nm, "symbol %d: name not representable" % k
        val, sec, typ, cls, aux = struct.unpack_from("<IhHBB", d, o + 8)
        if (typ, cls, aux) == (0x20, 2, 0): L.append("s %s %x %d" % (nm.decode(), val, sec))
        else: L.append("S %s %x %d %x %d %s" % (nm.decode(), val, sec, typ, cls, d[o + 18:o + 18 + 18 * aux].hex() or "-"))
        k += 1 + aux
    assert struct.unpack_from("<I", d, st)[0] == len(d) - st, "string table size"
    return "\n".join(L) + "\n"

def build(text):
    secs, syms, strtab = [], bytearray(), bytearray(4)
    for l in text.splitlines():
        q = l.split()
        if not q or q[0] == "objtable": continue
        if q[0] == "hdr": mach, ts, ch = (int(x, 16) for x in q[1:4])
        elif q[0] == "sec": secs.append([q[1], int(q[2], 16), int(q[3], 16), q[4], int(q[5], 16), None, bytearray()])
        elif q[0] == "r":
            s = secs[-1]
            if s[5] is None: s[5] = bytearray([int(s[3], 16)]) * s[1]
            o = int(q[1], 16); b = bytes.fromhex(q[2]); s[5][o:o + len(b)] = b
        elif q[0] == "x": secs[-1][6] += struct.pack("<IIH", int(q[1], 16), int(q[2], 16), int(q[3], 16))
        elif q[0] in ("s", "S"):
            nm = q[1].encode()
            if len(nm) <= 8: n8 = nm.ljust(8, b"\0")
            else: n8 = b"\0\0\0\0" + struct.pack("<I", len(strtab)); strtab += nm + b"\0"
            if q[0] == "s": syms += n8 + struct.pack("<IhHBB", int(q[2], 16), int(q[3]), 0x20, 2, 0)
            else:
                aux = bytes.fromhex(q[6]) if q[6] != "-" else b""
                syms += n8 + struct.pack("<IhHBB", int(q[2], 16), int(q[3]), int(q[4], 16), int(q[5]), len(aux) // 18) + aux
        else: raise ValueError("objtable: bad line: " + l[:60])
    struct.pack_into("<I", strtab, 0, len(strtab))
    p = 20 + 40 * len(secs); body, shdr = bytearray(), bytearray()
    for nm, sz, fl, fill, nrel, raw, rl in secs:
        if raw is None and fill != "-": raw = bytearray([int(fill, 16)]) * sz
        rp = p if raw else 0; p += len(raw or b""); body += raw or b""
        rr = p if rl else 0; p += len(rl); body += rl
        shdr += nm.encode().ljust(8, b"\0") + struct.pack("<IIIIIIHHI", 0, 0, sz, rp, rr, 0, nrel, 0, fl)
    return struct.pack("<HHIIIHH", mach, len(secs), ts, p, len(syms) // 18, 0, ch) + shdr + body + syms + strtab

if __name__ == "__main__":
    a = sys.argv[1:]
    if a[:1] == ["emit"]: open(a[2], "w").write(emit(a[1]))
    elif a[:1] == ["build"]: open(a[2], "wb").write(build(open(a[1]).read()))
    elif a[:1] == ["check"]:
        t = emit(a[1]); same = build(t) == open(a[1], "rb").read()
        print("objtable check %s: %s (%d table bytes, %d lines)" % (a[1], "SAME" if same else "FAIL: rebuilt object differs", len(t), t.count("\n")))
        sys.exit(0 if same else 1)
    else: sys.exit(__doc__)
