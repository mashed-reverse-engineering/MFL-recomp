#!/usr/bin/env python3
"""reginputs2.py: register inputs of EVERY code function of MFL.exe with real bounds (output/delink_sdk/ownership_items.csv, kind=code: lo..hi) and liveness.
Per function: objdump of exactly lo..hi, CFG from the entry (conditional / unconditional jumps, jump tables read from the exe, a jump out of the range = tail call,
ret = exit), forward must-written dataflow: a register (eax ecx edx ebx esi edi, 8/16 bit names map to the full register) is an INPUT if some path from the entry reads it before any
write.  call = reads the callee's inputs (fixpoint over the callee table, direct calls / tail jmps), then writes eax/ecx/edx; xor/sub/sbb r,r = write; push of an
unwritten callee saved register that is popped in the function = save, not a read; a lone `push eax/ecx/edx` in the first 3 instructions = stack reservation.  x87: inputs = st(i) read below
the entry depth (a stack underflow along any path), ST0 return = values left on the x87 stack at a ret.  Return registers: eax / edx written on every path to a ret.
Usage: reginputs2.py [out.csv=output/reginputs2.csv] [--nopropagate]   -> name,addr,reg_inputs,ret_n,x87_in,ret_regs,size,tail   (stdlib + objdump)"""
import csv, os, re, struct, subprocess, sys
HERE = os.path.dirname(os.path.abspath(__file__)); G = os.path.join(HERE, "..")
EXE = os.path.join(G, "..", "MashedGame", "FullyLoaded", "installed", "App_Executables", "MFL.exe")
OWN = os.path.join(G, "output", "delink_sdk", "ownership_items.csv")
R6 = ("eax", "ecx", "edx", "ebx", "esi", "edi")
FULL = {"eax": "eax", "ecx": "ecx", "edx": "edx", "ebx": "ebx", "esi": "esi", "edi": "edi", "ebp": "ebp", "esp": "esp",
        "ax": "eax", "al": "eax", "ah": "eax", "cx": "ecx", "cl": "ecx", "ch": "ecx", "dx": "edx", "dl": "edx", "dh": "edx", "bx": "ebx", "bl": "ebx", "bh": "ebx",
        "si": "esi", "di": "edi", "bp": "ebp", "sp": "esp"}
SAVED = ("ebx", "esi", "edi", "ebp")
RMW = {"add", "sub", "and", "or", "xor", "adc", "sbb", "shl", "shr", "sar", "sal", "rol", "ror", "rcl", "rcr", "inc", "dec", "neg", "not", "bswap", "shld", "shrd", "xadd", "cmpxchg"}
READONLY = {"cmp", "test", "bt"}
JCC = re.compile(r"^j(a|ae|b|be|c|e|z|g|ge|l|le|na|nae|nb|nbe|nc|ne|ng|nge|nl|nle|no|np|ns|nz|o|p|pe|po|s)$")
STR = ("movs", "stos", "lods", "cmps", "scas")
ADDR = re.compile(r"^0x[0-9a-f]+$")


def split_ops(a):
    return [x.strip() for x in re.split(r",(?![^\[]*\])", a)] if a else []


def regs_of(t):
    return {FULL[x] for x in re.findall(r"\b[a-z]{2,3}\b", t) if x in FULL}


def parse_objdump(out):
    ins, order = {}, []
    for l in out.splitlines():
        m = re.match(r"\s*([0-9a-f]+):\s+(\S.*)$", l)
        if not m: continue
        a = int(m.group(1), 16)
        rest = re.sub(r"\s*<[^>]*>\s*$", "", m.group(2).split("#")[0].strip())
        w = rest.split(None, 1)
        op = w[0].lower(); args = w[1].lower() if len(w) > 1 else ""
        pre = ""
        while op in ("rep", "repz", "repnz", "repe", "repne", "lock", "data16", "addr16") and args:
            if op.startswith("rep"): pre = "rep"
            w = args.split(None, 1); op = w[0]; args = w[1] if len(w) > 1 else ""
        if pre: op = "rep " + op
        ins[a] = (op, args); order.append(a)
    return ins, order


