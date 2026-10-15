#!/usr/bin/env python3
"""libsyms.py <out.csv> <lib>...: symbol -> address map from the relocations of byte matched library functions (idea: the exe has no relocations, but a matched library function has them in the lib object).
For every function of the libs that is found exactly once in MFL.exe (relocation fields wildcarded, like libmatch_lib.py) each DIR32 / REL32 relocation gives the address of its target symbol: DIR32: value in the exe minus the
addend stored in the object; REL32: exe position + 4 + value minus addend.  Votes per symbol; output csv: symbol,address,votes,disagreeing votes,kind(code|data guess).
This is the mapping a full link needs: game code that refers to RenderWare globals (RwEngineInstance, ...) by original address must refer to the lib's symbol instead.  Usage: libsyms.py out.csv lib..."""
import collections, os, re, struct, sys, tempfile
HERE = os.path.dirname(os.path.abspath(__file__))
src = open(os.path.join(HERE, "libmatch_lib.py")).read().replace("\nmain()\n", "\n")
ns = {"__file__": os.path.join(HERE, "libmatch_lib.py"), "__name__": "libmatch_lib"}
exec(compile(src, "libmatch_lib", "exec"), ns)
text, base = ns["text_of_exe"](); members = ns["members"]
def obj_funcs(path):
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
        relptr = struct.unpack_from("<I", d, o + 24)[0]; nrel = struct.unpack_from("<H", d, o + 32)[0]; flags = struct.unpack_from("<I", d, o + 36)[0]
        secs.append((rawsz, rawptr, flags, relptr, nrel))
    names = {}; i = 0
    while i < nsym:
        o = symoff + i * 18
        names[i] = nm_of(d[o:o + 8]); aux = d[o + 17]
        i += 1 + aux
    i = 0
    while i < nsym:
        o = symoff + i * 18
        value, secnum, typ, cls, aux = struct.unpack_from("<IhHBB", d, o + 8)
        if secnum > 0 and (typ >> 4) == 2 and cls in (2, 3):
            rawsz, rawptr, flags, relptr, nrel = secs[secnum - 1]
            if flags & 0x20 and rawsz:
                code = d[rawptr + value:rawptr + rawsz]; rels = []
                for k in range(nrel):
                    va, symidx, rtype = struct.unpack_from("<IIH", d, relptr + k * 10)
                    if rtype in (6, 0x14) and va >= value and va + 4 <= rawsz: rels.append((va - value, names.get(symidx, "?"), rtype, struct.unpack_from("<i", code, va - value)[0]))
                yield names[i], code, rels
        i += 1 + aux
out, libs = sys.argv[1], sys.argv[2:]
votes = collections.defaultdict(collections.Counter)
tmp = tempfile.mkdtemp(prefix="libsyms_"); seen = set(); nfn = 0
for lib in libs:
    for mem, body in ([(os.path.basename(lib), open(lib, "rb").read())] if lib.endswith(".obj") else members(lib)):      # a .lib archive or a single object file (library compiled from source)
        f = os.path.join(tmp, "m.obj"); open(f, "wb").write(body)
        try: fns = list(obj_funcs(f))
        except Exception: continue
        for name, code, rels in fns:
            if len(code) < 12 or name.startswith(("$", "??")) or (name, code) in seen: continue
            seen.add((name, code))
            masked = set()
            for off, s, rt, add in rels: masked.update(range(off, off + 4))
            pat = b"".join(b"." if i in masked else re.escape(bytes([b])) for i, b in enumerate(code.rstrip(b"\x90\xcc") or code))
            hits = [m.start() for m in re.finditer(pat, text, re.S)]
            if len(hits) != 1: continue
            nfn += 1; h = hits[0]
            for off, s, rt, add in rels:
                if off + 4 > len(pat): continue
                v = struct.unpack_from("<i", text, h + off)[0]
                addr = (v - add) & 0xFFFFFFFF if rt == 6 else (base + h + off + 4 + v - add) & 0xFFFFFFFF
                votes[s][addr] += 1
rows = []
for s, c in votes.items():
    (a, n), tot = c.most_common(1)[0], sum(c.values())
    rows.append((s, a, n, tot - n, "code" if base <= a < base + len(text) else "data"))
with open(out, "w") as f:
    for r in sorted(rows, key=lambda r: r[1]): f.write("%s,%08x,%d,%d,%s\n" % r)
print(nfn, "unique functions used;", len(rows), "symbols;", sum(1 for r in rows if r[3] == 0), "without disagreeing votes;", sum(1 for r in rows if r[4] == "data"), "data symbols")
