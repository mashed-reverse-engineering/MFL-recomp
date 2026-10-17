"""t2_stfix.py: scripted fixes of compile errors that come from a GLOBAL declared with the wrong type in the file (decls.h still says `undefined4`, Ghidra's text uses it as a pointer / pointer array).
fix(path, err) -> True when the file was changed (the caller recompiles).  Kinds:
  gptr   C2223 `left of '->m' must point to struct/union` on a global `extern undefined4|int|uint g;`: its type is that of the value stored into it (`g = pLocal;` with `T *pLocal;`, `g = F(..)` with `extern T * F();`)
  garr   C2440 `cannot convert from 'T' to 'T *'` on `*g` where `extern T *g;`: the global is the first element of a pointer array (`T *g[]`; `(int)g + off` then is the array address, as in the original)"""
import re
def _lines(err): return sorted({int(x) for x in re.findall(r"f\.c\((\d+)\)", err)})
def fix(path, err):
    t = open(path, errors="replace").read(); L = t.split("\n"); ch = False
    for ln in _lines(err):
        if ln - 1 >= len(L): continue
        s = L[ln - 1]
        if "C2223" in err and re.search(r"f\.c\(%d\) : error C2223" % ln, err):
            for m in re.finditer(r"\(?\b(\w+)\)?\s*(?:->|\)->)", s):
                g = m.group(1)
                if not re.search(r"^(?:/\*[0-9a-f]{8}\*/ )?extern (?:undefined4|int|uint) %s;" % re.escape(g), t, re.M): continue
                ty = None
                for r in re.finditer(r"^\s*%s = (?:\([^)]*\))?(\w+)(\(|;)" % re.escape(g), t, re.M):
                    x = r.group(1)
                    if r.group(2) == "(":
                        p = re.search(r"^(?:/\*[0-9a-f]{8}\*/ )?extern ([A-Za-z_][\w ]*?) ?\* ?(?:__\w+ )?%s\(" % re.escape(x), t, re.M)
                    else: p = re.search(r"^\s+([A-Za-z_][\w ]*?) ?\*\s*%s;" % re.escape(x), t, re.M)
                    if p and p.group(1).strip() not in ("void", "char", "undefined", "undefined4"): ty = p.group(1).strip(); break
                if ty:
                    t = re.sub(r"^((?:/\*[0-9a-f]{8}\*/ )?)extern (?:undefined4|int|uint) %s;" % re.escape(g), r"\1extern %s *%s;" % (ty, g), t, flags=re.M); ch = True
        m = re.search(r"cannot convert from '(\w+)' to '\1 \*'", err) if re.search(r"f\.c\(%d\) : error C2440" % ln, err) else None
        if m:
            for q in re.finditer(r"\*\s*(\w+)\b(?!\s*[\[(])", s):
                g = q.group(1)
                if re.search(r"^(?:/\*[0-9a-f]{8}\*/ )?extern %s \*%s;" % (re.escape(m.group(1)), re.escape(g)), t, re.M):
                    t = re.sub(r"^((?:/\*[0-9a-f]{8}\*/ )?extern %s \*%s);" % (re.escape(m.group(1)), re.escape(g)), r"\1[];", t, flags=re.M); ch = True
    if ch: open(path, "w").write(t)
    return ch

def arity(path, name, finds):
    """defectlint class 4 / 10 findings about the arity of the function's OWN definition (`prototype has 1 parameter(s), the original callers pass 2`, `the C definition declares 0 parameter(s) but the original
    reads / rewrites 1 stack argument slot(s)`) or of a callee prototype (`FUN_x called with 1 argument(s), prototype has 0`): Ghidra dropped trailing stack arguments the body never uses -> `undefined4 param_N` added
    (cdecl: an unused trailing argument changes no code).  finds = the lint strings of t2_auto.lint.  -> True when the file changed"""
    t = open(path, errors="replace").read(); t0 = t
    def add(sig, n, m):
        ps = [] if sig.strip() in ("", "void") else [x.strip() for x in sig.split(",")]
        if len(ps) != n: return None
        return ", ".join(ps + ["undefined4 st_arg%d" % (i + 1) for i in range(n, m)])
    for f in finds:
        m = re.search(r"prototype has (\d+) parameter\(s\), the original callers pass (\d+)", f) or re.search(r"definition declares (\d+) parameter\(s\) but the original reads / rewrites (\d+) stack", f)
        ln = re.search(r"line (\d+)", f); src = t.split("\n")[int(ln.group(1)) - 1] if ln and int(ln.group(1)) <= len(t.split("\n")) else ""
        own = "definition declares" in f or (re.search(r"\b%s\(" % re.escape(name), src) and not src.lstrip().startswith(("extern", "/*")))
        if m and int(m.group(2)) > int(m.group(1)) and int(m.group(2)) <= 8 and own:      # only the function's OWN definition line (the finding may be about a callee prototype on another line)
            r = re.search(r"^((?:[\w ]+\*? ?)?\b%s\()([^)]*)(\))" % re.escape(name), t, re.M) or re.search(r"^(\w[\w \*]*\n%s\()([^)]*)(\))" % re.escape(name), t, re.M)
            if r:
                ns = add(re.sub(r"\s+", " ", r.group(2)), int(m.group(1)), int(m.group(2)))
                if ns is not None: t = t[:r.start(2)] + ns + t[r.end(2):]
            continue
        m = re.search(r"\b(FUN_[0-9a-f]{8}|\w+) called with (\d+) argument\(s\), prototype has (\d+)", f)
        if m and int(m.group(2)) > int(m.group(3)) and m.group(1) != name and int(m.group(2)) <= 8:
            r = re.search(r"^((?:/\*[0-9a-f]{8}\*/ )?extern [^;\n(]*\b%s\()([^;\n)]*)(\);)" % re.escape(m.group(1)), t, re.M)
            if r:
                ns = add(r.group(2), int(m.group(3)), int(m.group(2)))
                if ns is not None: t = t[:r.start(2)] + ns + t[r.end(2):]
    if t != t0: open(path, "w").write(t)
    return t != t0
