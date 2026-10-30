#!/usr/bin/env python3
"""Compile one C function with the game's compiler (cl 13.00.9466 under Wine, see vc7.sh) and compare its code
with the bytes at a virtual address of MFL.exe.
Usage: cmpfn.py <file.c> <function symbol (without underscore)> <VA hex> [extra cl flags, default /O2]
Exit code 0 when the bytes match (trailing padding ignored)."""
import re, hashlib, os, pickle, struct, subprocess, sys, tempfile, shutil
from concurrent.futures import ThreadPoolExecutor

EXE = os.environ.get("MFL_EXE", os.path.join(os.path.dirname(os.path.abspath(__file__)), "..", "game", "MFL.exe"))
WORKROOT = os.environ.get("CMPFN_TMP", os.path.join(os.environ.get("TMPDIR", "/tmp/claude-%d" % os.getuid()), "cmpfn"))     # below the mounted path of the persistent container
os.environ.setdefault("VC7_CONTAINER", "cmpfn")                         # CMPFN_CONTAINER-less use: VC7_CONTAINER= (empty) gives one container per call
VC7 = os.path.join(os.path.dirname(os.path.abspath(__file__)), "vc7.sh")


_EXE = []


def exe_bytes(va, n):
    if not _EXE: _EXE.append(open(EXE, "rb").read())
    d = _EXE[0]
    e = struct.unpack_from("<I", d, 0x3C)[0]
    ns = struct.unpack_from("<H", d, e + 6)[0]
    osz = struct.unpack_from("<H", d, e + 20)[0]
    base = struct.unpack_from("<I", d, e + 24 + 28)[0]
    for i in range(ns):
        o = e + 24 + osz + i * 40
        vs, vaa, rs, rp = struct.unpack_from("<IIII", d, o + 8)
        if vaa <= va - base < vaa + max(vs, rs):
            p = rp + va - base - vaa
            return d[p:p + n]
    raise SystemExit("VA not in a section")


def obj_function(obj, sym):
    d = open(obj, "rb").read()
    machine, nsec, ts, symoff, nsym, opt, ch = struct.unpack_from("<HHIIIHH", d, 0)
    strtab = symoff + nsym * 18
    def name_of(raw):
        if raw[:4] == b"\0\0\0\0":
            o = struct.unpack_from("<I", raw, 4)[0]
            end = d.index(b"\0", strtab + o)
            return d[strtab + o:end].decode()
        return raw.rstrip(b"\0").decode()
    secs = []
    for i in range(nsec):
        o = 20 + opt + i * 40
        name, vs, va, rawsz, rawptr, *_rest = struct.unpack_from("<8sIIII", d, o)
        flags = struct.unpack_from("<I", d, o + 36)[0]
        relptr = struct.unpack_from("<I", d, o + 24)[0]
        nrel = struct.unpack_from("<H", d, o + 32)[0]
        secs.append((name_of(name), rawsz, rawptr, flags, relptr, nrel))
    i = 0
    while i < nsym:
        o = symoff + i * 18
        nm = name_of(d[o:o + 8])
        value, secnum, typ, cls, aux = struct.unpack_from("<IhHBB", d, o + 8)
        if (nm == sym if sym.startswith("@") else (nm == "_" + sym or re.fullmatch(r"_%s@\d+" % re.escape(sym), nm))) and secnum > 0:      # __stdcall objects carry _name@N
            n, rawsz, rawptr, flags, relptr, nrel = secs[secnum - 1]
            masked = set()
            for k in range(nrel):
                va, symidx, rtype = struct.unpack_from("<IIH", d, relptr + k * 10)
                if rtype in (6, 0x14, 7):  # DIR32, REL32, DIR32NB: 4 byte address fields differ after linking
                    masked.update(range(va - value, va - value + 4))
            return d[rawptr + value:rawptr + rawsz], masked
        i += 1 + aux
    raise SystemExit("symbol _%s not found in %s" % (sym, obj))


