#!/usr/bin/env python3
"""t2_unify.py: names for the register / spill-slot operands of the symbolic x87 trees (t2_x87.py), taken from the C expression Ghidra printed for the SAME computation.
Ghidra's text keeps `g_tick_seconds + *pfVar2` where the original has `fld [g_tick_seconds]; fadd [ebx]` (ebx = pfVar2): the x87 tree op(+, m ds:g_tick_seconds, m [ebx]) is unified with the parsed C
expression; the leaf that has no C name of its own (`M:[ebx]`) takes the text of the matching sub expression (`*pfVar2`).  The same for integer path conditions (`M:[S-68] != 0` <-> `if (bVar2 != 0)`).
bind(text, unknown canons, tree ids, leaf, TREES) -> {canon: C text}; only unambiguous bindings (every successful unification agrees) are returned; diffemu verifies the result."""
import re

TOK = re.compile(r"\s*(?:(0x[0-9a-fA-F]+|\d+\.\d*(?:[eE][-+]?\d+)?f?|\d+(?:[eE][-+]?\d+)?f?|\.\d+f?)|([A-Za-z_]\w*)|(->|<<|>>|<=|>=|==|!=|&&|\|\||[-+*/%&|^!~<>()\[\].,?:=]))")
def tokens(s):
    out = []; i = 0
    while i < len(s):
        m = TOK.match(s, i)
        if not m:
            if s[i:].strip() == "": break
            return None
        out.append(m.group(1) or m.group(2) or m.group(3)); i = m.end()
    return out

TYPES = {"float", "double", "int", "uint", "short", "ushort", "char", "uchar", "byte", "float10", "RwReal", "undefined4", "undefined", "undefined2", "undefined1", "long", "ulong", "bool", "unsigned", "signed", "void", "struct", "RwV3d", "RwMatrix"}
class P:
    def __init__(s, toks): s.t = toks; s.i = 0
    def peek(s, k=0): return s.t[s.i + k] if s.i + k < len(s.t) else None
    def eat(s, x=None):
        v = s.peek()
        if v is None or (x is not None and v != x): raise ValueError(x)
        s.i += 1; return v
    def expr(s): return s.binary(0)
    LV = [("||",), ("&&",), ("==", "!="), ("<", ">", "<=", ">="), ("+", "-"), ("*", "/", "%")]
    def binary(s, lv):
        if lv >= len(s.LV): return s.unary()
        l = s.binary(lv + 1)
        while s.peek() in s.LV[lv]:
            op = s.eat(); r = s.binary(lv + 1); l = ("bin", op, l, r)
        return l
    def unary(s):
        v = s.peek()
        if v in ("-", "!", "*", "&", "~", "+"): s.eat(); return ("un", v, s.unary())
        if v == "(" and s.is_cast():
            s.eat(); ty = []
            while s.peek() != ")": ty.append(s.eat())
            s.eat(")"); return ("cast", " ".join(ty), s.unary())
        return s.postfix()
    def is_cast(s):
        j = s.i + 1; ty = []
        while j < len(s.t) and s.t[j] != ")":
            ty.append(s.t[j]); j += 1
        if j >= len(s.t) or not ty or ty[0] not in TYPES: return False
        return all(x in TYPES or x == "*" for x in ty)
    def postfix(s):
        x = s.primary()
        while True:
            v = s.peek()
            if v == "[": s.eat(); i = s.expr(); s.eat("]"); x = ("idx", x, i)
            elif v in (".", "->"): s.eat(); x = ("mem", x, v, s.eat())
            elif v == "(" and x[0] == "id":
                s.eat(); a = []
                while s.peek() != ")":
                    a.append(s.expr())
                    if s.peek() == ",": s.eat()
                s.eat(")"); x = ("call", x[1], tuple(a))
            else: return x
    def primary(s):
        v = s.eat()
        if v == "(":
            e = s.expr(); s.eat(")"); return ("par", e)
        if re.fullmatch(r"0x[0-9a-fA-F]+|\d[\d.eE+-]*f?|\.\d+f?", v): return ("num", v)
        if re.fullmatch(r"[A-Za-z_]\w*", v): return ("id", v)
        raise ValueError(v)

def parse(s):
    tk = tokens(s)
    if not tk: return None
    p = P(tk)
    try:
        e = p.expr()
        return e if p.i == len(tk) else None
    except (ValueError, IndexError): return None

def src(n):
    k = n[0]
    if k == "id": return n[1]
    if k == "num": return n[1]
    if k == "par": return "(%s)" % src(n[1])
    if k == "un": return "%s%s" % (n[1], src(n[2]))
    if k == "cast": return "(%s)%s" % (n[1], src(n[2]))
    if k == "bin": return "%s %s %s" % (src(n[2]), n[1], src(n[3]))
    if k == "idx": return "%s[%s]" % (src(n[1]), src(n[2]))
    if k == "mem": return "%s%s%s" % (src(n[1]), n[2], n[3])
    if k == "call": return "%s(%s)" % (n[1], ", ".join(src(a) for a in n[2]))
    return "?"

