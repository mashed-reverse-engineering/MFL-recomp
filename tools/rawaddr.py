#!/usr/bin/env python3
"""rawaddr.py: find (and rewrite) raw image data addresses in the C sources.  A literal like `return 0x68b028;` only works while .data stays at its
original address; in a linked build (tools/delink/link_ours.sh) it must be `(int)&symbol` so the linker relocates it.
A literal counts as an address when it lies in the exe data ranges (.rdata/.data/bss, PE header) AND the original code of the enclosing function
has a non-const relocation (output/delink_relocs.csv, tools/delink/classify.py) with exactly that target; everything else (masks, sizes, loop
limits, functions the symbol table cannot place) is not touched.
Usage: rawaddr.py [--quiet] [files...]               scan (default: src/{,tier1,automatch,automatch_agent,automatch_near,bridges}/*.c); exit 0 always
       rawaddr.py --rewrite [--check] [files...]     rewrite in place; --check recompiles each changed file (cmpfn.py, /O2 /Gy) and reverts it when a
                                                     function that matched before no longer matches (relocation masked compare)"""
import bisect, collections, csv, glob, os, re, struct, sys
H = os.path.dirname(os.path.abspath(__file__)); G = os.path.normpath(os.path.join(H, ".."))
R = os.path.join(G, "src")
EXE = os.environ.get("MFL_EXE", os.path.join(os.path.dirname(os.path.abspath(__file__)), "..", "game", "MFL.exe"))

def data_range():      # [.rdata start, end of the last data section incl. bss) from the PE section table (classify.py: .rdata .data _rwdseg)
    d = open(EXE, "rb").read(); e = struct.unpack_from("<I", d, 0x3C)[0]; ns, = struct.unpack_from("<H", d, e + 6); osz, = struct.unpack_from("<H", d, e + 20)
    base, = struct.unpack_from("<I", d, e + 24 + 28); sec = {}
    for i in range(ns):
        o = e + 24 + osz + i * 40; nm = d[o:o + 8].rstrip(b"\0").decode(); vs, va = struct.unpack_from("<II", d, o + 8); sec[nm] = (base + va, base + va + vs)
    return sec[".rdata"][0], sec["_rwdseg"][1], sec[".data"][0]
LO, HI, DATA0 = data_range()

AUTO = re.compile(r"^(g_data|g_physics|g_game|g_rec|g_ai_f|g_ftol_tbl|DAT|PTR|s|u|f|d|p)_|_[0-9a-f]{6}$")
def load():
    syms = {}; byaddr = collections.defaultdict(list)
    for f in (os.path.join(R, "symbols.txt"), os.path.join(R, "aliases.txt")):
        for l in open(f):
            q = l.split()
            if len(q) == 2 and re.fullmatch(r"[A-Za-z_]\w*", q[0]):
                a = int(q[1], 16)
                if q[0] not in syms: syms[q[0]] = a
                if LO <= a < HI and q[0] not in byaddr[a]: byaddr[a].append(q[0])
    for a in byaddr: byaddr[a].sort(key=lambda n: (bool(AUTO.search(n)), len(n)))
    fns = sorted((int(l.split()[0], 16), int(l.split()[2 - 1], 16) if False else int(l.split()[2])) for l in open(os.path.join(G, "output", "fnsizes.txt")))
    rel = []
    for r in csv.DictReader(open(os.path.join(G, "output", "delink_relocs.csv"))):
        if r["kind"] != "const": rel.append((int(r["site"], 16), int(r["target"], 16)))
    rel.sort()
    return syms, byaddr, sorted(byaddr), fns, rel, [s for s, t in rel]
SYMS, BYADDR, ADDRS, FNS, REL, RSITES = load()
FNSZ = dict(FNS); FSTARTS = [a for a, n in FNS]

def mask(t):    # comments and string/char literals -> spaces (same length)
    return re.sub(r'/\*.*?\*/|//[^\n]*|"(?:\\.|[^"\\\n])*"|\'(?:\\.|[^\'\\\n])*\'', lambda m: re.sub(r"[^\n]", " ", m.group(0)), t, flags=re.S)

def functions(m):    # [(name, start, end)] of the top level function bodies of masked text m
    out = []; depth = 0; last = 0
    for i, c in enumerate(m):
        if c == "{":
            if depth == 0:
                hd = re.sub(r"(?m)^[ \t]*#.*$", "", m[last:i]); k = re.search(r"\b([A-Za-z_]\w*)\s*\(", hd)
                if k and k.group(1) not in ("if", "while", "for", "switch", "sizeof", "__declspec"): out.append([k.group(1), last, None])
                else: out.append(None)
            depth += 1
        elif c == "}":
            depth -= 1
            if depth == 0:
                last = i + 1
                if out and out[-1] and out[-1][2] is None: out[-1][2] = i + 1
        elif c == ";" and depth == 0: last = i + 1
    return [f for f in out if f and f[2]]

