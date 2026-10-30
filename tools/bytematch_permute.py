#!/usr/bin/env python3
"""bytematch_permute.py: permuter-style multi-site search that turns behaviour-verified tier-2 functions (src/automatch_near/an_*.c) into byte-identical ones.
Usage: bytematch_permute.py [--work DIR] [--names a,b|@file] [--maxdiff N] [--maxinsn N] [--gs] [--budget N] [--batch B] [--jobs J] [--seed S] [--flags "/O2"] [--list]
  Candidate selection: --names, or from the census csv (output/bytediff_causes_2026-10-29.csv, rerun tools/bytediff_causes.py first) every name with 0 < bytes_diff_count <= --maxdiff
  (--maxinsn: instruction-aligned diff <= N, computed from the census scan pickle DIR/../scan.pkl or by compiling), --gs: names whose original loads the stack cookie.
Search per function: hill climb over the source text.  A batch = B mutants (1-2 mutations each, drawn from the mutators below) of the current text, compiled in ONE translation unit as
  copies name__v0..name__vB-1 (cl keeps per-function state apart), each compared with the original bytes (cmpfn.check: relocation fields masked, RELCHECK target rules).
  Score = (instruction-aligned diff count of the normalised disassembly, differing bytes).  The best mutant of a batch replaces the current text when its score is <= the current (plateau moves
  allowed: arm order alone often changes nothing until the compare order is fixed); after 25 batches without a new best the climb restarts from the best.  --budget = distinct variants compiled per
  function (default 400, the original brief asked for up to 3000).  Everything seen is cached in DIR/cache/<name>.json (score per sha1 of the function text, best text): re-running resumes.
  Hits (byte-identical, size ok) are written to DIR/hits/<name>.c (the whole chunk with the function replaced) and listed in DIR/hits.txt; src/ is never touched.
  Install with tools/bytematch_install.py --work DIR/hitsw (hitsw/out/<chunk>) after reviewing the readability of the diff (printed).
--frame: draw only the frame-layout mutators (T-M4: decl_split/merge/rev/sink/hoist, init_split, array_group, arr_resize, float_double, spill; array_group/arr_resize hits are UGLY: review) plus decl_swap/stmt_swap.
  --names takes @file lists as well; --gs lists the stack-cookie functions.
Mutators (each `text -> [variant]`, comments are never edited): rel_swap (a<b -> b>a, mirrored operator: NaN-safe), ifelse_not (swap if/else arms with the condition written !( ) ; == / != / ! are flipped
  directly, relational operators never), mul_swap (a*b, a+b operand order, only where no precedence/float-association changes), decl_swap (two leading local declarations), stmt_swap, inline_temp,
  loop_form (for<->while, not with `continue`), case_swap (adjacent case blocks that both end in break/return/goto and follow a terminated block), cast_toggle (drop a (float)/(double)/(int) cast,
  or wrap the right-hand side of an assignment to a float/double local in (float)(double)/(double)), lit_width (1.0f <-> 1.0), regroup (a + b + c <-> a + (b + c)), type_swap (`*(undefined4 *)` <-> float/int/uint pointer cast).
Not mutated: types, semantics of pointer compares ((int)ptr casts grow portcheck), anything needing liveness (local reuse, temp introduction with a new type)."""
import difflib, hashlib, json, os, random, re, shutil, sys, threading, time
from concurrent.futures import ThreadPoolExecutor
HERE = os.path.dirname(os.path.abspath(__file__)); R = os.path.join(HERE, "..", "src")
sys.path.insert(0, HERE)
import cmpfn, asmdiff, bytediff_causes as bc, bytematch_rewrite as rw

COMMENT = re.compile(r"/\*.*?\*/|//[^\n]*", re.S)


def in_comment(t, pos):
    return any(m.start() <= pos < m.end() for m in COMMENT.finditer(t))


def first_diff(a, b):
    n = min(len(a), len(b)); i = 0
    while i < n and a[i] == b[i]: i += 1
    return i


def code_only(f):
    """wrap a text->variants function: variants whose first edit lies inside a comment are dropped"""
    def g(t):
        spans = [(m.start(), m.end()) for m in COMMENT.finditer(t)]
        out = []
        for v in f(t):
            if v == t: continue
            p = first_diff(t, v)
            if any(s <= p < e or (p == e) and False for s, e in spans): continue
            out.append(v)
        return out
    return g


def has_call(x): return bool(re.search(r"[A-Za-z_]\w*\s*\(", re.sub(r"\((?:float|double|int|short|char|unsigned|undefined\d?|void|struct \w+)[\s\*]*\)", "", x)))


