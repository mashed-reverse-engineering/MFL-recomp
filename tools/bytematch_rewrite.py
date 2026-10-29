#!/usr/bin/env python3
"""bytematch_rewrite.py: mechanical source rewrites that turn behaviour-verified tier-2 functions (src/automatch_near/an_*.c) into byte-identical ones.
Usage: bytematch_rewrite.py [--apply] [--work DIR] [--only a,b,c] [--patterns p1,p2] [--flags "/O2"] [--report FILE]
  dry run (default): list the candidates (name, pattern, number of variants) and verify nothing is written;
  --apply: every candidate variant is compiled in a COPY of the chunk (DIR/src), compared with the original bytes (cmpfn.check: masked relocation fields + RELCHECK target rules,
           trailing padding ignored, function size must agree with fnsizes), and only a byte-identical variant is kept: the patched chunk copies are left in DIR/out
           (a line `name pattern` per success in DIR/rewritten.txt); src/ is never touched.  Install with tools/bytematch_install.py (tier-1 move) after review.
Patterns (one function each, `def pat_<name>(fn_text, ctx) -> [(variant text, ctx note)]`; each yields single-site variants, a match needs exactly the right site):
  ret_forward  void wrapper that ends with a call of an int-returning extern -> `return call;` (original keeps eax live: `add esp,N` instead of `pop ecx`)
  ptr_signed_cmp  relational operator with a pointer / address operand: both operands cast to (int) (original compares signed: jl instead of jb)
  rel_const    `x <= K` <-> `x < K+1`, `x >= K` <-> `x > K-1`, `x < K` <-> `x <= K-1`, `x > K` <-> `x >= K+1`   (cmp K / jle vs cmp K+1 / jl, test / jl)
  rel_swap     `a < b` -> `b > a` (mirrored operands: float compare operand order, fld/fcomp order)
  mul_swap     `a * b`, `a + b` -> `b * a`, `b + a` for simple operands (x87 fld/fmul operand order)
  inline_temp  single-use temp assigned on one line and used on the next: `v = E; g(v);` -> `g(E);`
"""
import collections, os, re, shutil, sys
HERE = os.path.dirname(os.path.abspath(__file__)); R = os.path.join(HERE, "..", "src")
sys.path.insert(0, HERE)
import cmpfn, bytediff_causes as bc

DEF = re.compile(r"^(?:[A-Za-z_][\w \*]*\n)?(?:[A-Za-z_][^\n;]*\b)?(\w+)\(([^;{]*)\)\s*\n\s*\{.*?\n\}\n", re.M | re.S)
REL = re.compile(r"(?<![<>=!\-])(<=|>=|<|>)(?![<>=])")
NUM = re.compile(r"^\(?\s*(-?(?:0x[0-9a-fA-F]+|\d+))[uUlL]*\s*\)?$")


def body_span(t):
    """text after the opening brace of the function"""
    return t.index("{")


def operand_spans(t, i, oplen):
    """(start, end) of the left and the right operand of the operator at t[i:i+oplen] inside a condition: stops at unbalanced parens, && || ? : , ; and single ="""
    d = 0; s = i
    while s > 0:
        c = t[s - 1]
        if c in ")]": d += 1
        elif c in "([":
            if d == 0: break
            d -= 1
        elif d == 0 and (c in ",;{}?:" or t[s - 2:s] in ("&&", "||") or (c == "=" and t[s - 2] not in "=!<>")): break
        s -= 1
    d = 0; e = i + oplen
    while e < len(t):
        c = t[e]
        if c in "([": d += 1
        elif c in ")]":
            if d == 0: break
            d -= 1
        elif d == 0 and (c in ",;{}?:" or t[e:e + 2] in ("&&", "||")): break
        e += 1
    return s, e


def rel_sites(t):
    b = body_span(t)
    for m in REL.finditer(t, b):
        i = m.start()
        if t[i - 1:i + 1] == "->" or t[i:i + 2] == "<<": continue
        s, e = operand_spans(t, i, len(m.group(1)))
        L, Rr = t[s:i].strip(), t[i + len(m.group(1)):e].strip()
        if L and Rr: yield s, i, e, m.group(1), L, Rr