LIT = re.compile(r"(?<![\w.])(0[xX][0-9a-fA-F]+|\d{6,8})[uUlL]*(?![\w.])")
def fn_targets(name):
    a = SYMS.get(name) or SYMS.get(re.sub(r"_c$", "", name))      # bridges: foo_c is the C body of the naked foo
    if a is None or a not in FNSZ: return None
    i = bisect.bisect_left(RSITES, a); k = bisect.bisect_right(FSTARTS, a); j = bisect.bisect_left(RSITES, FSTARTS[k] if k < len(FSTARTS) else a + FNSZ[a])      # up to the next function (fnsizes can be short)
    return sorted({t for s, t in REL[i:j]})      # a literal is an address when it is a target or within 0x800 of one (the compiler folds field offsets into it)

def sym_for(v):
    """(name, offset) of a symbol at or below v in the same section (.rdata / .data+bss), or None"""
    sec0 = DATA0 if v >= DATA0 else LO
    if v in BYADDR: return BYADDR[v][0], 0
    i = bisect.bisect_right(ADDRS, v) - 1
    if i < 0 or ADDRS[i] < sec0 or v - ADDRS[i] >= 0x1000: return None
    return BYADDR[ADDRS[i]][0], v - ADDRS[i]

def header_text(src):      # the text of every header the file includes (recursively; looked up next to the file, in src/, src/auto/)
    seen = {}; todo = [src]
    while todo:
        f = todo.pop()
        for h in re.findall(r'^[ \t]*#[ \t]*include[ \t]*"([^"]+)"', open(f, errors="replace").read(), re.M):
            for d in (os.path.dirname(src), R, os.path.join(R, "auto")):
                q = os.path.join(d, h)
                if os.path.exists(q):
                    if q not in seen: seen[q] = open(q, errors="replace").read(); todo.append(q)
                    break
    return "\n".join(seen.values())

CAST = re.compile(r"\(\s*(?:(?:const|unsigned|struct)\s+)*[A-Za-z_]\w*\s*\*+\s*\)\s*$")
def scan(path):
    """-> (text, hits [(start, end, value, fn, name, off, ptrcast)], skipped [(value, why)])"""
    t = open(path, errors="surrogateescape").read(); m = mask(t); fs = functions(m); hits = []; skipped = []
    for k in LIT.finditer(m):
        s = k.group(1); v = int(s, 16) if s[:2] in ("0x", "0X") else int(s)
        if not (LO <= v < HI): continue
        f = next((f for f in fs if f[1] <= k.start() < f[2]), None)
        dm = re.match(r"[ \t]*#[ \t]*define[ \t]+(\w+)[ \t]+\(?[ \t]*$", m[m.rfind("\n", 0, k.start()) + 1:k.start()])
        if dm:      # `#define NAME 0x...`: the functions using NAME decide
            us = [g for g in fs if re.search(r"\b%s\b" % dm.group(1), m[g[1]:g[2]])]
            tg = [x for g in us for x in (fn_targets(g[0]) or [])]; f = [dm.group(1), 0, 0] if us else None; fnames = [g[0] for g in us]
            if not f: skipped.append((v, "macro %s unused" % dm.group(1))); continue
        elif not f: skipped.append((v, "outside a function body")); continue
        else: tg = fn_targets(f[0]); fnames = [f[0]]
        if tg is None: skipped.append((v, "no address for %s" % f[0])); continue
        if not any(-0x800 < v - x < 0x800 for x in tg): skipped.append((v, "no relocation to it in %s (constant)" % f[0])); continue
        sy = sym_for(v)
        if not sy: skipped.append((v, "no symbol")); continue
        cast = CAST.search(m[max(0, k.start() - 60):k.start()])
        hits.append((k.start(1), k.end(1), v, ",".join(fnames), sy[0], sy[1], bool(cast)))
    return t, hits, skipped

def files(args):
    if args: return [os.path.abspath(a) for a in args]
    return sorted(sum((glob.glob(os.path.join(R, d, "*.c")) for d in ("", "tier1", "automatch", "automatch_agent", "automatch_near", "bridges")), []))

