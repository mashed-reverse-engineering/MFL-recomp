#!/usr/bin/env python3
"""cluster_regcheck.py RUNDIR [ROOT...]   (RUNDIR = a cluster_auto run; default roots = RUNDIR/names_pass.txt)
Independent check of the entry contract that diffemu cluster mode can miss (in-game crashes of earlier batches): for every root a CFG dataflow (capstone) over
  - the ORIGINAL bytes: registers read before written (callee reads from reginputs.csv / reginputs_interior.csv), `ret N` values (tail jmp = callee ret N), stack slots read,
    callee-saved registers written but never pushed (= output register the entry thunk would lose);
  - OUR compiled root (diffemu.compile_obj of RUNDIR/c/<root>.c): registers read before written (a leftover register dependence), `ret N` (the thunk calls it cdecl and pops: must be 0);
and compares them with the clusters.txt row (register tokens, stack slot tokens) and the natural prototype (a 2 word parameter gets ONE pushed dword in the generated thunk).
Prints one line per root, `OK` or `MISMATCH reason; ...`; writes RUNDIR/regcheck.csv and RUNDIR/names_regcheck_ok.txt.  Reads only; needs the diffemu venv."""
import os, sys
try: import capstone
except ImportError:
    py = os.environ.get("DIFFEMU_PY") or os.path.expanduser("~/.cache/mfl_diffemu_venv/bin/python"); os.execv(py, [py] + sys.argv)
import csv, re
HERE = os.path.dirname(os.path.abspath(__file__)); G = os.path.normpath(os.path.join(HERE, ".."))
sys.path[:0] = [HERE, os.path.join(HERE, "diffemu")]
import core, diffemu
from capstone import x86_const as X
RUN = os.path.abspath(sys.argv[1]); want = sys.argv[2:]
CS = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32); CS.detail = True
R32 = {}
for r in "eax ebx ecx edx esi edi ebp esp".split():
    R32[r] = r; R32[r[1:]] = r
for r in "abcd": R32[r + "l"] = "e%sx" % r; R32[r + "h"] = "e%sx" % r
for r in ("si", "di", "bp"): R32[r] = "e" + r
GP = "eax ecx edx ebx esi edi ebp".split(); CSAVED = "ebx esi edi ebp".split()
tb = core.T()
callee, retn = {}, {}
for f in ("reginputs.csv", "reginputs_interior.csv"):
    for r in csv.DictReader(open(os.path.join(G, "output", f))):
        a = int(r["addr"], 16); callee[a] = {x for x in (r.get("reg_inputs") or r.get("regs") or "").split("|") if x}; retn[a] = int(r["ret_n"] or 0)