def pat_ptr_signed_cmp(t, ctx):
    out = []
    for s, i, e, op, L, Rr in rel_sites(t):
        if not (re.search(r"\*\s*\)", L + Rr) or "&" in L + Rr): continue
        if "(int)" in L and "(int)" in Rr: continue
        wrap = lambda x: x if x.startswith("(int)") else "(int)%s" % (x if re.fullmatch(r"\w+", x) or x.startswith("(") and x.endswith(")") and x.count("(") == 1 else "(%s)" % x)
        out.append(t[:s] + "%s %s %s" % (wrap(L), op, wrap(Rr)) + t[e:])
    return out


def pat_rel_const(t, ctx):
    out = []
    for s, i, e, op, L, Rr in rel_sites(t):
        m = NUM.match(Rr)
        if not m: continue
        k = int(m.group(1), 0); hexf = m.group(1).lower().lstrip("-").startswith("0x")
        new = {"<=": ("<", k + 1), "<": ("<=", k - 1), ">=": (">", k - 1), ">": (">=", k + 1)}[op]
        out.append(t[:s] + "%s %s %s" % (L, new[0], hex(new[1]) if hexf and new[1] >= 0 else new[1]) + t[e:])
    return out


def pat_rel_swap(t, ctx):
    out = []
    flip = {"<": ">", ">": "<", "<=": ">=", ">=": "<="}
    for s, i, e, op, L, Rr in rel_sites(t):
        out.append(t[:s] + "%s %s %s" % (Rr, flip[op], L) + t[e:])
    return out


ATOM = r"(?:\([^()]*\)\s*)?[\w.]+(?:\[[^\]]*\]|->\w+|\.\w+)*|\([^()]*\)"
BIN = re.compile(r"(?<![\w.\)\]])(%s)\s*([*+])\s*(%s)(?![\w.\(\[])" % (ATOM, ATOM))


def pat_mul_swap(t, ctx):
    out = []; b = body_span(t)
    for m in BIN.finditer(t, b):
        if t[max(0, m.start() - 1)] in "*+-/" and False: continue
        out.append(t[:m.start()] + "%s %s %s" % (m.group(3), m.group(2), m.group(1)) + t[m.end():])
    return out


def pat_ret_forward(t, ctx):
    m = re.match(r"^(void)\s+(\w+\([^)]*\))\s*\{?", t)
    b = re.search(r"\n\s*(?:(\w+)\((.*)\);)\s*\n\s*return;\s*\n\}\n$", t, re.S)
    if not m or not b: return []
    callee = b.group(1)
    ty = ctx["ret_of"].get(callee)
    if not ty or ty in ("void", "undefined"): return []
    head = re.sub(r"^void", ty, t[:b.start()], count=1)
    return [head + "\n  return %s(%s);\n}\n" % (callee, b.group(2))]


def pat_inline_temp(t, ctx):
    """single-use temporary assigned on one line and used on the next statement line: `v = E; g(v, ..);` -> `g(E, ..);` (nested call evaluated after the other arguments,
    one callee-saved register less: documented patterns A and 'extra push')"""
    L = t.split("\n"); out = []
    for i in range(len(L) - 1):
        m = re.match(r"^(\s{2})(\w+) = (.*);$", L[i])
        if not m: continue
        v, e = m.group(2), m.group(3)
        j = i + 1
        while j < len(L) and L[j].strip() == "": j += 1
        if j >= len(L) or len(re.findall(r"\b%s\b" % v, t)) != 3: continue
        dec = next((k for k, x in enumerate(L) if re.match(r"^\s{2}[A-Za-z_][\w \*]*\b%s;$" % v, x)), None)
        if dec is None or not re.search(r"\b%s\b" % v, L[j]) or L[j].count(";") != 1 or L[j].lstrip().startswith(("for", "while", "do", "if (", "else")) and "{" not in L[j] and False: continue
        if re.match(r"^\s*\w+ = .*\b%s\b" % v, L[j]) and re.match(r"^\s*%s =" % v, L[j]): continue
        rep = e if re.fullmatch(r"[\w.\[\]>-]+(\(.*\))?", e) else "(%s)" % e
        N = list(L); N[j] = re.sub(r"\b%s\b" % v, lambda _: rep, L[j], count=1)
        for k in sorted((i, dec), reverse=True): del N[k]
        out.append("\n".join(N))
    return out


