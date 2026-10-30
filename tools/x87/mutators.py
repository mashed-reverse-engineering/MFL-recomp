"""mutators.py: x87 source-form mutators for tools/bytematch_permute.py (enabled with the environment variable X87=1 / `--x87`; imported by that file).
Every mutator maps a function text (the shape tools/x87/lift.py emits: one declaration per line, `fN = expr;` float temps, `p[i]` float-pointer indexing) to a list of variants.  They are NOT
semantics-preserving in general (hoisting a load across a store): a byte-identical hit is the oracle, and the hit is reviewed before it is installed.
  temp_order     swap two adjacent float-temp assignments / reverse a run of them (the original often computes z, y, x)
  temp_decl      permute the order of the float temp declarations (frame / register order)
  hoist          `h = a[i];` before the first use of a repeated float load, uses replaced (loads kept live in a local)
  unhoist        inline a one-use hoisted load / temp (the reverse of hoist and of split)
  split          `x = A * B + C * D;` -> `t = A * B; x = t + C * D;` (and the same for a parenthesised sub expression)
  ptr_split      `float *q = a + k;` and uses `a[k+j]` -> `q[j]` (pointer locals instead of one index base: pointer-local indexing)
  ptr_merge      the reverse of ptr_split
  cache_expr     a repeated `*(int *)(...)` chain becomes an int local assigned before its first use (cached pointer instead of the re-evaluated macro)
  uncache_expr   substitute an int local of that kind back into its uses (re-evaluated macro)
  tmp_float_double / int_cast: the generic float_double of bytematch_permute covers the temp type"""
import re

DECL = re.compile(r"^  (float|double|int)( \*?)(\w+)(\[\d+\])?;$")
def _split(t):
    """(head, decl lines, body lines, tail): declarations are the 2-space lines right after `{`"""
    L = t.split("\n"); i = next((k for k, l in enumerate(L) if l.strip() == "{"), None)
    if i is None: return None
    j = i + 1
    while j < len(L) and (DECL.match(L[j]) or L[j].strip() == ""): j += 1
    k = max(j, len(L) - 1)
    while k > j and L[k].strip() != "}": k -= 1
    return L[:i + 1], L[i + 1:j], L[j:k], L[k:]

TEMPASSIGN = re.compile(r"^(\s+)(f\d+|h\d+|t\d+) = (.*);$")
def _ids(s): return set(re.findall(r"[A-Za-z_]\w*", s))

def m_temp_order(t):
    sp = _split(t)
    if not sp: return []
    h, d, b, tl = sp; out = []
    def build(nb): return "\n".join(h + d + nb + tl)
    for i in range(len(b) - 1):
        m1, m2 = TEMPASSIGN.match(b[i]), TEMPASSIGN.match(b[i + 1])
        if m1 and m2 and m1.group(2) not in _ids(m2.group(3)) and m2.group(2) not in _ids(m1.group(3)):
            nb = list(b); nb[i], nb[i + 1] = nb[i + 1], nb[i]; out.append(build(nb))
    runs = []; i = 0
    while i < len(b):
        if TEMPASSIGN.match(b[i]):
            j = i
            while j < len(b) and TEMPASSIGN.match(b[j]): j += 1
            if j - i >= 3: runs.append((i, j))
            i = j
        else: i += 1
    for i, j in runs:
        seg = b[i:j]
        names = [TEMPASSIGN.match(x).group(2) for x in seg]
        if all(not (_ids(TEMPASSIGN.match(x).group(3)) & set(names)) for x in seg): out.append(build(b[:i] + seg[::-1] + b[j:]))
    return out

def m_temp_decl(t):
    sp = _split(t)
    if not sp: return []
    h, d, b, tl = sp; out = []
    idx = [k for k, l in enumerate(d) if re.match(r"^  (float|double) f\d+;$", l)]
    for a, c in zip(idx, idx[1:]):
        if c == a + 1:
            nd = list(d); nd[a], nd[c] = nd[c], nd[a]; out.append("\n".join(h + nd + b + tl))
    if len(idx) >= 3 and idx == list(range(idx[0], idx[0] + len(idx))):
        nd = d[:idx[0]] + d[idx[0]:idx[-1] + 1][::-1] + d[idx[-1] + 1:]; out.append("\n".join(h + nd + b + tl))
    return out

