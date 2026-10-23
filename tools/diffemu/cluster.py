"""diffemu cluster mode (`diffemu.py --cluster ROOT`): a cluster = one register convention (usercall) function ROOT + its not-yet-C callers, written as natural C in ONE file
(output/cluster_pilot/<root>.c).  Original side: the ORIGINAL bytes of every member (ROOT with its registers, the callers call it the original way).  Our side: the compiled C file, the
members placed next to each other (calls between members stay inside our code, also static helpers); ROOT is entered with its NATURAL parameters on the stack (the entry thunk of
bridgegen is emulated by the harness: the case's natural arguments are loaded into the registers of output/regsigs.csv for the original side).  Only callees outside the cluster
are stubbed (identically on both sides, hooked at their original address).  Every member is an entry point; verdict per entry as in diffemu.py.
Spec file (output/cluster_pilot/clusters.txt), one line per cluster:  root | file.c | entries (root first) | natural parameter order of root | stub returns
 order: one token per natural parameter of ROOT: a register name (eax ecx edx ebx esi edi) or sN = original stack slot N (dword index);  stub returns: `name=f` callee returns a float in ST0; `name=@esi,s1` the callee outside the cluster is already natural C: the original passes its natural parameters in these registers / stack slots, our side on the stack (cdecl)."""
# Additions (2026-10-18, hand round "hb1"; none of them touches core.py, whose hash keys the diffemu cache; all are installed by importing this module):
#  - entry_va / entry_size: roots that are not in the symbol table (gap_<va>, FUN_<va>, thunk_FUN_<va>) resolve to their address; size from output/callgraph_waves.csv
#  - indirect calls (`call [eax+N]`, vtables): the callee pops its arguments (thiscall / stdcall / __fastcall pointer types) unless `add esp` follows (indirect_pops); the trace entry carries
#    (ecx where the original sets it = the `this` of a thiscall, dwords popped): C that calls a thiscall / stdcall method cdecl or drops `this` differs.  Write such calls as
#    `(*(int (__fastcall **)(int *self, int unused_edx, int arg))(*self + N))(self, 0, arg)` / `__stdcall` pointer types.  Calls into the image / the stack / wild data pages are stubbed.
#  - fs:[0] (SEH frame setup of the original) goes to a scratch word (patch_fs): a C file needs no EH frame; `dllimport` externs of the file give imports without a prototype their argument count
#  - <spec dir>/frame_allow.txt (frame objects among C++ EH words), keeps_regs.txt (callee leaves ecx / edx alone though its jump table hides that): same lines as src/diffemu_frame_allow.txt
# Re-verification round (2026-10-19, 16 clusters had flipped to DIFF / INCONCLUSIVE; none of these touches the cache key except the two core.py edits):
#  - real helpers (_ftol ...) run for real: the whole-image code hook of _run_w treated their instructions as a call into the void (`indirect 004b2b48`), leaving the x87 value on the stack (dead-temp "memory: 2 bytes differ" at FPOUT)
#  - indirect_pops: look 40 instructions ahead for the batched `add esp` (was 9: `push eax; call [eax+0x48]` x5, `add esp, 0x14` = cdecl, not 4 popped dwords per call), and `add esp, F + 4m` behind `sub esp, F` cleans the pending arguments too
#  - a prototype of the cluster file for an import beats names/MFL_import_protos.txt (GetDlgItem: 5 there, really 2); the literal pool / argument windows are data (a call into them is stubbed); the original sees our code bytes as data (a pointer that wraps into OURS read zeros there)
#  - coverage guidance (seed.py) is on: float constants of the code, switch selectors 0..40, fields tested against zero cleared together; x87 stub results / float out parameters from small pools
import os, re, struct
import core
from core import T, ROOT, OURS, SCR, Fn

SPEC = os.path.join(ROOT, "output", "cluster_pilot", "clusters.txt")