_SYMS = {}


def _undecorate(n):      # as tools/overlay.py
    if n.startswith("@"): return n[1:].split("@")[0]
    if n.startswith("_"): n = n[1:]
    return n.split("@")[0] if re.match(r"^\w+@\d+$", n) else n


def _symtab():
    if not _SYMS:
        R = os.path.join(os.path.dirname(os.path.abspath(__file__)), "..", "src")
        ov = os.environ.get("T2_OVERLAY_DIR")      # tools/t2_auto.py run overlay: aliases of that run (read in addition to src/aliases.txt)
        for f in ("symbols.txt", "aliases.txt") + (("@" + os.path.join(ov, "aliases.txt"),) if ov and os.path.exists(os.path.join(ov, "aliases.txt")) else ()):
            for l in open(f[1:] if f[0] == "@" else os.path.join(R, f)):
                p = l.split()
                if len(p) == 2:
                    try: _SYMS.setdefault(p[0], set()).add(int(p[1], 16))
                    except ValueError: pass
    return _SYMS


def reloc_targets(obj, sym, va, code):
    """The relocated fields of `sym` must lead to the same place as the original's (tools/overlay.py RELCHECK does this at link time; a masked byte compare cannot see it).
    Returns (problems, warnings): lists of strings.  Same rules as overlay.py: same address (name -> src/symbols.txt + aliases.txt), a jmp thunk to it, an equal literal
    (string / float constant compared by value), own section + offset (jump tables).  Unresolvable names are warnings."""
    d = open(obj, "rb").read()
    machine, nsec, ts, symoff, nsym, opt, ch = struct.unpack_from("<HHIIIHH", d, 0)
    strtab = symoff + nsym * 18
    def name_of(raw):
        if raw[:4] == b"\0\0\0\0":
            o = struct.unpack_from("<I", raw, 4)[0]
            return d[strtab + o:d.index(b"\0", strtab + o)].decode()
        return raw.rstrip(b"\0").decode()
    secs = []
    for i in range(nsec):
        o = 20 + opt + i * 40
        rawsz, rawptr, relptr = struct.unpack_from("<III", d, o + 16)
        nrel = struct.unpack_from("<H", d, o + 32)[0]
        secs.append((rawsz, rawptr, relptr, nrel, struct.unpack_from("<I", d, o + 36)[0]))
    syms = {}
    fsec = fval = None
    i = 0
    while i < nsym:
        o = symoff + i * 18
        nm = name_of(d[o:o + 8])
        value, secnum, typ, cls, aux = struct.unpack_from("<IhHBB", d, o + 8)
        syms[i] = (nm, value, secnum)
        if fsec is None and secnum > 0 and (nm == sym if sym.startswith("@") else (nm == "_" + sym or re.fullmatch(r"_%s@\d+" % re.escape(sym), nm))): fsec, fval = secnum, value
        i += 1 + aux
    rawsz, rawptr, relptr, nrel, _ = secs[fsec - 1]
    table = _symtab()
    def thunk(a):
        b = exe_bytes(a, 6)
        if len(b) >= 5 and b[0] == 0xE9: return (a + 5 + struct.unpack_from("<i", b, 1)[0], None)
        if len(b) >= 6 and b[:2] == b"\xFF\x25": return (None, struct.unpack_from("<I", b, 2)[0])
        return (None, None)
    def same_addr(a, b):
        if a == b: return True
        ta, tb = thunk(a), thunk(b)
        return b == ta[0] or a == tb[0] or (ta[1] is not None and ta[1] == tb[1]) or (ta[0] is not None and ta[0] == tb[0])
    bad, warn = [], []
    for k in range(nrel):
        rva, symidx, rtype = struct.unpack_from("<IIH", d, relptr + k * 10)
        if rtype not in (6, 0x14) or not (fval <= rva < fval + len(code)): continue
        o = rva - fval
        add = struct.unpack_from("<i", d, rawptr + rva)[0]
        of = exe_bytes(va + o, 4)
        if len(of) < 4: continue
        orig_t = struct.unpack_from("<I", of)[0] if rtype == 6 else (va + o + 4 + struct.unpack_from("<i", of)[0]) & 0xFFFFFFFF
        nm, sval, sn = syms[symidx]
        un = _undecorate(nm)
        where = "+0x%x %s" % (o, nm)
        if nm.startswith("__imp_"):
            continue      # IAT slot: the import itself is not named in the table
        if sn == fsec:      # jump table / label inside the function itself
            if orig_t != va + (sval - fval) + add: bad.append("%s: original -> %08x, expected %08x (own code + offset)" % (where, orig_t, va + sval - fval + add))
            continue
        if sn > 0 and not (secs[sn - 1][4] & 0x20) and (nm.startswith(("??_C@", "_real@", "__real@", "__xmm@")) or un not in table):      # literal / file local data: compare by value
            sz, sp = secs[sn - 1][0], secs[sn - 1][1]
            ours = d[sp + sval + add:sp + min(sz, sval + add + 40)]
            theirs = exe_bytes(orig_t, 40)
            if nm.startswith("??_C@") or (b"\0" in ours[:39] and ours[:ours.index(b"\0")].isascii() and ours[:1] != b"\0" and len(ours) > 4):
                n = ours.index(b"\0") + 1 if b"\0" in ours else len(ours)
                if ours[:n] != theirs[:n]: bad.append("%s: string %r vs original %08x %r" % (where, ours[:n], orig_t, theirs[:n]))
            elif nm.startswith(("_real@", "__real@", "__xmm@")):
                n = 8 if re.match(r"_?_real@8@", nm) else 4
                if ours[:n] != theirs[:n]: bad.append("%s: constant %s vs original %08x %s" % (where, ours[:n].hex(), orig_t, theirs[:n].hex()))
            else:
                warn.append("%s: file local data, not verifiable" % where)
            continue
        vas = table.get(un) or table.get("_" + un)
        if not vas:
            warn.append("%s: unknown name" % where); continue
        vas = {v + add for v in vas}
        csz = 8 if re.match(r"(k_d_|double_)", un) else 4 if re.match(r"(k_f_|float_)", un) else 0      # the same float / double constant stored twice in the original data
        if not any(same_addr(v, orig_t) or (csz and exe_bytes(v, csz) == exe_bytes(orig_t, csz)) for v in vas):
            bad.append((o, vas, orig_t, "%s (%s): original -> %08x" % (where, ",".join("%08x" % v for v in sorted(vas)), orig_t)))
    # `a + b` compiled as `fld [b]; fadd [a]`: two operands of a commutative operation swapped, result bit identical (same rule as overlay.py)
    for i1 in range(len(bad)):
        for i2 in range(i1 + 1, len(bad)):
            f1, f2 = bad[i1], bad[i2]
            if not (isinstance(f1, tuple) and isinstance(f2, tuple)) or f1[2] not in f2[1] or f2[2] not in f1[1]: continue
            lo, hi = (f1, f2) if f1[0] < f2[0] else (f2, f1)
            if 0 <= hi[0] - lo[0] - 6 <= 2 and all(0x50 <= c <= 0x5f for c in code[lo[0] + 4:hi[0] - 2]) and code[lo[0] - 2:lo[0]] == b"\xd9\x05" and code[hi[0] - 2:hi[0]] in (b"\xd8\x05", b"\xd8\x0d"):
                bad[i1] = bad[i2] = None
    bad = [f for f in bad if f is not None]
    wl = os.path.join(os.path.dirname(os.path.abspath(__file__)), "..", "src", "relcheck_whitelist.txt")      # same addresses in another order, verified by hand (name [sha1 prefix of our code])
    if bad and os.path.exists(wl):
        for l in open(wl):
            q = l.split()
            if q and not l.startswith("#") and q[0] == sym and len(q) > 1 and q[1] == hashlib.sha1(code).hexdigest()[:12] and all(isinstance(f, tuple) for f in bad) and sorted(min(f[1]) for f in bad) == sorted(f[2] for f in bad):
                return [], warn
    return [f[3] if isinstance(f, tuple) else f for f in bad], warn