def m_rel_swap(t):
    out = []
    flip = {"<": ">", ">": "<", "<=": ">=", ">=": "<="}
    for s, i, e, op, L, Rr in rw.rel_sites(t):
        if re.search(r"\|\||&&|\||\^|\?|(?<=[\w\)\]])\s*&(?!&)", L + " " + Rr): continue      # a lower-precedence operator inside the span
        if has_call(L) and has_call(Rr): continue
        out.append(t[:s] + "%s %s %s" % (Rr, flip[op], L) + t[e:])
    return out


def m_mul_swap(t):
    out = []; b = rw.body_span(t)
    for m in rw.BIN.finditer(t, b):
        op = m.group(2)
        pre = t[:m.start()].rstrip(); post = t[m.end():].lstrip()
        if pre and not (pre[-1] in "(=,;{}[?:" or pre.endswith("return")): continue      # a*b+c / a-b+c / a+b+c regrouping would change
        if post[:1] in ("*", "/", "%") and op == "+": continue
        if has_call(m.group(1)) and has_call(m.group(3)): continue
        out.append(t[:m.start()] + "%s %s %s" % (m.group(3), op, m.group(1)) + t[m.end():])
    return out


def m_ifelse_not(t):
    out = []; b = rw.body_span(t)
    for m in re.finditer(r"\bif\s*\(", t[b:]):
        i = b + m.start(); po = b + m.end() - 1; pe = rw._paren_end(t, po)
        if pe < 0: continue
        cond = t[po + 1:pe - 1].strip()
        j = pe
        while t[j] in " \n\t": j += 1
        if t[j] != "{": continue
        e1 = rw._block_end(t, j); k = e1
        while k < len(t) and t[k] in " \n\t": k += 1
        if not t.startswith("else", k): continue
        k += 4
        while t[k] in " \n\t": k += 1
        if t[k] != "{": continue
        e2 = rw._block_end(t, k)
        if e1 < 0 or e2 < 0: continue
        A, B = t[j:e1], t[k:e2]
        if re.fullmatch(r"!\s*\((.*)\)", cond, re.S) and rw._paren_end(cond, cond.index("(")) == len(cond): nc = cond[cond.index("(") + 1:-1].strip()
        elif re.fullmatch(r"!\w+", cond): nc = cond[1:]
        elif re.fullmatch(r"\w+", cond): nc = "!" + cond
        else:
            fm = re.fullmatch(r"(.+?)\s*(==|!=)\s*(.+)", cond)
            if fm and not re.search(r"[<>=!&|]", fm.group(1) + fm.group(3)): nc = "%s %s %s" % (fm.group(1), "!=" if fm.group(2) == "==" else "==", fm.group(3))
            else: nc = "!(%s)" % cond
        out.append(t[:i] + "if (%s) %s else %s" % (nc, B, A) + t[e2:])
    return out


def m_decl_swap(t):
    head, rest = rw._body_lines(t)
    n = 0
    while n < len(rest) and rw.DECL.match(rest[n]) and "," not in rest[n].split("=")[0]: n += 1
    if n < 2: return []
    names = [re.search(r"(\w+)(\[\d+\])?(\s*=|;)", d).group(1) for d in rest[:n]]
    for d in rest[:n]:
        if "=" in d and any(re.search(r"\b%s\b" % x, d.split("=", 1)[1]) for x in names): return []
    out = []
    for i in range(n):
        for j in range(i + 1, n):
            D = rest[:]; D[i], D[j] = D[j], D[i]
            out.append("\n".join(head + D))
    return out


def m_stmt_swap(t): return rw.pat_stmt_swap(t, {})
def m_inline_temp(t): return rw.pat_inline_temp(t, {})


def m_loop_form(t):
    if re.search(r"\bcontinue\b", t): return []
    return rw.pat_loop_form(t, {})


def m_case_swap(t):
    out = []
    parts = list(re.finditer(r"(?m)^(\s*)(case [^:\n]+:|default:)(.*?)(?=^\s*(?:case [^:\n]+:|default:)|^\s*\}\s*$)", t, re.S))
    term = re.compile(r"(break|return[^;]*|goto \w+);\s*$")
    for idx, (a, b) in enumerate(zip(parts, parts[1:])):
        if a.end() != b.start(): continue
        if not term.search(a.group(0).rstrip()) or not term.search(b.group(0).rstrip()): continue
        pre = t[:a.start()].rstrip()
        if not (pre.endswith("{") or term.search(pre)): continue
        out.append(t[:a.start()] + b.group(0) + a.group(0) + t[b.end():])
    return out


CASTS = re.compile(r"\((?:float|double|int)\)\s*")