def load_spec(path=SPEC):
    out = {}
    for l in open(path):
        if l.startswith("#") or l.count("|") < 4: continue
        p = [x.strip() for x in l.split("|")]
        out[p[0]] = dict(root=p[0], file=os.path.join(os.path.dirname(path), p[1]), entries=p[2].split(), order=p[3].split(), stubret=dict(x.split("=") for x in p[4].split() if "=" in x and not x.split("=")[1].startswith("@")),
                       natural={x.split("=")[0]: x.split("=")[1][1:].split(",") for x in p[4].split() if "=@" in x})
    return out

def layout(obj, names):
    """-> {symbol: (offset, size)}: the members and every static function of the object they (transitively) call, 16 byte aligned"""
    co = core.Coff(obj); tb = T(); todo = list(names); lay = {}; pos = 0
    while todo:
        n = todo.pop(0)
        if n in lay: continue
        code, rel, fsn, fval = co.function(n); size = len(code.rstrip(b"\xcc") if rel else code.rstrip(b"\x90\xcc"))
        lay[n] = (pos, size); pos = (pos + size + 15) & ~15
        for off, rt, si, add in rel:
            nm, sval, sn = co.syms[si]; un = core.cmpfn._undecorate(nm)
            if sn > 0 and sn != fsn and co.secs[sn - 1][4] & 0x20 and un not in lay and (un in names or (un not in tb.sym and "_" + un not in tb.sym)): todo.append(un)
    return lay

def entry_va(tb, n):
    """address of an entry: symbol table, else the address in the name of an unnamed root (gap_4acfc0, FUN_004acfc0, thunk_FUN_.., func_0x..)"""
    if n in tb.sym: return min(tb.sym[n])
    m = re.search(r"(?:gap_|FUN_|func_0x)([0-9a-f]{6,8})$", n)
    if not m: raise KeyError(n)
    return int(m.group(1), 16)

_WAVES = {}
def entry_size(n, va):
    """size of a function fnsizes.txt does not list (gap_ items): bytes column of output/callgraph_waves.csv"""
    if not _WAVES:
        import csv
        for r in csv.DictReader(open(os.path.join(ROOT, "output", "callgraph_waves.csv"))): _WAVES[r["name"]] = int(r["bytes"] or 0)
    return _WAVES.get(n) or 0x400

