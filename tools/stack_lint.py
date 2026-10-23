#!/usr/bin/env python3
"""stack_lint.py [--all] [--csv F] [--weak-equal] [--selftest] [files...]: stack / call-boundary lint for the blind spots of diffemu + argcheck (an EQUAL verdict with a crash in game).  Needs the diffemu venv (re-execs itself).
Scope (default): src/automatch_near, src/clusters, src/bridges, src/tier1, output/tier2_candidates*, output/usercall_candidates*; `files` = explicit .c files.  Output: output/stack_lint.csv + one summary line.
  A frameoverflow  `&frame.m` / `frame.arr` / `&local_N` passed to a call whose ORIGINAL body (store-extent scan of its asm, [reg+disp] through the pointer argument; typed `RwV3d *` = 12, `RwMatrix *` = 64 win) writes or reads
                   more bytes through that pointer than are left from the member to the END of the local frame struct (or to the frame top for a loose `local_N`): the callee overwrites the saved registers / the return address
                   (ai_find_target_c: `undefined1 local_c[4]; undefined4 local_8;` for the 12 byte RwV3d car_get_forward writes: page fault at BF800000).  Write overflow = high, read-only = medium.
  B argkind        a stack address (`&local`, frame array) is passed in an argument slot where the ORIGINAL function passes none to that callee at any of its call instances (its own `lea reg,[ebp-..]; push reg` sites, CFG walk, hoisted
                   pushes included): point_project_onto_line_2d(&local_20, &local_20, ..) put a local address where the car index belongs.
  C callarity      call with fewer arguments than the stack slots the callee's ORIGINAL body reads (output/orig_stackargs.txt, a lower bound also for `addr` / `dyn` rows; calls through `()` / undeclared `func_0x..` included): ai_bot_update
                   called ai_drive_a() with no arguments (the original pushes the 4 words once before a branch and the three drivers share them).  Exact-slot hits on typed prototypes are defectlint class 10 and not repeated.
  E regcallee_int  plain call of a callee that only reginputs_interior.csv knows (interior entry of a multi-function item with register inputs).
  F floatlit       hex literal >= 0x10000000 (a float bit pattern) passed to a float / double parameter of the prototype: the compiler converts the integer value (FUN_005b7300 4th argument).
  G regstack       cluster style callee (parameters named reg_*, regsigs.csv register inputs): the declaration / call has fewer parameters than register inputs + the stack slots of the original
                   (course_lod_atomic_set_geometries(esi, edi) for esi + edi + one stack argument: the near LOD atomic was an uninitialised slot).
  H frameleak      a code-range literal / `&global` stored into a pad member or through `*(int *)&local` of a local aggregate (course_rouabout_cb7: `object_matrix.pad3 = &g_course_8c53e0; *(int *)&object_matrix.pos.z = 4554602;` = the original's `push offset; call`).
  D2 stale         output/orig_stackargs.txt older than tools/origargs.py / reginputs2.py (stale `dyn` / 0 slot rows).
  --weak-equal     lists diffemu EQUAL verdicts that prove little (coverage of the original < 0.6, more skipped than completed cases, loop patched): the harness cannot see callee loops / runaway cases.
Warning tool: exit 0 (check_all.sh prints the summary as WARNING).  Suppress a checked finding in src/stacklint_allow.txt (`function class reason`)."""
import os, sys
try: import capstone
except ImportError:
    py = os.environ.get("DIFFEMU_PY") or os.path.expanduser("~/.cache/mfl_diffemu_venv/bin/python"); os.execv(py, [py] + sys.argv)