def effects(op, args):
    """-> (reads, writes, kind)  kind None | call | jmp | jcc | ret | stop"""
    p = split_ops(args)
    rd, wr = set(), set()
    rep = op.startswith("rep ")
    b = op.split()[-1]
    if (b in STR or (b[:4] in STR and b[-1] in "bwd" and len(b) == 5)) and ("es:" in args or "ds:" in args or not args):
        c = {"ecx"} if rep else set()
        if b.startswith("movs"): return {"esi", "edi"} | c, {"esi", "edi"} | c, None
        if b.startswith("stos"): return {"edi", "eax"} | c, {"edi"} | c, None
        if b.startswith("lods"): return {"esi"} | c, {"eax", "esi"} | c, None
        if b.startswith("cmps"): return {"esi", "edi"} | c, {"esi", "edi"} | c, None
        return {"edi", "eax"} | c, {"edi"} | c, None
    if op == "call":
        t = p[0] if p else ""
        return (set() if ADDR.match(t) else regs_of(t)), {"eax", "ecx", "edx"}, "call"
    if op == "jmp":
        t = p[0] if p else ""
        return (set() if ADDR.match(t) else regs_of(t)), wr, "jmp"
    if JCC.match(op) or op in ("loop", "loope", "loopne", "jecxz", "jcxz"):
        return ({"ecx"} if op.startswith(("loop", "j")) and op in ("loop", "loope", "loopne", "jecxz", "jcxz") else set()), wr, "jcc"
    if op in ("ret", "retn"): return rd, wr, "ret"
    if op in ("int3", "hlt", "ud2", "int", "into"): return rd, wr, "stop"
    if op in ("cdq", "cwd"): return {"eax"}, {"edx"}, None
    if op in ("cwde", "cbw"): return {"eax"}, {"eax"}, None
    if op == "sahf": return {"eax"}, wr, None
    if op == "lahf": return rd, {"eax"}, None
    if op == "rdtsc": return rd, {"eax", "edx"}, None
    if op == "cpuid": return {"eax", "ecx"}, {"eax", "ebx", "ecx", "edx"}, None
    if op in ("fnstsw", "fstsw") and p and p[0] == "ax": return rd, {"eax"}, None
    if op in ("mul", "div", "idiv") or (op == "imul" and len(p) == 1):
        rd |= regs_of(p[0]) | {"eax"}
        if op in ("div", "idiv"): rd.add("edx")
        return rd, {"eax", "edx"}, None
    if op == "imul" and len(p) == 3: return regs_of(p[1]), {FULL[p[0]]}, None
    if op == "push": return (regs_of(p[0]) if p else set()), wr, None
    if op == "pop":
        if p and p[0] in FULL: wr.add(FULL[p[0]])
        elif p: rd |= regs_of(p[0])
        return rd, wr, None
    if op.startswith("cmov"):
        for t in p: rd |= regs_of(t)
        return rd, wr, None            # conditional write: not a must-write
    if op.startswith("set"):
        if p and p[0] in FULL: wr.add(FULL[p[0]])
        else: rd |= regs_of(args)
        return rd, wr, None
    if op in ("mov", "movzx", "movsx", "lea", "movd", "movq"):
        if len(p) == 2:
            if p[0] in FULL:
                wr.add(FULL[p[0]])
                if p[0] in ("ah", "bh", "ch", "dh"): rd.add(FULL[p[0]])
            else: rd |= regs_of(p[0])
            rd |= regs_of(p[1])
        return rd, wr, None
    if op == "xchg" and len(p) == 2:
        for t in p:
            rd |= regs_of(t)
            if t in FULL: wr.add(FULL[t])
        return rd, wr, None
    if op in READONLY or op in RMW:
        if op in ("xor", "sub", "sbb") and len(p) == 2 and p[0] == p[1] and p[0] in FULL: return rd, {FULL[p[0]]}, None
        if op == "or" and len(p) == 2 and p[0] in FULL and p[1] in ("0xffffffff", "-1"): return rd, {FULL[p[0]]}, None
        for t in p: rd |= regs_of(t)
        if op not in READONLY and p and p[0] in FULL: wr.add(FULL[p[0]])
        return rd, wr, None
    for t in p: rd |= regs_of(t)               # everything else (x87 memory operands, sse ...): registers in the operands are read
    return rd, wr, None