def m_cast_toggle(t):
    out = []; b = rw.body_span(t)
    for m in CASTS.finditer(t, b):
        out.append(t[:m.start()] + t[m.end():])
    fl = set(re.findall(r"^\s{2}(?:float|double)\s+(\w+)\s*[;,=]", t[b:], re.M)) | set(re.findall(r"^\s{2}(?:float|double)\s+\w+\s*,\s*(\w+)", t[b:], re.M))
    for m in re.finditer(r"(?m)^(\s{2,}(\w+) = )([^;{}\n]+);", t[b:]):
        if m.group(2) not in fl or CASTS.match(m.group(3)): continue
        s = b + m.start(3); e = b + m.end(3)
        for c in ("(float)(double)", "(double)", "(float)"):
            out.append(t[:s] + "%s(%s)" % (c, t[s:e]) + t[e:])
    return out


def m_lit_width(t):
    out = []; b = rw.body_span(t)
    for m in re.finditer(r"(?<![\w.])(\d+\.\d*|\.\d+)([fF]?)(?![\w.])", t[b:]):
        s = b + m.start(); e = b + m.end()
        out.append(t[:s] + m.group(1) + ("" if m.group(2) else "f") + t[e:])
    return out


def m_regroup(t):
    out = []; b = rw.body_span(t)
    A = r"[\w.\]\[>\-]+?|\([^()]*\)"
    for m in re.finditer(r"(?<![\w.\)\]])(%s)\s*\+\s*(%s)\s*\+\s*(%s)(?![\w.\(\[*/%%])" % (A, A, A), t[b:]):
        s = b + m.start(); e = b + m.end()
        out.append(t[:s] + "%s + (%s + %s)" % m.groups() + t[e:])
    for m in re.finditer(r"(?<![\w.\)\]])(%s)\s*\+\s*\((%s)\s*\+\s*(%s)\)(?![\w.\(\[*/%%])" % (A, A, A), t[b:]):
        s = b + m.start(); e = b + m.end()
        out.append(t[:s] + "%s + %s + %s" % m.groups() + t[e:])
    return out


PTYPES = ["undefined4", "float", "int", "uint"]


def m_type_swap(t):
    """retype one dereferencing pointer cast `*(T *)` (T in undefined4/float/int/uint): a word copy as float load/store (fld/fstp), signed/unsigned compares"""
    out = []; b = rw.body_span(t)
    for m in re.finditer(r"\*\s*\((%s) \*\)" % "|".join(PTYPES), t[b:]):
        for T in PTYPES:
            if T != m.group(1):
                s_ = b + m.start(1); e_ = b + m.end(1); out.append(t[:s_] + T + t[e_:])
    return out


# ---------------------------------------------------------------- frame-layout mutators (T-M4): stack slot allocation follows declaration order, scope and local types in cl 13
_TL = threading.local()      # .pre = preamble of the chunk being searched (set by prep): the prototypes give the return type of a callee for m_spill
LDECL = re.compile(r"^(\s{2})((?:unsigned |struct |const )?[A-Za-z_]\w*[\s\*]+)(\**)(\w+)((?:\[\w+\])?)(\s*=\s*[^;]+)?;\s*$")


def _lead(t):
    """(head lines, leading declaration lines, rest lines): declarations are the 2-space indented declarator lines right after the function's opening brace"""
    head, rest = rw._body_lines(t)
    n = 0
    while n < len(rest) and (LDECL.match(rest[n]) or re.match(r"^\s{2}[A-Za-z_][\w \*]*[\s\*]\**\w+(,\s*\**\w+)+;\s*$", rest[n])) and not re.match(r"^\s*(return|goto|else)\b", rest[n]): n += 1
    return head, rest[:n], rest[n:]


def _name(d):
    m = LDECL.match(d)
    return m.group(4) if m else None


def m_decl_split(t):
    head, D, R_ = _lead(t); out = []
    for i, d in enumerate(D):
        m = re.match(r"^(\s{2}[A-Za-z_][\w ]*?[\s\*]+)((?:\**\w+(?:\[\w+\])?)(?:,\s*\**\w+(?:\[\w+\])?)+);\s*$", d)
        if not m: continue
        parts = [x.strip() for x in m.group(2).split(",")]
        base = m.group(1).rstrip("*").rstrip() + " "
        mk = lambda ps: [base + p + ";" for p in ps]
        out.append("\n".join(head + D[:i] + mk(parts) + D[i + 1:] + R_))
        out.append("\n".join(head + D[:i] + mk(parts[::-1]) + D[i + 1:] + R_))
    return out


def m_decl_merge(t):
    head, D, R_ = _lead(t); out = []
    for i in range(len(D) - 1):
        a, b = LDECL.match(D[i]), LDECL.match(D[i + 1])
        if a and b and '*' not in a.group(2) and a.group(2).strip() == b.group(2).strip() and not a.group(6) and not b.group(6) and not a.group(5) and not b.group(5) and a.group(3) == b.group(3):
            out.append("\n".join(head + D[:i] + ["  %s%s%s, %s%s;" % (a.group(2), a.group(3), a.group(4), b.group(3), b.group(4))] + D[i + 2:] + R_))
    return out


