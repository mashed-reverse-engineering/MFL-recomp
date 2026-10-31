#!/usr/bin/env python3
"""seqsum.py IN.c OUT.c [ACC=acc]: T-N6 helper.  Rewrites every `LHS = EXPR;` statement of a lifted/hand typed x87 function so that a chain of 3+ sum terms or 3+ product factors
is evaluated in the written order as a sequence of one-operation statements on a double temp (`acc = a; acc = acc + b; ...`).  cl /O2 regroups a plain `a*b + c*d + e*f` (z,x,y instead of
y,x,z; X + X as ((a+b)+a)+b) and the result is no longer bit-equal to the original under diffemu; the one-operation form is kept in order.  `X + X` (second term = sum of the previous terms)
becomes `acc = acc + acc`.  Declares `double acc0..accN` after the first `{` of the function.  Statements inside if/for headers are not touched."""
import re, sys
TOK = re.compile(r"\s*(->|\d+\.\d*f?|\d+f?|[A-Za-z_]\w*|[()\[\].+\-*/,])")
def tokens(s):
    out = []; i = 0
    while i < len(s):
        m = TOK.match(s, i)
        if not m: return None
        out.append(m.group(1)); i = m.end()
    return out
class P:
    def __init__(p, t): p.t = t; p.i = 0
    def peek(p): return p.t[p.i] if p.i < len(p.t) else None
    def eat(p): p.i += 1; return p.t[p.i - 1]
    def expr(p):
        n = p.term(); 
        while p.peek() in ("+", "-"): op = p.eat(); n = ("bin", op, n, p.term())
        return n
    def term(p):
        n = p.unary()
        while p.peek() in ("*", "/"): op = p.eat(); n = ("bin", op, n, p.unary())
        return n
    def unary(p):
        if p.peek() == "-": p.eat(); return ("neg", p.unary())
        if p.peek() == "(":
            if p.i + 2 < len(p.t) and p.t[p.i + 1] in ("float", "double", "int") and p.t[p.i + 2] == ")":
                p.eat(); ty = p.eat(); p.eat(); return ("cast", ty, p.unary())
            p.eat(); n = p.expr(); assert p.eat() == ")"; return ("par", n)
        return p.post()
    def post(p):
        a = p.eat(); s = [a]
        while p.peek() in ("[", ".", "->", "("):
            t = p.eat(); s.append(t)
            if t == "[": d = 1
            elif t == "(": d = 1
            else: s.append(p.eat()); continue
            while d:
                x = p.eat(); s.append(x); d += (x in "[(") - (x in "])")
        return ("atom", " ".join(s).replace(" [", "[").replace("[ ", "[").replace(" ]", "]").replace(" ( ", "(").replace(" )", ")").replace(" . ", ".").replace(" -> ", "->").replace(" ,", ",") if False else "".join(s))
def flat(n, ops):
    if n[0] == "par" and False: return flat(n[1], ops)
    if n[0] == "bin" and n[1] in ops: return flat(n[2], ops) + [(n[1], n[3])]
    return [("+" if ops[0] == "+" else "*", n)]
def show(n):
    k = n[0]
    if k == "atom": return n[1]
    if k == "neg": return "-" + show(n[1])
    if k == "cast": return "(%s)%s" % (n[1], show(n[2]))
    if k == "par": return "(" + show(n[1]) + ")"
    return show(n[2]) + " " + n[1] + " " + show(n[3])
class G:
    def __init__(g): g.n = 0; g.st = []
    def tmp(g): g.n += 1; return "acc%d" % (g.n - 1)
    def emit(g, n, top=False):
        k = n[0]
        if k == "atom": return n[1]
        if k == "neg": return "-" + g.emit(n[1])
        if k == "cast": return "(%s)%s" % (n[1], g.emit(n[2]))
        if k == "par": return "(" + g.emit(n[1]) + ")" if n[1][0] == "bin" and not g.chain(n[1]) else (g.emit(n[1]) if g.chain(n[1]) else "(" + g.emit(n[1]) + ")")
        ops = ("+", "-") if n[1] in "+-" else ("*", "/")
        ch = flat(n, ops); 
        if len(ch) < 3: return g.inl(n)
        t = g.tmp(); first = g.emit(ch[0][1]); g.st.append("%s = %s;" % (t, first)); prev = [show(ch[0][1])]
        for op, x in ch[1:]:
            xs = show(x)
            if xs.strip("()") == " + ".join(prev) and op == "+" and ops[0] == "+": g.st.append("%s = %s + %s;" % (t, t, t))      # X + X
            else: g.st.append("%s = %s %s %s;" % (t, t, op, g.emit(x)))
            prev.append(xs)
        return t
    def chain(g, n): return n[0] == "bin" and len(flat(n, ("+", "-") if n[1] in "+-" else ("*", "/"))) >= 3
    def inl(g, n):
        if n[0] == "bin": return g.sub(n[2]) + " " + n[1] + " " + g.sub(n[3])
        return g.emit(n)
    def sub(g, n):
        if n[0] == "bin" and not g.chain(n): return g.inl(n)
        return g.emit(n)
def conv(lines, T=0):
    out = []; m = 0
    for l in lines:
        mm = re.match(r"^(\s*)((?:\*\([^;=]*\)\([^;=]*\)|[A-Za-z_][\w\[\]\.>\-]*)) = (.+);\s*$", l)
        tk = tokens(mm.group(3)) if mm else None
        if tk and not any(x in ("?", "<", ">", "&", "|") for x in mm.group(3)):
            try:
                p = P(tk); n = p.expr()
                if p.i != len(tk): raise ValueError
                g = G(); g.n = 0
                if g.chain(n) or any(c for c in [1] if False) or "(" in mm.group(3):
                    g.n = 0; r = g.emit(n)
                    if g.st:
                        for s in g.st: out.append(mm.group(1) + s)
                        out.append("%s%s = %s;" % (mm.group(1), mm.group(2), r)); m = max(m, g.n); continue
            except (AssertionError, ValueError, IndexError): pass
        out.append(l)
    return out, m
def main():
    src = open(sys.argv[1]).read().split("\n"); out, m = conv(src)
    txt = "\n".join(out)
    if m: txt = txt.replace("{\n", "{\n  double %s;\n" % ", ".join("acc%d" % i for i in range(m)), 1) if "double acc0" not in txt else txt
    open(sys.argv[2], "w").write(txt)
if __name__ == "__main__": main()