def indirect_pops(va, size, read=None, ours=False):
    """return address -> dwords popped by the callee, for the INDIRECT calls (`call [eax+0x38]`, `call reg`) of the original function at va.  The original calls virtual methods thiscall (`push arg; mov ecx,this; call [eax+N]`,
    callee `ret N`), the stub for a call through data would leave the arguments on the stack (esp delta orig 4 ours 4 ...).  Heuristic: the pushes in the straight line directly before the call, minus the prologue pushes
    (register saves / SEH frame).  A wrong guess unbalances the stack and shows as DIFF, never as a false EQUAL."""
    ins = []; a = va; read = read or core.img_read
    while a < va + size:
        i = next(core.CS.disasm(read(a, 16), a, 1), None)
        if i is None: a += 1; continue
        ins.append(i); a += i.size
    pro = 0      # instructions that belong to the prologue (leading pushes, the SEH frame setup)
    for k, i in enumerate(ins):
        if i.mnemonic == "push" or (i.mnemonic == "mov" and "fs:" in i.op_str): pro = k + 1
        else: break
    out = {}
    frame = next((int(i.op_str[5:], 0) for i in ins[:8] if i.mnemonic == "sub" and re.fullmatch(r"esp, (?:0x[0-9a-f]+|\d+)", i.op_str)), 0)      # `sub esp, F` of the prologue
    wr_ = lambda y, r: y.mnemonic in ("mov", "lea", "xor", "movzx", "movsx", "pop") and y.op_str.split(",")[0] == r
    for k, i in enumerate(ins):
        if i.mnemonic != "call" or not ("[" in i.op_str or i.op_str in ("eax", "ecx", "edx", "ebx", "esi", "edi", "ebp")) or i.op_str.startswith("dword ptr [0x"): continue
        j = k - 1; tc = False; pe = False; dec = False
        while j >= pro:      # the window: straight line before the call
            m = ins[j].mnemonic
            if m in ("call", "jmp", "ret", "pop", "cmp", "test", "leave") or m.startswith("j") or (m in ("add", "sub") and ins[j].op_str.startswith("esp")): break
            j -= 1
        win = ins[j + 1:k]; n = 0
        cm_ = re.match(r"dword ptr \[(e[a-d]x|e[sd]i|ebp)[ +\]]", i.op_str); cb_ = cm_.group(1) if cm_ else None      # `mov ecx, [edi]; push edi; call [ecx+N]`: the pushed register is the COM `this` (its vtable is loaded from it), not a register save
        for q in range(j + 1, k):
            x = ins[q]
            if x.mnemonic != "push": continue
            r = x.op_str
            if r in ("ebx", "esi", "edi", "ebp") and not (cb_ and any(y.mnemonic == "mov" and y.op_str.startswith(cb_ + ", dword ptr [" + r + "]") for y in ins[max(0, k - 8):q])) and not any(wr_(y, r) for y in ins[j + 1:q]) and any(wr_(y, r) and y.mnemonic != "pop" for y in ins[q + 1:]) and any(y.mnemonic == "pop" and y.op_str == r for y in ins[q + 1:]): continue      # `push esi ... mov esi, ... pop esi`: a register save (esi is written afterwards, restored later), not an argument
            n += 1
        for x in reversed(win):      # which ecx is it: the `this` register or a temporary
            if x.mnemonic == "push" and x.op_str == "ecx": pe = True
            if not dec and x.mnemonic in ("mov", "lea", "xor", "movzx", "movsx") and x.op_str.split(",")[0] == "ecx": dec = True; tc = not pe
        q = k + 1; run = 0      # `add esp, K` behind the call (before the next jump / return / pop; `run` = dwords pushed after this call that a later `add esp` may clean up first: batched cleanup) = cdecl; else the callee (stdcall / thiscall / __fastcall pointer type) pops the pushed arguments
        while q < min(len(ins), k + 40) and not (ins[q].mnemonic in ("jmp", "ret", "pop", "leave") or ins[q].mnemonic.startswith("j")):
            if ins[q].mnemonic == "push": run += 1
            if ins[q].mnemonic == "add" and ins[q].op_str.startswith("esp"):
                kk = int(ins[q].op_str.split(",")[1], 0) // 4
                if run < kk <= 24: n = 0; break      # (a bigger `add esp` is the frame teardown)
                if kk > 24:      # the frame teardown: `sub esp, F` at the entry; `add esp, F + 4 * m` (m >= the pending argument dwords) cleans the arguments too = cdecl (`push 2; push 7; call [eax+0x20]; add esp, 0xa8` after `sub esp, 0xa0`)
                    if frame and 4 * kk - frame >= 4 * n > 0: n = 0
                    break
                run -= kk
            q += 1
        if "[ecx" in i.op_str: tc = False      # `mov ecx,[eax]; call [ecx+N]`: ecx is the vtable pointer, `this` is elsewhere (stdcall COM method)
        out[i.address + i.size] = (n, tc)
    return out

TIB = core.SCR + 0xff00      # where the patched fs:[0] accesses of the original go (scratch area: not compared, not page 0)
def patch_fs(va, size):
    """the original's SEH frame setup (`mov eax, fs:[0]` / `mov fs:[0], esp`) would touch linear address 0 in the emulator, where it pollutes the low memory both sides read through wild pointers:
    rewrite those instructions to access TIB (prefix removed, a nop appended)"""
    img = bytearray(core.IMG); a = va
    while a < va + size:
        i = next(core.CS.disasm(bytes(img[a - core.BASE:a - core.BASE + 16]), a, 1), None)
        if i is None: a += 1; continue
        b = bytes(i.bytes)
        if b[0] == 0x64 and "fs:[0]" in i.op_str and i.disp_offset == i.size - 4 and b[-4:] == b"\0\0\0\0":
            nb = b[1:-4] + TIB.to_bytes(4, "little") + b"\x90"; img[a - core.BASE:a - core.BASE + i.size] = nb
        a += i.size
    core.IMG = bytes(img)

def words(cls): return 2 if cls in ("d", "ll") else 1

