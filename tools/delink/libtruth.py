#!/usr/bin/env python3
"""Ground truth for classify.py: library functions that byte match (output/rw_libmatch.csv, crt_libmatch.csv, unique rows only) carry their real
relocations in the SDK / CRT .lib objects.  Every IMAGE_REL_I386_DIR32 field of such a function is an absolute pointer site in MFL.exe; every other
offset is not.  Prints recall / precision of output/delink_relocs.csv on those byte ranges and lists the misses.  Usage: libtruth.py"""
import csv, os, re, struct, collections
H = os.path.dirname(os.path.abspath(__file__)); G = os.path.normpath(os.path.join(H, "..", "..")); M = os.path.dirname(G)
LIBDIRS = {"libcmt.lib": os.path.join(G, "toolchain/vc7/Vc7/lib"),
           None: os.path.join(G, "toolchain/rwsdk/lib/d3d9/release")}

def members(path):
    d = open(path, "rb").read(); p, longnames = 8, b""
    while p + 60 <= len(d):
        name, size = d[p:p + 16].decode().strip(), int(d[p + 48:p + 58]); body = d[p + 60:p + 60 + size]
        if name == "//": longnames = body
        elif name not in ("/", ""):
            if name.startswith("/") and name[1:].isdigit():
                o = int(name[1:]); name = re.split(rb"[\n\0]", longnames[o:o + 260])[0].decode().rstrip("/")
            yield name.rstrip("/"), body
        p += 60 + size + (size & 1)

def dir32(d):
    """{function name: (offsets of DIR32 fields relative to the function start, code length)}"""
    nsec, symoff, nsym, opt = struct.unpack_from("<H", d, 2)[0], *struct.unpack_from("<II", d, 8), struct.unpack_from("<H", d, 16)[0]
    strtab = symoff + nsym * 18
    nm = lambda raw: d[strtab + struct.unpack_from("<I", raw, 4)[0]:d.index(b"\0", strtab + struct.unpack_from("<I", raw, 4)[0])].decode() if raw[:4] == b"\0\0\0\0" else raw.rstrip(b"\0").decode()
    secs = [struct.unpack_from("<IIIIHHI", d, 20 + opt + i * 40 + 16) for i in range(nsec)]     # rawsz, rawptr, relptr, lnptr, nrel, nln, flags
    out, i = {}, 0
    while i < nsym:
        o = symoff + i * 18; value, secnum, typ, cls, aux = struct.unpack_from("<IhHBB", d, o + 8)
        if secnum > 0 and (typ >> 4) == 2 and cls in (2, 3):
            rawsz, rawptr, relptr, _, nrel, _, fl = secs[secnum - 1]
            code = d[rawptr + value:rawptr + rawsz]; code = code.rstrip(b"\x90\xcc") or code
            offs = {va - value for va, si, t in (struct.unpack_from("<IIH", d, relptr + k * 10) for k in range(nrel)) if t == 6 and value <= va < value + len(code)}
            k = nm(d[o:o + 8]).lstrip("_")
            out[k] = None if k in out else (offs, len(code))          # a static label name used twice in one object (asm ProcPrologue): ambiguous
        i += 1 + aux
    return out

rows = []
for f in ("rw_libmatch.csv", "crt_libmatch.csv"):
    for r in csv.reader(open(os.path.join(G, "output", f))):
        if len(r) >= 4 and not (len(r) > 4 and r[4]): rows.append((int(r[0], 16), r[1], r[2]))
cache = {}
truth, covered = set(), []
for va, name, mem in rows:
    lib, member = mem.split(":", 1)
    if lib not in cache:
        cache[lib] = {m: b for m, b in members(os.path.join(LIBDIRS.get(lib, LIBDIRS[None]), lib))}
    body = cache[lib].get(member)
    if body is None: continue
    fn = dir32(body).get(name.lstrip("_"))
    if not fn: continue
    offs, n = fn
    covered.append((va, va + n)); truth.update(va + o for o in offs)
exe = open(os.path.join(M, "MashedGame/FullyLoaded/installed/App_Executables/MFL.exe"), "rb").read()
absolute = {t for t in truth if not 0x400000 <= struct.unpack_from("<I", exe, t - 0x400000)[0] < 0x9f9000}     # DIR32 to an absolute symbol (__except_list = 0)
truth -= absolute
print("DIR32 to absolute symbols (not image pointers, ignored):", len(absolute))
pred = {}
for r in csv.DictReader(open(os.path.join(G, "output", "delink_relocs.csv"))):
    if r["kind"] != "rel32": pred[int(r["site"], 16)] = r          # REL32 fields are not DIR32 truth
cb = {x for a, b in covered for x in range(a, b)}          # ranges overlap (several matched names per address)
pin = {s: r for s, r in pred.items() if s in cb}
tp = truth & set(pin); fn_ = truth - set(pin); fp = set(pin) - truth
print("library ground truth: %d functions, %d distinct bytes, %d DIR32 sites" % (len(covered), len(cb), len(truth)))
print("predicted in range %d: true %d, missed %d (recall %.3f %%), false %d (precision %.3f %%)" % (len(pin), len(tp), len(fn_), 100.0 * len(tp) / max(1, len(truth)), len(fp), 100.0 * len(tp) / max(1, len(pin))))
print("false positives by confidence/kind:", collections.Counter((pin[s]["confidence"], pin[s]["kind"]) for s in fp).most_common(10))
print("true positives by confidence:", collections.Counter((pin[s]["confidence"], pin[s]["src"], pin[s]["why"]) for s in tp).most_common(8))
for s in sorted(fn_)[:15]: print("  missed %08x" % s)
for s in sorted(fp)[:15]: print("  false  %08x %s" % (s, pin[s]))