def m_decl_rev(t):
    head, D, R_ = _lead(t); out = []
    if len(D) < 2: return out
    for p in (D[::-1], D[1:] + D[:1], D[-1:] + D[:-1]):
        if p != D and not any(LDECL.match(x) and LDECL.match(x).group(6) for x in D): out.append("\n".join(head + p + R_))
    return out


def _uses(text, x): return [m.start() for m in re.finditer(r"(?<![\w.])%s\b" % re.escape(x), text)]


def m_decl_sink(t):
    """move a leading `T x;` into the innermost nested block that holds every use of it (scope change = slot reuse / allocation order)"""
    head, D, R_ = _lead(t); out = []
    body = "\n".join(R_)
    for i, d in enumerate(D):
        m = LDECL.match(d)
        if not m or m.group(6) and False: continue
        x = m.group(4); u = _uses(body, x)
        if not u or any("=" in o and re.search(r"\b%s\b" % x, o.split("=", 1)[1]) for o in D if o is not d): continue
        best = None
        for bm in re.finditer(r"\{", body):
            e = rw._block_end(body, bm.start())
            if e > 0 and all(bm.start() < p < e for p in u) and (best is None or e - bm.start() < best[1] - best[0]): best = (bm.start(), e)
        if not best: continue
        j = best[0] + 1
        ind = re.match(r"\s*", body[body.rfind("\n", 0, best[0]) + 1:]).group(0)
        nb = body[:j] + "\n" + ind + "  " + d.strip() + body[j:]
        out.append("\n".join(head + D[:i] + D[i + 1:]) + "\n" + nb)
    return out


def m_decl_hoist(t):
    """move the first declaration of a nested block to the top of the function"""
    head, D, R_ = _lead(t); out = []
    names = {_name(d) for d in D}
    for i, l in enumerate(R_):
        m = re.match(r"^(\s{4,})((?:unsigned |struct )?[A-Za-z_]\w*[\s\*]+\**)(\w+)((?:\[\w+\])?);\s*$", l)
        if not m or m.group(3) in names or re.match(r"^\s*(return|goto|else)\b", l): continue
        out.append("\n".join(head + D + ["  " + l.strip()] + R_[:i] + R_[i + 1:]))
    return out


def m_init_split(t):
    """`T x = e;` (leading) <-> `T x;` + `x = e;` as the first statement; and `T x;` + first-statement `x = e;` -> merged"""
    head, D, R_ = _lead(t); out = []
    for i, d in enumerate(D):
        m = LDECL.match(d)
        if m and m.group(6) and not m.group(5):
            D2 = D[:i] + ["  %s%s%s;" % (m.group(2), m.group(3), m.group(4))] + D[i + 1:]
            out.append("\n".join(head + D2 + ["  %s%s;" % (m.group(4), m.group(6))] + R_))
    if R_:
        fm = re.match(r"^\s{2}(\w+)(\s*=\s*[^;]+);\s*$", R_[0])
        if fm:
            for i, d in enumerate(D):
                m = LDECL.match(d)
                if m and m.group(4) == fm.group(1) and not m.group(6) and not m.group(5):
                    out.append("\n".join(head + D[:i] + ["  %s%s%s%s;" % (m.group(2), m.group(3), m.group(4), fm.group(2))] + D[i + 1:] + R_[1:]))
    return out


def m_array_group(t):
    """UGLY (review before installing): consecutive same-type scalar locals -> one array `T name_name[N]`"""
    head, D, R_ = _lead(t); out = []
    for i in range(len(D) - 1):
        a, b = LDECL.match(D[i]), LDECL.match(D[i + 1])
        if not (a and b) or a.group(6) or b.group(6) or a.group(5) or b.group(5) or a.group(2).strip() != b.group(2).strip() or a.group(3) or b.group(3): continue
        if not re.match(r"(float|int|uint|undefined4|double)$", a.group(2).strip()): continue
        body = "\n".join(R_); nm = "%s_%s" % (a.group(4), b.group(4))
        body = re.sub(r"(?<![\w.])%s\b" % a.group(4), nm + "[0]", body); body = re.sub(r"(?<![\w.])%s\b" % b.group(4), nm + "[1]", body)
        out.append("\n".join(head + D[:i] + ["  %s%s[2];" % (a.group(2), nm)] + D[i + 2:]) + "\n" + body)
    return out