class Cluster:
    def __init__(c, spec, obj, srcdefs):
        tb = T(); c.spec = spec; names = spec["entries"]; c.lay = layout(obj, names)
        addr = {n: OURS + o for n, (o, _) in c.lay.items()}; lits = bytearray(); lit_at = {}; blob = bytearray(max(o + s for o, s in c.lay.values()))
        for n, (o, s) in c.lay.items():
            code, _ = core.our_code(obj, n, OURS + o, addr, lits, lit_at); blob[o:o + len(code)] = code
        c.code, c.lits = bytes(blob), bytes(lits)
        c.vas = {n: entry_va(tb, n) for n in names}; c.sz = {n: tb.size.get(c.vas[n]) or entry_size(n, c.vas[n]) for n in names}
        member_vas = set(c.vas.values()); real = core.real_helpers(); ext_o, ext_u = set(), set(); c.leaders_o, c.leaders_u = set(), set()
        for n, va in c.vas.items():
            l, e = core.scan(core.img_read, va, va, va + c.sz[n]); c.leaders_o |= l; ext_o |= e
        for n, (o, s) in c.lay.items():
            l, e = core.scan(lambda a, k: c.code[a - OURS:a - OURS + k], OURS + o, OURS, OURS + len(c.code)); c.leaders_u |= l; ext_u |= e
        try:      # <spec dir>/frame_allow.txt: `function callee argindex reason` (same format as src/diffemu_frame_allow.txt): frame objects whose neighbouring words are residue of the original's C++ EH frame
            for l_ in open(os.path.join(os.path.dirname(spec["file"]), "frame_allow.txt")):
                q_ = l_.split(None, 3)
                if len(q_) == 4 and not l_.startswith("#"): core.frame_allow().add((q_[0], q_[1], int(q_[2])))
        except OSError: pass
        c.imports, c.islots = core.import_stubs(); c.callees = {}
        # imports without a prototype in names/MFL_import_protos.txt (EnterCriticalSection ...): the stdcall prototype of the cluster file gives the argument count (else the stub pops nothing)
        for m_ in re.finditer(r"extern\s+__declspec\(dllimport\)\s+[^;(]*?\b(\w+)\s*\(([^)]*)\)", open(spec["file"]).read()):
            n_ = 0 if m_.group(2).strip() in ("", "void") else m_.group(2).count(",") + 1
            for ci in c.imports.values():
                if ci.name == m_.group(1) and (not ci.known or m_.group(2).strip()): ci.pop, ci.nargs, ci.known = 4 * n_, n_, True      # a prototype in the file beats names/MFL_import_protos.txt (GetDlgItem: 5 there, really 2: the votes of the pushes before the IAT call)
        for t in sorted((ext_o | ext_u) - real - member_vas):
            if core.BASE <= t < core.IMGEND and not tb.rev.get(t, "").startswith("LAB_"):
                ci = core.callee_info(t)
                if ci.name in spec["stubret"]: ci.ret = spec["stubret"][ci.name]
                if ci.name in spec["natural"]: ci.natural = spec["natural"][ci.name]
                c.callees[t] = ci
        c.cov_o, c.cov_u = set(), set(); c.defs = srcdefs; c.indpop = {}
        for n, v in c.vas.items(): c.indpop.update(indirect_pops(v, c.sz[n]))
        for n, v in c.vas.items(): patch_fs(v, c.sz[n])
        c.indpop_u = {}
        for n, (o, z) in c.lay.items(): c.indpop_u.update(indirect_pops(OURS + o, z, lambda a, k: c.code[a - OURS:a - OURS + k], True))

    def entry(c, name, sig):
        """a Fn-like object for one entry point"""
        f = Fn.__new__(Fn); tb = T(); va = c.vas[name]
        f.name, f.va, f.sig, f.notes = name, va, sig, []; f.hi = max(v + c.sz[n] for n, v in c.vas.items()); f.olo = min(c.vas.values())
        f.code, f.lits, f.oentry = c.code, c.lits, OURS + c.lay[name][0]
        f.imports, f.islots, f.callees = c.imports, c.islots, c.callees; f.indpop, f.indpop_u = c.indpop, c.indpop_u
        # coverage per entry: only the blocks of THIS function (the span between root and caller holds other functions: root coverage 0.1 for a cluster whose caller is far away)
        o, z = OURS + c.lay[name][0], OURS + c.lay[name][0] + c.lay[name][1]
        f.leaders_o = {a for a in c.leaders_o if va <= a < va + c.sz[name]}; f.leaders_u = {a for a in c.leaders_u if o <= a < z}
        f.cov_o, f.cov_u, f.mutate, f.noindargs = set(), set(), None, True; f.regin = tb.reg.get(va, ([], 0, []))[0]; f.retn = tb.reg.get(va, ([], 0, []))[1]
        if name == c.spec["root"]:
            order = c.spec["order"]; nw = sum(words(p[0]) for p in sig.params)
            if len(order) != len(sig.params): raise ValueError("%s: order has %d tokens, the natural prototype %d parameters" % (name, len(order), len(sig.params)))
            f.notes.append("root: register inputs %s, natural parameters %s, ret %d" % ("|".join(f.regin) or "-", " ".join(order), f.retn))
            def mapcase(case, ours, sig=sig, order=order):
                if ours: return case.args, {}
                regs, slots, i = {}, {}, 0
                for p, tok in zip(sig.params, order):
                    w = case.args[i:i + words(p[0])]; i += len(w)
                    if tok[0] == "s":
                        for k, v in enumerate(w): slots[int(tok[1:]) + k] = v
                    else: regs[tok] = w[0]
                return [slots.get(k, 0) for k in range(max(slots) + 1)] if slots else [], regs
            f.mapcase = mapcase; f.skipsaved = set(order)
            f.espok = lambda eo, eu, retn=f.retn, nw=nw: eo == 4 + retn and eu in (4, 4 + 4 * nw)      # original `ret N`; ours cdecl (0) or stdcall (all natural parameters)
        return f


