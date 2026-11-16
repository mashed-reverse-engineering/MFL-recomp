#!/usr/bin/env python3
"""bindcheck.py check <A link dir> <B link dir> <batch dir> [--allow-ptr] [--assert-bind OLD=NEW ...] [--log F]  |  sites  |  --selftest
--assert-bind _func_0x0056e810=_rwp_obj_vec_at_20 (map names): a call that A binds to OLD without a stub (mkobj /alternatename, here libcmt
__tolower = the same 3 instructions) and B binds to NEW is accepted as ALIAS; at least one such site must exist; both targets' first bytes are printed.
R34 (W12, plan W11 v2 4.1).  A = scratch standalone link of the tree, B = the same link with the batch files (linkpair.py, one PYTHONHASHSEED).
 point 1  every byte that differs between A/ours.exe and B/ours.exe is part of a 4-byte operand whose A value reaches an original-VA entry stub
          (5 bytes `E9 rel32`) and whose B value reaches that stub's jmp destination: REL32 (E8 / E9 / 0F 8x before it) = a call rebound, DIR32 = a
          pointer value rebound (FAIL unless --allow-ptr: the stored pointer is no longer the original VA).  Anything else FAILs.  Section table and
          PE header (TimeDateStamp / CheckSum masked) must be equal.  Map: every symbol in both maps sits at the same address; a symbol only in A must
          be a decompiler name (func_0x / FUN_ / LAB_ <va>) = the alias mkobj no longer defines; none only in B.
 point 2b each rebound stub is still in B (bytes at the original VA unchanged) and still jumps to the same symbol.
 point 2a each batch file's object (A/o vs B/o, build flags of the link) is equal in code bytes and relocation offsets / types; target names equal
          after tools/readable_check (RD2 f0x copy) canon(): a name and its alias VA count as one (masked compare).
 point 2  each rebound original VA is checked against the hook / patch lists (`sites`): hooks.txt (site range and the decoded callee), dwords.txt,
          link_ours.sh objcopy renames, HS spec files with HOOK_ROWS or in hook_allow.txt, d3dtrace (d3d9.c, run.sh; the N1 statehash shim lives in
          d3d9.c), tools/statehash.  A hit FAILs the file that holds the call.
Per file verdict lines `FILE OK|DROP <file>: reason`; last line PASS / FAIL.  Exit 0 = PASS."""
import collections, glob, os, re, struct, sys
G = os.path.normpath(os.path.join(os.path.dirname(os.path.abspath(__file__)), "..", ".."))      # tools/delink/ or agents/<lane>/: both two below ghidra/
ORIG = os.path.join(os.path.dirname(G), "MashedGame", "FullyLoaded", "installed", "App_Executables", "MFL.exe")
CODE = (0x401000, 0x5dd000)

def pe(path):
    """-> (masked header bytes, [(name, va, vsize, raw bytes)], image base)"""
    b = open(path, "rb").read(); o = struct.unpack_from("<I", b, 0x3c)[0]
    nsec, optsz = struct.unpack_from("<H", b, o + 6)[0], struct.unpack_from("<H", b, o + 20)[0]
    base = struct.unpack_from("<I", b, o + 24 + 28)[0]
    h = bytearray(b[:o + 24 + optsz + 40 * nsec]); h[o + 8:o + 12] = bytes(4); h[o + 24 + 64:o + 24 + 68] = bytes(4)
    secs = []
    for i in range(nsec):
        s = o + 24 + optsz + 40 * i
        nm = b[s:s + 8].rstrip(b"\0").decode(errors="replace"); vs, va, rs, rp = struct.unpack_from("<IIII", b, s + 8)
        secs.append((nm, base + va, vs, b[rp:rp + rs]))
    return bytes(h), secs, base

class Img:
    def __init__(self, secs): self.secs = secs
    def at(self, va, n):
        for _, a, vs, raw in self.secs:
            if a <= va and va + n <= a + len(raw): return raw[va - a:va - a + n]
        return None
    def stub(self, va):
        """destination of the 5-byte entry jmp at va, or None"""
        x = self.at(va, 5)
        return (va + 5 + struct.unpack_from("<i", x, 1)[0]) & 0xFFFFFFFF if x and x[0] == 0xE9 else None