def rewrite(path, t, hits):
    """-> new text; externs added after the file's last extern (else last #include) before the first function"""
    hdr = header_text(path); need = []; orig = t; first = min(h[0] for h in hits)
    for h in sorted(hits, reverse=True):
        a, b, v, fn, nm, off, pc = h
        e = "&%s" % nm if (pc and off == 0) else ("((int)&%s)" % nm if off == 0 else ("((int)&%s + 0x%x)" % (nm, off)))
        if pc and off: e = "((int)&%s + 0x%x)" % (nm, off)
        t = t[:a] + e + t[b:]
        if nm not in need: need.append(nm)
    new = []
    for nm in need:
        dl = re.compile(r"^[^\n]*\bextern\b[^;\n]*\b%s\b[^\n]*;[ \t]*$" % nm, re.M)
        if dl.search(hdr): continue
        d = dl.search(orig)
        if d and d.start() < first: continue
        if d: new.append(d.group(0)); continue      # declared in the file but after the first use: copy the declaration up
        if re.search(r"\b%s\b" % nm, hdr): return None, "name %s clashes with a header declaration" % nm
        new.append(nm)
    if new:
        L = t.split("\n"); fl = orig[:first].count("\n")      # line of the first rewritten literal: declare before it
        ext = [i for i, l in enumerate(L[:fl]) if re.match(r"(/\*[0-9a-f]{8}\*/ )?extern ", l)]
        inc = [i for i, l in enumerate(L[:fl]) if l.startswith("#include")]
        at = (ext[-1] if ext else inc[-1] if inc else -1) + 1
        style = ext and re.match(r"/\*[0-9a-f]{8}\*/ ", L[ext[-1]])
        for nm in new:
            L.insert(at, nm if nm.lstrip().startswith(("extern", "/*")) else ("/*%08x*/ " % SYMS[nm] if style else "") + "extern char %s[];" % nm); at += 1
        t = "\n".join(L)
    return t, ""

def main():
    a = [x for x in sys.argv[1:] if not x.startswith("--")]; quiet = "--quiet" in sys.argv; rw = "--rewrite" in sys.argv; chk = "--check" in sys.argv
    tot = nf = 0; sk = collections.Counter(); rewritten = reverted = 0; log = []
    for p in files(a):
        t, hits, skipped = scan(p)
        for v, why in skipped: sk[re.sub(r"\S+ ?$", "", why) if why.startswith("no relocation") else why.split(" for ")[0]] += 1
        if not hits: continue
        tot += len(hits); nf += 1
        if not quiet and not rw: print("%s: %s" % (os.path.relpath(p, G), ", ".join("%s 0x%x" % (h[3], h[2]) for h in hits[:6])))
        if rw:
            nt, why = rewrite(p, t, hits)
            if nt is None: print("SKIP %s: %s" % (os.path.relpath(p, G), why)); sk["clash"] += len(hits); continue
            open(p, "w", errors="surrogateescape").write(nt)
            if chk and not verify(p, t, nt, sorted({n for h in hits for n in h[3].split(',')})):
                open(p, "w", errors="surrogateescape").write(t); reverted += 1; print("REVERTED %s" % os.path.relpath(p, G)); continue
            rewritten += len(hits); log.append(p)
    print("rawaddr: %d raw data address literal(s) in %d file(s)%s" % (tot, nf, "" if not rw else "; rewritten %d, reverted files %d" % (rewritten, reverted)))
    if not quiet and sk: print("not touched:", dict(sk))

def verify(path, old, new, fns):
    """every function that matched before must still match; one that did not must keep the same code (compared with the relocated fields zeroed)"""
    sys.path.insert(0, H); import cmpfn
    items = [(f, SYMS.get(f) or SYMS[re.sub(r"_c$", "", f)]) for f in fns]
    r = cmpfn.check(path, items, ["/O2", "/Gy"]); after = dict(cmpfn.LAST)
    bad = [f for f, (ok, det) in r.items() if not ok]
    if not bad: return True
    open(path, "w", errors="surrogateescape").write(old)
    r0 = cmpfn.check(path, items, ["/O2", "/Gy"]); before = dict(cmpfn.LAST)
    open(path, "w", errors="surrogateescape").write(new)
    for f in bad:
        if r0[f][0] or f not in after or f not in before: return False
        (ca, ma), (cb, mb) = after[f], before[f]
        if len(ca) != len(cb) or bytes(0 if k in ma else x for k, x in enumerate(ca)) != bytes(0 if k in ma else x for k, x in enumerate(cb)): return False
    return True

if __name__ == "__main__": main()