def m_struct_group(t):
    """readable cousin of array_group (T-B4): consecutive same-type scalar locals -> one anonymous struct `loop` (members keep their names, a.b order = declaration order);
    cl 13 allocates the struct as one stack object, i.e. the same slot order as array_group without the [0]/[1] names"""
    head, D, R_ = _lead(t); out = []
    for i in range(len(D) - 1):
        a, b = LDECL.match(D[i]), LDECL.match(D[i + 1])
        if not (a and b) or a.group(6) or b.group(6) or a.group(5) or b.group(5) or a.group(2).strip() != b.group(2).strip() or a.group(3) or b.group(3): continue
        if not re.match(r"(float|int|uint|undefined4|double)$", a.group(2).strip()): continue
        body = "\n".join(R_)
        for x in (a.group(4), b.group(4)): body = re.sub(r"(?<![\w.])%s\b" % x, "loop." + x, body)
        for fl in (False, True):
            m1, m2 = (b, a) if fl else (a, b)
            out.append("\n".join(head + D[:i] + ["  struct { %s%s; %s%s; } loop;" % (a.group(2), m1.group(4), a.group(2), m2.group(4))] + D[i + 2:]) + "\n" + body)
    return out


def m_arr_resize(t):
    out = []; b = rw.body_span(t)
    for m in re.finditer(r"(?m)^(\s+[A-Za-z_][\w \*]*?\w+\[)(\d+)(\];)", t[b:]):
        n = int(m.group(2))
        for d in (-4, -2, -1, 1, 2, 4, 8, 12, 16, 32, 64):
            if n + d > 0: out.append(t[:b + m.start(2)] + str(n + d) + t[b + m.end(2):])
    return out


def m_float_double(t):
    head, D, R_ = _lead(t); out = []
    for i, d in enumerate(D):
        m = re.match(r"^(\s{2})(float|double)(\s+\w+(?:\[\w+\])?(?:\s*=[^;]+)?;\s*)$", d)
        if m: out.append("\n".join(head + D[:i] + [m.group(1) + ("double" if m.group(2) == "float" else "float") + m.group(3)] + D[i + 1:] + R_))
    return out


def m_spill(t):
    """`f(g(a), b)` -> `T tmp = g(a); f(tmp, b)` for a statement-level call whose nested call returns a type known from the preamble prototypes; pre-declared local at the top"""
    out = []; head, D, R_ = _lead(t)
    for k, l in enumerate(R_):
        m = re.match(r"^(\s+)(?:(\w+)\s*=\s*)?(\w+)\((.*)\);\s*$", l)
        if not m or m.group(3) in ("if", "while", "for", "return", "switch"): continue
        for cm in re.finditer(r"(?<![\w.])(\w+)\(([^()]*)\)", m.group(4)):
            pm = re.search(r"extern\s+(?:__cdecl\s+)?((?:struct \w+|unsigned \w+|\w+)[\s\*]+\**)\s*%s\b" % cm.group(1), getattr(_TL, "pre", ""))
            ty = pm.group(1).strip() if pm else None
            if not ty or ty == "void" or ty == "undefined": continue
            rest = m.group(4)[:cm.start()] + m.group(4)[cm.end():]
            if re.search(r"\w+\s*\(", rest): continue
            tn = "tmp%d" % (len(out) % 7)
            decl = "  %s %s;" % (ty if not ty.endswith("*") else ty[:-1].rstrip() + " *", tn)
            if ty.endswith("*"): decl = "  %s%s;" % (ty[:-1].rstrip() + " *", tn)
            nl = m.group(1) + (m.group(2) + " = " if m.group(2) else "") + m.group(3) + "(" + m.group(4)[:cm.start()] + tn + m.group(4)[cm.end():] + ");"
            out.append("\n".join(head + D + [decl] + R_[:k] + [m.group(1) + tn + " = " + cm.group(0) + ";", nl] + R_[k + 1:]))
    return out


def m_live_range(t):
    """callee-saved-set mutator (T-B4): the set of saved registers follows the live ranges of values across calls.
    param_copy: `T q = param;` at the top and the later uses of one parameter go through q (extends/changes what is kept across calls);
    global_cache: a global read >= 2 times and never assigned becomes a local loaded once at the top (type from the preamble extern)."""
    out = []; head, D, R_ = _lead(t)
    sig = " ".join(head)
    m = re.search(r"\(([^()]*)\)\s*\{?\s*$", sig)
    params = []
    if m and m.group(1).strip() not in ("", "void"):
        for a in m.group(1).split(","):
            mm = re.match(r"^\s*((?:unsigned |struct |const )?[A-Za-z_]\w*[\s\*]+)(\w+)\s*$", a)
            if mm: params.append((mm.group(1).strip(), mm.group(2)))
    body = "\n".join(R_)
    for ty, nm_ in params:
        n = len(re.findall(r"\b%s\b" % nm_, body))
        if n < 2 or re.search(r"[&]\s*%s\b" % nm_, body) or re.search(r"\b%s\s*(=[^=]|\+\+|--|[-+*|&]=)" % nm_, body): continue
        q = nm_ + "_"
        nb = re.sub(r"\b%s\b" % nm_, q, body)
        out.append("\n".join(head + D + ["  %s %s;" % (ty, q)] + [("  %s = %s;" % (q, nm_))] + nb.split("\n")))
    pre = getattr(_TL, "pre", "")
    for g in sorted(set(re.findall(r"\b(g_\w+)\b", body))):
        if len(re.findall(r"\b%s\b" % g, body)) < 2 or re.search(r"[&]\s*%s\b" % g, body) or re.search(r"\b%s\s*(\[[^\]]*\])?\s*(=[^=]|\+\+|--|[-+*|&]=)" % g, body): continue
        pm = re.search(r"extern\s+((?:unsigned \w+|\w+))\s+%s\b" % g, pre)
        if not pm or pm.group(1) in ("undefined", "void") or re.search(r"\b%s\s*\[" % g, body): continue
        q = g[2:] + "_c"
        out.append("\n".join(head + D + ["  %s %s;" % (pm.group(1), q), "  %s = %s;" % (q, g)] + re.sub(r"\b%s\b" % g, q, body).split("\n")))
    return out