def classify(ia, ib, runs, alias=()):
    """runs: [(va, length)] of differing bytes -> ([(kind, site, old target, new target)], [failure text]); alias: {(old va, new va)} allowed
    without a stub (--assert-bind: a decompiler name that mkobj bound by /alternatename, e.g. to a split CRT member)"""
    ok, bad = [], []
    for va, n in runs:
        hit = None
        for o in range(va - 3, va + 1):
            if o + 4 < va + n: continue
            xa, xb, pre, preb = ia.at(o, 4), ib.at(o, 4), ia.at(o - 2, 2), ib.at(o - 2, 2)
            if not xa or not xb or not pre or not preb: continue
            # opcode bytes must be equal in A and B; the byte before a 1-byte opcode may be the tail of the previous (also rebound) operand
            rel = (pre[1] in (0xE8, 0xE9) and preb[1] == pre[1]) or (pre[0] == 0x0F and 0x80 <= pre[1] <= 0x8F and preb == pre)
            for kind, f in (("REL32", lambda x: (o + 4 + struct.unpack_from("<i", x)[0]) & 0xFFFFFFFF),) * rel + (("DIR32", lambda x: struct.unpack_from("<I", x)[0]),):
                ta, tb = f(xa), f(xb); d = ia.stub(ta)
                if kind == "REL32" and (ta, tb) in alias: hit = ("ALIAS", o, ta, tb); break
                if d is not None and d == tb and ta != tb:
                    if ib.stub(ta) != d: bad.append("%08x: stub %08x changed in B (2b)" % (o, ta)); hit = True; break
                    hit = (kind, o, ta, tb); break
            if hit: break
        if hit is True: continue
        if hit: ok.append(hit)
        else:
            xa, xb = ia.at(va - 4, n + 8), ib.at(va - 4, n + 8)
            ta = (va - 1 + 4 + struct.unpack_from("<i", ia.at(va - 1, 4))[0]) & 0xFFFFFFFF if ia.at(va - 1, 4) else 0
            bad.append("%08x+%d: difference is no stub rebind (A %s / B %s; A rel32 at %08x -> %08x, bytes there %s)" % (
                va, n, xa.hex() if xa else "?", xb.hex() if xb else "?", va - 1, ta, (ia.at(ta, 8) or b"").hex()))
    return ok, bad

def runs_of(sa, sb):
    out = []
    for (_, a, _, ra), (_, _, _, rb) in zip(sa, sb):
        i = 0
        while i < len(ra):
            if ra[i] != rb[i]:
                j = i
                while j < len(ra) and ra[j] != rb[j]: j += 1
                out.append((a + i, j - i)); i = j
            else: i += 1
    return out

MAPL = re.compile(r"^\s*[0-9a-f]{4}:[0-9a-f]{8}\s+(\S+)\s+([0-9a-f]{8})\s+(?:f\s+)?(?:i\s+)?(\S+)?\s*$")
def mapsyms(p):
    """-> {name: (va, obj)} from the 'Publics by Value' part of a link.exe map"""
    d = {}
    for l in open(p, errors="replace"):
        m = MAPL.match(l)
        if m: d.setdefault(m.group(1), (int(m.group(2), 16), m.group(3) or ""))
    return d

DECOMP = re.compile(r"^_?(func_0x|FUN_|LAB_)[0-9a-f]{8}$")

def alias_va():
    d = collections.defaultdict(set)
    for f in ("src/aliases.txt", "src/symbols.txt"):
        for l in open(os.path.join(G, f), errors="replace"):
            w = l.split()
            if len(w) >= 2 and re.fullmatch(r"[0-9a-f]{6,8}", w[1]): d[w[0]].add(int(w[1], 16))
    return d

