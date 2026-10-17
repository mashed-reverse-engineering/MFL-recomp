#!/usr/bin/env python3
"""pseudofix.py: Ghidra pseudo variables (unaff_*, in_stack_*, stack0x*, extraout_*) of a decompiled body -> ordinary C, from asm evidence.
  clean(name, body)  early text stage (t2_auto.load_funcs, after gsfix.clean):
    stackparam  in_stack_000000NN / &stack0x000000NN (NN >= 4: caller frame slots) -> param_(NN/4) (missing parameters appended): thunks forwarding the caller's argument area, printf-style `&stack0x8` (va_list)
    stacklocal  in_stack_ffffffNN / &stack0xffffffNN (below the entry esp: Ghidra could not type the slot) -> one int local per offset
    falsereg    unaff_ESI/EDI/EBX/EBP passed to a callee whose Ghidra head names register parameters (reg_edi, reg_esi) the asm does not read (output/reginputs.csv): the argument is dropped
    regname     leftover `reg_e??` / `unaff_E??` variables that no call passes any more are ordinary locals: renamed pu_*
  resolve(path)      late stage on the generated file (t2_auto after t2_postfix.fix): extraout_* markers -> value of the register at the original call site (asm_sites)
"""
import csv, os, re, sys
HERE = os.path.dirname(os.path.abspath(__file__)); G = os.path.normpath(os.path.join(HERE, ".."))
sys.path.insert(0, HERE)
NAMED = os.path.join(G, "output", "named", "MFL_app_named.c")

def _split(ps):
    out, d, cur = [], 0, ""
    for c in ps:
        if c in "([": d += 1
        elif c in ")]": d -= 1
        if c == "," and d == 0: out.append(cur.strip()); cur = ""
        else: cur += c
    if cur.strip(): out.append(cur.strip())
    return [p for p in out if p != "void"]
def _head(name, body):
    """-> (start, end, [params]) of the parameter list of the definition of name in body"""
    m = re.search(r"\b%s\s*\(([^()]*)\)\s*\{" % re.escape(name), body)
    return (m.start(1), m.end(1), _split(m.group(1))) if m else None

_HEADS = {}
def heads():
    """callee name -> [param texts] from the Ghidra export (heads of every function)"""
    if not _HEADS:
        t = open(NAMED, errors="replace").read()
        for m in re.finditer(r"// ==== (\S+) @ [0-9a-f]+\n(.*?)\n\{", t, re.S):
            h = re.search(r"\b%s\s*\(([^()]*)\)\s*$" % re.escape(m.group(1)), m.group(2).strip(), re.S)
            if h: _HEADS[m.group(1)] = _split(h.group(1))
    return _HEADS
_RI = {}
def reginputs():
    """function name -> set of register inputs the asm reads (output/reginputs.csv)"""
    if not _RI:
        for r in csv.DictReader(open(os.path.join(G, "output", "reginputs.csv"))): _RI.setdefault(r["name"], set(x for x in r["reg_inputs"].split("|") if x))
    return _RI

def _calls(body):
    """-> [(name, start of '(' , [(s, e)] argument spans, index of ')')]"""
    import gsfix
    out = []
    for m in re.finditer(r"(?<![\w.>])(\w+)\s*\(", body):
        if m.group(1) in ("if", "while", "for", "switch", "return", "sizeof"): continue
        al, close = gsfix._call_args(body, m.end() - 1)
        if al: out.append((m.group(1), m.end() - 1, al, close))
    return out