def fpu_delta(op, args):
    """-> (need, delta) = x87 entries the instruction reads and the net push/pop; None if not x87"""
    if not op.startswith("f") or op in ("fwait", "fnstcw", "fldcw", "fnstsw", "fstsw", "fnclex", "fninit", "fnop", "fnstenv", "fldenv", "frstor", "fnsave"): return None
    sti = [int(m) for m in re.findall(r"st\((\d)\)", args)]
    mx = max(sti) + 1 if sti else 0
    if op in ("fld", "fild", "fbld"): return (mx, 1)
    if op in ("fld1", "fldz", "fldpi", "fldl2e", "fldl2t", "fldlg2", "fldln2"): return (0, 1)
    if op == "fxch": return (max(mx, 2) if not sti else max(mx, 1), 0)
    if op in ("fst", "fist", "fbstp"): return (max(mx, 1), 0)
    if op in ("fstp", "fistp", "fisttp"): return (max(mx, 1), -1)
    if op in ("fcompp", "fucompp"): return (2, -2)
    if op in ("fcomp", "fucomp", "ficomp", "fcomip", "fucomip"): return (max(mx, 1), -1)
    if op in ("fcom", "fucom", "ficom", "ftst", "fxam", "fcomi", "fucomi"): return (max(mx, 1), 0)
    if op in ("fsincos", "fptan", "fxtract"): return (1, 1)
    if op in ("fpatan", "fyl2x", "fyl2xp1"): return (2, -1)
    if op in ("fscale", "fprem", "fprem1"): return (2, 0)
    if op.endswith("p") and op.startswith(("fadd", "fsub", "fmul", "fdiv")): return (max(mx, 2), -1)
    if op.startswith(("fadd", "fsub", "fmul", "fdiv", "fiadd", "fisub", "fimul", "fidiv")): return (max(mx, 1), 0)
    if op.startswith("fcmov"): return (max(mx, 2), 0)
    if op.startswith("ffree"): return (0, 0)
    return (max(mx, 1), 0)             # fchs fabs fsqrt fsin fcos frndint ...


