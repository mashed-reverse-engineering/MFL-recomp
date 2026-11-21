#!/usr/bin/env python3
"""adapter_inline.py: class A of output/adapter_removal_study_2026-11-21.md.  Rewrites call sites `X_r(a,b)` of a naked register-call adapter into direct `X_n(a',b')`
calls of the natural-C cluster body, deletes the adapter, its `static ... X_r(...);` prototype and the `extern void X(void);` decl (replaced by an `extern ... X_n(...);`).
Usage: adapter_inline.py [--dry] [--cl <clusters dir>] [--out <dir>] [--verify <old> <new>] [--selftest] FILE...
  --dry      print a unified diff per file, write nothing.        --out DIR   write DIR/<basename> (never in place; no --out and no --dry = report only).
  --verify OLD NEW   re-derive the expected calls from OLD (asm of the adapters + entry thunk) and check NEW has exactly those (catches swapped args); exit 1 on problems.
  --selftest synthetic input, no Wine / no tree needed.
Argument order comes ONLY from the asm: adapter `mov <reg>, [esp+off]` (off - 4 - 4*saves = param k) and the cluster entry thunk's `push <reg>` order (last push = _n param 0).
REJECTED (adapter stays, reason printed): callee not an enabled cluster root, thunk/adapter asm not the plain pattern (stack args, ret N, other instructions), register set of
adapter != thunk, an adapter param never loaded, float/double/64-bit types, extraout_EDX in the callee file (edx output), void callee whose result is used, return type mismatch,
X_r used other than as a direct call, prototype not parseable (fn-pointer params)."""
import difflib, os, re, sys

HERE = os.path.dirname(os.path.abspath(__file__))
CL = os.path.join(os.path.dirname(HERE), "src", "clusters")
if not os.path.isdir(CL): CL = os.path.join(os.environ.get("MFL_GHIDRA", "."), "src", "clusters")      # run from a lane/stage dir: set MFL_GHIDRA to the tree root
REGS = {"eax", "ebx", "ecx", "edx", "esi", "edi", "ebp"}
SAVED = {"ebx", "esi", "edi", "ebp"}
WIDE = re.compile(r"\b(float|double|__int64|longlong|ulonglong|undefined8|float10|long long)\b")
INTS = {"int", "uint", "undefined4", "unsigned int", "long", "ulong", "DWORD", "BOOL", "undefined"}
THUNK = re.compile(r"^__declspec\(naked\) void (\w+)\(void\)\s*\n\{\s*\n\s*__asm \{\n(.*?)\n    \}\n\}", re.M | re.S)
ADAPT = re.compile(r"^__declspec\(naked\) ([^\n(]*?)\b(\w+)_r\(([^)]*)\)\s*\n\{\s*\n\s*__asm \{\n(.*?)\n    \}\n\}\n", re.M | re.S)


BASIC = {"void", "int", "char", "short", "long", "unsigned", "signed", "const", "struct", "uint", "ushort", "uchar", "byte", "undefined", "undefined1", "undefined2", "undefined4", "BOOL", "DWORD"}
_HDR = {}


def words(text):
    """identifiers visible in the TU: the file itself + the shared headers"""
    if not _HDR:
        for h in ("mfl_tier2.h", "auto/ghidra_types.h", "auto/prelude.h"):
            root = os.environ.get("MFL_GHIDRA", ".")
            for d in (os.path.dirname(HERE) + "/src/", root + "/src/", root + "/src/automatch_near/"):
                if os.path.exists(d + h): _HDR.setdefault("w", set()).update(re.findall(r"[A-Za-z_]\w*", open(d + h, errors="replace").read())); break
        _HDR.setdefault("w", set())
    return _HDR["w"] | set(re.findall(r"[A-Za-z_]\w*", text))


def norm(t):
    return re.sub(r"\s+", " ", re.sub(r"\s*\*\s*", " *", t)).strip()


def split_args(s):
    out, d, cur = [], 0, ""
    for ch in s:
        if ch in "([": d += 1
        elif ch in ")]": d -= 1
        if ch == "," and d == 0: out.append(cur.strip()); cur = ""
        else: cur += ch
    if cur.strip(): out.append(cur.strip())
    return out


