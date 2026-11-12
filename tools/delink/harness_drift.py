#!/usr/bin/env python3
"""harness_drift.py [link dir] [--selftest]: harness-hook drift lint (plan_testing M4) on a HARNESS standalone link (tools/delink/link_ours.sh, default: the newest output/gate/*_standalone/link).
The test harness (src/harness/freeze.c) reaches the game three ways; each must still be live in the link:
  override  a freeze.c function named like an original function (controller_present = 0x4a5b20): the entry jmp at that address must go to freeze.c's function, and no other object of
            ours may call the address by another name (controller_present_thunk_4a5b20): such a C caller bypasses the harness (link_ours.sh renames the known one with objcopy --redefine-sym)
  hook      src/harness/hooks.txt `VA=sym`: the call at VA must target sym, and the original function containing VA must not be replaced by our C (its body is then dead code),
            unless our object for that function references sym itself (link_ours.sh renames the original symbol to the harness one there)
  dword     src/harness/dwords.txt `VA=sym`: the operand at VA must hold &sym, same containing-function rule
Prints one line per problem; exit 1 when there is any.  --selftest: the same checks on the newest link with injected drift (a wrong entry jmp, an unrenamed thunk caller) must fire."""
import bisect, glob, os, re, struct, sys
H = os.path.dirname(os.path.abspath(__file__)); G = os.path.normpath(os.path.join(H, "..", ".."))
R = os.path.join(G, "src")


def coff_undefs(path):
    """external symbols an object leaves undefined (COFF symbol table, as mkobj.coff_syms)"""
    d = open(path, "rb").read(); so, ns = struct.unpack_from("<II", d, 8); st = so + ns * 18; i = 0; out = set()
    while i < ns:
        o = so + i * 18; raw = d[o:o + 8]
        nm = d[st + struct.unpack_from("<I", raw, 4)[0]:d.index(b"\0", st + struct.unpack_from("<I", raw, 4)[0])].decode() if raw[:4] == b"\0\0\0\0" else raw.rstrip(b"\0").decode()
        val, sec, typ, cls, aux = struct.unpack_from("<IhHBB", d, o + 8)
        if cls == 2 and sec == 0 and val == 0: out.add(nm)
        i += 1 + aux
    return out


def pe_reader(exe):
    d = open(exe, "rb").read(); pe = struct.unpack_from("<I", d, 0x3c)[0]; n = struct.unpack_from("<H", d, pe + 6)[0]; opt = struct.unpack_from("<H", d, pe + 20)[0]
    base = struct.unpack_from("<I", d, pe + 24 + 28)[0]; secs = [struct.unpack_from("<8xIIII", d, pe + 24 + opt + 40 * k) for k in range(n)]
    def read(va, k):
        r = va - base
        for vs, sva, rs, rp in secs:
            if sva <= r < sva + max(vs, rs): return d[rp + r - sva:rp + r - sva + k]
        return b""
    return read


def undec(n): return n[1:].split("@")[0] if n.startswith(("_", "@")) else n