# the original side pops the arguments of indirect (thiscall) calls: wrap core.side / Machine._indirect here (not in core.py: its hash keys the diffemu cache)
_CUR = []
_side, _ind0 = core.side, core.Machine._indirect
def _ind(m, uc, addr, size, ud):
    """the stub of a call through data; the trace entry carries (ecx, callee pops N dwords) of the call site: ecx only where the code sets it (thiscall `this`; a stdcall COM call leaves garbage there), N = what the callee pops
    (stdcall / thiscall) vs. the caller (cdecl): C that calls a thiscall method cdecl / drops the `this` register or calls a stdcall method cdecl differs"""
    from unicorn.x86_const import UC_X86_REG_ECX, UC_X86_REG_ESP
    _ind0(m, uc, addr, size, ud); t = m.trace[-1]
    ra = struct.unpack("<I", bytes(uc.mem_read(uc.reg_read(UC_X86_REG_ESP), 4)))[0]
    n, tc = getattr(_CUR[0], "indpop_u" if m.ours else "indpop", {}).get(ra, (0, False)) if _CUR else (0, False)
    m.trace[-1] = (t[0], t[1], (m.norm(uc.reg_read(UC_X86_REG_ECX)) if (tc or m.ours) else None, n))

def _side_w(fn, case, ours, *a, **k):
    _CUR[:] = [fn]; return _side(fn, case, ours, *a, **k)
def _ind_w(m, uc, addr, size, ud):
    _ind(m, uc, addr, size, ud)
    pops = getattr(_CUR[0], "indpop_u" if m.ours else "indpop", None) if _CUR else None
    if pops:
        from unicorn.x86_const import UC_X86_REG_ESP, UC_X86_REG_EIP
        sp = uc.reg_read(UC_X86_REG_ESP); ra = struct.unpack("<I", bytes(uc.mem_read(sp, 4)))[0]; n = pops.get(ra, (0, 0))[0]
        if n: uc.reg_write(UC_X86_REG_ESP, sp + 4 + 4 * n); uc.reg_write(UC_X86_REG_EIP, ra)