def strip(n):
    """parentheses and float / int conversion casts do not change which memory operand is read"""
    while n[0] == "par" or (n[0] == "cast" and re.sub(r"[ *]", "", n[1]) in ("float", "double", "float10", "RwReal", "int", "uint", "undefined4")): n = n[1] if n[0] == "par" else n[2]
    return n

def subnodes(n):
    yield n
    k = n[0]
    for c in ((n[1],) if k in ("par",) else (n[2],) if k in ("un", "cast") else (n[2], n[3]) if k == "bin" else (n[1], n[2]) if k == "idx" else (n[1],) if k == "mem" else n[2] if k == "call" else ()):
        yield from subnodes(c)

def exprs_of(t, nm):
    """parsed C expressions of the function body: right hand sides, conditions, call arguments"""
    h = re.search(r"^[A-Za-z_][^;{}]*?\b%s\s*\([^)]*\)\s*\{" % re.escape(nm), t, re.M)
    body = t[h.end():] if h else t
    body = re.sub(r"/\*.*?\*/", "", body, flags=re.S)
    out = []
    for st in re.split(r"[;{}]", body):
        st = " ".join(st.split())
        if not st: continue
        for m in re.finditer(r"\b(?:if|while)\s*\((.*)\)\s*$", st): out.append(m.group(1))
        mm = re.match(r"(?:else\s+)?(?:[\w.\[\]>*\-() ]+?)\s=\s(.*)$", st)
        if mm and not st.startswith(("if", "while", "for")): out.append(mm.group(1))
        else: out.append(st)
        for m in re.finditer(r"\b\w+\(((?:[^()]|\([^()]*\))*)\)", st):
            for a in re.split(r",(?![^()]*\))", m.group(1)): out.append(a)
    res = []
    for e in out:
        a = parse(e)
        if a: res.append(a)
    return res

def assigns_of(t, nm):
    """(lhs text, parsed rhs) of the plain assignments `v = expr;` of the function body"""
    h = re.search(r"^[A-Za-z_][^;{}]*?\b%s\s*\([^)]*\)\s*\{" % re.escape(nm), t, re.M)
    body = re.sub(r"/\*.*?\*/", "", t[h.end():] if h else t, flags=re.S); out = []
    for st in re.split(r"[;{}]", body):
        m = re.fullmatch(r"\s*(?:else\s+)?([A-Za-z_][\w.]*)\s=\s(.*)", " ".join(st.split()))
        if m:
            a = parse(m.group(2))
            if a: out.append((m.group(1), a))
    return out

def norm(s): return re.sub(r"[\s()]", "", s)

class U:
    def __init__(s, TREES, leaf0):
        s.T = TREES; s.leaf0 = leaf0
    def uni(s, tid, node, bind):
        t = s.T[tid]; node = strip(node); k = t[0]
        if k == "op" and node[0] == "bin" and node[1] == t[1]:
            for a, b in ((t[2], t[3]), (t[3], t[2]) if t[1] in "+*" else (None, None)):
                if a is None: continue
                b0 = dict(bind)
                if s.uni(a, node[2], b0) and s.uni(b, node[3], b0): bind.update(b0); return True
            return False
        if k == "neg": return node[0] == "un" and node[1] == "-" and s.uni(t[1], node[2], bind)
        if k in ("abs", "sqrt", "sin", "cos"): return node[0] == "call" and node[1] in ({"abs": "fabs", "sqrt": "sqrt", "sin": "sin", "cos": "cos"}[k], "f" + k) and len(node[2]) == 1 and s.uni(t[1], node[2][0], bind)
        if k == "m":
            r = s.leaf0(t[1], t[2])
            if r is not None:
                m = re.fullmatch(r"\*\((?:float|double) \*\)&(\w+)", r)
                return norm(r) == norm(src(node)) or bool(m and m.group(1) == src(node))
            if node[0] in ("num", "bin", "call", "cast"): return False
            if t[1] in bind: return bind[t[1]] == src(node)
            bind[t[1]] = src(node); return True
        if k in ("i", "iu"):
            r = s.leaf0(t[1], "i%d" % t[2])
            if r is not None: return norm(r) == norm(src(node))
            if node[0] in ("num", "bin", "call"): return False
            if t[1] in bind: return bind[t[1]] == src(node)
            bind[t[1]] = src(node); return True
        if k == "k":
            if node[0] == "num":
                try: return abs(float(node[1].rstrip("f")) - t[1]) <= abs(t[1]) * 1e-5
                except ValueError: return False
            return False
        return False
    def unic(s, c, node, bind):
        """integer path condition ('i', op, a, b) against a comparison node (or a bare / negated variable)"""
        node = strip(node)
        ops = {"==": "==", "!=": "!=", "<": "<", "<=": "<=", ">": ">", ">=": ">="}
        a, b = c[2], c[3]
        if node[0] == "id" and b == "I:0x0" and c[1] in ("!=", "=="): cand = [(c[1], node)]
        elif node[0] == "un" and node[1] == "!" and b == "I:0x0" and c[1] in ("!=", "=="): cand = [({"!=": "==", "==": "!="}[c[1]], strip(node[2]))]
        elif node[0] == "bin" and node[1] in ops and node[1] == c[1].rstrip("u"):
            r = s.leaf0(b, "i4")
            if r is not None and norm(r) != norm(src(node[3])): return False
            cand = [(c[1], node[2])]
        else: return False
        n = cand[0][1]
        if cand[0][0] != c[1] and not (node[0] == "un"): return False
        if cand[0][0] != c[1]: return False
        r = s.leaf0(a, "i4")
        if r is not None: return norm(r) == norm(src(n))
        if n[0] in ("num", "bin", "call"): return False
        if a in bind: return bind[a] == src(n)
        bind[a] = src(n); return True