MAXVAR = 200      # bound of variants per function and pattern for the search patterns below


def _body_lines(t):
    """(head, lines, tail) of a function text: head up to and including the line with the opening brace"""
    L = t.split("\n"); k = next(i for i, x in enumerate(L) if x.strip() == "{" or x.rstrip().endswith("{"))
    return L[:k + 1], L[k + 1:]


DECL = re.compile(r"^\s{2}(?!return|goto|if|for|while|do|switch|else)[A-Za-z_][\w ]*?[\s\*]+\**\w+(\[\d+\])?(\s*=[^;,]*)?;\s*$")


def pat_decl_perm(t, ctx):
    """permute the order of the leading local declarations (stack slot / callee-saved register assignment follows declaration order in cl 13):
    all permutations for <= 5 declarations, otherwise swaps of every pair + reversal + rotations"""
    import itertools
    head, rest = _body_lines(t)
    n = 0
    while n < len(rest) and (DECL.match(rest[n]) and ("," not in rest[n].split("=")[0])): n += 1
    if n < 2: return []
    D = rest[n - n:n]; names = [re.search(r"(\w+)(\[\d+\])?(\s*=|;)", d).group(1) for d in D]
    for i, d in enumerate(D):      # an initialiser that uses another declared name pins the order
        if "=" in d and any(re.search(r"\b%s\b" % x, d.split("=", 1)[1]) for x in names): return []
    perms = []
    if n <= 5: perms = [list(p) for p in itertools.permutations(range(n))][1:]
    else:
        base = list(range(n))
        perms.append(base[::-1])
        for i in range(n):
            perms.append(base[i:] + base[:i])
            for j in range(i + 1, n):
                q = base[:]; q[i], q[j] = q[j], q[i]; perms.append(q)
    out = []
    for p in perms[:MAXVAR]:
        out.append("\n".join(head + [D[i] for i in p] + rest[n:]))
    return out


SIMPLE = re.compile(r"^\s{2}([\w\.\[\]>\*\(\)-]+?)\s*=\s*([^;{}=]*);\s*$")


def pat_stmt_swap(t, ctx):
    """swap two adjacent independent single-line assignments (no call on either side, neither mentions the other's target)"""
    head, rest = _body_lines(t); out = []
    for i in range(len(rest) - 1):
        a, b = SIMPLE.match(rest[i]), SIMPLE.match(rest[i + 1])
        if not a or not b: continue
        if "(" in a.group(2) + b.group(2) and re.search(r"\w\(", a.group(2) + b.group(2)): continue
        if "*" in a.group(1) + b.group(1) or "[" in a.group(1) + b.group(1) or ">" in a.group(1) + b.group(1): continue      # store order through pointers (aliasing)
        ta, tb = re.escape(a.group(1)), re.escape(b.group(1))
        if re.search(r"\b%s\b" % tb, a.group(2)) or re.search(r"\b%s\b" % ta, b.group(2)) or a.group(1) == b.group(1): continue
        R_ = rest[:]; R_[i], R_[i + 1] = R_[i + 1], R_[i]
        out.append("\n".join(head + R_))
    return out[:MAXVAR]


def _block_end(t, k):
    """index just after the brace block whose '{' is at t[k]"""
    d = 0
    for i in range(k, len(t)):
        if t[i] == "{": d += 1
        elif t[i] == "}":
            d -= 1
            if d == 0: return i + 1
    return -1


def _paren_end(t, k):
    d = 0
    for i in range(k, len(t)):
        if t[i] == "(": d += 1
        elif t[i] == ")":
            d -= 1
            if d == 0: return i + 1
    return -1