FNSZ = {}
LAST = {}      # symbol -> (our code bytes, set of relocated byte offsets) of the last check() (tools/check_one.py)
_fp = os.path.join(os.path.dirname(os.path.abspath(__file__)), "..", "output", "fnsizes.txt")
if os.path.exists(_fp):
    for _l in open(_fp):
        _p = _l.split()
        if len(_p) == 3: FNSZ[int(_p[0], 16)] = int(_p[2])


def check(src, items, flags):
    """compile src once, compare every (symbol, va) in items; returns {symbol: (ok, detail)}"""
    os.makedirs(WORKROOT, exist_ok=True)
    w = tempfile.mkdtemp(prefix="cmpfn_", dir=WORKROOT)
    res = {}
    try:
        shutil.copy(src, os.path.join(w, "f.c"))
        d = os.path.dirname(os.path.abspath(src))
        for h in os.listdir(d):
            if h.endswith(".h"):
                shutil.copy(os.path.join(d, h), os.path.join(w, h))
        _m = open(src, "rb").read(); env = dict(os.environ, VC7_CL="1310_3077") if b"compile: cl1310b" in _m else dict(os.environ, VC7_CL="1310") if b"compile: cl1310" in _m else None   # alternate compiler cl 13.10 (marker or VC7_CL=1310 in the environment)
        try:
            r = subprocess.run([VC7, w, "cl.exe", "/nologo", "/c", "/Gy"] + flags + ["f.c"], capture_output=True, text=True, env=env, timeout=int(os.environ.get("CMPFN_TIMEOUT", "180")))
        except subprocess.TimeoutExpired:
            return {s: (False, "compile error: timeout") for s, _ in items}
        if not os.path.exists(os.path.join(w, "f.obj")):
            return {s: (False, "compile error: " + (r.stdout + r.stderr)[-300:]) for s, _ in items}
        for sym, va in items:
            try:
                code, masked = obj_function(os.path.join(w, "f.obj"), sym)
            except SystemExit as e:
                res[sym] = (False, str(e)); continue
            code = code.rstrip(b"\x90\xcc")
            LAST[sym] = (code, masked)
            orig = exe_bytes(va, len(code) + 16)
            ok = all(a == b for i, (a, b) in enumerate(zip(code, orig)) if i not in masked)
            gs = FNSZ.get(va)
            if ok and gs is not None and gs - len(code) > 3: ok = False; code_short = True      # a byte prefix of the original is no match (overlay.py checks the same size later)
            else: code_short = False
            rbad = rwarn = []
            if ok and not os.environ.get("CMPFN_LOOSE"):
                rbad, rwarn = reloc_targets(os.path.join(w, "f.obj"), sym, va, code)
            if ok and rbad:
                res[sym] = (False, "RELOC TARGET differs: " + "; ".join(rbad[:6]))
            elif ok:
                res[sym] = (ok, "%d bytes" % len(code) + (" (reloc warnings: %d)" % len(rwarn) if rwarn else ""))
            elif code_short:
                res[sym] = (False, "[LENGTH 0.00] ours %d bytes, Ghidra function %d bytes ours %s orig %s" % (len(code), FNSZ[va], code.hex(), orig[:len(code)].hex()))
            else:
                try:
                    import asmdiff
                    cm = bytearray(code)
                    ms = sorted(masked)
                    for g in range(0, len(ms) - 3, 4):      # relocated 4 byte fields -> a placeholder address in the exe range
                        for k, v in enumerate((0x00, 0x00, 0x50, 0x00)):
                            cm[ms[g + k]] = v
                    kind, score, why = asmdiff.classify(bytes(cm), exe_bytes(va, len(code) + 32))
                    cls = "[%s %.2f] %s" % (kind, score, why[:160])
                except Exception as e:      # objdump missing etc.
                    cls = ""
                res[sym] = (ok, "%s ours %s orig %s" % (cls, code.hex(), orig[:len(code)].hex()))
        return res
    finally:
        shutil.rmtree(w, ignore_errors=True)