LOAD = re.compile(r"\b[a-z]\w*\[\d+\]")
def m_hoist(t):
    sp = _split(t)
    if not sp: return []
    h, d, b, tl = sp; out = []; body = "\n".join(b)
    cnt = {}
    for m in LOAD.finditer(body): cnt[m.group(0)] = cnt.get(m.group(0), 0) + 1
    n = 1 + max([int(x) for x in re.findall(r"\bh(\d+)\b", t)] or [0])
    for ld, c in cnt.items():
        if c < 2: continue
        first = next(i for i, l in enumerate(b) if ld in l)
        nb = [re.sub(re.escape(ld), "h%d" % n, l) if i >= first else l for i, l in enumerate(b)]
        nb.insert(first, "  h%d = %s;" % (n, ld))
        base = ld.split("[")[0]
        out.append("\n".join(h + d + ["  float h%d;" % n] + nb + tl))
    return out

def m_unhoist(t):
    sp = _split(t)
    if not sp: return []
    h, d, b, tl = sp; out = []
    for i, l in enumerate(b):
        m = re.match(r"^  ([fht]\d+) = ([^;]+);$", l)
        if not m: continue
        v, e = m.group(1), m.group(2)
        uses = [j for j, x in enumerate(b) if j != i and re.search(r"\b%s\b" % v, x)]
        if not uses or any(j < i for j in uses): continue
        if len(uses) > 3: continue
        pe = e if re.fullmatch(r"[\w\[\]]+", e) else "(" + e + ")"
        nb = [re.sub(r"\b%s\b" % v, lambda _: pe, x) if j in uses else x for j, x in enumerate(b)]
        del nb[i]
        nd = [x for x in d if not re.match(r"^  \w+ %s;$" % v, x)]
        out.append("\n".join(h + nd + nb + tl))
    return out

def m_split(t):
    sp = _split(t)
    if not sp: return []
    h, d, b, tl = sp; out = []
    n = 1 + max([int(x) for x in re.findall(r"\bs(\d+)\b", t)] or [0])
    for i, l in enumerate(b):
        m = re.match(r"^(\s+)([^=;{}]+?) = (.+);$", l)
        if not m or "(" in m.group(3) and "sqrt" in m.group(3) or "if" in l: continue
        rhs = m.group(3)
        terms = re.split(r" \+ ", rhs)
        if len(terms) >= 2 and all("(" not in x for x in terms):
            for k in range(1, len(terms)):
                a_, b_ = " + ".join(terms[:k]), " + ".join(terms[k:])
                nb = list(b); nb[i:i + 1] = ["  s%d = %s;" % (n, a_), "%s%s = s%d + %s;" % (m.group(1), m.group(2), n, b_)]
                out.append("\n".join(h + d + ["  float s%d;" % n] + nb + tl))
        for pm in re.finditer(r"\(([^()]+)\)", rhs):
            if re.search(r"[*/+-]", pm.group(1)) and "float" not in pm.group(1) and "int *" not in pm.group(1):
                nb = list(b); nb[i:i + 1] = ["  s%d = %s;" % (n, pm.group(1)), "%s%s = %s;" % (m.group(1), m.group(2), rhs[:pm.start()] + "s%d" % n + rhs[pm.end():])]
                out.append("\n".join(h + d + ["  float s%d;" % n] + nb + tl))
    return out

def m_ptr_split(t):
    sp = _split(t)
    if not sp: return []
    h, d, b, tl = sp; out = []; body = "\n".join(b)
    n = 1 + max([int(x) for x in re.findall(r"\bq(\d+)\b", t)] or [0])
    for base in sorted(set(re.findall(r"\b([a-z]\w*)\[\d+\]", body))):
        if not any(re.match(r"^  float \*%s;$" % base, x) for x in d) and not re.search(r"\bfloat \*%s\b" % base, "\n".join(h)): continue
        idx = sorted(set(int(x) for x in re.findall(r"\b%s\[(\d+)\]" % base, body)))
        if len(idx) < 2: continue
        for cut in sorted(set(idx[1:])):
            hi = [i for i in idx if i >= cut]
            nb = [re.sub(r"\b%s\[(\d+)\]" % base, lambda m: ("q%d[%d]" % (n, int(m.group(1)) - cut)) if int(m.group(1)) >= cut else m.group(0), l) for l in b]
            at = 0
            for k_, l_ in enumerate(nb):
                if re.match(r"^  %s = " % base, l_): at = k_ + 1
            if any(re.search(r"\bq%d\[" % n, l_) and k_ < at for k_, l_ in enumerate(nb)): continue
            nb.insert(at, "  q%d = %s + %d;" % (n, base, cut))
            out.append("\n".join(h + d + ["  float *q%d;" % n] + nb + tl))
    return out