def pat_ifelse_swap(t, ctx):
    """`if (C) {A} else {B}` -> `if (!(C)) {B} else {A}` (branch polarity / block layout); a leading `!` or == / != / relational operator is flipped instead of wrapped"""
    out = []; b = body_span(t)
    for m in re.finditer(r"\bif\s*\(", t[b:]):
        i = b + m.start(); po = b + m.end() - 1; pe = _paren_end(t, po)
        if pe < 0: continue
        cond = t[po + 1:pe - 1].strip()
        j = pe
        while t[j] in " \n\t": j += 1
        if t[j] != "{": continue
        e1 = _block_end(t, j)
        k = e1
        while k < len(t) and t[k] in " \n\t": k += 1
        if not t.startswith("else", k): continue
        k += 4
        while t[k] in " \n\t": k += 1
        if t[k] != "{": continue
        e2 = _block_end(t, k)
        if e1 < 0 or e2 < 0: continue
        A, B = t[j:e1], t[k:e2]
        if "&&" in cond or "||" in cond: nc = "!(%s)" % cond
        elif cond.startswith("!") and re.fullmatch(r"!\w+|!\([^()]*\)", cond): nc = cond[1:].strip("() ") if re.fullmatch(r"!\([^()]*\)", cond) else cond[1:]
        else:
            fm = re.fullmatch(r"(.+?)\s*(==|!=|<=|>=|<|>)\s*(.+)", cond)
            flip = {"==": "!=", "!=": "==", "<": ">=", ">": "<=", "<=": ">", ">=": "<"}
            nc = "%s %s %s" % (fm.group(1), flip[fm.group(2)], fm.group(3)) if fm and not re.search(r"[<>=!]", fm.group(1) + fm.group(3)) else "!(%s)" % cond if not re.fullmatch(r"\w+", cond) else "!" + cond
        out.append(t[:i] + "if (%s) %s else %s" % (nc, B, A) + t[e2:])
    return out[:MAXVAR]


def pat_loop_form(t, ctx):
    """for (I; C; S) {B} -> I; while (C) {B S;}  and  while (C) {B} -> for (; C; ) {B}"""
    out = []; b = body_span(t)
    for m in re.finditer(r"\bfor\s*\(", t[b:]):
        i = b + m.start(); po = b + m.end() - 1; pe = _paren_end(t, po)
        hdr = t[po + 1:pe - 1].split(";")
        j = pe
        while t[j] in " \n\t": j += 1
        if len(hdr) != 3 or t[j] != "{": continue
        e = _block_end(t, j)
        I, C, S = [x.strip() for x in hdr]
        body = t[j + 1:e - 1].rstrip()
        out.append(t[:i] + ("%s;\n  " % I if I else "") + "while (%s) {%s\n    %s;\n  }" % (C or "1", body, S) + t[e:])
    for m in re.finditer(r"\bwhile\s*\(", t[b:]):
        i = b + m.start(); po = b + m.end() - 1; pe = _paren_end(t, po)
        if t[max(0, i - 3):i].strip().endswith("}"): continue
        j = pe
        while t[j] in " \n\t": j += 1
        if t[j] != "{": continue
        out.append(t[:i] + "for (; %s; )" % t[po + 1:pe - 1] + t[pe:])
    return out[:MAXVAR]


def pat_case_swap(t, ctx):
    """swap two adjacent `case K: ... break;` / `return` blocks of a switch (jump-table / compare-chain order)"""
    out = []
    parts = list(re.finditer(r"(?m)^(\s*)(case [^:\n]+:|default:)(.*?)(?=^\s*(?:case [^:\n]+:|default:)|^\s*\}\s*$)", t, re.S))
    for a, b in zip(parts, parts[1:]):
        if a.end() != b.start(): continue
        if not re.search(r"(break|return[^;]*|goto \w+);\s*$", a.group(0).rstrip()): continue
        out.append(t[:a.start()] + b.group(0) + a.group(0) + t[b.end():])
    return out[:MAXVAR]


PATTERNS = {"ret_forward": pat_ret_forward, "ptr_signed_cmp": pat_ptr_signed_cmp, "rel_const": pat_rel_const, "rel_swap": pat_rel_swap, "mul_swap": pat_mul_swap, "inline_temp": pat_inline_temp, "decl_perm": pat_decl_perm, "stmt_swap": pat_stmt_swap, "ifelse_swap": pat_ifelse_swap, "loop_form": pat_loop_form, "case_swap": pat_case_swap}


def extern_returns(text):
    out = {}
    for m in re.finditer(r"^(?:/\*[0-9a-f]+\*/\s*)?extern\s+([\w\s\*]+?)\s*(?:__cdecl\s+|__stdcall\s+|__fastcall\s+)?\*?(\w+)\s*\(", text, re.M):
        out[m.group(2)] = m.group(1).strip()
    return out