def sites():
    """-> [(lo, hi, source)]: original-VA ranges a hook / patch touches (a callee VA = a 5 byte range at it)"""
    out = []; av = alias_va()
    ob = open(ORIG, "rb").read(); _, osecs, _ = pe(ORIG); oi = Img(osecs)
    for l in open(G + "/src/harness/hooks.txt"):
        if not l.strip() or l.startswith("#"): continue
        s, nm = l.split()[0].split("="); s = int(s, 16); x = oi.at(s, 5)
        out.append((s, s + 5, "hooks.txt site %08x=%s" % (s, nm)))
        if x and x[0] == 0xE8: t = (s + 5 + struct.unpack_from("<i", x, 1)[0]) & 0xFFFFFFFF; out.append((t, t + 5, "hooks.txt callee %08x of site %08x" % (t, s)))
    for l in open(G + "/src/harness/dwords.txt"):
        if not l.strip() or l.startswith("#"): continue
        s = int(l.split()[0].split("=")[0], 16); out.append((s, s + 4, "dwords.txt %08x" % s))
    lo = open(G + "/tools/delink/link_ours.sh").read()
    for nm in sorted(set(re.findall(r"--redefine-sym \$?\w*?_?(\w+?)=", lo)) | set(re.findall(r'"_(\w+)=_\w+"', lo))):
        for va in av.get(nm, ()): out.append((va, va + 5, "link_ours.sh objcopy rename %s" % nm))
    hf = [f for f in glob.glob(G + "/tools/diffemu_audit/tweak/specs/*.py") if "HOOK_ROWS" in open(f, errors="replace").read()]
    al = {l.split()[0] for l in open(G + "/tools/diffemu_audit/tweak/hook_allow.txt") if l.strip() and not l.startswith("#")}
    hf += [G + "/tools/diffemu_audit/tweak/specs/" + f for f in sorted(al)]
    for f in sorted(set(hf)) + [G + "/tools/d3dtrace/d3d9.c", G + "/tools/d3dtrace/run.sh"] + sorted(glob.glob(G + "/tools/statehash/*.py") + glob.glob(G + "/tools/statehash/*.sh")):
        if not os.path.exists(f): continue
        for h in set(re.findall(r"\b0x([0-9a-fA-F]{6,8})\b", open(f, errors="replace").read())):
            va = int(h, 16)
            if CODE[0] <= va < CODE[1]: out.append((va, va + 1, "literal in %s" % os.path.relpath(f, G)))
    return out

def hookhits(va, S):
    return [s for lo, hi, s in S if lo < va + 5 and va < hi]

def objcmp(pa, pb):
    sys.path.insert(0, G + "/tools"); import readable_check as rc
    _c0 = rc.canon
    def canon(n):      # + func_0x<va> = that VA (the RD2 readable_check_f0x.py rule): a decompiler name and the alias of its VA are one target
        m = re.fullmatch(r"[_@]?func_0x([0-9a-f]{8})", n)
        return "@VA" + m.group(1) if m else _c0(n)
    rc.canon = canon
    a, b = rc.parse(pa), rc.parse(pb)
    if set(a) != set(b): return "sections differ: %s" % sorted(set(a) ^ set(b))[:4]
    for k in sorted(a):
        if a[k][1] != b[k][1]: return "%s: code bytes differ" % k
        if [r[:2] for r in a[k][2]] != [r[:2] for r in b[k][2]]: return "%s: relocation offsets / types differ" % k
        if a[k][2] != b[k][2]:
            x = sorted(set(a[k][2]) ^ set(b[k][2]))[:2]; return "%s: relocation target differs after canon: %s" % (k, x)
    return None