def m_ptr_merge(t):
    sp = _split(t)
    if not sp: return []
    h, d, b, tl = sp; out = []
    for i, l in enumerate(b):
        m = re.match(r"^  (q\d+) = (\w+) \+ (\d+);$", l)
        if not m: continue
        q, base, k = m.group(1), m.group(2), int(m.group(3))
        nb = [re.sub(r"\b%s\[(\d+)\]" % q, lambda mm: "%s[%d]" % (base, int(mm.group(1)) + k), x) for x in b[:i] + b[i + 1:]]
        nd = [x for x in d if not re.match(r"^  float \*%s;$" % q, x)]
        out.append("\n".join(h + nd + nb + tl))
    return out

CHAIN = re.compile(r"\*\(int \*\)\((?:[^()]|\((?:[^()]|\([^()]*\))*\))*\)")
def m_cache_expr(t):
    sp = _split(t)
    if not sp: return []
    h, d, b, tl = sp; out = []; body = "\n".join(b)
    n = 1 + max([int(x) for x in re.findall(r"\bc(\d+)\b", t)] or [0])
    seen = {}
    for m in CHAIN.finditer(body):
        seen[m.group(0)] = seen.get(m.group(0), 0) + 1
    for e, c in seen.items():
        if c < 2: continue
        first = next(i for i, l in enumerate(b) if e in l)
        nb = [l.replace(e, "c%d" % n) if i >= first else l for i, l in enumerate(b)]
        nb.insert(first, "  c%d = %s;" % (n, e))
        out.append("\n".join(h + d + ["  int c%d;" % n] + nb + tl))
    return out

def m_uncache_expr(t):
    sp = _split(t)
    if not sp: return []
    h, d, b, tl = sp; out = []
    for i, l in enumerate(b):
        m = re.match(r"^  (c\d+) = (.+);$", l)
        if not m: continue
        v, e = m.group(1), m.group(2)
        nb = [re.sub(r"\b%s\b" % v, lambda _: e, x) for j, x in enumerate(b) if j != i]
        out.append("\n".join(h + [x for x in d if not re.match(r"^  int %s;$" % v, x)] + nb + tl))
    return out

def m_ternary(t):
    """`if (c) { x = A; } else { x = B; }` -> `x = c ? A : B;`; `if (c) return A; return B;` -> `return c ? A : B;`; and the reverse (ifelse)"""
    out = []
    pat = re.compile(r"^(\s*)if \((.+)\) \{\n\s*(\w+) = ([^;]+);\n\s*\} else \{\n\s*\3 = ([^;]+);\n\s*\}$", re.M)
    for m in pat.finditer(t):
        out.append(t[:m.start()] + "%s%s = %s ? %s : %s;" % (m.group(1), m.group(3), m.group(2), m.group(4), m.group(5)) + t[m.end():])
    pat = re.compile(r"^(\s*)if \((.+)\) \{\n\s*return ([^;]+);\n\s*\}\n\s*return ([^;]+);$", re.M)
    for m in pat.finditer(t):
        out.append(t[:m.start()] + "%sreturn %s ? %s : %s;" % (m.group(1), m.group(2), m.group(3), m.group(4)) + t[m.end():])
    pat = re.compile(r"^(\s*)if \((.+)\) return ([^;]+);\n\s*return ([^;]+);$", re.M)
    for m in pat.finditer(t):
        out.append(t[:m.start()] + "%sreturn %s ? %s : %s;" % (m.group(1), m.group(2), m.group(3), m.group(4)) + t[m.end():])
    pat = re.compile(r"^(\s*)(\w+) = (.+) \? ([^;:]+) : ([^;:]+);$", re.M)
    for m in pat.finditer(t):
        p_ = m.group(1)
        out.append(t[:m.start()] + "%sif (%s) {\n%s  %s = %s;\n%s} else {\n%s  %s = %s;\n%s}" % (p_, m.group(3), p_, m.group(2), m.group(4), p_, p_, m.group(2), m.group(5), p_) + t[m.end():])
    return out