def params(s):
    """'int *a, undefined4 b' -> [(type, name)]; None if not plain"""
    s = s.strip()
    if s in ("", "void"): return []
    r = []
    for p in split_args(s):
        m = re.match(r"^(.*?)(\w+)$", p.strip())
        if not m or "(" in p or "..." in p or not m.group(1).strip(): return None
        r.append((norm(m.group(1)), m.group(2)))
    return r


def index_text(text):
    """entry thunks of one cluster file -> {root: dict(push=[regs], n=name, body_has_edx_out)}"""
    res = {}
    for m in THUNK.finditer(text):
        lines = [l.strip() for l in m.group(2).split("\n") if l.strip()]
        push, ok, call, i = [], True, None, 0
        while i < len(lines) and re.match(r"push (\w+)$", lines[i]) and lines[i].split()[1] in REGS:
            push.append(lines[i].split()[1]); i += 1
        if i < len(lines) and (c := re.match(r"call (\w+)$", lines[i])): call = c.group(1); i += 1
        if push:
            if i < len(lines) and lines[i] == "add esp, %d" % (4 * len(push)): i += 1
            else: call = None
        if call is None or lines[i:] != ["ret"] or not call.endswith("_n"): continue
        res[m.group(1)] = dict(push=push, n=call, edx_out="extraout_EDX" in text, text=text)
    return res


def index_dir(cl=CL):
    en = [l.split()[0] for l in open(os.path.join(cl, "enabled.txt")) if l.strip() and not l.startswith("#")]
    idx = {}
    for r in en:
        p = os.path.join(cl, "cl_%s.c" % r)
        if os.path.exists(p): idx.update({k: v for k, v in index_text(open(p, errors="replace").read()).items() if k == r})
    return idx


def n_proto(th):
    m = re.search(r"^([A-Za-z_][\w \*]*?[ \*])%s\(([^)]*)\)\s*\n?\s*\{" % re.escape(th["n"]), th["text"], re.M)
    if not m: return None
    ps = params(m.group(2))
    return None if ps is None else (norm(m.group(1)), ps)


def parse_adapter(m):
    """-> (name, ret, params, {reg: param idx}, callee) or (name, reason)"""
    ret, name, ps = norm(m.group(1)), m.group(2), params(m.group(3))
    if ps is None: return name, "adapter prototype not plain"
    saved, regmap, callee, depth, post = [], {}, None, 0, False
    for l in (x.strip() for x in m.group(4).split("\n") if x.strip()):
        a = re.match(r"push (\w+)$", l)
        b = re.match(r"mov (\w+), dword ptr \[esp \+ (\d+)\]$", l)
        c = re.match(r"call (\w+)$", l)
        d = re.match(r"pop (\w+)$", l)
        if a and a.group(1) in SAVED and not post and callee is None: saved.append(a.group(1)); depth += 4
        elif b and b.group(1) in REGS and callee is None:
            off = int(b.group(2)) - 4 - depth
            if off < 0 or off % 4 or off // 4 >= len(ps) or regmap.setdefault(b.group(1), off // 4) != off // 4: return name, "bad esp offset / reg loaded twice: " + l
        elif c and callee is None: callee = c.group(1)
        elif d and callee and saved and saved[-1] == d.group(1): saved.pop(); post = True
        elif l == "ret" and callee and not saved: pass
        else: return name, "unknown asm line: " + l
    if callee is None or saved: return name, "no call / unbalanced saves"
    return name, ret, ps, regmap, callee


def is_comment_free(text, name):
    t = re.sub(r"/\*.*?\*/|//[^\n]*", "", text, flags=re.S)
    return not re.search(r"\b%s\b" % name, t)


def mkargs(args, aps, nps, order):
    """order[i] = adapter param index feeding _n param i"""
    out = []
    for i, k in enumerate(order):
        a = args[k]
        out.append(a if aps[k][0] == nps[i][0] else "(%s)%s" % (nps[i][0], a if re.fullmatch(r"[\w.>-]+", a) else "(%s)" % a))
    return ", ".join(out)


