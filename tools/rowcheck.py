#!/usr/bin/env python3
"""rowcheck.py [--selftest] [--dll <mflrw.dll>]  (W7 T8, run by check_all.sh)
(1) every `GetProcAddress(mflrw, "<name>")` in src/automatch_near/an_hk_612.c / an_hk_612_*.inc, and every DLL side of a tools/delink/rw_<g>.txt row of the
    gate's LIB_RW groups (gate.sh default list), is an export of the pinned output/librw/mflrw.dll: a missing bind export makes the group run unbound.
(2) every installed tier 2 row (src/automatch_near_names.txt `va name file`) and every exe side of a LIB_RW row (resolved like mkobj: src/symbols.txt,
    src/aliases.txt, output/delink_sdk/lib_names.txt) has an output/fnsizes.txt row at its address: without one mkobj only patches the entry jmp and the
    body stays original (D3D9RasterClearGeneric, W6).  Known exceptions: tools/rowcheck_allow.txt (`name  # reason`).
Exit 1 on any problem; last line `rowcheck: ...`."""
import glob, os, re, struct, subprocess, sys

G = os.path.normpath(os.path.join(os.path.dirname(os.path.abspath(__file__)), ".."))


def dll_exports(path):
    """export names of a PE file (stdlib only)"""
    d = open(path, "rb").read()
    pe = struct.unpack_from("<I", d, 0x3c)[0]
    nsec, optsz = struct.unpack_from("<H", d, pe + 6)[0], struct.unpack_from("<H", d, pe + 20)[0]
    opt = pe + 24
    erva = struct.unpack_from("<I", d, opt + (96 if struct.unpack_from("<H", d, opt)[0] == 0x10b else 112))[0]
    secs = [struct.unpack_from("<IIII", d, opt + optsz + 40 * i + 8) for i in range(nsec)]      # vsize, va, rawsize, rawptr
    off = lambda rva: next(rp + rva - va for vs, va, rs, rp in secs if va <= rva < va + max(vs, rs))
    if not erva: return set()
    e = off(erva); n, names = struct.unpack_from("<I", d, e + 24)[0], struct.unpack_from("<I", d, e + 32)[0]
    out = set()
    for i in range(n):
        o = off(struct.unpack_from("<I", d, off(names) + 4 * i)[0]); out.add(d[o:d.index(b"\0", o)].decode())
    return out


def lib_rw_groups(gate):
    m = re.search(r"lr=\$\{LIB_RW-librw:([^}]*)\}", open(gate).read())
    return m.group(1).split(",") if m else []


def rw_rows(g):
    """-> [(exe name, dll export)] of tools/delink/rw_<g>.txt"""
    p = os.path.join(G, "tools", "delink", "rw_%s.txt" % g)
    out = []
    for l in (open(p) if os.path.exists(p) else []):
        l = l.split("#")[0].strip()
        if l:
            a, _, b = l.partition("=")
            out.append((a.strip(), (b or a).strip()))
    return out


def symmap():
    m = {}
    for f in ("src/symbols.txt", "src/aliases.txt", "output/delink_sdk/lib_names.txt"):
        for l in (open(os.path.join(G, f), errors="replace") if os.path.exists(os.path.join(G, f)) else []):
            q = l.split()
            if len(q) >= 2 and not q[0].startswith("#") and re.fullmatch(r"[0-9a-fA-F]{1,8}", q[1]): m.setdefault(q[0], int(q[1], 16))
    return m