def check(A, B, bd, allow_ptr, asserts=()):
    fails = []; drop = collections.defaultdict(list)
    files = sorted(os.path.basename(l.split()[0]) for l in open(bd + "/MANIFEST") if l.strip() and not l.startswith("#"))      # stage layout (scr.py MANIFEST)
    ha, sa, _ = pe(A + "/ours.exe"); hb, sb, _ = pe(B + "/ours.exe")
    if ha != hb: fails.append("PE header differs (timestamp / checksum masked)")
    if [s[:3] + (len(s[3]),) for s in sa] != [s[:3] + (len(s[3]),) for s in sb]: print("FAIL section tables differ"); print("FAIL"); return 1
    ia, ib = Img(sa), Img(sb)
    ma, mb = mapsyms(A + "/ours.map"), mapsyms(B + "/ours.map")
    alias = set()
    for old, new in asserts:      # --assert-bind OLD=NEW (map names with the leading _): A binds OLD, B binds NEW at the same call sites
        if old not in ma or new not in mb: fails.append("assert-bind %s=%s: %s not in the %s map" % (old, new, old if old not in ma else new, "A" if old not in ma else "B")); continue
        alias.add((ma[old][0], mb[new][0]))
        print("assert-bind %s %08x (%s) -> %s %08x (%s); first 8 bytes A %s / B %s" % (old, ma[old][0], ma[old][1], new, mb[new][0], mb[new][1],
              (ia.at(ma[old][0], 8) or b"").hex(), (ib.at(mb[new][0], 8) or b"").hex()))
    ok, bad = classify(ia, ib, runs_of(sa, sb), alias)
    for old, new in asserts:
        if alias and not any(k == "ALIAS" and (ta, tb) == (ma.get(old, (0,))[0], mb.get(new, (0,))[0]) for k, _, ta, tb in ok): fails.append("assert-bind %s=%s: no call site rebound" % (old, new))
    for n in sorted(set(ma) & set(mb)):
        if ma[n][0] != mb[n][0]: fails.append("map: %s %08x -> %08x" % (n, ma[n][0], mb[n][0]))
    gone = sorted(set(ma) - set(mb)); new = sorted(set(mb) - set(ma))
    fails += ["map: only in A, not a decompiler name: %s" % n for n in gone if not DECOMP.match(n)]
    fails += ["map: only in B: %s" % n for n in new]
    byva = sorted((v, n, o) for n, (v, o) in mb.items()); vas = [x[0] for x in byva]
    import bisect
    def owner(va):
        i = bisect.bisect_right(vas, va) - 1
        return byva[i] if i >= 0 else (0, "?", "?")
    for x in bad:      # a difference that is no rebind: the file holding it is dropped (the run stays FAIL until it is out of the batch)
        _, fn, obj = owner(int(x[:8], 16)); src = re.sub(r"\.obj$", ".c", obj)
        (drop[src] if src in files else fails).append("%s (in %s, %s)" % (x, fn, obj) if src in files else x)
    S = sites(); dst = {v: n for n, (v, o) in mb.items()}
    for kind, site, ta, tb in ok:
        _, fn, obj = owner(site); src = re.sub(r"\.obj$", ".c", obj)
        hh = hookhits(ta, S); hs = [h for h in hh if "/specs/" in h]; hh = [h for h in hh if h not in hs]
        print("%s %08x in %s (%s): %08x stub -> %s %08x%s%s" % (kind, site, fn, obj, ta, dst.get(tb, "?"), tb, "  HOOK: " + "; ".join(hh) if hh else "",
              "  HS-NOTE (diffemu core.py binds the alias to the same VA; HS rerun covers it): " + "; ".join(hs) if hs else ""))
        if src not in files: fails.append("rebind %08x in %s: not a batch file" % (site, obj))
        if hh: drop[src].append("rebound %08x is a hook / patch site (%s)" % (ta, hh[0]))
        if kind == "DIR32" and not allow_ptr: drop[src].append("pointer value %08x -> %08x (DIR32)" % (ta, tb))
    print("rebinds: %d REL32, %d DIR32, %d ALIAS; map: %d decompiler names gone" % tuple([sum(k == x for k, *_ in ok) for x in ("REL32", "DIR32", "ALIAS")] + [len(gone)]))
    for f in files:
        st = os.path.splitext(f)[0] + ".obj"; pa, pb = A + "/o/" + st, B + "/o/" + st
        if not (os.path.exists(pa) and os.path.exists(pb)): drop[f].append("object o/%s missing in A or B" % st); continue
        r = objcmp(pa, pb)
        if r: drop[f].append("2a " + r)
    for f in files:
        print("FILE %s %s%s" % ("DROP" if drop[f] else "OK", f, ": " + "; ".join(drop[f]) if drop[f] else ""))
    for x in fails: print("FAIL", x)
    v = not fails and not any(drop.values()); print("PASS" if v else "FAIL"); return 0 if v else 1

