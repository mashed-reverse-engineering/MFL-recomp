"""diffemu cluster mode (`diffemu.py --cluster ROOT`): a cluster = one register convention (usercall) function ROOT + its not-yet-C callers, written as natural C in ONE file
(output/cluster_pilot/<root>.c).  Original side: the ORIGINAL bytes of every member (ROOT with its registers, the callers call it the original way).  Our side: the compiled C file, the
members placed next to each other (calls between members stay inside our code, also static helpers); ROOT is entered with its NATURAL parameters on the stack (the entry thunk of
bridgegen is emulated by the harness: the case's natural arguments are loaded into the registers of output/regsigs.csv for the original side).  Only callees outside the cluster
are stubbed (identically on both sides, hooked at their original address).  Every member is an entry point; verdict per entry as in diffemu.py.
Spec file (output/cluster_pilot/clusters.txt), one line per cluster:  root | file.c | entries (root first) | natural parameter order of root | stub returns
 order: one token per natural parameter of ROOT: a register name (eax ecx edx ebx esi edi) or sN = original stack slot N (dword index);  stub returns: `name=f` callee returns a float in ST0; `name=@esi,s1` the callee outside the cluster is already natural C: the original passes its natural parameters in these registers / stack slots, our side on the stack (cdecl)."""
import os, struct
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

def words(cls): return 2 if cls in ("d", "ll") else 1

class Cluster:
    def __init__(c, spec, obj, srcdefs):
        tb = T(); c.spec = spec; names = spec["entries"]; c.lay = layout(obj, names)
        addr = {n: OURS + o for n, (o, _) in c.lay.items()}; lits = bytearray(); lit_at = {}; blob = bytearray(max(o + s for o, s in c.lay.values()))
        for n, (o, s) in c.lay.items():
            code, _ = core.our_code(obj, n, OURS + o, addr, lits, lit_at); blob[o:o + len(code)] = code
        c.code, c.lits = bytes(blob), bytes(lits)
        c.vas = {n: min(tb.sym[n]) for n in names}; c.sz = {n: tb.size.get(c.vas[n], 0x400) for n in names}
        member_vas = set(c.vas.values()); real = core.real_helpers(); ext_o, ext_u = set(), set(); c.leaders_o, c.leaders_u = set(), set()
        for n, va in c.vas.items():
            l, e = core.scan(core.img_read, va, va, va + c.sz[n]); c.leaders_o |= l; ext_o |= e
        for n, (o, s) in c.lay.items():
            l, e = core.scan(lambda a, k: c.code[a - OURS:a - OURS + k], OURS + o, OURS, OURS + len(c.code)); c.leaders_u |= l; ext_u |= e
        c.imports, c.islots = core.import_stubs(); c.callees = {}
        for t in sorted((ext_o | ext_u) - real - member_vas):
            if core.BASE <= t < core.IMGEND and not tb.rev.get(t, "").startswith("LAB_"):
                ci = core.callee_info(t)
                if ci.name in spec["stubret"]: ci.ret = spec["stubret"][ci.name]
                if ci.name in spec["natural"]: ci.natural = spec["natural"][ci.name]
                c.callees[t] = ci
        c.cov_o, c.cov_u = set(), set(); c.defs = srcdefs

    def entry(c, name, sig):
        """a Fn-like object for one entry point"""
        f = Fn.__new__(Fn); tb = T(); va = c.vas[name]
        f.name, f.va, f.sig, f.notes = name, va, sig, []; f.hi = max(v + c.sz[n] for n, v in c.vas.items()); f.olo = min(c.vas.values())
        f.code, f.lits, f.oentry = c.code, c.lits, OURS + c.lay[name][0]
        f.imports, f.islots, f.callees = c.imports, c.islots, c.callees
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