def analyse(lo, hi, ins, order, tbl, callee, calleeN={}, calleeF={}):
    addrs = [a for a in order if lo <= a < hi]
    if lo not in ins: return None
    idx = {a: i for i, a in enumerate(addrs)}
    popped = set()
    for a in addrs:
        o, ar = ins[a]
        if o == "pop" and ar.strip() in FULL: popped.add(FULL[ar.strip()])
    lead_at = set()                   # three consecutive pushes of ebx / esi / edi (any order) = a save sequence even without a pop in range (SEH_prolog)
    for k in range(len(addrs) - 2):
        t3 = [ins[addrs[k + m]] for m in range(3)]
        if all(o == "push" and x in ("ebx", "esi", "edi") for o, x in t3) and len({x for o, x in t3}) == 3: lead_at |= {addrs[k], addrs[k + 1], addrs[k + 2]}
    def save_like(k, r):
        """push of the unwritten register r: True = a save (callee saved: r is overwritten before it is read) or a stack reservation (volatile r: `push ecx` for a local, the slot is
        then accessed as [esp+k] / [ebp-k]); False = r is pushed as a value (argument: the slot is released by add esp / pop / a callee `ret N`, r is read, or it is never accessed)."""
        depth = 4
        vol = r in ("eax", "ecx", "edx")
        ebp_off = None                                           # bytes pushed between `mov ebp,esp` and this push (slot = [ebp-ebp_off-4 .. ebp-ebp_off))
        for kk in range(k - 1, -1, -1):
            o0, a0 = ins[addrs[kk]]
            if o0 == "mov" and split_ops(a0) == ["ebp", "esp"]: ebp_off = 4 * sum(1 for x in addrs[kk + 1:k] if ins[x][0] == "push"); break
        written = False
        dup = 0
        for n, a2 in enumerate(addrs[k + 1:k + 80]):
            op2, ar2 = ins[a2]
            if (op2, ar2) == ("push", r): depth += 4; dup += 1; continue          # `push ecx; push ecx` stack reservation
            rd2, wr2, kd = effects(op2, ar2)
            q = split_ops(ar2)
            if vol:
                for m in re.finditer(r"\[esp(?:\+0x([0-9a-f]+))?\]", ar2):
                    off = int(m.group(1), 16) if m.group(1) else 0
                    if depth - 4 - 4 * dup <= off < depth: return True            # the slot is used as a local
                if ebp_off is not None:
                    for m in re.finditer(r"\[ebp-0x([0-9a-f]+)\]", ar2):
                        off = int(m.group(1), 16)
                        if ebp_off < off <= ebp_off + 4 + 4 * dup: return True
            if r in rd2 and op2 != "push" and not written: return False
            if op2 == "push": depth += 4
            elif op2 == "pop": depth -= 4
            elif op2 in ("add", "sub") and q and q[0] == "esp" and len(q) == 2 and re.match(r"^-?(0x[0-9a-f]+|\d+)$", q[1]):
                depth += int(q[1], 16) * (1 if op2 == "sub" else -1)
            elif op2 == "leave" or (op2 == "mov" and q[:1] == ["esp"]): return (not vol) or written or r in wr2
            if depth <= 0: return False                                 # the slot is released: it was an argument
            if kd == "call":
                t = q[0] if q else ""
                if ADDR.match(t):
                    if vol and r in callee.get(int(t, 16), set()) and not written: return False
                    if calleeN.get(int(t, 16), 0) >= depth: return False
                nxt2 = ins.get(addrs[k + 2 + n]) if k + 2 + n < len(addrs) else None
                if nxt2 and nxt2[0] == "add" and split_ops(nxt2[1])[:1] == ["esp"]:
                    try:
                        if int(split_ops(nxt2[1])[1], 16) >= depth: return False
                    except ValueError: pass
                if vol: written = True
            if r in wr2 and kd != "call":
                if not vol: return True
                written = True
            if kd == "jmp" and vol: return False
            if kd in ("ret", "stop"): return not vol
        return False
    pro = set()                       # addresses inside the prologue (only pushes / mov ebp,esp / sub esp before them): a volatile push there can be a stack reservation
    for a in addrs:
        o, ar = ins[a]; q = split_ops(ar)
        pro.add(a)
        if not (o == "push" or (o == "mov" and q == ["ebp", "esp"]) or (o == "sub" and q[:1] == ["esp"])): break
    W = {lo: frozenset()}; D = {lo: 0}; WU = {lo: frozenset()}; strict = {}
    work = [lo]
    first_read, reserve, via = {}, {}, {}
    mind = 0; fpu_at = 0
    ret_ws, ret_ds = [], []
    retn = 0; tails = set()
    while work:
        a = work.pop()
        op, args = ins[a]
        w = W[a]; d = D[a]; wu = WU[a]
        rd, wr, kind = effects(op, args)
        p = split_ops(args)
        reads = set(rd)
        if op == "push" and p and p[0] in FULL:
            r = FULL[p[0]]
            if r in SAVED and (r in popped or a in lead_at or save_like(idx[a], r)) and r not in w: reads.discard(r)
            elif r in ("eax", "ecx", "edx") and r not in w and (r in popped or save_like(idx[a], r)): reserve[r] = a; reads.discard(r)
        if (op == "mov" and len(p) == 2 and p[0] == p[1]) or (op == "lea" and len(p) == 2 and re.fullmatch(r"\[(e..)\+0x0\]", p[1]) and p[1][1:4] == p[0]): reads = set(); wr = set()   # mov edi,edi / lea ebx,[ebx+0x0] padding
        tgt = int(p[0], 16) if kind in ("call", "jmp") and p and ADDR.match(p[0]) else None
        if tgt is not None and (kind == "call" or not (lo <= tgt < hi)):
            cin = {r for r in callee.get(tgt, set()) if r in ("eax", "ecx", "edx")}      # callee saved registers read by a callee are saves / spills (setjmp), not propagated
            for r in cin:
                if r not in reads: via.setdefault(r, (a, tgt))
            reads |= cin
            if kind == "jmp": tails.add(tgt)
        for r in reads:
            if r in R6 and r not in w and r not in first_read: first_read[r] = a
            if r in R6 and r not in wu and r not in strict: strict[r] = a
        w2 = frozenset(w | {r for r in wr if r in R6})
        wu2 = frozenset(wu | {r for r in wr if r in R6})
        fd = fpu_delta(op, args)
        d2 = d
        if fd:
            if d - fd[0] < mind: mind = d - fd[0]; fpu_at = a
            d2 = d + fd[1]
        elif kind in ("call", "jmp") and (kind == "call" or (tgt is not None and not (lo <= tgt < hi))):
            fin_c, st0_c = 0, (calleeF.get(tgt, (0, False))[1] if tgt is not None else False)       # only the ST0 return of a callee is modelled (a callee's own x87 inputs are not propagated)
            if d - fin_c < mind: mind = d - fin_c; fpu_at = a
            d2 = d - fin_c + (1 if st0_c else 0)
            if tgt is None or tgt not in calleeF:                 # unknown callee (indirect call / import): when the next x87 instruction needs more than there is, the call returned ST0
                nx = ins.get(addrs[idx[a] + 1]) if idx[a] + 1 < len(addrs) else None
                nf = fpu_delta(*nx) if nx else None
                if nf and nf[0] > d2: d2 += 1
        nxt = addrs[idx[a] + 1] if idx[a] + 1 < len(addrs) else None
        succ = []
        if kind == "ret":
            if p: retn = max(retn, int(p[0], 16))
            ret_ws.append(w); ret_ds.append(d); continue
        if kind == "stop": continue
        if kind == "jmp":
            if tgt is not None:
                if lo <= tgt < hi: succ.append(tgt)
                else: ret_ws.append(w2); ret_ds.append(d2)         # tail call: the callee returns for us
            else:
                succ += [t for t in tbl.get(a, []) if lo <= t < hi]
        elif kind == "jcc":
            if p and ADDR.match(p[0]) and lo <= int(p[0], 16) < hi: succ.append(int(p[0], 16))
            if nxt is not None: succ.append(nxt)
        elif nxt is not None: succ.append(nxt)
        for s in succ:
            if s not in ins: continue
            if s not in W: W[s] = w2; WU[s] = wu2; D[s] = d2; work.append(s)
            else:
                nw = W[s] & w2; nu = WU[s] | wu2
                if nw != W[s] or nu != WU[s]: W[s] = nw; WU[s] = nu; work.append(s)
    # a reserved push whose register is also read (before a write) elsewhere is a real read, found above already
    inputs = sorted(first_read, key=lambda r: first_read[r])
    ret_regs = [r for r in ("eax", "edx") if ret_ws and all(r in x for x in ret_ws)]
    st0 = any(d - mind > 0 for d in ret_ds)
    return dict(fpu_at=fpu_at, first=first_read, via={r: v for r, v in via.items() if first_read.get(r) == v[0]}, inputs=inputs, strict=sorted(strict, key=lambda r: strict[r]), fpu=-mind, ret_regs=ret_regs, st0=st0, retn=retn, tails=tails)