def check(dll, exports=None, drop_fs=()):
    bad = []
    exp = dll_exports(dll) if exports is None else exports
    d = os.path.join(G, "src", "automatch_near")
    for f in sorted(glob.glob(os.path.join(d, "an_hk_612_*.inc")) + [os.path.join(d, "an_hk_612.c")]):
        for n in re.findall(r'GetProcAddress\s*\(\s*mflrw\s*,\s*"(\w+)"', open(f, errors="replace").read()):
            if n not in exp: bad.append("(1) %s: GetProcAddress name %s is not exported by %s" % (os.path.basename(f), n, os.path.basename(dll)))
    groups = lib_rw_groups(os.path.join(G, "tools", "gate.sh"))
    assert groups, "no LIB_RW default list in gate.sh"
    fs = {int(l.split()[0], 16) for l in open(os.path.join(G, "output", "fnsizes.txt")) if l.strip()} - set(drop_fs)
    allow = {}
    ap = os.path.join(G, "tools", "rowcheck_allow.txt")
    for l in (open(ap) if os.path.exists(ap) else []):
        n, _, why = l.partition("#")
        if n.strip(): allow[n.strip()] = why.strip()
    sm, used = symmap(), set()
    for g in groups:
        for a, b in rw_rows(g):
            if b not in exp: bad.append("(1) rw_%s.txt: %s=%s, export %s missing in %s" % (g, a, b, b, os.path.basename(dll)))
            va = sm.get(a)
            if va is None or va not in fs:
                if a in allow: used.add(a); continue
                bad.append("(2) rw_%s.txt: %s %s" % (g, a, "does not resolve (symbols / aliases / lib_names)" if va is None else "at %08x has no fnsizes row" % va))
    for l in open(os.path.join(G, "src", "automatch_near_names.txt")):
        q = l.split()
        if len(q) >= 2 and re.fullmatch(r"[0-9a-f]{8}", q[0]) and int(q[0], 16) not in fs:
            if q[1] in allow: used.add(q[1]); continue
            bad.append("(2) automatch_near_names.txt: %s %s has no fnsizes row" % (q[0], q[1]))
    bad += ["stale allow line %s" % n for n in sorted(set(allow) - used)]
    return bad, len(groups)


def selftest():
    exp = dll_exports(os.path.join(G, "output", "librw", "mflrw.dll"))
    ok = "mflrw_exe_bind" in exp and len(exp) > 100
    print("%-4s pinned DLL exports parsed (%d, mflrw_exe_bind present)" % ("ok" if ok else "FAIL", len(exp))); bad = not ok
    b0, _ = check(os.path.join(G, "output", "librw", "mflrw.dll"), exp - {"mflrw_matfx_bind"})
    ok = any("mflrw_matfx_bind is not exported" in x for x in b0)
    print("%-4s a missing bind export (mflrw_matfx_bind removed) is caught" % ("ok" if ok else "FAIL")); bad |= not ok
    a0, one = rw_rows(lib_rw_groups(os.path.join(G, "tools", "gate.sh"))[0])[0]
    b1, _ = check(os.path.join(G, "output", "librw", "mflrw.dll"), exp - {one})
    ok = any("export %s missing" % one in x for x in b1)
    print("%-4s a missing LIB_RW row export (%s removed) is caught" % ("ok" if ok else "FAIL", one)); bad |= not ok
    va = symmap()[a0]
    b2, _ = check(os.path.join(G, "output", "librw", "mflrw.dll"), exp, {va})
    ok = any("%s at %08x has no fnsizes row" % (a0, va) in x for x in b2)
    print("%-4s a LIB_RW row without fnsizes row (%s %08x removed) is caught" % ("ok" if ok else "FAIL", a0, va)); bad |= not ok
    t2 = next(l.split() for l in open(os.path.join(G, "src", "automatch_near_names.txt")) if re.match(r"[0-9a-f]{8} ", l))
    b3, _ = check(os.path.join(G, "output", "librw", "mflrw.dll"), exp, {int(t2[0], 16)})
    ok = any("%s %s has no fnsizes row" % (t2[0], t2[1]) in x for x in b3)
    print("%-4s a tier 2 row without fnsizes row (%s %s removed) is caught" % ("ok" if ok else "FAIL", t2[0], t2[1])); bad |= not ok
    b4, _ = check(os.path.join(G, "output", "librw", "mflrw.dll"), exp)
    print("%-4s the tree itself: %d problems" % ("ok" if not b4 else "FAIL", len(b4))); bad |= bool(b4)
    print("rowcheck selftest: " + ("FAIL" if bad else "PASS"))
    return 1 if bad else 0


if __name__ == "__main__":
    a = sys.argv[1:]
    if "--selftest" in a: sys.exit(selftest())
    dll = a[a.index("--dll") + 1] if "--dll" in a else os.path.join(G, "output", "librw", "mflrw.dll")
    bad, ng = check(dll)
    print("\n".join(bad[:40] + (["... %d more" % (len(bad) - 40)] if len(bad) > 40 else [])))
    print("rowcheck: %d LIB_RW groups, %d problems" % (ng, len(bad)))
    sys.exit(1 if bad else 0)