def check(L, read=None, undefs=None):
    """problems (list of strings) of link dir L; read(va, n) / undefs {src: set(undefined names)} can be injected (selftest)"""
    names = {}      # every name -> va (symbols.txt, aliases.txt, fnsizes.txt)
    for f in ("src/symbols.txt", "src/aliases.txt"):
        for l in open(os.path.join(G, f)):
            q = l.split()
            if len(q) >= 2 and not q[0].startswith("#"): names.setdefault(q[0], int(q[1], 16))
    fn = sorted((int(l.split()[0], 16), l.split()[1]) for l in open(os.path.join(G, "output", "fnsizes.txt")) if len(l.split()) >= 2)
    for a, n in fn: names.setdefault(n, a)
    starts = [a for a, _ in fn]
    pub = {}      # map publics: name -> (va, object)
    for l in open(os.path.join(L, "ours.map"), errors="replace"):
        m = re.match(r"\s*[0-9a-f]{4}:[0-9a-f]{8}\s+(\S+)\s+([0-9a-f]{8})\s+(?:f\s+)?(?:i\s+)?(\S+)\s*$", l)
        if m: pub.setdefault(m.group(1), (int(m.group(2), 16), m.group(3)))
    objs = [l.split() for l in open(os.path.join(L, "objs_local.txt")) if len(l.split()) == 2]
    ours = {os.path.basename(o) for _, o in objs}      # our objects (the image's own code is app.obj / layers / mfl.obj)
    src_of = {os.path.basename(o): s for s, o in objs}
    replaced = {names[undec(n)] for n, (_, o) in pub.items() if o in ours and undec(n) in names}
    read = read or pe_reader(os.path.join(L, "ours.exe")); bad = []
    def addr(sym): return pub.get("_" + sym, (None,))[0]
    def owner(va):
        i = bisect.bisect_right(starts, va) - 1
        return fn[i] if i >= 0 else (0, "?")
    def target_name(t): return next((undec(n) for n, (v, o) in pub.items() if v == t), "0x%x" % t)
    # overrides
    src = open(os.path.join(R, "harness", "freeze.c")).read()
    defs = [m.group(1) for m in re.finditer(r"^(?!static\b)[A-Za-z_][\w \t\*]*?\b(\w+)\s*\([^;{]*\)\s*\{?\s*$", src, flags=re.M)]
    if undefs is None: undefs = {s: {undec(u) for u in coff_undefs(o)} for s, o in objs if s != "freeze.c" and os.path.exists(o)}
    for d in defs:
        va = names.get(d)
        if va is None or va >= 0x5d7000: continue      # a hook function, not an override of an original one
        b = read(va, 5); want = addr(d)
        got = va + 5 + struct.unpack("<i", b[1:5])[0] if len(b) == 5 and b[0] == 0xE9 else None
        if want is None or got != want: bad.append("override %s 0x%x: the entry %s, not to freeze.c's %s (0x%x)" % (d, va, "jumps to %s" % target_name(got) if got else "is no jmp", d, want or 0))
        al = {n for n, v in names.items() if v == va and n != d}
        for s, u in sorted(undefs.items()):
            for n in sorted(al & u): bad.append("override %s 0x%x: %s calls it as `%s`, bypassing the harness (add `objcopy --redefine-sym _%s=_%s` in link_ours.sh, or call it by `%s`)" % (d, va, s, n, n, d, d))
    # hooks / dwords
    for lst, kind in (("hooks.txt", "hook"), ("dwords.txt", "dword")):
        for l in open(os.path.join(R, "harness", lst)):
            if not l.strip() or l.startswith("#"): continue
            va_s, sym = l.split()[0].split("="); va = int(va_s, 16); want = addr(sym)
            if kind == "hook":
                b = read(va, 5); got = va + 5 + struct.unpack("<i", b[1:5])[0] if len(b) == 5 and b[0] == 0xE8 else None
            else:
                b = read(va, 4); got = struct.unpack("<I", b)[0] if len(b) == 4 else None
            fa, fname = owner(va)
            if fa not in replaced and (want is None or got != want): bad.append("%s 0x%x=%s: %s" % (kind, va, sym, "the site holds %s" % target_name(got) if got else "no call / operand there"))
            if fa in replaced and sym in undefs.get(src_of.get(pub.get("_" + fname, (0, ""))[1]), ()): continue      # link_ours.sh renamed the symbol in our object to the harness one: live
            if fa in replaced: bad.append("%s 0x%x=%s: inside %s (0x%x), which our C replaces in this link: the patched original body never runs (inert hook)" % (kind, va, sym, fname, fa))
    return bad


def newest():
    c = sorted(p for p in glob.glob(os.path.join(G, "output", "gate", "*_standalone", "link")) if os.path.exists(os.path.join(p, "ours.map")) and os.path.exists(os.path.join(p, "ours.exe")) and os.path.isdir(os.path.join(os.path.dirname(p), "link", "harness")))
    return c[-1] if c else None


def selftest():
    L = newest()
    if not L: print("harness_drift selftest: no standalone harness link under output/gate (skipped)"); return 0
    base = check(L, undefs={}); real = pe_reader(os.path.join(L, "ours.exe")); va = 0x4a5b20
    def wrong(a, n): return b"\xE9\0\0\0\0" if a == va else real(a, n)      # entry jmp to the next instruction: the tier 1 thunk instead of the harness
    a = [x for x in check(L, read=wrong, undefs={}) if x not in base]
    b = [x for x in check(L, undefs={"negtest.c": {"controller_present_thunk_4a5b20"}}) if x not in base]
    ok = any("override controller_present" in x and "entry" in x for x in a) and any("bypassing the harness" in x for x in b)
    for x in a + b: print("  injected: " + x)
    print("harness_drift selftest on %s: %s" % (os.path.relpath(L, G), "OK" if ok else "FAILED (injected drift not reported)")); return 0 if ok else 1


if __name__ == "__main__":
    a = sys.argv[1:]
    if "--selftest" in a: sys.exit(selftest())
    L = a[0] if a else newest()
    if not L: sys.exit("harness_drift: no link dir")
    bad = check(L)
    for x in bad: print(x)
    print("harness_drift %s: %d problem(s)" % (os.path.relpath(L, G), len(bad))); sys.exit(1 if bad else 0)