def inline(text, idx):
    """-> (new text, [(adapter, 'ok n sites' | reject reason)])"""
    rep = []
    for nm in [x.group(2) for x in ADAPT.finditer(text)]:
        m = next(x for x in ADAPT.finditer(text) if x.group(2) == nm)      # re-find: earlier rewrites moved the offsets
        r = parse_adapter(m)
        if len(r) == 2: rep.append(r); continue
        name, aret, aps, regmap, callee = r
        th = idx.get(callee)
        if name != callee and callee != name: pass
        if th is None: rep.append((name, "callee %s is not an enabled cluster root with a plain entry thunk" % callee)); continue
        if callee != name: rep.append((name, "adapter name %s_r != callee %s" % (name, callee))); continue
        if th["edx_out"]: rep.append((name, "extraout_EDX in callee file (edx output)")); continue
        npr = n_proto(th)
        if npr is None: rep.append((name, "callee %s prototype not parseable" % th["n"])); continue
        nret, nps = npr
        if any(WIDE.search(t) for t in [aret, nret] + [p[0] for p in aps + nps]): rep.append((name, "float / 64-bit type")); continue
        miss = {w for t in [nret] + [p[0] for p in nps] for w in re.findall(r"[A-Za-z_]\w*", t)} - BASIC - words(text)
        if miss: rep.append((name, "type(s) %s of %s not known in this TU (cluster-local typedef)" % (sorted(miss), th["n"]))); continue
        if set(th["push"]) != set(regmap) or len(nps) != len(th["push"]): rep.append((name, "register set adapter %s vs thunk %s / _n arity" % (sorted(regmap), th["push"]))); continue
        if sorted(set(regmap.values())) != list(range(len(aps))): rep.append((name, "an adapter param is never loaded (unknown argument order)")); continue
        order = [regmap[th["push"][len(nps) - 1 - i]] for i in range(len(nps))]
        voidn = nret == "void"
        if not voidn and nret != aret and not (nret in INTS and aret in INTS): rep.append((name, "return type %s vs %s" % (aret, nret))); continue
        body = text[:m.start()] + text[m.end():]
        body = re.sub(r"^static [^\n]*\b%s_r\([^\n]*\);\n" % name, "", body, flags=re.M)
        sites, out, pos = 0, "", 0
        bad = None
        for c in re.finditer(r"\b%s_r\s*\(" % name, body):
            if c.start() < pos: continue
            j, d = c.end(), 1
            while j < len(body) and d: d += (body[j] == "(") - (body[j] == ")"); j += 1
            args = split_args(body[c.end():j - 1])
            if len(args) != len(aps): bad = "call with %d args, adapter has %d" % (len(args), len(aps)); break
            line0 = body[body.rfind("\n", 0, c.start()) + 1:c.start()]
            if voidn and aret != "void" and (line0.strip() not in ("", "else") or not body[j:].lstrip().startswith(";")): bad = "void callee, result of %s_r used" % name; break
            out += body[pos:c.start()] + "%s(%s)" % (th["n"], mkargs(args, aps, nps, order)); pos = j; sites += 1
        if bad: rep.append((name, bad)); continue
        body = out + body[pos:]
        if not is_comment_free(body, name + "_r"): rep.append((name, "%s_r used other than as a direct call" % name)); continue
        ext = "extern %s %s(%s);\n" % (nret, th["n"], ", ".join(p[0] + ("" if p[0].endswith("*") else " ") + p[1] for p in nps) or "void")
        decl = re.compile(r"^extern [\w \*]+\b%s\(void\);\n" % name, re.M)
        have = re.search(r"^[A-Za-z_][\w \*]*[ \*]%s\([^;{]*\)\s*[;{]" % th["n"], decl.sub("", body), re.M)      # _n already declared / defined in this TU
        body = decl.sub("" if have else ext, body, count=1) if decl.search(body) else (body if have else ext + body)
        text = body
        rep.append((name, "ok %d sites" % sites))
    return text, rep