_cmp = core.compare
def _cmp_w(fn, mo, ro, mu, ru):
    """ecx of an indirect call is only compared where the ORIGINAL sets it (`mov ecx, this`): a passed-through ecx (`mov eax,[ecx]; call [eax+N]`) is not visible in the code, our side may set it or not"""
    to, tu = ro.get("trace"), ru.get("trace")
    if to and tu:
        tu = ru["trace"] = list(tu)
        for i, (a, b) in enumerate(zip(to, tu)):
            if a[0].startswith("indirect") and b[0].startswith("indirect") and isinstance(a[2], tuple) and len(a[2]) == 2 and a[2][0] is None and isinstance(b[2], tuple) and len(b[2]) == 2: tu[i] = (b[0], b[1], (None, b[2][1]))
    return _cmp(fn, mo, ro, mu, ru)
core.compare = _cmp_w
core.side, core.Machine._indirect = _side_w, _ind_w
_HR = []
def _helper_ranges():
    """address ranges of core.real_helpers() (_ftol, 64 bit arithmetic, _CIsin ...) and the image code they jump / call into (_pow_pentium4 ...): their instructions are real code, never a call into the void"""
    if not _HR:
        tb = T(); todo = sorted(core.real_helpers()); seen = set()
        while todo:
            h = todo.pop()
            if h in seen: continue
            seen.add(h); z = h + (tb.size.get(h) or 0x100); _HR.append((h, z))
            todo += [t for t in core.scan(core.img_read, h, h, z)[1] if core.BASE <= t < core.IMGEND and t not in seen]
    return _HR
_run = core.Machine.run
def _run_w(m, entry, args, regs, fill, lo, hi, count=core.MAXINS):
    """an indirect call into the image (a data pointer that happens to hold a code address) would run unrelated original code, unbalancing the stack of the thiscall: stub it like a call into the void"""
    from unicorn import UC_HOOK_CODE
    from unicorn.x86_const import UC_X86_REG_ESP, UC_X86_REG_EIP
    def cb(uc, addr, size, ud):
        if addr in m.hooked or addr == entry or any(a_ <= addr < z_ for a_, z_ in _helper_ranges()): return      # (real helpers run for real: _ftol ...)
        _ind(m, uc, addr, size, ud)      # the unpatched stub: trace entry + result
        sp = uc.reg_read(UC_X86_REG_ESP); ra = struct.unpack("<I", bytes(uc.mem_read(sp, 4)))[0]
        n = getattr(_CUR[0], "indpop_u" if m.ours else "indpop", {}).get(ra, (0, 0))[0] if _CUR else 0
        uc.reg_write(UC_X86_REG_ESP, sp + 4 + 4 * n); uc.reg_write(UC_X86_REG_EIP, ra)
    m.uc.hook_add(UC_HOOK_CODE, cb, begin=core.OURS + 0x10000, end=core.STUBS - 1)      # the literal pool and the argument windows (ARGBUF) are data: a call into them is a call through a data pointer (wild pages are mapped read / write only by _unm_w)
    for b, e in (((core.BASE, core.IMGEND - 1),) if m.ours else ((core.BASE, lo - 1), (hi + 0x40, core.IMGEND - 1))) + ((core.STACK, core.STACK + core.STK - 1),):      # (the stack holds data, never code)
        if e > b: m.uc.hook_add(UC_HOOK_CODE, cb, begin=b, end=e)
    if not m.ours and _CUR:      # our compiled code is DATA to the original: a pointer that wraps into [OURS, OURS+len) (esi = ARGBUF 0x72000000 + 4 * 0xBF800000, a float -1.0 pattern in an index) reads the same bytes on both sides
        m.uc.mem_write(core.OURS, _CUR[0].code[:0x10000])
    return _run(m, entry, args, regs, fill, lo, hi, count)
core.Machine.run = _run_w