import csv, glob, re
H = os.path.dirname(os.path.abspath(__file__)); G = os.path.normpath(os.path.join(H, ".."))
sys.path[:0] = [H, os.path.join(H, "diffemu")]
import defectlint as D
import core
CX = core.CX
SCOPE = ["src/automatch_near", "src/clusters", "src/bridges", "src/tier1", "output/tier2_candidates", "output/tier2_candidates2", "output/usercall_candidates*"]
SZ = {"undefined": 1, "undefined1": 1, "undefined2": 2, "undefined4": 4, "undefined8": 8, "char": 1, "uchar": 1, "byte": 1, "bool": 1, "short": 2, "ushort": 2, "word": 2, "int": 4, "uint": 4, "long": 4, "ulong": 4,
      "dword": 4, "float": 4, "RwReal": 4, "double": 8, "float10": 12, "BOOL": 4, "LONG_PTR": 4, "ULONG_PTR": 4, "RwUInt32": 4, "RwInt32": 4, "RwUInt16": 2, "RwInt16": 2, "RwUInt8": 1, "RwInt8": 1, "code": 4, "size_t": 4,
      "longlong": 8, "ulonglong": 8, "RwV3d": 12, "RwV2d": 8, "RwMatrix": 64, "RwMatrixTag": 64, "RwRGBA": 4, "RwSphere": 16, "RwBBox": 24, "RwLine": 24, "RwPlane": 16, "RwTexCoords": 8, "RwSurfaceProperties": 12}
RW_SUB = {"RwV3d": [("x", 0), ("y", 4), ("z", 8)], "RwV2d": [("x", 0), ("y", 4)], "RwMatrix": [("right", 0), ("flags", 12), ("up", 16), ("pad1", 28), ("at", 32), ("pad2", 44), ("pos", 48), ("pad3", 60)]}
KW = D.KW | {"defined"}
OPAQUE = {"undefined", "undefined1", "undefined4", "float", "char", "int", "RwReal", "void", "uint", "byte", "uchar"}


def tsize(t):
    """byte size of a C type name (one token + stars), None = unknown"""
    if "*" in t: return 4
    t = re.sub(r"\b(const|struct|union|enum|unsigned|signed|volatile)\b", " ", t).strip()
    t = t.split()[-1] if t.split() else ""
    return SZ.get(t) or core.T().structs.get(t)


