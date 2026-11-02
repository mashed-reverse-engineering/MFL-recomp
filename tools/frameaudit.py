#!/usr/bin/env python3
"""frameaudit.py [--all-tier2] [names...]: defectlint class 17 callee store-extent audit (also run by `defectlint.py --framesize`).  Needs the diffemu venv (re-execs itself).
For the ORIGINAL function and for OUR compiled object it lists every local address handed to a call (`lea r,[esp+x|ebp-x]` pushed, or still in ecx/edx/eax at the call) with c = the address
relative to the entry esp, and the ROOM of the object = distance to the next escaped local base above it (or to the top of the frame).  The ORIGINAL's callee is a direct call: its store / read extent
through that argument (stack_lint.extents) is the REQUIRED size; a vtable / indirect callee cannot be decoded, there the original's own room is the evidence.  Calls are aligned original <-> ours by
(callee, argument) sequence.  status: ok = our room >= max(required, original room) or >= required with required known; CHECK = our room is smaller than the original's (and than a known required extent).
Not covered: pointers to locals stored into other locals, register-saved pointers (esi/edi/ebx) that are not pushed, indirect callees' real extents (judge by hand, see ROADMAP T-Q1)."""
import os, sys, re, struct, csv, glob
try: import capstone
except ImportError:
    py = os.environ.get("DIFFEMU_PY") or os.path.expanduser("~/.cache/mfl_diffemu_venv/bin/python"); os.execv(py, [py] + sys.argv)
from difflib import SequenceMatcher
H = os.path.dirname(os.path.abspath(__file__)); G = os.path.normpath(os.path.join(H, ".."))
sys.path[:0] = [os.path.join(H, "diffemu"), H]
import core, diffemu, cmpfn, stack_lint
CX = core.CX; CS = core.CS
REGS = ("eax", "ecx", "edx")


def obj_code(obj, sym):
    """(code bytes, {offset of rel32/dir32 field: symbol name}) of function `sym` in a COFF object"""
    d = open(obj, "rb").read()
    machine, nsec, ts, symoff, nsym, opt, ch = struct.unpack_from("<HHIIIHH", d, 0)
    strtab = symoff + nsym * 18
    def nm(raw):
        if raw[:4] == b"\0\0\0\0": o = struct.unpack_from("<I", raw, 4)[0]; return d[strtab + o:d.index(b"\0", strtab + o)].decode()
        return raw.rstrip(b"\0").decode()
    secs = []
    for i in range(nsec):
        o = 20 + opt + i * 40
        rawsz, rawptr, relptr = struct.unpack_from("<III", d, o + 16); nrel = struct.unpack_from("<H", d, o + 32)[0]
        secs.append((rawsz, rawptr, relptr, nrel))
    names = {}; i = 0; found = None
    while i < nsym:
        o = symoff + i * 18; n = nm(d[o:o + 8]); value, secnum, typ, cls, aux = struct.unpack_from("<IhHBB", d, o + 8)
        names[i] = n
        if (n == "_" + sym or re.fullmatch(r"_%s@\d+" % re.escape(sym), n)) and secnum > 0: found = (secnum, value)
        i += 1 + aux
    if not found: raise KeyError(sym)
    secnum, value = found; rawsz, rawptr, relptr, nrel = secs[secnum - 1]
    # function extent: up to the next function symbol in the same section
    ends = []; i = 0
    while i < nsym:
        o = symoff + i * 18; v, sn, ty, cl, ax = struct.unpack_from("<IhHBB", d, o + 8)
        if sn == secnum and v > value and (ty >> 4) == 2: ends.append(v)
        i += 1 + ax
    end = min(ends) if ends else rawsz
    rel = {}
    for k in range(nrel):
        va, si, rt = struct.unpack_from("<IIH", d, relptr + k * 10)
        if value <= va < end: rel[va - value] = names.get(si, "?")
    return d[rawptr + value:rawptr + end], rel


def undecorate(n):
    if n.startswith("@"): return n[1:].split("@")[0]
    if n.startswith("_"): n = n[1:]
    return n.split("@")[0] if re.match(r"^\w+@\d+$", n) else n