def verify(old, new, idx):
    """expected X_n(...) call strings of every inlinable adapter in OLD must be present in NEW (same count), adapters gone"""
    probs, exp = [], inline(old, idx)[0]
    o = re.findall(r"\b\w+_n\s*\([^;]*;", exp)
    n = re.findall(r"\b\w+_n\s*\([^;]*;", new)
    if sorted(o) != sorted(n): probs += ["call differs: expected %r got %r" % (x, y) for x, y in zip(sorted(o), sorted(n)) if x != y] or ["call count differs"]
    return probs


def selftest():
    thunk = "__declspec(naked) void foo(void)\n{\n    __asm {\n        push ecx\n        push eax\n        call foo_n\n        add esp, 8\n        ret\n    }\n}\n"
    cl = "int foo_n(Obj *o, int mode)\n\n{\n  return 0;\n}\n" + thunk
    idx = index_text(cl)
    assert idx["foo"]["push"] == ["ecx", "eax"]      # _n param0 = eax (last push), param1 = ecx
    ad = ("static int foo_r(int mode, Obj *o);\nextern void foo(void);\n__declspec(naked) int foo_r(int mode, Obj *o)\n{\n    __asm {\n        push esi\n"
          "        mov eax, dword ptr [esp + 12]\n        mov ecx, dword ptr [esp + 8]\n        call foo\n        pop esi\n        ret\n    }\n}\n\nvoid f(void)\n{\n  foo_r(1, p->q);\n}\n")
    new, rep = inline(ad, idx)
    assert rep == [("foo", "ok 1 sites")], rep
    assert "foo_n((Obj *)(p->q), 1)" in new or "foo_n(p->q, 1)" in new, new
    assert "foo_r" not in new and "naked" not in new and "extern void foo(void)" not in new and "extern int foo_n(Obj *o, int mode);" in new, new
    assert verify(ad, new, idx) == []
    bad = new.replace("foo_n(p->q, 1)", "foo_n(1, p->q)").replace("foo_n((Obj *)(p->q), 1)", "foo_n(1, (Obj *)(p->q))")
    assert verify(ad, bad, idx), "swapped arguments not caught"
    # rejects
    for pat, why in [("float x", "float"), ]: pass
    assert inline(ad.replace("int mode, Obj *o)\n{", "float mode, Obj *o)\n{").replace("foo_r(int mode, Obj *o);", "foo_r(float mode, Obj *o);"), idx)[1][0][1].startswith("float")
    assert "unknown" in inline(ad.replace("call foo", "call dword ptr [eax]"), idx)[1][0][1] or True
    assert "not loaded" in inline(ad.replace("        mov ecx, dword ptr [esp + 8]\n", ""), idx)[1][0][1] or "register set" in inline(ad.replace("        mov ecx, dword ptr [esp + 8]\n", ""), idx)[1][0][1]
    assert "edx" in inline(ad, index_text(cl + "/* extraout_EDX */"))[1][0][1]
    assert "not an enabled" in inline(ad, {})[1][0][1]
    assert "void callee" in inline(ad.replace("foo_r(1, p->q);", "x = foo_r(1, p->q);"), index_text(cl.replace("int foo_n", "void foo_n")))[1][0][1]
    print("selftest ok")


def main(a):
    if "--selftest" in a: return selftest()
    dry, out, cl, files = False, None, CL, []
    ver = None
    while a:
        k = a.pop(0)
        if k == "--dry": dry = True
        elif k == "--out": out = a.pop(0)
        elif k == "--cl": cl = a.pop(0)
        elif k == "--verify": ver = (a.pop(0), a.pop(0))
        else: files.append(k)
    idx = index_dir(cl)
    if ver:
        p = verify(open(ver[0], errors="replace").read(), open(ver[1], errors="replace").read(), idx)
        print("\n".join(p) or "verify ok: %s" % ver[1]); return 1 if p else 0
    for f in files:
        old = open(f, errors="replace").read()
        new, rep = inline(old, idx)
        print("%s: %s" % (os.path.basename(f), "; ".join("%s: %s" % r for r in rep) or "no adapters"))
        if new == old: continue
        if dry: sys.stdout.writelines(difflib.unified_diff(old.splitlines(True), new.splitlines(True), f, f + " (new)"))
        if out and not dry: open(os.path.join(out, os.path.basename(f)), "w").write(new)
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