def _k(t):
    return (t[2], t[3]) if t[0] == "op" else (t[1],) if t[0] in ("neg", "abs", "sqrt", "sin", "cos") else ()
def _walk(T, tid, seen, out):
    if tid in seen: return
    seen.add(tid); t = T[tid]
    for k in _k(t): _walk(T, k, seen, out)
    if t[0] == "sel":
        for pcs, v in t[1]:
            _walk(T, v, seen, out)
            for c in pcs:
                if c[0] == "f": _walk(T, c[3], seen, out); _walk(T, c[4], seen, out)
                out.append(("c", c))
        _walk(T, t[2], seen, out)
    out.append(("t", tid))

def bind(t, nm, unknown, tids, TREES, leaf0):
    """-> {canon: C text} for the unknown leaf canons (the ones leaf0 returned None for)"""
    ex = exprs_of(t, nm); U_ = U(TREES, leaf0); nodes = [n for e in ex for n in subnodes(e)]
    items = []; seen = set()
    for tid in tids: _walk(TREES, tid, seen, items)
    res = {}; bad = set()
    unknown = list(unknown) + sorted({TREES[i][1] for i in tids if TREES[i][0] in ("m", "i", "iu") and TREES[i][1] not in unknown and re.fullmatch(r"M:\[e\w\w(?:[+-]0x[0-9a-f]+)?\]|M:\[S-\d+\]", TREES[i][1]) and leaf0(TREES[i][1], "f" if TREES[i][0] == "m" else "i4") is None})      # every unnamed register / spill slot operand: one binding names the base register for the others
    def contains(tid, u, memo={}):
        tt = TREES[tid]
        if tt[0] in ("m", "i", "iu"): return tt[1] == u
        return any(contains(k, u) for k in _k(tt))
    asg = assigns_of(t, nm)
    for tid in tids:      # a spill slot (fst [esp+k]) that Ghidra shows as a C variable: `fVar1 = <the stored expression>`
        tt = TREES[tid]
        if tt[0] == "m" and len(tt) > 3 and tt[1].startswith(("M:[S", "W:[S")) and leaf0(tt[1], "f") is None and tt[1] not in res:
            lhs = set()
            for l_, r_ in asg:
                if U_.uni(tt[3], r_, {}): lhs.add(l_)
            if len(lhs) == 1: res[tt[1]] = lhs.pop()
    for u in unknown:
        if u in res: continue
        found = set()
        for kind, it in items:
            if kind == "t" and TREES[it][0] in ("op", "neg", "abs", "sqrt", "sin", "cos") and contains(it, u):
                for n in nodes:
                    b = {}
                    if U_.uni(it, n, b) and u in b: found.add(b[u])
            elif kind == "c" and it[0] == "i" and u in (it[2], it[3]):
                for n in nodes:
                    b = {}
                    if U_.unic(it, n, b) and u in b: found.add(b[u])
            elif kind == "t" and TREES[it][0] in ("m", "i") and TREES[it][1] == u and False: pass
        if len(found) == 1: res[u] = found.pop()
    return res

if __name__ == "__main__":      # python3 tools/t2_unify.py : self check (parse, strip casts, unify a tree with a C expression)
    import t2_x87 as X
    e = parse("(float10)fVar1 * (float)*pfVar2 + g_tick_seconds[3].y"); assert e and src(strip(e[3])) == "g_tick_seconds[3].y" and e[1] == "+"
    a = X.mk(("m", "M:ds:0x7ef7f4", "f")); b = X.mk(("m", "M:[ebx]", "f")); o = X.mk(("op", "+", a, b))
    leaf0 = lambda c, k: "g_tick_seconds" if c == "M:ds:0x7ef7f4" else None
    r = bind("void f(void)\n{\n  fVar1 = g_tick_seconds + *pfVar2;\n}\n", "f", ["M:[ebx]"], [o], X.TREES, leaf0)
    assert r == {"M:[ebx]": "*pfVar2"}, r
    print("ok")