def analyse(code, base, rel=None, symva=None):
    """-> dict(calls=[(ord, kind, target, [(argkey, c)]...)], ftop, ok): linear sweep with esp / pending-push tracking.  rel: {offset: callee symbol} for our objects (call field at off+1)."""
    ins = list(CS.disasm(code, base)); d = 0; ebp_c = None; pend = []; tag = {}; dmap = {}; prev = None
    calls = []; badret = []; ftop = 0; base_i = 0; i0 = 0; ok = True; dbody = None; nm = CS.reg_name; aligned = False
    # prologue: leading pushes of ebp/ebx/esi/edi before the first frame allocation
    npro = 0
    for k, i in enumerate(ins[:8]):
        if i.mnemonic == "push" and i.op_str in ("ebp", "ebx", "esi", "edi") and k == npro: npro += 1
        elif i.mnemonic == "mov" and i.op_str == "ebp, esp": continue
        else: break
    ftop = -4 * npro
    for n, i in enumerate(ins):
        mn = i.mnemonic; ops = i.operands; a = i.address
        if prev in ("ret", "jmp"): d = dmap.get(a, dbody if dbody is not None else d)
        prev = "ret" if mn in ("ret", "retn") else "jmp" if mn == "jmp" else None
        if mn == "jmp" or mn.startswith("j"):
            if ops and ops[0].type == CX.X86_OP_IMM: dmap.setdefault(ops[0].imm, d)
        if a in dmap and prev is None and n: pass
        if mn in ("ret", "retn") and d != 0 and not aligned: badret.append((a, d))
        if mn == "and" and ops[0].type == CX.X86_OP_REG and nm(ops[0].reg) == "esp": aligned = True
        nxt = ins[n + 1] if n + 1 < len(ins) else None
        mems = [o for o in ops if o.type == CX.X86_OP_MEM]
        if mn == "push":
            o = ops[0]; t = tag.get(nm(o.reg)) if o.type == CX.X86_OP_REG else None
            pend.append(t); d -= 4
            if n < 12 and ops[0].type == CX.X86_OP_REG and nm(ops[0].reg) in ("ebx", "ebp", "esi", "edi"): dbody = d      # prologue register save
        elif mn == "pop":
            d += 4
            if pend: pend.pop()
        elif mn == "sub" and ops[0].type == CX.X86_OP_REG and nm(ops[0].reg) == "esp" and ops[1].type == CX.X86_OP_IMM: d -= ops[1].imm; dbody = d if dbody is None else dbody
        elif mn == "add" and ops[0].type == CX.X86_OP_REG and nm(ops[0].reg) == "esp" and ops[1].type == CX.X86_OP_IMM: d += ops[1].imm; pend = pend[:max(0, len(pend) - ops[1].imm // 4)]; base_i = min(base_i, len(pend))
        elif mn == "mov" and ops[0].type == CX.X86_OP_REG and ops[1].type == CX.X86_OP_IMM and nm(ops[0].reg) == "eax" and nxt and nxt.mnemonic == "call" and "chkstk" in (rel or {}).get(nxt.address - base + 1, ""):
            d -= ops[1].imm; dbody = d      # mov eax,N; call __chkstk (handled as one allocation: the call below is skipped)
        elif mn == "mov" and ops[0].type == CX.X86_OP_REG and nm(ops[0].reg) == "ebp" and ops[1].type == CX.X86_OP_REG and nm(ops[1].reg) == "esp": ebp_c = d
        elif mn == "lea" and ops[0].type == CX.X86_OP_REG and mems:
            m = mems[0].mem; b = nm(m.base) if m.base else ""
            if not m.index and b == "esp": tag[nm(ops[0].reg)] = d + m.disp; continue
            if not m.index and b == "ebp" and ebp_c is not None: tag[nm(ops[0].reg)] = ebp_c + m.disp; continue
        if dbody is None and mn not in ("push", "pop", "mov", "sub"): dbody = d
        if mn == "call":
            tgt = ops[0].imm if ops and ops[0].type == CX.X86_OP_IMM else None
            sym = (rel or {}).get(a - base + 1) if rel is not None and tgt is not None else None
            if sym and "chkstk" in sym: tag.clear(); continue
            args = [(("a%d" % k), t) for k, t in enumerate(reversed(pend[base_i:])) if t is not None]
            ci = None
            try: ci = core.callee_info(tgt) if tgt is not None and rel is None else core.callee_info(symva[undecorate(sym)]) if sym and symva and undecorate(sym) in symva else None
            except Exception: pass
            tb = core.T(); thisc = ci is not None and (tb.sig.get(ci.va) or ("",))[0] == "__thiscall"
            args += [(r, tag[r]) for r in REGS if r in tag and ci is not None and (r in ci.regs or (r == "ecx" and thisc))]
            if args: calls.append((len(calls), tgt, undecorate(sym) if sym else None, args, len(pend) - base_i))
            else: calls.append((len(calls), tgt, undecorate(sym) if sym else None, [], len(pend) - base_i))
            # cleanup
            pop = None
            if tgt is not None and rel is None and core.BASE <= tgt < core.IMGEND:
                try: pop = core.callee_info(tgt).pop // 4
                except Exception: pass
            elif sym and symva and undecorate(sym) in symva:
                try: pop = core.callee_info(symva[undecorate(sym)]).pop // 4
                except Exception: pass
            if pop is None:      # unknown callee (vtable): cdecl when an `add esp, small` cleanup follows before the next ret, else callee pops
                cd = False
                for j in ins[n + 1:n + 61]:
                    if j.mnemonic in ("ret", "retn"): break
                    if j.mnemonic == "add" and j.op_str.startswith("esp,") and j.operands[1].type == CX.X86_OP_IMM and 0 < j.operands[1].imm <= 0x40: cd = True; break
                pop = 0 if cd else len(pend) - base_i
            d += 4 * pop; pend = pend[:len(pend) - pop] if pop <= len(pend) else []; base_i = len(pend)
            for r in REGS: tag.pop(r, None)
            continue
        rd, wr = i.regs_access()
        for r in wr:
            n_ = nm(r)
            if mn == "mov" and ops[0].type == CX.X86_OP_REG and ops[1].type == CX.X86_OP_REG and tag.get(nm(ops[1].reg)) is not None: tag[n_] = tag[nm(ops[1].reg)]
            elif mn == "lea": pass
            else: tag.pop(n_, None)
    return dict(calls=calls, ftop=ftop, ok=(not aligned) or ebp_c is not None, drift=bool(badret), aligned=aligned, dend=(badret[0][1] if badret else d))


def rooms(calls, ftop):
    E = sorted({c for _, _, _, args, _ in calls for _, c in args})
    return {c: ((E[E.index(c) + 1] if E.index(c) + 1 < len(E) else ftop) - c) for c in E}


STARTS = []


def audit_function(obj, sym, va, symva):
    nm = lambda s: s
    import bisect
    big = cmpfn.exe_bytes(va, 0x4000); n_ = len(big); sz_ = core.T().size.get(va)
    for s_ in ([] if sz_ else STARTS[bisect.bisect_right(STARTS, va):]):      # first known start behind a ret / padding (aliases of interior labels do not end the function)
        k = s_ - va
        if k >= n_: break
        if big[k - 1] in (0xcc, 0x90, 0xc3) or big[k - 3] == 0xc2: n_ = k; break
    ocode = big[:min(sz_, n_) if sz_ else n_]
    O = analyse(ocode, va)
    ucode, rel = obj_code(obj, sym)
    U = analyse(ucode, 0, rel, symva)
    ro, ru = rooms(O["calls"], O["ftop"]), rooms(U["calls"], U["ftop"])
    # token per escaping (call, arg): callee identity
    def tokO(c): return ("%08x" % c[1]) if c[1] else "ind"
    def tokU(c):
        s = c[2]; v = symva.get(s) if s else None
        return ("%08x" % v) if v else (s or "ind")
    lo = [(tokO(c), k, cc, c[0], c[1], len(c[3])) for c in O["calls"] for k, cc in c[3]]
    lu = [(tokU(c), k, cc, c[0], c[2], len(c[3])) for c in U["calls"] for k, cc in c[3]]
    pairs = {}
    if len(O["calls"]) == len(U["calls"]):      # same call count: align by (call ordinal, argument)
        idx = {(x[3], x[1]): j for j, x in enumerate(lu)}
        for i_, x in enumerate(lo):
            if (x[3], x[1]) in idx: pairs[i_] = idx[(x[3], x[1])]
    else:
        sm = SequenceMatcher(None, [(t, k) for t, k, *_ in lo], [(t, k) for t, k, *_ in lu], autojunk=False)
        for blk in sm.get_matching_blocks():
            for j in range(blk.size): pairs[blk.a + j] = blk.b + j
    rows = []; used = set(pairs.values())
    for ia, (t, k, c, ordn, tgt, _) in enumerate(lo):
        wr = rd = 0; nargs = 0
        if tgt and core.BASE <= tgt < core.IMGEND and k.startswith("a"):
            try:
                nargs = max(int(k[1:]) + 1, 1); nargs = max(nargs, core.callee_info(tgt).pop // 4)
                rd, wr = stack_lint.extents(tgt, nargs)[int(k[1:])]
            except Exception: pass
        need = max(rd, wr)
        ib = pairs.get(ia)
        r_o = ro[c]
        if ib is None: rows.append((sym, va, ordn, t, k, r_o, need, wr, None, None, "UNMATCHED", "no call/arg of ours aligned with the original's")); continue
        r_u = ru[lu[ib][2]]
        if r_u >= r_o: st = "ok"
        elif need and r_u >= need and t != "ind": st = "ok"
        else: st = "CHECK"
        tail_u = U["ftop"] - lu[ib][2]; tail_o = O["ftop"] - c
        if tail_u <= 0 or tail_o <= 0: rows.append((sym, va, ordn, t, k, r_o, need, wr, lu[ib][3], r_u, "param", "address of an incoming parameter (above the frame): not a frame object")); continue
        if need and tail_u < need and tail_u < tail_o: st = "OVERFLOW"      # the callee reaches past the top of our frame: saved registers / return address
        if st == "CHECK" and tail_u >= tail_o: st = "ok"      # same or more room to the top of the frame than the original
        rows.append((sym, va, ordn, t, k, r_o, need, wr, lu[ib][3], r_u, st, "tail room ours %d orig %d" % (tail_u, tail_o)))
    for ib, (t, k, c, ordn, _, _) in enumerate(lu):
        if ib not in used: rows.append((sym, va, None, t, k, None, 0, 0, ordn, ru[c], "UNMATCHED", "extra escape in ours"))
    if O["drift"] or U["drift"]: rows = [r[:11] + (r[11] + " (esp tracking drifts at a ret: verify by hand)",) for r in rows]
    flag = "" if O["ok"] and U["ok"] else "frame tracking inconsistent (orig d=%d ours d=%d aligned=%s/%s)" % (O["dend"], U["dend"], O["aligned"], U["aligned"])
    return rows, flag


def main(a=None):
    a = list(sys.argv[1:] if a is None else a); allt = "--all-tier2" in a; D = os.path.join(G, "src", "automatch_near")
    if "--dir" in a: i = a.index("--dir"); D = os.path.abspath(a[i + 1]); del a[i:i + 2]      # sources (and their headers) of another directory, e.g. a scratch copy under test
    want = [x for x in a if not x.startswith("--")]
    syms = {}
    for f in ("symbols.txt", "aliases.txt"):
        for l in open(os.path.join(G, "src", f)):
            p = l.split()
            if len(p) == 2:
                try: syms.setdefault(p[0], int(p[1], 16))
                except ValueError: pass
    for hp in glob.glob(os.path.join(G, "output", "decls.h")) + glob.glob(os.path.join(G, "src", "automatch_near", "*.h")) + glob.glob(os.path.join(G, "src", "automatch_near", "an_*.c")):      # `/*0056ea00*/ extern int name(..)` address comments
        for m in re.finditer(r"/\*([0-9a-fA-F]{8})\*/\s*extern\s+[^;(]*?\b(\w+)\s*\(", open(hp, errors="replace").read()): syms.setdefault(m.group(2), int(m.group(1), 16))
    STARTS[:] = sorted({v for v in syms.values() if core.BASE <= v < core.IMGEND})
    byfile = {}; known = {}
    for l in open(os.path.join(G, "src", "automatch_near_names.txt")):
        q = l.split()
        if len(q) == 3: known[q[1]] = int(q[0], 16)
    fsz = {}
    for l in open(os.path.join(G, "output", "fnsizes.txt")):
        q = l.split()
        if len(q) >= 2: fsz.setdefault(q[1], int(q[0], 16))
    for k_, v_ in list(known.items()) + list(fsz.items()): syms.setdefault(k_, v_)
    for path in sorted(glob.glob(os.path.join(D, "an_*.c"))):
        f = os.path.basename(path)
        try: dfs = list(core.defs(path))
        except Exception: continue
        m = re.match(r"an_rwp_([0-9a-f]{8})\.c", f)
        for nm_ in dfs:
            va = known.get(nm_) or fsz.get(nm_) or (syms.get(nm_)) or (int(m.group(1), 16) if m and len(dfs) == 1 else None)
            if va is None or (want and nm_ not in want) or not (allt or f.startswith("an_rwp_") or "rwp" in nm_): continue
            byfile.setdefault(f, []).append((nm_, va))
    out = []
    for f, fns in sorted(byfile.items()):
        src = os.path.join(D, f)
        try: obj, _ = diffemu.compile_obj(src)
        except Exception as e: print("frameaudit: %s: %s" % (f, e), file=sys.stderr); continue
        for nm_, va in fns:
            try: rows, flag = audit_function(obj, nm_, va, syms)
            except Exception as e: out.append(dict(function=nm_, file=f, status="ERROR", note="%s %s" % (type(e).__name__, e))); continue
            if flag: out.append(dict(function=nm_, file=f, status="UNCERTAIN", note=flag))
            for r in rows:
                out.append(dict(function=r[0], file=f, orig_call=r[2], callee=r[3], arg=r[4], room_orig=r[5], required=r[6], required_write=r[7], our_call=r[8], room_ours=r[9], status=r[10], note=r[11]))
            if not rows and not flag: out.append(dict(function=nm_, file=f, status="none", note="no local address passed to a call"))
    return out


if __name__ == "__main__":
    rows = main(); fn = ["function", "file", "orig_call", "callee", "arg", "room_orig", "required", "required_write", "our_call", "room_ours", "status", "note"]
    w = csv.DictWriter(sys.stdout, fn, extrasaction="ignore"); w.writeheader(); w.writerows(rows)