# ---------------------------------------------------------------------------------------------------------- callee store extents (asm)
_EX = {}
def extents(va, nargs):
    """per stack argument: (read extent, write extent) in bytes of the object the ORIGINAL callee reaches through the pointer it loads from that argument slot; 0 = unknown / not a pointer.  Same alias scan as
    diffemu core.asm_limits, plus (1) read / write split, (2) the stack depth after a `ret` is restored from the branch that jumps behind it (asm_limits resets it to 0: car_get_forward's early `ret` after `pop esi`
    made its pointer slot appear as the car index slot)"""
    key = (va, nargs)
    if key in _EX: return _EX[key]
    BAD = 1 << 30; ext = [0] * nargs; wext = [0] * nargs; al = {}; d = 0; ebp_d = None; dmap = {}; prev = None
    sz = core.T().size.get(va, 0x400); name = lambda r: core.CS.reg_name(r)
    try:
        for ins in core.CS.disasm(core.img_read(va, sz), va):
            mn = ins.mnemonic; ops = ins.operands; mems = [o for o in ops if o.type == CX.X86_OP_MEM]
            if prev == "ret": d = dmap.get(ins.address, 0)
            elif prev == "jmp": d = dmap.get(ins.address, d)
            prev = "ret" if mn in ("ret", "retn") else "jmp" if mn == "jmp" else None
            direct = ops[0].type == CX.X86_OP_IMM if ops else False
            if direct and mn.startswith("j") and va <= ops[0].imm < va + sz: dmap.setdefault(ops[0].imm, d)
            if mn in ("ret", "retn"): continue
            if mn in ("and", "leave") and ops and ops[0].type == CX.X86_OP_REG and name(ops[0].reg) == "esp": ext = [BAD] * nargs; break
            slot = None
            for o in mems:
                b = name(o.mem.base) if o.mem.base else ""; x = o.mem.index
                if b == "esp" or (b == "ebp" and ebp_d is not None):
                    off = o.mem.disp - (d if b == "esp" else ebp_d)
                    if x == 0 and off >= 4 and (off - 4) % 4 == 0 and (off - 4) // 4 < nargs: slot = (off - 4) // 4
                    continue
                if b in al:
                    k = al[b]
                    if x: ext[k] = BAD
                    else:
                        e_ = o.mem.disp + max(o.size, 1); ext[k] = max(ext[k], e_)
                        if o.access & capstone.CS_AC_WRITE: wext[k] = max(wext[k], e_)
                elif x and name(x) in al: ext[al[name(x)]] = BAD
            rd, wr = ins.regs_access(); rn = {name(r) for r in rd}; wn = {name(r) for r in wr}
            membase = {name(o.mem.base) for o in mems if o.mem.base} | {name(o.mem.index) for o in mems if o.mem.index}
            if slot is not None:
                if mn == "mov" and ops[0].type == CX.X86_OP_REG and ops[0].size == 4 and ops[1].type == CX.X86_OP_MEM:
                    al[name(ops[0].reg)] = slot; continue
                if mn in ("fld", "fild", "fadd", "fmul", "fsub", "fdiv", "fcom", "fcomp", "fsubr", "fdivr", "cmp", "test"): pass
                else: ext[slot] = BAD
            if mn == "call":
                if direct and core.BASE <= ops[0].imm < core.IMGEND:
                    try: d -= core.callee_info(ops[0].imm).pop
                    except Exception: pass
                elif not direct:
                    for r in [r for r in al if r in rn]: ext[al.pop(r)] = BAD
                for r in ("eax", "ecx", "edx"):
                    if r in al: ext[al[r]] = BAD
                    al.pop(r, None)
                continue
            if mn == "jmp" and direct and not (va <= ops[0].imm < va + sz): ext = [BAD] * nargs; break
            if mn == "push": d += 4
            elif mn == "pop": d -= 4
            elif mn == "sub" and ops[0].type == CX.X86_OP_REG and name(ops[0].reg) == "esp" and ops[1].type == CX.X86_OP_IMM: d += ops[1].imm
            elif mn == "add" and ops[0].type == CX.X86_OP_REG and name(ops[0].reg) == "esp" and ops[1].type == CX.X86_OP_IMM: d -= ops[1].imm
            elif mn == "mov" and ops[0].type == CX.X86_OP_REG and name(ops[0].reg) == "ebp" and ops[1].type == CX.X86_OP_REG and name(ops[1].reg) == "esp": ebp_d = d
            for r in [r for r in al if r in rn and r not in membase]:
                if mn in ("test", "cmp"): continue
                if mn == "mov" and ops[0].type == CX.X86_OP_REG and ops[1].type == CX.X86_OP_REG and name(ops[1].reg) == r: al[name(ops[0].reg)] = al[r]; continue
                ext[al[r]] = BAD
            for r in wn:
                if r in al and r in rn and r not in membase and mn not in ("test", "cmp"): ext[al.pop(r)] = BAD
                elif r in al and not (mn == "mov" and ops[0].type == CX.X86_OP_REG and name(ops[0].reg) == r and ops[1].type == CX.X86_OP_REG and name(ops[1].reg) in al):
                    if r in rn and r in membase or r not in rn: al.pop(r, None)
    except Exception: return _EX.setdefault(key, [(0, 0)] * nargs)
    return _EX.setdefault(key, [(0, 0) if e >= BAD or e <= 0 else (e, 0 if wext[i] >= BAD else wext[i]) for i, e in enumerate(ext)])