FRAME = {"decl_split": m_decl_split, "decl_merge": m_decl_merge, "decl_rev": m_decl_rev, "decl_sink": m_decl_sink, "decl_hoist": m_decl_hoist, "init_split": m_init_split,
         "array_group": m_array_group, "arr_resize": m_arr_resize, "float_double": m_float_double, "spill": m_spill, "live_range": m_live_range, "struct_group": m_struct_group}
UGLY = {"array_group", "arr_resize"}      # tags whose hits need a manual readability review


MUT = {"type_swap": m_type_swap, "rel_swap": m_rel_swap, "ifelse_not": m_ifelse_not, "mul_swap": m_mul_swap, "decl_swap": m_decl_swap, "stmt_swap": m_stmt_swap, "inline_temp": m_inline_temp,
       "loop_form": m_loop_form, "case_swap": m_case_swap, "cast_toggle": m_cast_toggle, "lit_width": m_lit_width, "regroup": m_regroup}
MUT.update(FRAME)
if os.environ.get("X87"):      # tools/x87/mutators.py: x87 source-form mutators (float temp order, hoisting, pointer-local indexing, ...) for the lifted Rwp functions
    sys.path.insert(0, os.path.join(HERE, "x87")); import mutators as x87mut; MUT.update(x87mut.MUTATORS)
MUT = {k: code_only(v) for k, v in MUT.items()}
FRAME_ONLY = "--frame" in sys.argv      # --frame: draw only frame-layout mutators (plus decl_swap/stmt_swap)



def mutants(t, rng, n):
    """n random (1-2 mutation) variants of t: [(text, 'mutator+mutator')]"""
    cache = {}
    def get(k):
        if k not in cache:
            try: cache[k] = MUT[k](t)
            except Exception: cache[k] = []
        return cache[k]
    live = [k for k in MUT if (not FRAME_ONLY or k in FRAME or k in ('decl_swap', 'stmt_swap')) and get(k)]
    out = {}
    tries = 0
    while len(out) < n and live and tries < n * 6:
        tries += 1
        k = rng.choice(live); v = rng.choice(get(k)); tag = k
        if rng.random() < 0.35:      # second mutation on top
            try:
                k2 = rng.choice([k for k in MUT if not FRAME_ONLY or k in FRAME or k in ('decl_swap', 'stmt_swap')]); c = MUT[k2](v)
                if c: v = rng.choice(c); tag += "+" + k2
            except Exception: pass
        if v != t: out[v] = tag
    return list(out.items())


# ---------------------------------------------------------------- compile + score
def score_of(code, masked, va):
    ob = cmpfn.exe_bytes(va, cmpfn.FNSZ.get(va) or len(code))
    nd = sum(1 for i, (a, b) in enumerate(zip(code, ob)) if a != b and i not in masked) + abs(len(code) - len(ob))
    ours = bc.norm(asmdiff.disasm(bc.fill(code, masked, ob))); org = bc.norm(asmdiff.disasm(ob))
    sm = difflib.SequenceMatcher(None, ours, org, autojunk=False)
    ni = sum(max(o[2] - o[1], o[4] - o[3]) for o in sm.get_opcodes() if o[0] != "equal")
    return ni, nd


class Ctx:
    pass