ARRDECL = re.compile(r"^  (float|int) (\w+)\[(\d+)\];$")
def m_arr_size(t):
    """RWP-X1/X8: the frame size follows the memory-resident float array / the address-taken out buffer: `float fv[n];` and `float f12[3];` grow / shrink by 1 or 2 elements (indices stay valid)"""
    sp = _split(t)
    if not sp: return []
    h, d, b, tl = sp; out = []
    for i, l in enumerate(d):
        m = ARRDECL.match(l)
        if not m: continue
        n = int(m.group(3)); used = [int(x) for x in re.findall(r"\b%s\[(\d+)\]" % re.escape(m.group(2)), "\n".join(b))] or [0]
        for k in (n - 2, n - 1, n + 1, n + 2):
            if k >= 1 and k > max(used) and k != n:
                nd = list(d); nd[i] = "  %s %s[%d];" % (m.group(1), m.group(2), k); out.append("\n".join(h + nd + b + tl))
    return out

def m_decl_order(t):
    """swap two adjacent local declarations of any type (stack slot order: fv array, spilled int, out buffer ...)"""
    sp = _split(t)
    if not sp: return []
    h, d, b, tl = sp; out = []
    for i in range(len(d) - 1):
        if d[i].strip() and d[i + 1].strip() and d[i] != d[i + 1]:
            nd = list(d); nd[i], nd[i + 1] = nd[i + 1], nd[i]; out.append("\n".join(h + nd + b + tl))
    return out

def m_decl_sink(t):
    """move one declaration to the front / back of the declaration block"""
    sp = _split(t)
    if not sp: return []
    h, d, b, tl = sp; out = []; ds = [k for k, l in enumerate(d) if l.strip()]
    for k in ds:
        for pos in (ds[0], ds[-1]):
            if pos != k:
                nd = list(d); x = nd.pop(k); nd.insert(pos, x); out.append("\n".join(h + nd + b + tl))
    return out

def m_param_copy(t):
    """RWP-X8: a parameter copied into a local (`q = a2;`) at the top changes the register / operand choices of the x87 sums (rwp_aggregate_005895c0); and the reverse"""
    sp = _split(t)
    if not sp: return []
    h, d, b, tl = sp; out = []
    sig = "\n".join(h)
    for m in re.finditer(r"\b(float \*|int )(a\d+)\b", sig):
        ty, p = m.group(1), m.group(2)
        if sum(len(re.findall(r"\b%s\b" % p, l)) for l in b) < 2: continue
        n = 1 + max([int(x) for x in re.findall(r"\bq(\d+)\b", t)] or [0])
        nb = [re.sub(r"\b%s\b" % p, "q%d" % n, l) for l in b]
        out.append("\n".join(h + d + ["  %sq%d;" % (ty, n)] + ["  q%d = %s;" % (n, p)] + nb + tl))
    for i, l in enumerate(b):
        m = re.match(r"^  (q\d+) = (a\d+);$", l)
        if m:
            nb = [re.sub(r"\b%s\b" % m.group(1), m.group(2), x) for j, x in enumerate(b) if j != i]
            out.append("\n".join(h + [x for x in d if not re.match(r"^  (float \*|int )%s;$" % m.group(1), x)] + nb + tl))
    return out

def m_ret_ternary(t):
    """RWP-X10: `mov eax,A; jcc; mov eax,B` is an integer conditional: `if (c) { r = A; } else { r = B; } return r;` -> `return c ? A : B;` (and the condition negated / arms swapped)"""
    out = []
    pat = re.compile(r"^(\s*)if \((.+)\) \{\n\s*(\w+) = ([^;]+);\n\s*\} else \{\n\s*\3 = ([^;]+);\n\s*\}\n\s*return \3;$", re.M)
    for m in pat.finditer(t):
        c = m.group(2)
        rel = re.fullmatch(r"!\((\S+) (<=|>=|<|>) (\S+)\)", c)
        if rel: c = "%s %s %s" % (rel.group(1), {"<=": ">", ">=": "<", "<": ">=", ">": "<="}[rel.group(2)], rel.group(3))      # NaN: only the spelling of the original decides, the search tries both
        arms = [(c, m.group(4), m.group(5)), (m.group(2), m.group(4), m.group(5))]
        rel2 = re.fullmatch(r"(\S+) (<=|>=|<|>) (\S+)", m.group(2))
        if rel2: arms.append(("%s %s %s" % (rel2.group(1), {"<=": ">", ">=": "<", "<": ">=", ">": "<="}[rel2.group(2)], rel2.group(3)), m.group(5), m.group(4)))
        for cc, a_, b_ in arms:
            v = t[:m.start()] + "%sreturn %s ? %s : %s;" % (m.group(1), cc, a_, b_) + t[m.end():]
            v = re.sub(r"\n  int %s;" % m.group(3), "", v)
            if v not in out: out.append(v)
    return out

