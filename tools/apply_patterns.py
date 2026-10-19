#!/usr/bin/env python3
"""apply_patterns.py: scripted byte-match source patterns (public/docs/BYTEMATCH_PATTERNS.md) over the tier-2 TUs.

  apply_patterns.py [--tree src/automatch_near] [--names src/automatch_near_names.txt]
                    [--csv RESULT.csv] [--apply] [--jobs 4] [--only P1,P2] [--files an_hk_001.c,...]

DRY RUN by default: compiles and compares, writes only RESULT.csv; --apply rewrites the accepted edits into --tree.
Per file: baseline compile (tools/cmpfn.py: bytes + relocation targets vs MFL.exe), then for each pattern a variant
compile (all functions' edits of that pattern at once, then single-site variants); an edit is kept PER FUNCTION only if
its score (0 = byte identical, else number of +/- asm diff lines) gets lower (strict patterns: only if it becomes 0).
Already identical functions are never touched.  Global patterns (exit) are kept only if no function in the file gets worse.
Patterns (in order): exit, cmpL, cmpR, mulswap, lit, icmpswap, icmpoff(strict), inline(strict), fwdret(strict), retparam(strict).
Safe by construction: a kept byte-identical function is the same code as the original whatever the source says; the
non-identical 'improved' ones deserve a diffemu EQUAL check (swaps are IEEE-identical: a<b == b>a, NaN included).
Scratch tmp dir: env CMPFN_TMP (default below /tmp/claude-UID, must be mounted into the compiler container)."""
import argparse, csv, difflib, os, re, struct, sys, threading
from concurrent.futures import ThreadPoolExecutor

REPO = os.environ.get("MFL_REPO") or os.path.normpath(os.path.join(os.path.dirname(os.path.abspath(__file__)), ".."))
sys.path.insert(0, os.path.join(REPO, "tools"))
import cmpfn, asmdiff

MIRROR = {"<": ">", ">": "<", "<=": ">=", ">=": "<="}
OFF = {">": (">=", 1), ">=": (">", -1), "<": ("<=", -1), "<=": ("<", 1)}


def mask(t):
    """comments and string literals blanked (same length) so scans ignore them"""
    return re.sub(r'/\*.*?\*/|//[^\n]*|"(?:\\.|[^"\\\n])*"', lambda m: re.sub(r"[^\n]", " ", m.group(0)), t, flags=re.S)


# ---------------------------------------------------------------- scoring
_VA = {}


def score_of(name, ok, det):
    if ok: return 0
    if name not in cmpfn.LAST: return 999
    code, masked = cmpfn.LAST[name]
    try:
        va = _VA[name]
        cm = bytearray(code); ms = sorted(masked)
        for g in range(0, len(ms) - 3, 4):
            for k, v in enumerate((0, 0, 0x50, 0)): cm[ms[g + k]] = v
        gs = cmpfn.FNSZ.get(va)
        a = [x[2] for x in asmdiff.disasm(bytes(cm), 0x1000)]
        b = [x[2] for x in asmdiff.disasm(cmpfn.exe_bytes(va, max(len(code), gs or 0)), 0x1000)]
        n = sum(1 for l in difflib.unified_diff(a, b, lineterm="", n=0) if l[:1] in "+-" and not l.startswith(("+++", "---")))
        return max(1, n)
    except Exception:
        return 998


def compile_file(path, names):
    """-> {name: score} or None on compile error"""
    r = cmpfn.check(path, [(n, _VA[n]) for n in names], ["/O2"])
    if any(d.startswith("compile error") for _, d in r.values()): return None
    return {n: (999 if d.startswith(("symbol", "VA")) else score_of(n, ok, d)) for n, (ok, d) in r.items()}


# ---------------------------------------------------------------- function spans
def find_func(src, name):
    m = re.search(r"^(?!extern|#|/\*)[A-Za-z_][^\n;(]*\b%s\(" % re.escape(name), src, re.M)
    if not m: return None
    e = src.find("\n}", m.end())
    return (m.start(), e + 2) if e >= 0 else None


# ---------------------------------------------------------------- operand scanning
def left_span(M, i):
    """span of the operand ending before index i, scanning back at paren depth 0"""
    d = 0; j = i - 1
    while j >= 0:
        c = M[j]
        if c == ")": d += 1
        elif c == "(":
            if d == 0: break
            d -= 1
        elif d == 0 and (c in "?,;{}=:" or M[j - 1:j + 1] in ("&&", "||")): break
        j -= 1
    s = j + 1
    while s < i and M[s] in " \t\n": s += 1
    m = re.match(r"return\b\s*", M[s:i])
    if m: s += m.end()
    e = i
    while e > s and M[e - 1] in " \t\n": e -= 1
    return s, e