def main():
    if sys.argv[1] == "--manifest":
        base = os.path.dirname(os.path.abspath(sys.argv[2]))
        groups = {}
        near = set()
        for line in open(sys.argv[2]):
            line = line.strip()
            if not line or line.startswith("#"):
                continue
            f, sym, va, *fl = line.split()
            if f == "(none)":
                continue
            if "DIFFERENT" in fl:  # known non identical but compiled and kept as the working source
                fl = fl[:fl.index("DIFFERENT")]
                near.add(sym)
            elif fl and fl[-1] == "MATCH":
                fl = fl[:-1]
            groups.setdefault((f, tuple(fl) or ("/O2",)), []).append((sym, int(va, 16)))
        bad = 0
        CACHE = os.environ.get("CMPFN_CACHE", os.path.expanduser("~/.cache/mfl_cmpfn"))
        os.makedirs(CACHE, exist_ok=True)

        def run_group(key):
            """check one (file, flags) group; result cached by source + headers + flags + items (cmpfn.py itself and the exe are part of the key)"""
            f, fl = key
            items = groups[key]
            h = hashlib.sha1()
            for n in sorted(os.listdir(base)):
                if n.endswith(".h") or n == f:
                    h.update(n.encode() + open(os.path.join(base, n), "rb").read())
            h.update(os.environ.get("VC7_CL", "").encode() + repr((fl, items)).encode() + open(__file__, "rb").read() + open(os.path.join(os.path.dirname(os.path.abspath(__file__)), "asmdiff.py"), "rb").read())
            cp = os.path.join(CACHE, h.hexdigest() + ".pkl")
            if os.path.exists(cp):
                return pickle.load(open(cp, "rb"))
            r = check(os.path.join(base, f), items, list(fl))
            if not any(d.startswith("compile error") for _, d in r.values()):
                pickle.dump(r, open(cp, "wb"))
            return r

        os.makedirs(WORKROOT, exist_ok=True)
        subprocess.run([VC7, WORKROOT, "true"], env=dict(os.environ, VC7_SHELL="1"), capture_output=True)   # start the container once, before the threads
        # the groups are independent compiles: JOBS (default 6) of them at the same time in the one persistent container
        with ThreadPoolExecutor(int(os.environ.get("JOBS", "6"))) as ex:
            futs = [(k, ex.submit(run_group, k)) for k in groups]
            results = [(k, fu.result()) for k, fu in futs]
        for (f, fl), res in results:
            for sym, (ok, det) in res.items():
                if sym in near:
                    if ok:
                        status = "MATCH"
                    elif det.startswith("[REGISTERS") or det.startswith("[IDENTICAL"):
                        status = "EFFECTIVE"      # only register names / operand forms differ (reccmp's "effective match")
                    else:
                        status = "NEAR"
                    print("%-9s %s %s" % (status, sym, "" if ok else det[:70]))
                    bad += det.startswith("compile error") or det.startswith("symbol")
                    continue
                print("%-8s %s %s" % ("MATCH" if ok else "DIFFER", sym, det if not ok else ""))
                bad += not ok
        raise SystemExit(1 if bad else 0)
    src, sym, va = sys.argv[1], sys.argv[2], int(sys.argv[3], 16)
    flags = sys.argv[4:] or ["/O2"]
    ok, det = check(src, [(sym, va)], flags)[sym]
    print("%s @%08x: %s" % (sym, va, "MATCH " + det if ok else "DIFFERENT " + det))
    raise SystemExit(0 if ok else 1)


if __name__ == "__main__":
    main()
