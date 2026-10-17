"""strictfix.py: make a candidate clean for the strict build (cl warnings C4047 / C4022 / C4024 / C4133 are errors there).
The warnings come from Ghidra's untyped values (`undefined4 param_1`, `undefined1 auStack[1024]`) passed to pointer / int parameters of the callee prototypes of the same file (the F_r register adapters, SDK
prototypes).  Casts to the parameter type change no code.  fix(path, warn_lines) -> number of casts; warn_lines = {line number: [warning text]} from cl.
An int parameter of a static `_r` adapter that receives an array / address becomes `void *` (naked asm: the type is only for the caller), instead of an `(int)` pointer cast."""
import re
WARN = re.compile(r"^f\.c\((\d+)\) : warning (C4047|C4022|C4024|C4133)\b(.*)$")
def parse(out):
    d = {}
    for l in out.splitlines():
        m = WARN.match(l.strip())
        if m: d.setdefault(int(m.group(1)), []).append(m.group(2) + m.group(3))
    return d
def _args(t, i):
    dep = 0; st = i + 1; out = []
    for k in range(i, len(t)):
        c = t[k]
        if c in "([{": dep += 1
        elif c in ")]}":
            dep -= 1
            if dep == 0: out.append((st, k)); return out, k
        elif c == "," and dep == 1: out.append((st, k)); st = k + 1
    return None, -1
def protos(t):
    """name -> [(param type, param text)] of the `extern`/`static` prototypes of the file"""
    out = {}; t = re.sub(r"__declspec\((?:dllimport|dllexport)\) ?", "", t)      # `extern __declspec(dllimport) int __stdcall F(int);`: the parenthesis hid the prototype
    for m in re.finditer(r"^(?:/\*[0-9a-f]{8}\*/ )?(?:extern|static) [^;\n(]*?\b(\w+)\(([^;\n]*)\);", t, re.M):
        ps = [p.strip() for p in m.group(2).split(",")] if m.group(2).strip() not in ("", "void") else []
        out[m.group(1)] = [(re.sub(r"\s*\b\w+$", "", p).strip() if re.search(r"[\s*]\w+$", p) else p, p) for p in ps]
    return out