def right_span(M, i):
    d = 0; j = i
    while j < len(M):
        c = M[j]
        if c == "(": d += 1
        elif c == ")":
            if d == 0: break
            d -= 1
        elif d == 0 and (c in "?,;{}=:" or M[j:j + 2] in ("&&", "||")): break
        j += 1
    s = i
    while s < j and M[s] in " \t\n": s += 1
    e = j
    while e > s and M[e - 1] in " \t\n": e -= 1
    return s, e


FCONST = re.compile(r"^(\(\s*(?:float|double)\s*\)\s*)?(k_[fd]_\w+|-?\d+\.\d*f?|-?\d*\.\d+f?)$")
ICONST = re.compile(r"^-?(\d+|0x[0-9a-f]+)$", re.I)
RELOP = re.compile(r"(?<![<>\-=!])(<=|>=|<|>)(?![<>=])")
fconst = lambda t: bool(FCONST.match(t))
iconst = lambda t: bool(ICONST.match(t))


def rel_sites(body):
    """[(ls, le, rs, re, op)] for every relational operator whose operands have balanced parens"""
    M = mask(body); out = []
    for m in RELOP.finditer(M):
        ls, le = left_span(M, m.start()); rs, re_ = right_span(M, m.end())
        if ls >= le or rs >= re_: continue
        if M[ls:le].count("(") != M[ls:le].count(")") or M[rs:re_].count("(") != M[rs:re_].count(")"): continue
        out.append((ls, le, rs, re_, m.group(1)))
    return out


def cmp_sites(body, isconst, side):
    out = []
    for ls, le, rs, re_, op in rel_sites(body):
        L, R = body[ls:le], body[rs:re_]
        cl, cr = isconst(L), isconst(R)
        if (side == "L" and cl and not cr) or (side == "R" and cr and not cl):
            out.append((ls, re_, "%s %s %s" % (R, MIRROR[op], L)))
    return out


def apply_sites(body, sites):
    for s, e, t in sorted(sites, reverse=True): body = body[:s] + t + body[e:]
    return body


def site_variants(body, sites, cap=3):
    """all sites at once, then single sites (cap)"""
    if not sites: return []
    v = [apply_sites(body, sites)]
    if len(sites) > 1: v += [apply_sites(body, [s]) for s in sites[:cap]]
    return v


def p_cmpL(body, ctx): return site_variants(body, cmp_sites(body, fconst, "L"))
def p_cmpR(body, ctx): return site_variants(body, cmp_sites(body, fconst, "R"))
def p_icmpswap(body, ctx): return site_variants(body, cmp_sites(body, iconst, "L"))


def p_icmpoff(body, ctx):
    """x > N -> x >= N+1 etc (also constant-left forms). Only ever kept when byte identical."""
    sites = []
    for ls, le, rs, re_, op in rel_sites(body):
        L, R = body[ls:le], body[rs:re_]
        if iconst(R) and not iconst(L): x, n, o = L, int(R, 0), op
        elif iconst(L) and not iconst(R): x, n, o = R, int(L, 0), MIRROR[op]
        else: continue
        no, dn = OFF[o]
        sites.append((ls, re_, "%s %s %d" % (x, no, n + dn)))
    return site_variants(body, sites)


PRIM = r"(?:\(\s*(?:float|double|int)\s*\)\s*)?[A-Za-z_]\w*(?:\[[^\]\[]*\]|(?:\.|->)\w+)*|\([^()]*\)"
PRIM_L = re.compile(r"(%s)\s*$" % PRIM)
PRIM_R = re.compile(r"\s*(%s)(?!\s*[(\[.\w])" % PRIM)


def p_mulswap(body, ctx):
    """`x * k_f_c` <-> `k_f_c * x` (operand order of fmul / fld)"""
    M = mask(body); sites = []
    for m in re.finditer(r"(?<![*/])\*(?![*=])", M):
        i = m.start()
        a = PRIM_L.search(M[:i]); b = PRIM_R.match(M[i + 1:])
        if not a or not b: continue
        before = M[:a.start()].rstrip()
        if before.endswith(("*", "/")) or re.search(r"[\w)\]]$", before) and not before.endswith("return"): continue
        ga, gb = a.group(1), b.group(1)
        if gb.startswith("k_f_") and not ga.startswith("k_"):
            sites.append((a.start(1), i + 1 + b.end(1), "%s * %s" % (gb, body[a.start(1):a.end(1)])))
        elif ga.startswith("k_f_") and not gb.startswith("k_"):
            sites.append((a.start(1), i + 1 + b.end(1), "%s * %s" % (body[i + 1 + b.start(1):i + 1 + b.end(1)], ga)))
    return site_variants(body, sites)