def selftest():
    """images built here: a stub at S (E9 -> D); call / jmp / jcc / pointer to S in A, to D in B; then the bad cases"""
    S, D, C = 0x401100, 0x402000, 0x401000
    def img(call_t, ptr_t, stub_d=D, junk=0):
        b = bytearray(0x1100)
        b[S - C] = 0xE9; struct.pack_into("<i", b, S - C + 1, stub_d - (S + 5))
        b[0x10] = 0xE8; struct.pack_into("<i", b, 0x11, call_t - (C + 0x15))
        b[0x20] = 0x0F; b[0x21] = 0x84; struct.pack_into("<i", b, 0x22, call_t - (C + 0x26))
        b[0x30] = 0xC7; b[0x31] = 0x05; struct.pack_into("<I", b, 0x36, ptr_t)
        for k in (0x40, 0x45):      # back to back: the byte before the second E8 is the first operand's last byte
            b[k] = 0xE8; struct.pack_into("<i", b, k + 1, call_t - (C + k + 5))
        b[0x50] = junk
        return [(".text", C, len(b), bytes(b))]
    def run(a, b):
        return classify(Img(a), Img(b), runs_of(a, b))
    ok, bad = run(img(S, S), img(D, D))
    assert not bad and sorted(k for k, *_ in ok) == ["DIR32"] + ["REL32"] * 4, (ok, bad)
    ok, bad = run(img(S, S), img(D + 4, S)); assert bad and not ok, (ok, bad)                 # new target is not the stub's destination
    ok, bad = run(img(S, S), img(S, S, junk=1)); assert bad, bad                               # a byte that is no operand
    ok, bad = run(img(S, S), img(D, S, stub_d=D)); assert not bad and len(ok) == 4
    a = img(S, S); b = img(D, S, stub_d=D + 0x10)                                              # stub moved in B: 2b
    ok, bad = run(a, b); assert bad, (ok, bad)
    X = 0x401200                                                                                # no stub at X: allowed only through --assert-bind
    ok, bad = run(img(X, S), img(X + 0x10, S)); assert bad and not ok, (ok, bad)
    ok, bad = classify(Img(img(X, S)), Img(img(X + 0x10, S)), runs_of(img(X, S), img(X + 0x10, S)), {(X, X + 0x10)})
    assert not bad and [k for k, *_ in ok] == ["ALIAS"] * 4, (ok, bad)
    assert DECOMP.match("_func_0x00401100") and not DECOMP.match("_course_helper")
    print("bindcheck selftest: ok (rel32 / jcc / dir32 rebinds found; wrong target, stray byte, changed stub FAIL)")

if __name__ == "__main__":
    a = sys.argv[1:]
    if a[:1] == ["--selftest"]: selftest()
    elif a[:1] == ["rows"] and len(a) == 3:      # rows <link dir> <batch dir>: the map's functions of the batch objects = the diffemu rows to rerun (rule 21)
        objs = {os.path.splitext(os.path.basename(l.split()[0]))[0] + ".obj" for l in open(a[2] + "/MANIFEST") if l.strip() and not l.startswith("#")}
        ms = mapsyms(a[1] + "/ours.map")
        print(" ".join(sorted({re.sub(r"^_|@\d+$", "", n) for n, (v, o) in ms.items() if o in objs and not n.startswith(("?", "$", "__real", "__xmm")) and not n.endswith("_r")})))      # naked `_r` adapters are no rows
    elif a[:1] == ["sites"]:
        for lo, hi, s in sorted(sites()): print("%08x %08x %s" % (lo, hi, s))
    elif a[:1] == ["check"] and len(a) >= 4:
        if "--log" in a:      # --log <file>: the same lines into <file> too (READY reports quote it)
            lf = open(a[a.index("--log") + 1], "w"); so = sys.stdout
            class _T:
                def write(self, s): so.write(s); lf.write(s)
                def flush(self): so.flush(); lf.flush()
            sys.stdout = _T()
        ab = [tuple(a[i + 1].split("=", 1)) for i in range(len(a) - 1) if a[i] == "--assert-bind"]
        sys.exit(check(os.path.abspath(a[1]), os.path.abspath(a[2]), os.path.abspath(a[3]), "--allow-ptr" in a, ab))
    else: sys.exit(__doc__)