def analyse(read, base, lo, hi, ours):
    """-> dict(inputs, rets, slots, writes_cs, pushed, unresolved)"""
    inputs, rets, slots, wcs, pushed = set(), set(), set(), set(), set(); unres = []
    seen = {}; work = [(lo, frozenset(), 0, None)]; n = 0
    while work:
        a, df, dl, eb = work.pop()
        while True:
            if not (lo <= a < hi) or n > 400000: break
            k = (a, df, dl, eb)
            if seen.get(k): break
            seen[k] = 1; n += 1
            ins = next(CS.disasm(read(a, 16), a, 1), None)
            if ins is None: unres.append("bad opcode %x" % a); break
            m = ins.mnemonic; ops = ins.operands; nxt = a + ins.size
            rd, wr = ins.regs_access()
            R = {R32[ins.reg_name(x)] for x in rd if ins.reg_name(x) in R32} - {"esp"}
            W = {R32[ins.reg_name(x)] for x in wr if ins.reg_name(x) in R32} - {"esp"}
            if m in ("xor", "sub", "sbb") and len(ops) == 2 and ops[0].type == ops[1].type == X.X86_OP_REG and ops[0].reg == ops[1].reg: R = set()
            if m == "push" and ops[0].type == X.X86_OP_REG and R32.get(ins.reg_name(ops[0].reg)) in CSAVED:
                pushed.add(R32[ins.reg_name(ops[0].reg)]); R = set()      # a save, not an input
            if m == "pop": R = set()
            # memory operands: stack slots
            for o in ops:
                if o.type == X.X86_OP_MEM:
                    b = ins.reg_name(o.mem.base) if o.mem.base else ""
                    if o.mem.index == 0 and dl is not None:
                        if b == "esp": off = o.mem.disp - dl
                        elif b == "ebp" and eb is not None: off = o.mem.disp - eb
                        else: off = None
                        if off is not None and off >= 4 and off < 0x100: slots.add((off - 4) // 4)
            if not ours and m == "call" and ops[0].type == X.X86_OP_IMM:
                t = ops[0].imm; R = R | (callee.get(t, set()) - set())
                W = W | {"eax", "ecx", "edx"}
            elif m == "call": W = W | {"eax", "ecx", "edx"}
            for r in R:
                if r in GP and r not in df: inputs.add(r)
            ndf = df | frozenset(W)
            if m != "call" and not (m == "pop"): wcs.update(w for w in W if w in CSAVED and m != "push")
            if m == "pop": wcs.discard(None)
            # stack delta
            ndl, neb = dl, eb
            if dl is not None:
                if m == "push": ndl = dl + 4
                elif m == "pop": ndl = dl - 4
                elif m in ("sub", "add") and ops[0].type == X.X86_OP_REG and ins.reg_name(ops[0].reg) == "esp":
                    ndl = dl + (ops[1].imm if m == "sub" else -ops[1].imm) if ops[1].type == X.X86_OP_IMM else None
                elif m == "mov" and ops[0].type == X.X86_OP_REG and ins.reg_name(ops[0].reg) == "ebp" and ops[1].type == X.X86_OP_REG and ins.reg_name(ops[1].reg) == "esp": neb = dl
                elif m == "mov" and ops[0].type == X.X86_OP_REG and ins.reg_name(ops[0].reg) == "esp": ndl = (eb if ops[1].type == X.X86_OP_REG and ins.reg_name(ops[1].reg) == "ebp" else None)
                elif m == "leave": ndl = (eb - 4) if eb is not None else None
                elif m == "call" and not ours and ops[0].type == X.X86_OP_IMM: ndl = dl - retn.get(ops[0].imm, 0)
                elif m in ("pushad", "popad", "pushfd", "popfd"): ndl = dl + {"pushad": 32, "popad": -32, "pushfd": 4, "popfd": -4}[m]
            df, dl, eb = ndf, ndl, neb
            if m == "ret":
                rets.add(ops[0].imm if ops else 0); break
            if m == "jmp":
                if ops[0].type == X.X86_OP_IMM:
                    t = ops[0].imm
                    if lo <= t < hi: a = t; continue
                    rets.add(retn.get(t, 0)) if not ours else rets.add(0)
                    if not ours: inputs.update(r for r in callee.get(t, set()) if r not in df)
                    break
                o = ops[0]
                if o.type == X.X86_OP_MEM and o.mem.index and o.mem.scale == 4 and o.mem.disp:
                    t0 = o.mem.disp; i = 0; tg = []
                    while True:
                        v = int.from_bytes(read(t0 + 4 * i, 4), "little")
                        if not (lo <= v < hi) or i > 256: break
                        tg.append(v); i += 1
                    for v in tg: work.append((v, df, dl, eb))
                    if not tg: unres.append("jump table at %x" % a)
                else: unres.append("indirect jmp %x" % a)
                break
            if m.startswith("j") and ops and ops[0].type == X.X86_OP_IMM:
                work.append((ops[0].imm, df, dl, eb))
            a = nxt
    return dict(inputs=inputs, rets=rets, slots=slots, wcs=wcs - pushed, unres=unres)

def words(t): return 2 if re.search(r"\b(double|float10|longlong|ulonglong|undefined8|__int64)\b", t) and "*" not in t else 1
def split_top(s):
    out, d, cur = [], 0, ""
    for ch in s:
        d += {"(": 1, ")": -1, "[": 1, "]": -1}.get(ch, 0)
        if ch == "," and d == 0: out.append(cur); cur = ""
        else: cur += ch
    return out + [cur] if cur.strip() else out

rows = {}
for l in open(os.path.join(RUN, "c", "clusters.txt")):
    p = [x.strip() for x in l.split("|")]
    if len(p) >= 4: rows[p[0]] = p
roots = want or [x.split()[0] for x in open(os.path.join(RUN, "names_pass.txt")) if x.strip()]
out = []
for root in roots:
    why = []
    try:
        p = rows[root]; order = p[3].split(); va = min(tb.sym[root]); sz = tb.size.get(va, 0x400)
        o = analyse(core.img_read, 0, va, va + sz, False)
        regtok = {t for t in order if not re.fullmatch(r"s\d+", t)}; stk = [int(t[1:]) for t in order if re.fullmatch(r"s\d+", t)]
        src = open(os.path.join(RUN, "c", p[1]), errors="replace").read()
        hm = re.search(r"^(?!extern|static|typedef)[A-Za-z_][^\n(;]*?\b%s\s*\(([^)]*)\)\s*\{" % re.escape(root), src, re.M)
        ptypes = [re.sub(r"\s*\b\w+$", "", q.strip()) for q in split_top(hm.group(1)) if q.strip() and q.strip() != "void"] if hm else []
        if not hm: why.append("no root definition in source")
        if len(ptypes) != len(order): why.append("%d natural params vs %d order tokens" % (len(ptypes), len(order)))
        for t, tok in zip(ptypes, order):
            if words(t) == 2: why.append("2 word parameter '%s' (%s): generated thunk pushes one dword" % (t, tok))
        extra = o["inputs"] - regtok
        if extra: why.append("original reads %s, not an entry token" % ",".join(sorted(extra)))
        noread = regtok - o["inputs"]
        rn = max(o["rets"]) if o["rets"] else 0
        if len(o["rets"]) > 1: why.append("several ret N values %s" % sorted(o["rets"]))
        ri_n = retn.get(va, 0)
        if o["rets"] and rn != ri_n: why.append("ret N %d vs reginputs.csv %d" % (rn, ri_n))
        if rn and rn != 4 * len(stk): why.append("ret %d but %d stack tokens" % (rn, len(stk)))
        if rn and sorted(stk) != list(range(rn // 4)): why.append("stack tokens %s vs ret %d" % (stk, rn))
        over = {s for s in o["slots"] if s not in stk and s >= len(stk)} if not rn else {s for s in o["slots"] if s >= rn // 4}
        if over: why.append("original reads stack slot(s) %s beyond the declared %s" % (sorted(over), stk))
        if o["wcs"]: why.append("original writes %s without saving (output register)" % ",".join(sorted(o["wcs"])))
        if o["unres"]: why.append("unresolved: " + "; ".join(o["unres"][:2]))
        # our side
        obj, k = diffemu.compile_obj(os.path.join(RUN, "c", p[1]))
        co = core.Coff(obj); code, rel, fsn, fval = co.function(root)
        us = analyse(lambda a, n: (code + b"\xcc" * 16)[a:a + n], 0, 0, len(code.rstrip(b"\xcc") if rel else code), True)
        if us["rets"] - {0}: why.append("our root pops %s bytes (thunk calls it cdecl)" % sorted(us["rets"]))
        if us["inputs"]: why.append("our root reads %s before writing it" % ",".join(sorted(us["inputs"])))
        if us["unres"]:      # switch table in OUR code (relocated, not followable): linear sweep, a register read before ANY earlier write counts (the compiler writes before reading in straight code; flags only true leftovers)
            ucode = (code + b"\xcc" * 16)[:len(code.rstrip(b"\xcc") if rel else code)]; lin = set()
            q_ = 0; last = 0      # cut at the last `ret` (alignment padding / the switch table follow it)
            while q_ < len(ucode):
                i_ = next(CS.disasm(ucode[q_:q_ + 16], q_, 1), None)
                if i_ is None: break
                q_ += i_.size
                if i_.mnemonic == "ret": last = q_
            ucode = ucode[:last or len(ucode)]; dfl = set(); ap = 0
            while ap < len(ucode):
                ins = next(CS.disasm(ucode[ap:ap + 16], ap, 1), None)
                if ins is None: break
                rd, wr = ins.regs_access(); ap += ins.size
                Rr = {R32[ins.reg_name(x)] for x in rd if ins.reg_name(x) in R32}; Ww = {R32[ins.reg_name(x)] for x in wr if ins.reg_name(x) in R32}
                if ins.mnemonic in ("xor", "sub") and len(ins.operands) == 2 and ins.operands[0].type == ins.operands[1].type == X.X86_OP_REG and ins.operands[0].reg == ins.operands[1].reg: Rr = set()
                if ins.mnemonic in ("push", "pop"): Rr = set()
                lin |= {r for r in Rr if r in ("eax", "ecx", "edx") and r not in dfl}; dfl |= Ww | ({"eax", "ecx", "edx"} if ins.mnemonic == "call" else set())
            if lin: why.append("ours (linear sweep, switch table) reads %s before any write" % ",".join(sorted(lin)))
            else: us["unres"] = []
            if us["unres"]: why.append("ours unresolved: " + "; ".join(us["unres"][:2]))
        line = (root, "OK" if not why else "MISMATCH", "; ".join(why), ",".join(sorted(o["inputs"])), ",".join(order), rn, ",".join(map(str, sorted(o["slots"]))), "noread:" + ",".join(sorted(noread)))
    except Exception as e:
        line = (root, "ERROR", "%s %s" % (type(e).__name__, str(e)[:150]), "", "", "", "", "")
    out.append(line); print("%-40s %-8s %s" % line[:3], flush=True)
with open(os.path.join(RUN, "regcheck.csv"), "w", newline="") as f:
    w = csv.writer(f); w.writerow(["root", "verdict", "reasons", "orig_inputs", "order", "orig_ret_n", "orig_slots", "unused_tokens"]); w.writerows(out)
open(os.path.join(RUN, "names_regcheck_ok.txt"), "w").write("".join(l[0] + "\n" for l in out if l[1] == "OK"))
print("regcheck: %d roots, OK %d, MISMATCH %d, ERROR %d" % (len(out), sum(l[1] == "OK" for l in out), sum(l[1] == "MISMATCH" for l in out), sum(l[1] == "ERROR" for l in out)))