# ---------------------------------------------------------------------------------------------------------- original call sites (stack address kinds)
_SK = {}
def site_kinds(va):
    """-> {callee va (8 hex): {tuple of argument kinds (slot 0 first)}} of the ORIGINAL function at va (hex string); kind = 's' stack address (`lea r,[ebp-..|esp+..]` pushed, directly or through a register),
    'i' immediate, 'o' anything else.  CFG walk with the pending-push stack as state: pushes hoisted before a branch reach the calls behind it, `add esp` / pop / callee `ret N` drop them."""
    if va in _SK: return _SK[va]
    lo = int(va, 16); sz = core.T().size.get(lo, 0)
    out = {}
    if not sz: _SK[va] = out; return out
    ins = {}; a = lo
    while a < lo + sz:
        i = next(core.CS.disasm(core.img_read(a, 16), a, 1), None)
        if i is None: break
        ins[a] = i; a += i.size
    pro = set(); a = lo      # prologue register saves are no arguments
    while a in ins and (ins[a].mnemonic == "push" and ins[a].op_str in ("ebp", "esi", "edi", "ebx") or ins[a].mnemonic == "mov" and ins[a].op_str == "ebp, esp"): pro.add(a); a += ins[a].size
    seen = set(); work = [(lo, (), ())]; steps = 0
    nm = lambda r: core.CS.reg_name(r)
    while work and steps < 6000:
        pc, pend, regs = work.pop(); regs = dict(regs)
        while pc in ins and steps < 6000:
            key = (pc, pend, tuple(sorted(regs.items())))
            if key in seen: break
            seen.add(key); steps += 1; i = ins[pc]; mn = i.mnemonic; ops = i.operands; nxt = pc + i.size
            if pc in pro: pc = nxt; continue
            if mn == "push":
                o = ops[0]
                k = "i" if o.type == CX.X86_OP_IMM else regs.get(nm(o.reg), "o") if o.type == CX.X86_OP_REG else "o"
                pend = pend + (k,)
            elif mn == "pop":
                pend = pend[:-1]
                if ops and ops[0].type == CX.X86_OP_REG: regs.pop(nm(ops[0].reg), None)
            elif mn == "lea" and ops[0].type == CX.X86_OP_REG:
                m_ = ops[1].mem; b = nm(m_.base) if m_.base else ""; regs[nm(ops[0].reg)] = "s" if b in ("ebp", "esp") else "o"
            elif mn == "add" and ops[0].type == CX.X86_OP_REG and nm(ops[0].reg) == "esp" and ops[1].type == CX.X86_OP_IMM: pend = pend[:max(0, len(pend) - ops[1].imm // 4)]
            elif mn == "call":
                if ops and ops[0].type == CX.X86_OP_IMM:
                    t = ops[0].imm & 0xffffffff
                    if core.BASE <= t < core.IMGEND:
                        out.setdefault("%08x" % t, set()).add(tuple(reversed(pend)))
                        try: pop = core.callee_info(t).pop // 4
                        except Exception: pop = 0
                        if pop: pend = pend[:max(0, len(pend) - pop)]
                for r in ("eax", "ecx", "edx"): regs.pop(r, None)
            elif mn not in ("jmp", "ret"):
                for r in i.regs_access()[1]:
                    n_ = nm(r)
                    if mn == "mov" and ops[0].type == CX.X86_OP_REG and ops[1].type == CX.X86_OP_REG and regs.get(nm(ops[1].reg)) == "s": regs[n_] = "s"
                    else: regs.pop(n_, None)
            if mn in ("ret", "retn", "int3", "hlt", "ud2"): break
            if mn == "jmp" or mn.startswith("j"):
                if ops and ops[0].type == CX.X86_OP_IMM and lo <= ops[0].imm < lo + sz: work.append((ops[0].imm, pend, tuple(regs.items())))
                if mn == "jmp": break
            pc = nxt
    _SK[va] = out
    return out


# ---------------------------------------------------------------------------------------------------------- C side
def va_of(L, f):
    pr = L.protos.get(f)
    if pr and pr[2]: return pr[2]
    m = re.fullmatch(r"(?:func_0x|FUN_)([0-9a-f]{8})", f)
    if m: return m.group(1)
    return D.NAMEVA.get(f) or (D.DPROTO.get(f) or (0, None))[1]


def parse_frames(body):
    """`struct { .. } var;` frame definitions in a function body -> {var: (size, {member: (offset, size, elem size, is array, type name)})}"""
    frames = {}
    for m in re.finditer(r"\bstruct\s*\{(.*?)\}\s*(\w+)\s*;", body, flags=re.S):
        off, mx, ok, mem = 0, 4, True, {}
        for l in m.group(1).split(";"):
            l = l.strip()
            if not l: continue
            q = re.match(r"^((?:[A-Za-z_]\w*\s+)*?[A-Za-z_]\w*)\s*(\*+\s*)?([A-Za-z_]\w*)\s*((?:\[[^\]]*\])*)$", l)
            if not q: ok = False; break
            ty = q.group(1) + (q.group(2) or ""); es = tsize(ty); dims = re.findall(r"\[\s*(0x[0-9a-fA-F]+|\d+)\s*\]", q.group(4))
            if es is None: ok = False; break
            n = 1
            for x in dims: n *= int(x, 0)
            al = 8 if ty.split()[-1] == "double" else min(es, 4) if es in (1, 2, 4) else 4
            off = (off + al - 1) // al * al; mx = max(mx, al)
            mem[q.group(3)] = (off, es * n, es, bool(dims), ty.split()[-1].strip("*")); off += es * n
        if ok: frames[m.group(2)] = ((off + mx - 1) // mx * mx, mem)
    return frames


CASTS = re.compile(r"^\(\s*(?:const\s+|struct\s+|unsigned\s+)*[A-Za-z_]\w*(?:\s*\*)*\s*\)\s*")
def strip_casts(a):
    a = a.strip()
    while True:
        b = CASTS.sub("", a, count=1).strip()
        while b.startswith("(") and D.balanced_after(b, 0) == len(b) - 1: b = b[1:-1].strip()
        if b == a: return a
        a = b


def frame_ref(a, frames):
    """argument text -> (frame var, member, byte offset inside the frame struct, is_address) or None"""
    a = strip_casts(a); addr = a.startswith("&")
    if addr: a = strip_casts(a[1:])
    m = re.fullmatch(r"(\w+)\.(\w+)((?:\.\w+)*)\s*((?:\[[^\]]*\])?)", a)
    if not m or m.group(1) not in frames: return None
    size, mem = frames[m.group(1)]
    if m.group(2) not in mem: return None
    off, msz, es, isarr, ty = mem[m.group(2)]
    if not addr and not isarr: return None      # a member value, not a pointer
    sub = [x for x in m.group(3).split(".") if x]
    if sub and ty in RW_SUB and sub[0] in dict(RW_SUB[ty]) and len(sub) == 1: off += dict(RW_SUB[ty])[sub[0]]
    elif sub: return None
    if m.group(4):
        ix = re.fullmatch(r"\[\s*(0x[0-9a-fA-F]+|\d+)\s*\]", m.group(4))
        if not ix: return None
        off += int(ix.group(1), 0) * es
    return m.group(1), m.group(2), off, addr


def loose_locals(body):
    """name -> (distance below the frame top in bytes (Ghidra's N), is_array)"""
    out = {}
    for m in re.finditer(r"^\s+(?:(?:unsigned|signed|const)\s+)*\w+\s*\**\s*((?:local|[a-z]*Stack)_([0-9a-fA-F]+))\s*(\[[^\]]*\])?\s*;", body, flags=re.M):
        out[m.group(1)] = (int(m.group(2), 16), bool(m.group(3)))
    return out


def loose_ref(a, loose):
    a = strip_casts(a); addr = a.startswith("&")
    if addr: a = strip_casts(a[1:])
    if a in loose and (addr or loose[a][1]): return a, loose[a][0]
    return None


class Finder:
    def __init__(self, rel, text, gd):
        self.rel = rel; self.L = D.Lint(rel, text, gd); self.rows = []

    def add(self, cls, sev, line, fn, sym, text, note):
        self.rows.append(dict(cls=cls, sev=sev, file=self.rel, line=line, function=fn, symbol=sym, text=" ".join(text.split())[:140], note=note))

    def run(self):
        L = self.L
        for fn, params, s, e in L.functions():
            body = "\n".join(L.lines[s:e + 1]); base_line = s + 1
            frames = parse_frames(body); loose = loose_locals(body)
            fva = D.NAMEVA.get(fn) or D.NAMEVA.get(fn[:-2] if fn.endswith("_c") else fn)
            myorig = site_kinds(fva) if fva else {}
            for m in re.finditer(r"\b([A-Za-z_]\w*)\s*\(", body):
                f = m.group(1)
                if f in KW or f == fn or re.search(r"printf", f): continue
                j = D.balanced_after(body, m.end() - 1)
                if j < 0: continue
                args = [x.strip() for x in D.split_args(body[m.end():j]) if x.strip()]
                pr = L.protos.get(f); va = va_of(L, f); line = base_line + body[:m.start()].count("\n"); st = body[m.start():j + 1]
                if va: va = va.lower()
                self.check_frame(fn, f, va, pr, args, frames, loose, line, st)
                if va:
                    self.check_arity(fn, f, va, pr, args, line, st)
                    self.check_kinds(fn, f, va, args, frames, loose, myorig, line, st)
                if pr: self.check_float(fn, f, pr, args, line, st)
            self.check_leak(fn, body, base_line, frames, loose)
        return self.rows

    # ---- H
    def check_leak(self, fn, body, base_line, frames, loose):
        """a pushed code / data address stored into a local aggregate (`obj.pad3 = (RwUInt32)&g_x; *(int *)&obj.pos.z = 4554602;`): the decompiler took the original's hoisted `push offset` / SEH-style frame words for locals"""
        for m in re.finditer(r"^\s*(\*\s*\(\s*\w+\s*\*\s*\)\s*&\s*)?(\w+(?:\.\w+)*)\s*=\s*(\([^)]*\)\s*)?(&\s*\w+|0x[0-9a-fA-F]{6}|\d{7})\s*;", body, flags=re.M):
            lhs, rhs = m.group(2), m.group(4); root = lhs.split(".")[0]
            if root not in frames and root not in loose and not ("." in lhs and re.search(r"^\s+(?:struct\s+)?\w+\s+%s\s*;" % re.escape(root), body, flags=re.M)): continue
            if rhs.startswith("&"):
                if not (m.group(1) is None and re.search(r"\.pad\d*$", lhs)) and root not in loose: continue      # only pad* members: a real pointer member is a legitimate store
            else:
                v = int(rhs, 16) if rhs.startswith("0x") else int(rhs)
                if not (0x401000 <= v < 0x5d7000): continue
                if m.group(1) is None and not re.search(r"pad\d*$", lhs): continue
            self.add("H", "medium", base_line + body[:m.start()].count("\n"), fn, lhs, m.group(0), "frameleak: a code / data address (%s) is stored into the local `%s`: the original pushed it as a call argument or SEH-style frame word (hoisted push), not a local" % (rhs, lhs))

    # ---- A
    def check_frame(self, fn, f, va, pr, args, frames, loose, line, st):
        if not va or not (frames or loose): return
        reg = core.T().reg.get(int(va, 16))
        if reg and reg[0]: return
        ex = None
        for i, a in enumerate(args):
            fr = frame_ref(a, frames)
            if fr:
                v, mname, off, addr = fr; top = frames[v][0] - off
                what = "from %s.%s (+%d) to the end of the %d byte frame struct" % (v, mname, off, frames[v][0])
            else:
                lr = loose_ref(a, loose)
                if not lr: continue
                top, what = lr[1], "from %s to the frame top" % lr[0]
            if ex is None: ex = extents(int(va, 16), len(args))
            rd, wr = ex[i]
            if pr and i < len(pr[1]):      # typed pointer parameter: the callee takes the whole object
                q = re.match(r"^(?:const\s+)?(?:struct\s+)?(\w+)\s*\*+\s*\w*$", pr[1][i].strip())
                if q and q.group(1) in SZ and q.group(1) not in OPAQUE: rd = max(rd, SZ[q.group(1)])
            if fr:      # M memberoverflow (N-verify 2026-10-23): the store leaves the MEMBER (into a neighbouring member or struct padding) though the struct has room; legal only when the original's frame is contiguous there (allow-list src/stacklint_allow.txt, class M)
                mo, ms = frames[fr[0]][1][fr[1]][:2]; mrem = mo + ms - fr[2]
                if 0 < wr and mrem < wr <= top: self.add("M", "low", line, fn, f, st, "memberoverflow: `%s` writes %d bytes through argument %d but the member %s.%s has %d left (struct has %d): the store lands in the neighbouring members / padding (callee asm 0x%s)" % (f, wr, i, fr[0], fr[1], mrem, top, va))
            if wr > top: self.add("A", "high", line, fn, f, st, "frameoverflow: `%s` writes %d bytes through argument %d but only %d bytes are left %s (callee asm 0x%s): it overwrites the saved registers / return address" % (f, wr, i, top, what, va))
            elif rd > top: self.add("A", "medium", line, fn, f, st, "frameoverflow: `%s` accesses %d bytes through argument %d but only %d bytes are left %s (callee asm 0x%s%s)" % (f, rd, i, top, what, va, "" if wr == 0 else ", writes %d" % wr))

    # ---- C / G / E
    def check_arity(self, fn, f, va, pr, args, line, st):
        k = len(args); reg = core.T().reg.get(int(va, 16)); o = D.ORIG.get(va); sl = o[0] if o else 0
        clusterstyle = bool(pr and any(re.search(r"\breg_\w+\s*$", p) for p in pr[1]))
        if reg and reg[0]:
            if clusterstyle and sl and k < len(reg[0]) + sl:
                self.add("G", "high", line, fn, f, st, "regstack: `%s` has %d register input(s) (%s) and its original reads %d stack slot(s) but the call passes %d argument(s)" % (f, len(reg[0]), "|".join(reg[0]), sl, k))
            if clusterstyle and pr and pr[0] is not None and sl and pr[0] < len(reg[0]) + sl:
                self.add("G", "high", pr[4], fn, f, "extern %s (%d params)" % (f, pr[0]), "regstack: the declaration lists %d parameter(s) but %d register input(s) + %d stack slot(s) of the original are needed" % (pr[0], len(reg[0]), sl))
            if not clusterstyle and va in INTERIOR and va not in D.REGVA:
                self.add("E", "high", line, fn, f, st, "regcallee_int: `%s` (0x%s) takes %s in registers (reginputs_interior.csv) but is called as plain cdecl" % (f, va, "|".join(reg[0])))
            return
        try:
            import gsfix; sl = gsfix.ARITY.get(f, sl)      # library callees whose Ghidra frame swallowed the caller's frame (RwStreamFindChunk: 20 slots, really 4)
        except ImportError: pass
        if o and ("dyn" in o[3] or sl > 12): return      # untracked esp: slots may be locals; no game function takes more than 12 stack words
        if not sl:
            pm = D.PMALL.get(va)      # no original-body row (callee inside a gap_ item): the pushes pending at its call sites
            if not o and pm and pm[0] > k and pm[1] >= 2 and (pr is None or pr[0] is None) and not re.search(r"printf", f) and va not in D.GT:
                self.add("C", "medium" if pm[1] < 3 else "high", line, fn, f, st, "callarity: `%s` is called with %d argument(s), every original call site (%d) has >= %d pushes pending (call_pushmin.txt; no orig_stackargs row)" % (f, k, pm[1], pm[0]))
            return
        fl = o[3]
        if not (fl & {"addr", "dyn"}) and (pr is not None or f in D.DPROTO): return      # exact slots + a declaration: defectlint class 8 / 10
        if k < sl: self.add("C", "high", line, fn, f, st, "callarity: `%s` is called with %d argument(s), its original body reads %d stack slot(s) (orig_stackargs.txt%s)" % (f, k, sl, ", " + ",".join(sorted(fl)) if fl != {"-"} else ""))

    # ---- B
    def check_kinds(self, fn, f, va, args, frames, loose, myorig, line, st):
        if va in D.REGVA: return
        kinds = ["s" if frame_ref(a, frames) or loose_ref(a, loose) else "o" for a in args]
        inst = myorig.get(va)
        if not inst: return
        for i, k in enumerate(kinds):
            if k == "s" and all(len(t) > i and t[i] != "s" for t in inst):
                self.add("B", "high", line, fn, f, st, "argkind: a stack address is passed as argument %d of `%s`, the original passes %s there (%d call instance(s) in its own body)" % (i, f, "/".join(sorted({t[i] for t in inst})), len(inst)))

    # ---- F
    def check_float(self, fn, f, pr, args, line, st):
        for i, a in enumerate(args):
            if i >= len(pr[1]): break
            if "*" in pr[1][i] or not re.search(r"\b(float|double|RwReal)\s*\w*\s*$", pr[1][i].strip()): continue
            q = strip_casts(a)
            if re.fullmatch(r"0x[0-9a-fA-F]{7,8}", q) and int(q, 16) >= 0x10000000 and not re.match(r"\(\s*(float|double)\s*\)", a):
                self.add("F", "high", line, fn, f, st, "floatlit: argument %d of `%s` is a float parameter but gets the integer literal %s (converted as an integer, not the bit pattern)" % (i, f, q))


INTERIOR = set()
def load_interior():
    p = os.path.join(G, "output", "reginputs_interior.csv")
    if os.path.exists(p):
        for r in csv.DictReader(open(p)): INTERIOR.add(r["addr"].lower())


def files_of(args):
    out = []
    for pat in (args or SCOPE):
        pat = pat if os.path.isabs(pat) else os.path.join(G, pat)
        for p in sorted(glob.glob(pat)):
            out += sorted(glob.glob(os.path.join(p, "*.c"))) if os.path.isdir(p) else [p]
    return out


def weak_equal():
    n = 0
    for r in csv.DictReader(open(os.path.join(G, "output", "diffemu_results.csv"))):
        if r["verdict"] != "EQUAL": continue
        why = []
        if float(r["coverage_orig"] or 0) < 0.6: why.append("coverage %s" % r["coverage_orig"])
        m = re.search(r"skipped (\d+)", r["note"])
        if m and int(m.group(1)) > int(r["cases"] or 0): why.append("skipped %s of %s cases" % (m.group(1), r["cases"]))
        if "loop-bound patch" in r["note"]: why.append("loop patched")
        if why: n += 1; print("weak EQUAL %-44s %s" % (r["name"], "; ".join(why)))
    print("weak EQUAL: %d EQUAL verdicts prove little (output/diffemu_results.csv)" % n)


SELF = """/*0047b590*/ extern int car_get_forward();
/*00418dd0*/ extern int ai_drive_a();
void bad(int car)
{
  struct {
    int a;
    undefined1 v [4];
    undefined4 tail;
  } frame;

  car_get_forward(frame.v,car);
  ai_drive_a();
}
void good(int car)
{
  struct {
    int a;
    RwV3d v;
  } frame;

  car_get_forward(&frame.v,car);
}
"""
def selftest():
    D.load_gt(); load_interior(); rows = Finder("selftest.c", SELF, D.load_decls()).run()
    got = sorted((r["cls"], r["function"]) for r in rows)
    assert got == [("A", "bad"), ("C", "bad")], got
    print("stack_lint selftest OK")


def main():
    a = sys.argv[1:]
    if "--selftest" in a: return selftest()
    if "--weak-equal" in a: return weak_equal()
    allv = "--all" in a; out_csv = os.environ.get("STACKLINT_OUT") or os.path.join(G, "output", "stack_lint.csv")
    if "--csv" in a: i = a.index("--csv"); out_csv = a[i + 1]; del a[i:i + 2]
    quiet = "--quiet" in a
    a = [x for x in a if not x.startswith("--")]
    D.load_gt(); gd = D.load_decls(); load_interior()
    rows = []
    for f in files_of(a):
        rel = os.path.relpath(f, G)
        try: rows += Finder(rel, open(f, errors="replace").read(), gd).run()
        except Exception as e: print("stack_lint: %s: %s %s" % (rel, type(e).__name__, e), file=sys.stderr)
    for gen in ("origargs.py", "reginputs2.py"):      # stale generated data
        if os.path.getmtime(os.path.join(G, "output", "orig_stackargs.txt")) < os.path.getmtime(os.path.join(H, gen)):
            rows.append(dict(cls="D2", sev="medium", file="output/orig_stackargs.txt", line=0, function="-", symbol=gen, text="", note="output/orig_stackargs.txt is older than tools/%s: regenerate (python3 tools/origargs.py)" % gen)); break
    allow = set(); ap = os.path.join(G, "src", "stacklint_allow.txt")
    if os.path.exists(ap):
        for l in open(ap):
            q = l.split(None, 2)
            if len(q) == 3 and not l.startswith("#"): allow.add((q[0], q[1]))
    rows = [r for r in rows if (r["function"], r["cls"]) not in allow]
    rows = list({(r["cls"], r["file"], r["line"], r["symbol"], r["note"]): r for r in rows}.values())
    rows.sort(key=lambda r: (r["cls"], r["file"], r["line"]))
    with open(out_csv, "w", newline="") as fh:
        w = csv.DictWriter(fh, ["cls", "sev", "file", "line", "function", "symbol", "text", "note"], extrasaction="ignore"); w.writeheader(); w.writerows(rows)
    names = "A B C D2 E F G H M".split()
    print("stack_lint: %d finding(s) (%s)  -> %s" % (len(rows), ", ".join("%s %d" % (c, sum(1 for r in rows if r["cls"] == c)) for c in names), os.path.relpath(out_csv, G)))
    if not quiet:
        for r in rows: print("%s:%s: %s (%s) %s: %s" % (r["file"], r["line"], r["cls"], r["sev"], r["function"], r["note"]))
    return 0


if __name__ == "__main__": sys.exit(main())