def eval_batch(cx, texts):
    """compile the function texts as copies of one TU; -> [(score|None, ok, det)]"""
    n = len(texts)
    syms = ["%s__v%d" % (cx.name, k) for k in range(n)]
    body = "\n".join(re.sub(r"\b%s\b" % re.escape(cx.name), s, t) for s, t in zip(syms, texts))
    uid = "%d_%d" % (os.getpid(), threading.get_ident())
    fp = os.path.join(cx.tmp, "p%s_%s.c" % (uid, cx.name))
    with open(fp, "w", errors="surrogateescape") as f: f.write(cx.pre + "\n" + body + "\n")
    try:
        res = cmpfn.check(fp, [(s, cx.va) for s in syms], cx.flags)
    finally:
        try: os.unlink(fp)
        except OSError: pass
    out = []
    for s in syms:
        ok, det = res[s]
        last = cmpfn.LAST.pop(s, None)
        if det.startswith("compile error") or last is None: out.append((None, False, det))
        else: out.append((score_of(last[0], last[1], cx.va), ok, det))
    if n > 1 and all(o[0] is None and o[2].startswith("compile error") for o in out):      # one bad variant kills the TU: bisect
        h = n // 2
        return eval_batch(cx, texts[:h]) + eval_batch(cx, texts[h:])
    return out


def prep(name, va, ch, src_dir, flags):
    cx = Ctx(); cx.name = name; cx.va = va; cx.chunk = ch
    if not os.path.exists(os.path.join(src_dir, ch)): return None
    t = open(os.path.join(src_dir, ch), errors="surrogateescape").read()
    ms = [m for m in rw.DEF.finditer(t)]
    m = next((m for m in ms if m.group(1) == name), None)
    if not m: return None
    cx.fn = m.group(0); cx.span = (m.start(), m.end()); cx.chunk_text = t
    cx.pre = t[:ms[0].start()]; _TL.pre = cx.pre
    cx.flags = flags + (["/GS"] if "compile: /GS" in t and "/GS" not in flags else [])
    cx.tmp = os.path.join(src_dir, "_tmp"); os.makedirs(cx.tmp, exist_ok=True)
    for h in os.listdir(src_dir):      # headers next to the generated TU
        if h.endswith(".h") and not os.path.exists(os.path.join(cx.tmp, h)): shutil.copy(os.path.join(src_dir, h), cx.tmp)
    return cx


def loadwait(lm):
    while os.getloadavg()[0] > lm: time.sleep(20)


def search(cx, work, budget, B, seed, lm, log):
    cd = os.path.join(work, "cache"); os.makedirs(cd, exist_ok=True)
    cf = os.path.join(cd, cx.name + ".json")
    st = json.load(open(cf)) if os.path.exists(cf) else {"seen": {}, "best": None, "best_score": None, "hit": False}
    if st["hit"]: return st
    if st.get("full"): cx.pre = cx.chunk_text[:cx.span[0]] + "\n" + cx.chunk_text[cx.span[1]:]
    rng = random.Random(seed ^ hash(cx.name) & 0xffff)
    h = lambda s: hashlib.sha1(s.encode("utf-8", "surrogateescape")).hexdigest()[:16]
    if st["best"] is None:
        loadwait(lm); (sc, ok, det), = eval_batch(cx, [cx.fn])
        if sc is None:      # static helpers / structs defined between the functions: retry with the whole chunk (other functions kept) as the preamble
            st["full"] = True; cx.pre = cx.chunk_text[:cx.span[0]] + "\n" + cx.chunk_text[cx.span[1]:]; (sc, ok, det), = eval_batch(cx, [cx.fn])
        if sc is None: st["error"] = det[:200]; json.dump(st, open(cf, "w")); return st
        st.pop("error", None); st["seen"][h(cx.fn)] = list(sc); st["best"] = cx.fn; st["best_score"] = list(sc); st["base_score"] = list(sc); st["hit"] = ok
        log("%s base score %s" % (cx.name, sc))
    cur, cur_sc = st["best"], tuple(st["best_score"]); since = 0
    while not st["hit"] and len(st["seen"]) < budget:
        muts = [(v, tag) for v, tag in mutants(cur, rng, B * 2) if h(v) not in st["seen"]][:B]
        if not muts:
            if cur != st["best"]: cur = st["best"]; cur_sc = tuple(st["best_score"]); since = 0; continue
            if not mutants(st["best"], rng, 1): break
            since += 1
            if since > 60: break
            continue
        loadwait(lm)
        res = eval_batch(cx, [v for v, _ in muts])
        bi = None
        for (v, tag), (sc, ok, det) in zip(muts, res):
            st["seen"][h(v)] = list(sc) if sc else None
            if sc is None: continue
            if ok:
                st["hit"] = True; st["best"] = v; st["best_score"] = list(sc); st["tag"] = tag; bi = None; log("HIT %s via %s after %d variants" % (cx.name, tag, len(st["seen"]))); break
            if bi is None or sc < res[bi][0]: bi = muts.index((v, tag))
        if st["hit"]: break
        if bi is not None and res[bi][0] <= cur_sc:
            if res[bi][0] < cur_sc: since = 0
            cur, cur_sc = muts[bi][0], res[bi][0]
            if cur_sc < tuple(st["best_score"]): st["best"] = cur; st["best_score"] = list(cur_sc); log("%s -> %s (%s)" % (cx.name, cur_sc, muts[bi][1]))
        else:
            since += 1
        if since >= 25: cur = st["best"]; cur_sc = tuple(st["best_score"]); since = 0
        json.dump(st, open(cf, "w"))
    json.dump(st, open(cf, "w"))
    return st