def va_dword(exe, secs, va):
    for vs, vsz, raw in secs:
        if vs <= va and va + 4 <= vs + vsz: return struct.unpack_from("<I", exe, raw + va - vs)[0]
    return None


def tables(lo, hi, ins, order, exe, secs):
    tb = {}
    for k, a in enumerate(order):
        if lo <= a < hi and ins[a][0] == "jmp":
            m = re.search(r"\*4\+0x([0-9a-f]+)\]", ins[a][1])
            if m:
                t = int(m.group(1), 16); ts = []
                n = 256                                   # entries: from the `cmp reg,N` / `ja` bounds check just before (N + 1), else until a value is not an instruction start
                for b in order[max(0, k - 8):k][::-1]:
                    mm = re.match(r"(?:cmp|sub)\s+\w+,(0x[0-9a-f]+)$", ins[b][0] + " " + ins[b][1])
                    if mm and ins[b][0] == "cmp": n = int(mm.group(1), 16) + 1; break
                while len(ts) < n:
                    v = va_dword(exe, secs, t)
                    if v is None or not (lo <= v < hi) or v not in ins: break
                    ts.append(v); t += 4
                tb[a] = ts
    return tb


def load_pe():
    exe = open(EXE, "rb").read()
    pe = struct.unpack_from("<I", exe, 0x3c)[0]
    n = struct.unpack_from("<H", exe, pe + 6)[0]; osz = struct.unpack_from("<H", exe, pe + 20)[0]
    base = struct.unpack_from("<I", exe, pe + 52)[0]
    secs = []
    for i in range(n):
        vsz, vs, rsz, raw = struct.unpack_from("<IIII", exe, pe + 24 + osz + 40 * i + 8)
        secs.append((base + vs, min(vsz, rsz) if rsz else vsz, raw))
    return exe, secs