def main():
    args = sys.argv[1:]
    def opt(k, d):
        if k in args: i = args.index(k); v = args[i + 1]; del args[i:i + 2]; return v
        return d
    apply_ = "--apply" in args
    if apply_: args.remove("--apply")
    work = opt("--work", os.path.join(os.environ.get("TMPDIR", "/tmp"), "bytematch_rewrite"))
    only = opt("--only", None); only = set(only.split(",")) if only else None
    pats = opt("--patterns", ",".join(PATTERNS)).split(","); flags = opt("--flags", "/O2").split()
    pkl = opt("--pkl", None); maxdiff = int(opt("--maxdiff", "24")); csvp = opt("--causes", os.path.join(HERE, "..", "output", "bytediff_causes_2026-10-29.csv"))
    src = os.path.join(work, "src"); outd = os.path.join(work, "out")
    for d in (src, outd): os.makedirs(d, exist_ok=True)
    for h in os.listdir(R):
        if h.endswith(".h"): shutil.copy(os.path.join(R, h), src)
    nd = os.path.join(R, "automatch_near")
    for f in os.listdir(nd):
        if f.endswith((".c", ".h")) and not os.path.islink(os.path.join(nd, f)): shutil.copy(os.path.join(nd, f), src)
    nm = bc.names()
    if pkl:
        import pickle; base = pickle.load(open(pkl, "rb"))
    else:
        base = bc.scan(src, flags, only={n for n in nm if only is None or n in only})
    small = None
    if os.path.exists(csvp):
        import csv; small = {r["name"] for r in csv.DictReader(open(csvp)) if r["bytes_diff_count"].isdigit() and 0 < int(r["bytes_diff_count"]) <= maxdiff}      # only near misses: a variant cannot fix a big structural difference
    todo = [n for n, (ok, det, last) in base.items() if n in nm and not ok and not det.startswith("compile") and (only is None or n in only) and (small is None or n in small)]
    texts = {}      # chunk -> current text
    cands = []
    for n in sorted(todo):
        ch = nm[n][1]
        t = texts.setdefault(ch, open(os.path.join(src, ch)).read())
        m = next((m for m in DEF.finditer(t) if m.group(1) == n), None)
        if not m: continue
        ctx = {"ret_of": extern_returns(t)}
        for p in pats:
            vs = [v for v in PATTERNS[p](m.group(0), ctx) if v != m.group(0)]
            if vs: cands.append((n, p, m, vs))
    cnt = collections.Counter(p for _, p, _, _ in cands)
    print("candidates: %d functions-patterns, variants %d; per pattern %s" % (len(cands), sum(len(v) for *_, v in cands), dict(cnt)))
    if not apply_:
        for n, p, m, vs in cands[:80]: print("  %-40s %-16s %d variant(s)" % (n, p, len(vs)))
        return
    done = {}; rep = open(os.path.join(work, "rewritten.txt"), "a")
    from concurrent.futures import ThreadPoolExecutor
    jobs = int(opt("--jobs", "2"))
    tmps = []
    def try_var(a):
        n, ch, t0, m, v, k = a
        tv = t0.replace(m.group(0), v, 1)
        fp = os.path.join(src, "tmp%d_%s" % (k, ch)); tmps.append(fp); open(fp, "w").write(tv)
        return cmpfn.check(fp, [(n, nm[n][0])], flags)[n][0], tv
    for n, p, m, vs in cands:
        if n in done: continue
        ch = nm[n][1]; t0 = texts[ch]
        with ThreadPoolExecutor(jobs) as ex:
            for ok, tv in ex.map(try_var, [(n, ch, t0, m, v, k) for k, v in enumerate(vs)]):
                if ok:
                    texts[ch] = tv; done[n] = p
                    open(os.path.join(outd, ch), "w").write(tv)
                    rep.write("%s %s\n" % (n, p)); rep.flush()
                    print("MATCH %s %s" % (n, p), flush=True); break
    print("rewritten byte-identical: %d (%s)" % (len(done), dict(collections.Counter(done.values()))))


if __name__ == "__main__":
    main()