def float_lit(name):
    t = cmpfn._symtab(); vas = t.get(name) or t.get("_" + name)
    if not vas: return None
    va = min(vas)
    try:
        if name.startswith("k_f_"):
            raw = cmpfn.exe_bytes(va, 4); v = struct.unpack("<f", raw)[0]
            if v != v or abs(v) == float("inf"): return None
            for n in range(1, 10):
                s = "%.*g" % (n, v)
                if struct.pack("<f", float(s)) == raw: break
            if "e" in s: return None
            if "." not in s: s += ".0"
            return ("(%sf)" if v < 0 else "%sf") % s
        raw = cmpfn.exe_bytes(va, 8); v = struct.unpack("<d", raw)[0]
        if v != v or abs(v) == float("inf"): return None
        s = repr(v)
        return "(%s)" % s if v < 0 else s
    except Exception:
        return None


def p_lit(body, ctx):
    """k_f_X identifiers -> the float literal (same rdata value; the relocation check compares literals by value)"""
    M = mask(body); sites = []
    for m in re.finditer(r"\bk_[fd]_\w+\b", M):
        l = float_lit(m.group(0))
        if l: sites.append((m.start(), m.end(), l))
    return site_variants(body, sites, cap=2)


def p_inline(body, ctx):
    """`v = f(..);` immediately followed by a statement using v once  ->  inline the call (arg evaluation order)"""
    lines = body.split("\n"); out = []
    for i in range(len(lines) - 1):
        m = re.match(r"^(\s*)(\w+) = (\w+\(.*\));\s*$", lines[i])
        if not m or "=" in m.group(3).replace("==", ""): continue
        v = m.group(2); nxt = lines[i + 1]
        if len(re.findall(r"\b%s\b" % v, nxt)) != 1 or re.match(r"\s*%s\b" % v, nxt): continue
        if len(re.findall(r"\b%s\b" % v, "\n".join(lines))) not in (2, 3): continue
        n2 = list(lines); n2[i + 1] = re.sub(r"\b%s\b" % v, lambda _: m.group(3), nxt); del n2[i]
        out.append("\n".join(n2))
    return out[:3]


def p_fwdret(body, ctx):
    """void wrapper(a) { g(a); }  ->  int wrapper(a) { return g(a); }"""
    m = re.match(r"^void( +__cdecl)? +(\w+)\(([^)]*)\)\s*\{\s*(\w+\([^;]*\));\s*(?:return;\s*)?\}$", mask(body).strip(), re.S)
    if not m: return []
    m = re.match(r"^void( +__cdecl)? +(\w+)\(([^)]*)\)\s*\{\s*(\w+\([^;]*\));\s*(?:return;\s*)?\}$", re.sub(r"/\*.*?\*/", "", body, flags=re.S).strip(), re.S)
    return ["int%s %s(%s)\n{\n  return %s;\n}" % (m.group(1) or "", m.group(2), m.group(3), m.group(4))] if m else []


def p_retparam(body, ctx):
    """void f(.., T *p) with a REGALLOC-sized diff: return the last pointer parameter (eax stays live)"""
    m = re.match(r"^void( +__cdecl)? +(\w+)\(([^)]*)\)", body)
    if not m or ctx["score"] > 8: return []
    pm = re.search(r"\*\s*(\w+)$", m.group(3).split(",")[-1].strip())
    if not pm: return []
    p = pm.group(1); b = "int" + body[4:]
    if re.search(r"\breturn;", b): b = re.sub(r"\breturn;", "return %s;" % p, b)
    else: b = b[:b.rstrip().rfind("}")] + "  return %s;\n}" % p
    return [b]


def g_exit(src):
    n = re.sub(r"extern int exit\(\);", "extern __declspec(noreturn) void __cdecl exit(int);", src)
    return n if n != src else None


# name, kind, transform, strict
PATTERNS = [("exit", "global", g_exit, False), ("cmpL", "func", p_cmpL, False), ("cmpR", "func", p_cmpR, False),
            ("mulswap", "func", p_mulswap, False), ("lit", "func", p_lit, False), ("icmpswap", "func", p_icmpswap, False),
            ("icmpoff", "func", p_icmpoff, True), ("inline", "func", p_inline, True), ("fwdret", "func", p_fwdret, True),
            ("retparam", "func", p_retparam, True)]


# ---------------------------------------------------------------- driver
LOCK = threading.Lock()