def main():
    args = sys.argv[1:]
    def opt(k, d):
        if k in args: i = args.index(k); v = args[i + 1]; del args[i:i + 2]; return v
        return d
    work = opt("--work", os.path.join(os.environ.get("TMPDIR", "/tmp"), "bytematch_permute")); names_a = opt("--names", None); maxdiff = int(opt("--maxdiff", "0")); maxinsn = int(opt("--maxinsn", "0"))
    budget = int(opt("--budget", "400")); B = int(opt("--batch", "6")); jobs = min(3, int(opt("--jobs", "3"))); seed = int(opt("--seed", "1")); flags = opt("--flags", "/O2").split()
    lm = float(os.environ.get("LOADMAX", "9")); only_list = "--list" in args; args = [a for a in args if a != "--frame"]; gs = "--gs" in args
    src = opt("--src", None) or os.path.join(work, "src"); os.makedirs(src, exist_ok=True)
    if not os.path.exists(os.path.join(src, "mfl_tier2.h")) or "--resync" in args:
        for h in os.listdir(R):
            if h.endswith(".h"): shutil.copy(os.path.join(R, h), src)
        nd = os.path.join(R, "automatch_near")
        for f in os.listdir(nd):
            if f.endswith((".c", ".h")) and not os.path.islink(os.path.join(nd, f)): shutil.copy(os.path.join(nd, f), src)
    nm = bc.names(); cand = []
    if os.environ.get("X87"): nm.update(x87mut.names(src))      # lifted functions: DIR/names.txt `VA NAME`, chunk = DIR/NAME.c
    if names_a:
        cand = [l.split()[0] for l in open(names_a[1:])] if names_a.startswith("@") else names_a.split(",")
    else:
        import csv
        for r in csv.DictReader(open(os.path.join(HERE, "..", "output", "bytediff_causes_2026-10-29.csv"))):
            if r["name"] not in nm or not r["bytes_diff_count"].isdigit() or r["category"] == "MATCH": continue
            va = nm[r["name"]][0]
            if gs and not (cmpfn.FNSZ.get(va) and b"\xd8\xc5\x62\x00" in cmpfn.exe_bytes(va, cmpfn.FNSZ[va])): continue
            if maxdiff and int(r["bytes_diff_count"]) > maxdiff and not gs: continue
            cand.append((int(r["bytes_diff_count"]), r["name"]))
        cand = [n for _, n in sorted(cand)]
    cand = [n for n in cand if n in nm]
    print("candidates:", len(cand), flush=True)
    if only_list: print(" ".join(cand)); return
    os.makedirs(os.path.join(work, "hits"), exist_ok=True)
    hits = open(os.path.join(work, "hits.txt"), "a"); lock = threading.Lock()
    def log(s):
        with lock: print(time.strftime("%H:%M:%S"), s, flush=True)
    def one(n):
        va, ch = nm[n]
        cx = prep(n, va, ch, src, flags)
        if not cx: log("%s: definition not found" % n); return
        try: st = search(cx, work, budget, B, seed, lm, log)
        except Exception as e: log("%s: EXC %r" % (n, e)); return
        if st.get("error"): log("%s: baseline error %s" % (n, st["error"][:100]))
        elif st["hit"]:
            new = cx.chunk_text[:cx.span[0]] + st["best"] + cx.chunk_text[cx.span[1]:]
            os.makedirs(os.path.join(work, "hitsw", "out"), exist_ok=True)
            tgt = os.path.join(work, "hitsw", "out", ch)
            if os.path.exists(tgt): # several hits in one chunk: apply onto the previous hit text
                t0 = open(tgt, errors="surrogateescape").read(); m = next((m for m in rw.DEF.finditer(t0) if m.group(1) == n), None)
                if m: new = t0[:m.start()] + st["best"] + t0[m.end():]
            open(tgt, "w", errors="surrogateescape").write(new)
            open(os.path.join(work, "hits", n + ".c"), "w", errors="surrogateescape").write(st["best"])
            with lock: hits.write("%s %s %s\n" % (n, ch, st.get("tag", ""))); hits.flush()
            log("HIT-SAVED %s (%s)" % (n, ch))
        else: log("%s: no hit, best %s of base %s, %d variants" % (n, st["best_score"], st.get("base_score"), len(st["seen"])))
    with ThreadPoolExecutor(jobs) as ex: list(ex.map(one, cand))


if __name__ == "__main__":
    main()