def m_temps_double(t):
    """RWP-X11: float temps that stay on the x87 stack across stores are `double` locals in the original (memory operand first: `fld [m]; fmul st,st(i)`, pops at the end of the live range;
    rwp_convexhullmassprops_00583ca0).  All float temps at once (the search only changes one site at a time and each single change is worse), and the reverse."""
    sp = _split(t)
    if not sp: return []
    h, d, b, tl = sp; out = []
    ft = [k for k, l in enumerate(d) if re.match(r"^  float [fsht]\d+;$", l) or re.match(r"^  float f_a\d+;$", l)]
    dt = [k for k, l in enumerate(d) if re.match(r"^  double [fsht]\d+;$", l) or re.match(r"^  double f_a\d+;$", l)]
    if len(ft) >= 2:
        out.append("\n".join(h + [l.replace("float", "double", 1) if k in ft else l for k, l in enumerate(d)] + b + tl))
        out.append("\n".join(h + [l.replace("float", "double", 1) if k in ft[:len(ft) // 2 + 1] else l for k, l in enumerate(d)] + b + tl))
    if dt: out.append("\n".join(h + [l.replace("double", "float", 1) if k in dt else l for k, l in enumerate(d)] + b + tl))
    return out

def m_temp_reuse(t):
    """RWP-X8: cl gives two float locals with disjoint live ranges the same frame slot only when the source uses ONE variable (rwp_aggregate_005891c0: `f1 = f8 - ...` lives in f8's slot):
    rename B to A when B's first assignment follows A's last use (floats only, plain scalar locals, not address-taken, not arrays)."""
    sp = _split(t)
    if not sp: return []
    h, d, b, tl = sp; out = []
    names = [m.group(1) for l in d for m in [re.match(r"^  float (\w+);$", l)] if m]
    body = "\n".join(b)
    for A in names:
        if re.search(r"&%s\b" % A, body): continue
        ua = [i for i, l in enumerate(b) if re.search(r"\b%s\b" % A, l)]
        if not ua: continue
        for B in names:
            if A == B or re.search(r"&%s\b" % B, body): continue
            ub = [i for i, l in enumerate(b) if re.search(r"\b%s\b" % B, l)]
            if not ub or min(ub) <= max(ua): continue
            if not re.match(r"^\s+%s = " % B, b[min(ub)]) or re.search(r"\b%s\b" % B, b[min(ub)].split(" = ", 1)[1]): continue
            if any(b[i].strip().startswith(("}", "else")) or "if (" in b[i] for i in range(max(ua) + 1, min(ub))) and False: continue
            nb = [re.sub(r"\b%s\b" % B, A, l) for l in b]
            out.append("\n".join(h + [x for x in d if not re.match(r"^  float %s;$" % B, x)] + nb + tl))
    return out

def m_double_one(t):
    """RWP-X11: one float temp <-> double (the all-at-once temps_double covers the group); for a double temp, `(float)` on one use flips the var-first / memory-first operand form
    (rwp_cylinder_0056fd20: `(float)inv * in->x` for the first use only)."""
    sp = _split(t)
    if not sp: return []
    h, d, b, tl = sp; out = []; body = "\n".join(b)
    for k, l in enumerate(d):
        m = re.match(r"^  float ((?:f|s|h|t)\d+|f_a\d+);$", l)
        if m: out.append("\n".join(h + d[:k] + [l.replace("float", "double", 1)] + d[k + 1:] + b + tl))
        m = re.match(r"^  double (\w+);$", l)
        if m:
            v = m.group(1); out.append("\n".join(h + d[:k] + [l.replace("double", "float", 1)] + d[k + 1:] + b + tl))
            uses = [x for x in re.finditer(r"(?<![\w\]])%s\b(?!\s*=[^=])" % re.escape(v), body)]
            for u in uses[:6]:
                nb = body[:u.start()] + "(float)" + v + body[u.end():]
                out.append("\n".join(h + d + nb.split("\n") + tl))
    return out

def _chain(e):
    """split the top-level `a + b - c` chain of an expression into (terms, ops); terms keep their own parentheses"""
    terms, ops, d, cur, i = [], [], 0, "", 0
    while i < len(e):
        c = e[i]
        if c in "([": d += 1
        elif c in ")]": d -= 1
        if d == 0 and e[i:i + 3] in (" + ", " - "): terms.append(cur); ops.append(e[i + 1]); cur = ""; i += 3; continue
        cur += c; i += 1
    terms.append(cur)
    return terms, ops

def m_paren(t):
    """T-X89 (RWP-X12): cl reorders the operands of a FLAT sum chain (`y*y + x*x + z*z` comes out z, y, x) but evaluates an explicitly parenthesised left pair in source order:
    `(y*y + x*x) + z*z` is y, x, z.  Variants: first pair in parentheses (rest flat), first k terms nested, and the reverse (drop parentheses of a left chain)."""
    out = []; L = t.split("\n")
    for i, l in enumerate(L):
        m = re.match(r"^(\s*(?:\w[\w\[\]\.>-]* = |return |if \())(.*?)(\)? \{|;)$", l)
        if not m or "(float)" in m.group(2) and False: continue
        terms, ops = _chain(m.group(2))
        if len(terms) < 3: continue
        def build(k, nest):
            if nest:
                s = terms[0]
                for j in range(1, k): s = "(%s %s %s)" % (s, ops[j - 1], terms[j]) if j > 1 or True else s
                head = s
            else: head = "(%s %s %s)" % (terms[0], ops[0], terms[1]); k = 2
            return head + "".join(" %s %s" % (ops[j - 1], terms[j]) for j in range(k, len(terms)))
        for k, nest in [(2, False)] + [(k, True) for k in range(3, len(terms))]:
            nl = list(L); nl[i] = m.group(1) + build(k, nest) + m.group(3); out.append("\n".join(nl))
        # drop one level of parentheses of a leading `(a + b)` term
        if re.fullmatch(r"\([^()]*\)", terms[0]) and ops[0] == "+":
            nl = list(L); nl[i] = m.group(1) + terms[0][1:-1] + "".join(" %s %s" % (o, x) for o, x in zip(ops, terms[1:])) + m.group(3); out.append("\n".join(nl))
    return out

def m_pair(t):
    """T-X89: two moves at once (the single-site hill climb never reaches pairs such as split + (float) cast, param_copy + ptr_split): every ordered pair of different pair-moves, capped"""
    mv = [m_paren, m_split, m_double_one, m_param_copy, m_temps_double, m_temp_reuse, m_ptr_split, m_temp_order, m_decl_order, m_ternary]
    out = []; seen = {t}
    for a in mv:
        for v1 in a(t)[:8]:
            for b in mv:
                if b is a: continue
                for v2 in b(v1)[:5]:
                    if v2 not in seen: seen.add(v2); out.append(v2)
    return out[:400]

MUTATORS = {"paren": m_paren, "pair": m_pair, "double_one": m_double_one, "temp_reuse": m_temp_reuse, "temps_double": m_temps_double, "ret_ternary": m_ret_ternary, "arr_size": m_arr_size, "decl_order": m_decl_order, "decl_sink": m_decl_sink, "param_copy": m_param_copy, "ternary": m_ternary, "temp_order": m_temp_order, "temp_decl": m_temp_decl, "hoist": m_hoist, "unhoist": m_unhoist, "split": m_split, "ptr_split": m_ptr_split,
            "ptr_merge": m_ptr_merge, "cache_expr": m_cache_expr, "uncache_expr": m_uncache_expr}

def names(src_dir):
    """pseudo `name -> (va, chunk)` table for bytematch_permute: DIR/names.txt lines `VA NAME` (chunk = NAME.c in the --src directory)"""
    import os
    out = {}; p = os.path.join(src_dir, "names.txt")
    if os.path.exists(p):
        for l in open(p):
            q = l.split()
            if len(q) >= 2: out[q[1]] = (int(q[0], 16), q[1] + ".c")
    return out

if __name__ == "__main__":      # self-check: every mutator yields compilable-looking variants on a lifted function
    import sys, os
    sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
    import lift
    src = lift.lift(0x56bd20)
    for k, f in MUTATORS.items():
        v = f(src); print(k, len(v)); assert all(x != src for x in v)
    assert len(m_ptr_split(lift.lift(0x56f180))) >= 1