def dropjunk(name, body):
    """in_stack_* / unaff_* / extraout_* arguments behind the last stack slot the callee really reads (output/orig_stackargs.txt, no tail / addr / dyn flags): Ghidra's wrong callee head swallowed the caller frame"""
    import t2_postfix as T
    S = symbols()["n"]; J = re.compile(r"(?:in_stack_\w+|unaff_\w+|extraout_\w+|&?\s*stack0x\w+)")
    for cn, op, al, close in reversed(_calls(body)):
        if cn == name or cn not in S or len(al) < 2: continue
        k = T.orig_slots(S[cn][0])
        if k is None or len(al) <= k: continue
        args = [body[s:e].strip() for s, e in al]
        j = len(args)
        while j > k and J.fullmatch(args[j - 1]): j -= 1
        if j < len(args):
            body = body[:al[j - 1][1] if j else op + 1] + body[close:]
            if len(heads().get(cn, [])) >= k: _proto(cn, heads()[cn][:k])      # the extern keeps the callee's real arity
    return body
def stackparam(name, body):
    h = _head(name, body)
    if not h: return body
    s, e, ps = h
    if any(re.search(r"double|longlong|__int64|float10|\bint64", p) for p in ps): return body
    slots = {}
    for m in re.finditer(r"\bin_stack_([0-9a-f]{8})\b|&\s*stack0x([0-9a-f]{8})\b", body):
        off = int(m.group(1) or m.group(2), 16)
        if 4 <= off < 0x1000 and off % 4 == 0: slots[off // 4] = 1
    if not slots: return body
    n = len(ps)
    if any(re.search(r"\bparam_%d\b" % k, body) for k in slots if k > n): return body
    pname = lambda k: re.findall(r"\w+", ps[k - 1])[-1] if k <= n else "param_%d" % k
    ty = {}
    for m in re.finditer(r"^  ([\w ]+?\**) ?\*?in_stack_([0-9a-f]{8});\n", body, re.M): ty[int(m.group(2), 16) // 4] = m.group(1).strip() + (" *" if "*" in m.group(0).split("in_stack")[0] else "")
    body = re.sub(r"^  [\w ]+? ?\*?in_stack_(?:[0-9a-f]{8});\n", lambda m: "" if 4 <= int(re.search(r"in_stack_([0-9a-f]{8})", m.group(0)).group(1), 16) < 0x1000 else m.group(0), body, flags=re.M)
    body = re.sub(r"\bin_stack_([0-9a-f]{8})\b", lambda m: pname(int(m.group(1), 16) // 4) if 4 <= int(m.group(1), 16) < 0x1000 and int(m.group(1), 16) % 4 == 0 else m.group(0), body)
    body = re.sub(r"&\s*stack0x([0-9a-f]{8})\b", lambda m: "&" + pname(int(m.group(1), 16) // 4) if 4 <= int(m.group(1), 16) < 0x1000 and int(m.group(1), 16) % 4 == 0 else m.group(0), body)
    k = max(slots)
    if k > n:
        h = _head(name, body); s, e, ps = h
        add = ["%s param_%d" % (ty.get(j, "undefined4"), j) for j in range(n + 1, k + 1)]
        body = body[:s] + ", ".join(ps + add) + body[e:]
    return body

def stacklocal(name, body):
    offs = {}
    for m in re.finditer(r"\bin_stack_([f]{4,8}[0-9a-f]+)\b|&\s*\(?(?:[\w ]+ ?\*\))?\s*stack0x(f{4,8}[0-9a-f]+)\b|&\s*stack0x(0{8})\b", body):
        pass
    for m in re.finditer(r"\bin_stack_(f[0-9a-f]{7})\b|\bstack0x(f[0-9a-f]{7})\b|&\s*stack0x(00000000)\b", body): offs[int(m.group(1) or m.group(2) or m.group(3), 16) - ((1 << 32) if m.group(3) is None else 0)] = 1
    if not offs: return body
    m0 = re.search(r"\n\{\n", body)
    if not m0: return body
    ty = {}
    for m in re.finditer(r"^  ([\w ]+?) \*?in_stack_(f[0-9a-f]{7});\n", body, re.M): ty[int(m.group(2), 16) - (1 << 32)] = m.group(1)
    body = re.sub(r"^  [\w ]+? \*?in_stack_f[0-9a-f]{7};\n", "", body, flags=re.M)
    nm = lambda o: "pu_stk%x" % -o if o else "pu_stk0"
    body = re.sub(r"\bin_stack_(f[0-9a-f]{7})\b", lambda m: nm(int(m.group(1), 16) - (1 << 32)), body)
    body = re.sub(r"&\s*stack0x(f[0-9a-f]{7})\b", lambda m: "&" + nm(int(m.group(1), 16) - (1 << 32)), body)
    body = re.sub(r"&\s*stack0x00000000\b", "&pu_stk0", body)
    decl = "".join("  %s %s;\n" % (ty.get(o, "undefined4"), nm(o)) for o in sorted(offs))
    p = body.find("\n{\n") + 3
    return body[:p] + decl + body[p:]

def _proto(cn, ps):
    """the callee's real prototype (its Ghidra head minus the false register parameters) -> this run's overlay (MFL_callee_protos_auto.txt: replaces the declaration of that address)"""
    ov = os.environ.get("T2_OVERLAY_DIR"); va = symbols()["n"].get(cn)
    if not ov or not va: return
    f = os.path.join(ov, "MFL_callee_protos_auto.txt"); line = "/*%s*/ extern int %s(%s);" % (va[0], cn, ", ".join(ps) or "void")
    old = open(f).read() if os.path.exists(f) else ""
    if line not in old: open(f, "a").write(("" if old.endswith("\n") or not old else "\n") + line + "\n")
def falsereg(name, body):
    H = heads(); RI = reginputs(); ch = False
    for cn, op, al, close in reversed(_calls(body)):
        if cn == name or cn not in H: continue
        gr = [re.findall(r"reg_(e\w\w)", p) for p in H[cn][:6]]
        lead = 0
        while lead < len(H[cn]) and re.search(r"\breg_e\w\w$", H[cn][lead]): lead += 1
        if not lead: continue
        truth = RI.get(cn)
        if truth is None or truth: continue      # asm reads registers there: argsite / F_r adapters, not dropped
        args = [body[s:e].strip() for s, e in al]
        if len(args) < lead: continue
        if not all(re.fullmatch(r"[\w.]+", a) for a in args[:lead]): continue      # plain values only (a call among them stays: side effects)
        _proto(cn, H[cn][lead:])
        st = al[0][0]; en = al[lead][0] if lead < len(al) else close
        body = body[:st] + body[en:] if lead < len(al) else body[:st] + body[close:]; ch = True
    return body

def regname(name, body):
    for v in sorted(set(re.findall(r"\b(reg_e\w\w|unaff_E\w\w)\b", body))):
        uses = [m for m in re.finditer(r"\b%s\b" % v, body)]
        if any(re.search(r"[(,]\s*$", body[max(0, m.start() - 3):m.start()]) or re.match(r"\s*[,)]", body[m.end():m.end() + 3]) for m in uses): continue
        body = re.sub(r"\b%s\b" % v, "pu_" + v.split("_")[1].lower(), body)
    return body


# ---------------------------------------------------------------------------------------------------------------- extraout_ECX / EDX
_SYM = {}
def symbols():
    """name -> [va hex], va hex -> name (src/symbols.txt)"""
    if not _SYM:
        byn, byv = {}, {}
        for l in open(os.path.join(G, "src", "symbols.txt")):
            q = l.split()
            if len(q) == 2: byn.setdefault(q[0], []).append(q[1].zfill(8)); byv.setdefault(q[1].zfill(8), q[0])
        _SYM.update(n=byn, v=byv)
    return _SYM
_WR = {}
def may_write(va, reg, depth=0):
    """True when the function at va (hex) may write reg (ecx / edx): any write, an indirect call or a callee that writes it (direct calls followed to depth 4)"""
    k = (va, reg)
    if k in _WR: return _WR[k]
    _WR[k] = True      # recursion guard
    import subprocess, reginputs2 as R, t2_postfix as T
    T.orig_const_args("x", set())
    fn = {v: n for n, v in ((n, "%08x" % lo) for n, (lo, sz) in T._FN.items())}.get(va)
    if not fn or depth > 4: return True
    lo, sz = T._FN[fn]
    o = subprocess.run(["objdump", "-w", "-M", "intel", "-D", "--no-show-raw-insn", "--start-address=0x%x" % lo, "--stop-address=0x%x" % (lo + sz), R.EXE], capture_output=True, text=True).stdout
    ins, order = R.parse_objdump(o); res = False
    for a in order:
        op, ar = ins[a]
        rd, wr, kind = R.effects(op, ar)
        if kind == "call":
            m = re.fullmatch(r"0x([0-9a-f]+)", ar)
            if not m or may_write("%08x" % int(m.group(1), 16), reg, depth + 1): res = True; break
        elif kind == "jmp":
            m = re.fullmatch(r"0x([0-9a-f]+)", ar)
            if m and not (lo <= int(m.group(1), 16) < lo + sz) and may_write("%08x" % int(m.group(1), 16), reg, depth + 1): res = True; break
            if not m: res = True; break
        elif reg in wr or (op.startswith(("cmov", "set")) and R.split_ops(ar) and R.FULL.get(R.split_ops(ar)[0]) == reg) or (op in ("xchg",) and reg in rd): res = True; break
    _WR[k] = res
    return res

def _after_decls(body):
    """offset of the first statement of the function body (behind the declaration block, which Ghidra ends with a blank / two space line)"""
    p = body.find("\n{\n") + 3
    m = re.compile(r"^  ?\n", re.M).search(body, p)
    return m.end() if m else p
def extraout(name, body):
    """extraout_ECX / EDX of a callee that never touches the register: the value the register held at the original call (asm_sites), snapshotted into a local before the first candidate call"""
    xs = {}
    for m in re.finditer(r"\bextraout_(ECX|EDX)(?:_\d+)?\b", body):
        isdecl = body[m.end():m.end() + 1] == ";" and re.fullmatch(r"  [\w ]+ \*?", body[body.rfind("\n", 0, m.start()) + 1:m.start()])
        if m.start() > body.find("\n{\n") and not isdecl: xs.setdefault(m.group(0), (m.start(), m.group(1).lower()))
    if not xs: return body
    import t2_postfix as T
    S = symbols()
    bs = body.find("\n{\n")
    calls = [c for c in _calls(body) if c[1] > bs and c[0] in S["n"] and c[0] != name]
    # call statement = begins its line (optionally `x = `)
    def stmt_start(pos):
        ls = body.rfind("\n", 0, pos) + 1
        return ls if re.fullmatch(r"\s*(?:[\w.\[\]>*-]+ = (?:\([\w \*]+\))?)?", body[ls:pos]) else None
    vas = {c[0]: S["n"][c[0]][0] for c in calls}
    T.orig_const_args(name, set())
    if name not in T._FN: return body
    sites = T.asm_sites(name, set(vas.values()))
    if not sites: return body
    edits = []; decls = []; used = {}
    for var, (pos, reg) in sorted(xs.items(), key=lambda kv: kv[1][0]):
        cand = [c for c in calls if c[1] < pos and stmt_start(body.rfind(c[0], 0, c[1] + 1)) is not None and not may_write(vas[c[0]], reg)]
        if not cand: continue
        expr = first = None
        H = heads()
        for c in sorted(cand, key=lambda c: -c[1]):      # the register was an argument of the nearest candidate call (a register callee that keeps its input: loop pointers in ecx)
            ps = H.get(c[0], [])
            j = next((i for i, p_ in enumerate(ps) if re.search(r"\breg_%s$" % reg, p_)), None)
            if j is not None and j < len(c[2]): expr = body[c[2][j][0]:c[2][j][1]].strip(); first = c; break
        if expr is not None:
            ln = "pu_%s_%d" % (reg, len(used)); used[var] = ln
            ss_ = stmt_start(body.rfind(first[0], 0, first[1] + 1))
            edits.append((ss_, ss_, "%s%s = %s;\n" % (re.match(r"\s*", body[ss_:]).group(0), ln, expr if re.fullmatch(r"-?(?:0x[0-9a-f]+|\d+)", expr) else "(int)(%s)" % expr))); decls.append("  int %s;\n" % ln)
            continue
        T._XP.update(tail=False, own=0, max=0, ret=None)
        cn = {}
        for c in cand:
            for s in sites.get(vas[c[0]], []): cn.setdefault(c[0], []).append(s["regs"].get(reg))
        flat = {x for v in cn.values() for x in v}
        glob = {a: n for a, n in S["v"].items()}
        expr = None; first = None
        if len(flat) == 1 and None not in flat and not next(iter(flat)).startswith(("R:", "?", "ret:")):
            expr = T._cname(next(iter(flat)), body, glob, {}, name)
            first = "top" if expr else None      # value of the register before the first call: snapshot at the top of the function (the candidate calls may sit in branches)
        if expr is None:      # the register was pushed as an argument of the nearest candidate: its C argument text (loop counters)
            c = max(cand, key=lambda c: c[1]); ss = sites.get(vas[c[0]], []); same = [x for x in calls if x[0] == c[0]]
            if len(ss) == len(same) and not any(s.get("al") for s in ss):
                j = [k for k, x in enumerate(ss[same.index(c)]["stack"]) if x == "R:" + reg]
                if len(j) == 1 and j[0] < len(c[2]): expr = body[c[2][j[0]][0]:c[2][j[0]][1]].strip(); first = c
        if expr is None: continue
        ln = "pu_%s_%d" % (reg, len(used)); used[var] = ln
        ss_ = _after_decls(body) if first == "top" else stmt_start(body.rfind(first[0], 0, first[1] + 1))
        edits.append((ss_, ss_, "%s%s = %s;\n" % ("  " if first == "top" else re.match(r"\s*", body[ss_:]).group(0), ln, expr if re.fullmatch(r"-?(?:0x[0-9a-f]+|\d+)", expr) else "(int)(%s)" % expr))); decls.append("  int %s;\n" % ln)
    if not used: return body
    for s, e, x in sorted(edits, reverse=True): body = body[:s] + x + body[e:]
    for var, ln in used.items(): body = re.sub(r"^  [\w ]+ \*?%s;\n" % var, "", body, flags=re.M)
    body = re.sub(r"\b(%s)\b" % "|".join(map(re.escape, used)), lambda m: used[m.group(1)], body)
    p = body.find("\n{\n") + 3
    return body[:p] + "".join(decls) + body[p:]

def unused_decls(body):
    return re.sub(r"^  [\w ]+? \**((?:pu_|unaff_|extraout_|in_stack_)\w+);\n", lambda m: "" if len(re.findall(r"\b%s\b" % m.group(1), body)) == 1 else m.group(0), body, flags=re.M)
def clean(name, body):
    if "extraout_" not in body and "unaff_E" not in body and "in_stack_" not in body and "stack0x" not in body and "reg_e" not in body: return body
    try:
        b = unused_decls(dropjunk(name, body)); b = extraout(name, b); b = falsereg(name, b); b = stackparam(name, b); b = stacklocal(name, b); b = regname(name, b); b = unused_decls(b)
    except Exception as ex:
        print("pseudofix %s: %s" % (name, ex)); return body
    return b
if __name__ == "__main__":
    t = open(NAMED, errors="replace").read()
    for n in sys.argv[1:]:
        m = re.search(r"// ==== %s @ [0-9a-f]+\n(.*?)(?=\n// ==== |\Z)" % re.escape(n), t, re.S)
        print(clean(n, m.group(1)))