def process(fname, funcs, tree, only, apply, rows, log):
    tree = os.path.abspath(tree); path = os.path.join(tree, fname)
    src = open(path).read(); orig_src = src
    names = [n for n, _ in funcs]
    for n, va in funcs: _VA[n] = va
    scratch = os.path.join(os.environ["CMPFN_TMP"], "w_" + fname[:-2]); os.makedirs(scratch, exist_ok=True)
    for h in os.listdir(tree):
        if h.endswith(".h"):
            try: os.symlink(os.path.join(tree, h), os.path.join(scratch, h))
            except FileExistsError: pass
    tmp = os.path.join(scratch, "f.c")

    def comp(text):
        open(tmp, "w").write(text)
        return compile_file(tmp, names)

    base = comp(src)
    if base is None: log("%s: baseline compile error" % fname); return
    cur = dict(base); start = dict(base)
    chain = {n: [] for n in names}
    for pname, kind, fn, strict in PATTERNS:
        if only and pname not in only: continue
        todo = [n for n in names if cur[n] > 0]
        if not todo: break
        if kind == "global":
            new = fn(src)
            if new is None: continue
            sc = comp(new)
            if sc is None or any(sc[n] > cur[n] for n in names): continue
            better = [n for n in names if sc[n] < cur[n] and (not strict or sc[n] == 0)]
            if not better: continue
            for n in better: chain[n].append((pname, cur[n], sc[n]))
            src, cur = new, {n: sc[n] for n in names}
            continue
        alts = {}
        for n in todo:
            sp = find_func(src, n)
            if not sp: continue
            body = src[sp[0]:sp[1]]
            try: v = [x for x in fn(body, {"score": cur[n]}) if x != body]
            except Exception as ex: log("%s %s %s: %r" % (fname, n, pname, ex)); v = []
            if v: alts[n] = v
        for r in range(max([len(v) for v in alts.values()] or [0])):
            live = {n: v[r] for n, v in alts.items() if r < len(v) and cur[n] > 0}
            if not live: continue
            new = src
            for n, b in live.items():
                sp = find_func(new, n); new = new[:sp[0]] + b + new[sp[1]:]
            sc = comp(new)
            if sc is None: continue
            for n in [n for n in live if sc[n] < cur[n] and (not strict or sc[n] == 0)]:
                sp = find_func(src, n); src = src[:sp[0]] + live[n] + src[sp[1]:]
                chain[n].append((pname + ("" if r == 0 else "#%d" % r), cur[n], sc[n])); cur[n] = sc[n]; alts.pop(n)
    if src != orig_src:
        fin = comp(src)      # verify the merged file
        if fin is None: log("%s: merged file does not compile; dropped" % fname); return
        for n in names:
            if fin[n] > start[n]: log("%s: %s got worse in the merge (%d>%d)" % (fname, n, fin[n], start[n]))
        with LOCK:
            for n in names:
                for pn, b, a in chain[n]: rows.append((n, pn, b, a, fname))
            if apply: open(path, "w").write(src)
        log("%s: %d fn edited, identical %d -> %d" % (fname, sum(1 for n in names if chain[n]), sum(1 for n in names if start[n] == 0), sum(1 for n in names if fin[n] == 0)))
    else:
        log("%s: no change" % fname)


def main():
    ap = argparse.ArgumentParser(description=__doc__.split("\n")[0])
    ap.add_argument("--tree", default=os.path.join(REPO, "src/automatch_near"))
    ap.add_argument("--names", default=os.path.join(REPO, "src/automatch_near_names.txt"))
    ap.add_argument("--csv", default="RESULT.csv")
    ap.add_argument("--apply", action="store_true")
    ap.add_argument("--jobs", type=int, default=4)
    ap.add_argument("--only", default="")
    ap.add_argument("--files", default="")
    a = ap.parse_args()
    os.environ.setdefault("CMPFN_TMP", "/tmp/claude-%d/apply_patterns_tmp" % os.getuid())
    by = {}
    for l in open(a.names):
        p = l.split()
        if len(p) >= 3: by.setdefault("an_" + p[2], []).append((p[1], int(p[0], 16)))
    files = sorted(by)
    if a.files: files = [f for f in files if f in a.files.split(",")]
    only = set(a.only.split(",")) - {""}
    rows = []
    def log(s):
        with LOCK: print(s, flush=True)
    cmpfn._symtab()
    with ThreadPoolExecutor(a.jobs) as ex:
        futs = [ex.submit(process, f, by[f], a.tree, only, a.apply, rows, log) for f in files]
        for f in futs:
            try: f.result()
            except Exception as e: log("ERROR %r" % (e,))
    with open(a.csv, "w", newline="") as fh:
        w = csv.writer(fh); w.writerow(["name", "pattern", "before", "after", "file"]); w.writerows(sorted(rows, key=lambda r: (r[4], r[0])))
    ident = len({r[0] for r in rows if r[3] == 0}); imp = len({r[0] for r in rows} - {r[0] for r in rows if r[3] == 0})
    print("functions newly byte-identical: %d, improved only: %d  (%s)" % (ident, imp, "applied" if a.apply else "dry run"))


if __name__ == "__main__":
    main()