# coverage guidance (seed.py, opt-in there): cluster mode switches it on and adds the float constants the original loads / compares (x87 `fcomp [0x5dd338]`: the branch needs a field around that value) to the immediates pool
import seed as _seed
_hints = _seed.hints
def _hints_w(fn):
    if hasattr(fn, "_seedh"): return fn._seedh
    imms, fields, ptrf, glob = _hints(fn)
    fl = set(); a = fn.va; n = 0
    import struct as _st
    while a < fn.hi and n < 4000:
        i = next(core.CS.disasm(core.img_read(a, 16), a, 1), None)
        if i is None: a += 1; continue
        a += i.size; n += 1
        if i.mnemonic.startswith("f") and i.operands and i.operands[-1].type == core.CX.X86_OP_MEM and i.operands[-1].mem.base == 0 and i.operands[-1].mem.index == 0 and i.operands[-1].size == 4 and core.BASE <= i.operands[-1].mem.disp < core.IMGEND:
            try: v = _st.unpack("<f", core.img_read(i.operands[-1].mem.disp, 4))[0]
            except Exception: continue
            for w in (v, -v, v * 1.01 + 0.001, v * 0.99 - 0.001, 0.0):
                try: fl.add(_st.unpack("<I", _st.pack("<f", w))[0])
                except Exception: pass
    for v in (0.0, 1.0, -1.0, 0.5, 2.0, 10.0, 100.0, 0.001): fl.add(_st.unpack("<I", _st.pack("<f", v))[0])
    fl |= set(range(0, 41))      # switch selectors (jump tables)
    # `mov eax,[esi+0x6c]; test eax,eax; jne`: the field is tested against zero (no immediate compare in the code): 0 and 1 are the interesting values
    a = fn.va; n = 0; last = {}; fields = {k: set(v) for k, v in fields.items()}
    while a < fn.hi and n < 4000:
        i = next(core.CS.disasm(core.img_read(a, 16), a, 1), None)
        if i is None: a += 1; continue
        a += i.size; n += 1; ops = i.operands
        if i.mnemonic == "mov" and len(ops) == 2 and ops[0].type == core.CX.X86_OP_REG and ops[1].type == core.CX.X86_OP_MEM and ops[1].size == 4 and ops[1].mem.base not in (0, core.CX.X86_REG_ESP, core.CX.X86_REG_EBP) and ops[1].mem.index == 0 and 0 <= ops[1].mem.disp < 0x400:
            last[ops[0].reg] = (ops[1].mem.disp, n)
        elif i.mnemonic == "test" and len(ops) == 2 and ops[0].type == ops[1].type == core.CX.X86_OP_REG and ops[0].reg == ops[1].reg and ops[0].reg in last and n - last[ops[0].reg][1] <= 4:
            fields.setdefault((last[ops[0].reg][0], 4), set()).update({0, 1})
        elif i.mnemonic == "cmp" and len(ops) == 2 and ops[0].type == core.CX.X86_OP_MEM and ops[1].type == core.CX.X86_OP_REG and ops[0].size == 4 and 0 <= ops[0].mem.disp < 0x400: pass
    fn._zf = sorted({d for (d, sz), v in fields.items() if sz == 4 and 0 in v and 1 in v})
    fn._seedh = (sorted(set(imms) | fl)[:260], {k: sorted(v) for k, v in fields.items()}, ptrf, glob)
    return fn._seedh
_seed.hints = _hints_w
if os.environ.get("CLUSTER_SEED", "1") != "0": _seed.OFF = False; _seed.LIMIT = 0.97


# float out parameters: the stubbed callee writes a small integer (denormal float) behind every pointer to a local; in a function that computes with x87 the branches on that
# value (`> 1.0`, `< -1.0`) stay uncovered.  A quarter of the writes become a float from a small pool (same on both sides: derived from the call number and the case seed)
_callee0 = core.Machine._callee
_RPOOL = (0.0, 0.1, 0.2, 0.05, -0.1, -0.2, -0.05, 1.0, -1.0, 0.5, 0.9, 2.0, 100.0, 0.3, 0.29, -0.29)
_FPOOL = (0.0, 1.0, -1.0, 2.0, 0.5, -0.5, 30.0, 100.0, -100.0, 5.0, 0.01, 1000.0, 3.0, -3.0)
def _callee_w(m, uc, addr, size, ud):
    from unicorn.x86_const import UC_X86_REG_ESP
    c = m.hooked.get(addr); sp = uc.reg_read(UC_X86_REG_ESP)
    _callee0(m, uc, addr, size, ud)
    if c is None or not _CUR or not getattr(_CUR[0], "usesfp", False): return
    if c.ret == "f":      # float result of the stub (ST0): the default values are multiples of 0.125 in +-250: small intervals around 0 are almost never hit
        h = int(core.sha(c.va, m.cnt.get(addr, 0), m.seed, "fr"), 16)
        if h % 3 == 0: uc.mem_write(core.FPSRC, struct.pack("<d", _RPOOL[(h >> 4) % len(_RPOOL)]))
    if not c.nargs or getattr(c, "natural", None) or c.name.startswith("RpPTankAtomicLock"): return      # the lock struct {data, stride} written by core._callee must stay a pointer (a pool float behind it was a wild store address)
    try: args = struct.unpack("<%dI" % c.nargs, bytes(uc.mem_read(sp + 4, 4 * c.nargs)))
    except Exception: return
    i = m.cnt.get(addr, 0)
    for k, a in enumerate(args):
        if core.isstack(a):
            h = int(core.sha(c.va, i, m.seed, k, "fp"), 16)
            if h % 4 == 0: uc.mem_write(a, struct.pack("<f", _FPOOL[(h >> 4) % len(_FPOOL)]))