def fix(path, warns):
    t = open(path, errors="replace").read(); pr = protos(t); n = 0
    if not warns: return 0
    starts = [0]
    for m in re.finditer(r"\n", t): starts.append(m.end())
    edits = []; retype = set()
    for ln, ws in warns.items():
        if ln - 1 >= len(starts): continue
        lo = starts[max(ln - 3, 0)]; hi = starts[min(ln + 2, len(starts) - 1)]
        ptr_to_int = any(re.search(r"'(?:int|uint|undefined4|undefined)' differs in levels of indirection from '[^']*(?:\[|\*)", w) for w in ws)
        for c in re.finditer(r"(?<![\w.>])(\w+)\s*\(", t[lo:hi]):
            nm = c.group(1)
            if nm not in pr: continue
            ls = t.rfind("\n", 0, lo + c.start()) + 1
            if t[ls:lo + c.start()].lstrip().startswith(("extern", "static", "/*", "__declspec")): continue
            al, close = _args(t, lo + c.end() - 1)
            if not al or len(al) != len(pr[nm]): continue
            if not (t.count("\n", 0, lo + c.start()) + 1 <= ln <= t.count("\n", 0, close) + 1): continue
            for (s, e), (ty, ptext) in zip(al, pr[nm]):
                a = t[s:e].strip()
                if not a or re.fullmatch(r"-?(?:0x[0-9a-f]+|\d+)[uUlL]*|\(void \*\)0|NULL", a): continue
                if ty in ("int", "uint", "undefined4") and re.fullmatch(r"\((?:HWND|HANDLE|HINSTANCE|LPVOID|LPCSTR|\w+ \*)\)(?:0x0|0)", a):      # a null pointer cast passed to an int parameter: plain 0
                    edits.append((s, e, " " * (len(t[s:e]) - len(t[s:e].lstrip())) + "0")); continue
                if re.match(r"\(\s*%s\s*\)" % re.escape(ty), a) or ty in ("float", "double", "RwReal"): continue
                if re.fullmatch(r"\w+", a) and any("from 'int (__cdecl *)()'" in w_ for w_ in ws) and re.fullmatch(r"Rp\w+CallBack|Rw\w+CallBack|\w+Callback|\w+CB", ty): new = "(%s)%s" % (ty, a)      # a K&R function passed as a callback typedef
                elif "*" in ty and not ty.startswith("const char"): new = "(%s)%s" % (ty, a if re.fullmatch(r"[\w.>\[\]&-]+|\*?\w+", a) else "(" + a + ")")
                elif ptr_to_int and ty in ("int", "uint", "undefined4") and ((re.fullmatch(r"&?[\w.>\[\]-]+", a) and not re.fullmatch(r"\w+\[[^\]]*\]", a) and ("&" in a or "." in a or "frame" in a or a.startswith(("auStack", "local_")) or re.search(r"^  [\w ]+ \*\s*%s;|^(?:/\*\w+\*/ )?extern [\w ]+ \*\s*%s;|[(,]\s*[\w ]+ \*\s*%s\s*[,)]" % ((re.escape(a),) * 3), t, re.M))) or re.fullmatch(r"&\w+(?: [-+*] (?:\w+|0x[0-9a-f]+))+", a)):
                    if " " in a: a = "(" + a + ")"      # `&g + i * 0x114`: the cast covers the whole address expression
                    if nm.endswith("_r") and re.search(r"^static [^;\n]*\b%s\(" % nm, t, re.M):
                        retype.add((nm, ptext)); new = "(void *)" + a
                    else: new = "(%s)%s" % (ty, a)
                else: continue
                edits.append((s, e, " " * (len(t[s:e]) - len(t[s:e].lstrip())) + new))
    lines = t.split("\n"); asg = 0
    for ln, ws in warns.items():      # `p = expr;` with a pointer of another type: C4133 '=' : incompatible types - from 'A' to 'B' / C4047 '=' : 'B' differs in levels of indirection from 'A'
        for w in ws:
            if re.search(r"'(?:!=|==)' : '(?:undefined4|uint|int|undefined)' differs in levels of indirection from '[^']*\*'", w) and ln - 1 < len(lines):      # integer object compared with a null pointer cast: plain 0
                l2 = re.sub(r"(?<=[=!]= )\(\w[\w ]*\*\)(?:0x0|0)\b", "0", lines[ln - 1])
                if l2 != lines[ln - 1]: lines[ln - 1] = l2; asg += 1
                continue
            mi = re.search(r"'=' : '(undefined4|uint|int|undefined)' differs in levels of indirection from '[^']*\*'", w)
            if mi and ln - 1 < len(lines):      # integer object = pointer expression: the object keeps its (Ghidra untyped) type, the value is cast
                r = re.match(r"^(\s*)([\w.>\[\]*()-]+) = (\([^()]*\*\)[^;]*);$", lines[ln - 1])
                if r: lines[ln - 1] = "%s%s = (%s)%s;" % (r.group(1), r.group(2), mi.group(1), "(" + r.group(3) + ")" if not re.fullmatch(r"\([^()]*\)[\w.>\[\]&-]+", r.group(3)) else r.group(3)); asg += 1
                continue
            m = re.search(r"'=' : incompatible types - from '[^']*' to '([^']*)'", w) or re.search(r"'=' : '([^']*)' differs in levels of indirection from", w)
            if not m or ln - 1 >= len(lines): continue
            ty = m.group(1).strip()
            if "*" not in ty: continue
            r = re.match(r"^(\s*)([\w.>\[\]*()-]+) = (?!\()([^;]*);$", lines[ln - 1]) or re.match(r"^(\s*)([\w.>\[\]*()-]+) = (\([^()]*\)[^;]*);$", lines[ln - 1])
            if r and not re.match(r"^\(\s*%s\s*\)" % re.escape(ty), r.group(3)): lines[ln - 1] = "%s%s = (%s)%s;" % (r.group(1), r.group(2), ty, r.group(3) if re.fullmatch(r"[\w.>\[\]&-]+", r.group(3)) else "(" + r.group(3) + ")"); asg += 1
    if asg: t = "\n".join(lines); n += asg; starts = [0] + [m.end() for m in re.finditer(r"\n", t)]; edits = []
    seen = set(); es = []
    for s, e, new in sorted(edits, reverse=True):
        if s in seen: continue
        seen.add(s); es.append((s, e, new))
    for s, e, new in es: t = t[:s] + new + t[e:]; n += 1
    for nm, ptext in retype:      # the adapter's own prototype + definition: `int reg_edi` -> `void *reg_edi`
        pn = ptext.split()[-1]
        t = re.sub(r"(?m)^((?:static|__declspec\(naked\))[^;\n]*\b%s\([^)\n]*?)%s(?=[,)])" % (re.escape(nm), re.escape(ptext)), lambda m: m.group(1) + "void *" + pn, t)
    if n or retype: open(path, "w").write(t)
    return n