def main():
    args = [a for a in sys.argv[1:] if not a.startswith("--")]
    prop = "--nopropagate" not in sys.argv
    out = args[0] if args else os.path.join(G, "output", "reginputs2.csv")
    exe, secs = load_pe()
    items = sorted((int(r["lo"], 16), int(r["hi"], 16), r["name"]) for r in csv.DictReader(open(OWN)) if r["kind"] == "code")
    def dis(lo, hi):
        o = subprocess.run(["objdump", "-w", "-M", "intel", "-D", "--no-show-raw-insn", "--start-address=0x%x" % lo, "--stop-address=0x%x" % hi, EXE], capture_output=True, text=True).stdout
        return parse_objdump(o)
    # the ownership boundaries are not always function starts: (a) a boundary inside an instruction (gap_405ddc = tail of the `mov [m],imm32` of gamesave_gather), (b) an unnamed
    # gap_ item that the previous item falls into (no ret / jmp at its end) and that nobody calls or jumps to: merged into the previous item
    refs = {}                      # target -> [(call?, source address)]
    for lo, hi, name in items:
        ins, order = dis(lo, hi)
        for a in order:
            if ins[a][0] in ("call", "jmp") or JCC.match(ins[a][0]):
                t = ins[a][1].strip()
                if ADDR.match(t): refs.setdefault(int(t, 16), []).append((ins[a][0] == "call", a))
    merged = []
    nmerge = [0, 0]
    unnamed = lambda n: n.startswith(("gap_", "FUN_", "source match", "switchdata"))
    for lo, hi, name in items:
        if merged and merged[-1][1] == lo and unnamed(name):
            plo, phi, pn = merged[-1]
            ins, order = dis(plo, phi + 16)
            if phi not in ins:
                merged[-1] = (plo, hi, pn); nmerge[0] += 1; continue
            if not any(c or not (plo <= src < hi) for c, src in refs.get(lo, [])):
                last = [a for a in order if a < phi]
                if last:
                    lop = ins[last[-1]][0]
                    k = len(last) - 1
                    while k > 0 and ins[last[k]][0] in ("nop", "int3"): k -= 1
                    lop = ins[last[k]][0]
                    if lop not in ("ret", "retn", "jmp", "hlt", "ud2", "int3") and not lop.startswith("ret"):
                        merged[-1] = (plo, hi, pn); nmerge[1] += 1; continue
        merged.append((lo, hi, name))
    print("boundaries merged: mid-instruction %d, unnamed fallthrough %d" % tuple(nmerge), file=sys.stderr)
    items = merged
    cache = {}
    for lo, hi, name in items:
        ins, order = dis(lo, hi)
        cache[lo] = (ins, order, tables(lo, hi, ins, order, exe, secs))
    # entry = a real function entry (named, a call target or a data / pointer target in Ghidra's references); unnamed gap_ items that nobody calls are code chunks / switch data
    callT, dataT, sw = set(), set(), set()
    for l in open(os.path.join(G, "output", "delink", "ghidra_refs.csv")):
        w = l.rstrip("\n").split(",")
        if w[0] == "L":
            if w[2].startswith("switchdata"): sw.add(int(w[1], 16))
        elif w[0] != "C" and len(w) > 2:
            if w[2] == "UNCONDITIONAL_CALL": callT.add(int(w[1], 16))
            elif w[2] == "DATA": dataT.add(int(w[1], 16))
    entry = {lo: (not name.startswith(("gap_", "source match"))) or lo in callT or (lo in dataT and lo not in sw) for lo, hi, name in items}
    callee, res, calleeN, calleeF = {}, {}, {}, {}
    for it in range(8):
        changed = False
        for lo, hi, name in items:
            ins, order, tb = cache[lo]
            r = analyse(lo, hi, ins, order, tb, callee if prop else {}, calleeN, calleeF)
            if r is None: continue
            res[lo] = r; calleeN[lo] = r["retn"]
            if prop and calleeF.get(lo) != (r["fpu"], r["st0"]): calleeF[lo] = (r["fpu"], r["st0"]); changed = True
            if callee.get(lo, set()) != set(r["inputs"]) or calleeN.get(lo) != r["retn"] and False: callee[lo] = set(r["inputs"]); changed = True
        print("iteration", it, "changed" if changed else "stable", file=sys.stderr)
        if not changed or not prop: break
    with open(out, "w") as f:
        f.write("name,addr,reg_inputs,ret_n,x87_in,ret_regs,size,tail,why,entry,strict_inputs\n")
        for lo, hi, name in items:
            r = res.get(lo)
            if r: f.write("%s,%08x,%s,%d,%d,%s,%d,%s,%s,%d,%s\n" % (name, lo, "|".join(r["inputs"]), r["retn"], r["fpu"], "|".join(r["ret_regs"] + (["st0"] if r["st0"] else [])), hi - lo, "|".join("%08x" % t for t in sorted(r["tails"])), "|".join(["x87@%x" % r["fpu_at"]] * (r["fpu"] > 0) + ["%s@%x%s" % (x, r["first"][x], ">%x" % r["via"][x][1] if x in r["via"] else "") for x in r["inputs"]]), entry[lo], "|".join(r["strict"])))


if __name__ == "__main__":
    main()