core.Machine._callee = _callee_w
_ent0 = Cluster.entry
def _entry_w(c, name, sig):
    f = _ent0(c, name, sig)
    if not hasattr(c, "usesfp"):
        c.usesfp = False
        for n, v in c.vas.items():
            a = v
            while a < v + c.sz[n]:
                i = next(core.CS.disasm(core.img_read(a, 16), a, 1), None)
                if i is None: a += 1; continue
                a += i.size
                if i.mnemonic.startswith("f") and i.mnemonic not in ("fwait",): c.usesfp = True
    f.usesfp = c.usesfp
    return f
Cluster.entry = _entry_w

_mut0 = _seed.Guide.mutate
def _mutate_w(g, c):
    """the original mutation, then the fields the code tests against zero are cleared together (`if (a == 0 && b == 0 && c == 0)` needs all of them at once)"""
    c2 = _mut0(g, c); zf = getattr(g.fn, "_zf", None)
    if zf and c2.wins and g.r.random() < 0.5:
        ws = [[a_, bytearray(b_)] for a_, b_ in c2.wins]; w = ws[0] if g.r.random() < 0.7 else g.r.choice(ws)
        for d in zf:
            if d + 4 <= len(w[1]) and g.r.random() < 0.7: w[1][d:d + 4] = b"\0\0\0\0"
        c2.wins = [(a_, bytes(b_)) for a_, b_ in ws]
    return c2
_seed.Guide.mutate = _mutate_w


# a wild data page (mapped on the first read / write of a pointer the case holds) is data, never code: a call through a data pointer into it ran random bytes (different on the two sides)
_unm0 = core.Machine._unmapped
def _unm_w(m, uc, access, addr, size, val, ud):
    from unicorn import UC_MEM_FETCH_UNMAPPED, UC_PROT_READ, UC_PROT_WRITE
    r = _unm0(m, uc, access, addr, size, val, ud)
    p = addr & ~0xfff
    if r and access != UC_MEM_FETCH_UNMAPPED and not (core.BASE <= p < core.IMGEND):
        try: uc.mem_protect(p, 0x1000, UC_PROT_READ | UC_PROT_WRITE)
        except Exception: pass
    return r
core.Machine._unmapped = _unm_w


# <spec dir>/keeps_regs.txt (`callee register [reason]`): callees whose bytes the register-write analysis cannot follow (a jump table: `jmp [eax*4+..]`) but that leave the register alone; the stub keeps the caller's value
_keeps0 = core.keeps_reg
_KEEP = set()
def _keeps_w(va, reg):
    return (va, reg) in _KEEP or _keeps0(va, reg)
core.keeps_reg = _keeps_w
_init0 = Cluster.__init__
def _init_w(c, spec, obj, srcdefs):
    try:
        for l_ in open(os.path.join(os.path.dirname(spec["file"]), "keeps_regs.txt")):
            q_ = l_.split()
            if len(q_) >= 2 and not l_.startswith("#"):
                for v_ in core.T().sym.get(q_[0], ()): _KEEP.add((v_, q_[1]))
    except OSError: pass
    _init0(c, spec, obj, srcdefs)
Cluster.__init__ = _init_w
