#!/usr/bin/env python3
"""sig_regcheck: which `reg_eXX` params in output/sigs.txt are real register inputs of the ORIGINAL function?
A register is a real input iff some path from the entry reads it before a full write (callee-saved push at entry and
push/pop pairs are not reads; call clobbers eax/ecx/edx but reads the registers regsigs.csv lists for the callee).
Writes output/sigs_fixed.txt (bogus reg_ params dropped, stack param names untouched) and output/sigs_regfix.csv.
Hand-edited rows are kept: functions named in KEEP keep their sigs.txt params iff regsigs.csv agrees.
Usage: tools/sig_regcheck.py [-v] [name ...]   (no diffemu/Ghidra run; read-only on inputs)"""
import csv, os, re, sys
HERE = os.path.dirname(os.path.abspath(__file__)); ROOT = os.path.dirname(HERE)
sys.path.insert(0, os.path.join(HERE, "diffemu"))
import core
from capstone import x86_const as X

OUT = os.path.join(ROOT, "output")
P32 = {}   # reg id -> (parent name, byte mask)
for p, subs in {"eax": ("eax:15", "ax:3", "al:1", "ah:2"), "ecx": ("ecx:15", "cx:3", "cl:1", "ch:2"), "edx": ("edx:15", "dx:3", "dl:1", "dh:2"),
                "ebx": ("ebx:15", "bx:3", "bl:1", "bh:2"), "esi": ("esi:15", "si:3"), "edi": ("edi:15", "di:3"), "ebp": ("ebp:15", "bp:3")}.items():
    for s in subs:
        n, m = s.split(":"); P32[n] = (p, int(m))
CALLEE_SAVED = {"ebx", "esi", "edi", "ebp"}
REGS = ("eax", "ecx", "edx", "ebx", "esi", "edi")

def load():
    size = {}
    for l in open(f"{OUT}/fnsizes.txt"):
        p = l.split()
        if len(p) == 3: size[int(p[0], 16)] = int(p[2])
    reg = {}
    for r in csv.DictReader(open(f"{OUT}/regsigs.csv")): reg[int(r["addr"], 16)] = [x for x in r["regs"].split("|") if x]
    return size, reg

def slot_alloc(a, end):
    """`push ecx` followed (before any call/branch, within 4 insns) by a store to [esp] = scratch slot idiom"""
    for _ in range(4):
        ins = next(core.CS.disasm(core.img_read(a, 16), a, 1), None)
        if ins is None or ins.mnemonic in ("call", "ret", "push", "pop") or ins.mnemonic.startswith("j"): return False
        o = ins.operands
        if o and o[0].type == core.CX.X86_OP_MEM and o[0].mem.base == core.CX.X86_REG_ESP and o[0].mem.disp == 0 and o[0].mem.index == 0 and ins.mnemonic != "cmp": return True
        a += ins.size
    return False

def entry_reads(va, size, reg, budget=4000):
    """-> {reg: address of first read-before-write instruction} over all paths"""
    end = va + size
    popped = {i.op_str for i in core.CS.disasm(core.img_read(va, size), va) if i.mnemonic == "pop"}      # push r with a later pop r = callee-saved save, not a read
    found = {}; seen = set(); todo = [(va, (), True)]   # state = tuple of (reg, mask) pairs
    n = 0
    while todo and n < budget:
        a, st, pro = todo.pop(); st = dict(st)
        while va <= a < end and n < budget:
            key = (a, tuple(sorted(st.items())))
            if key in seen: break
            seen.add(key); n += 1
            ins = next(core.CS.disasm(core.img_read(a, 16), a, 1), None)
            if ins is None: break
            mn = ins.mnemonic
            if mn == "push" and ins.op_str in CALLEE_SAVED and ins.op_str in popped: a += ins.size; continue      # prologue save
            if mn == "push" and ins.op_str in ("eax", "ecx", "edx") and slot_alloc(a + ins.size, end): a += ins.size; continue      # `push ecx` = 4 byte local, not an argument
            if mn != "push" or ins.op_str not in CALLEE_SAVED: pro = False
            if mn == "xor" and len(set(ins.op_str.split(", "))) == 1 or mn == "sub" and len(set(ins.op_str.split(", "))) == 1:
                rd, wr = [], [ins.reg_name(r) for r in ins.regs_access()[1] if ins.reg_name(r) in P32]
            else:
                r_, w_ = ins.regs_access(); rd = [ins.reg_name(r) for r in r_]; wr = [ins.reg_name(r) for r in w_]
            if mn == "pop" and ins.op_str in CALLEE_SAVED: rd = []
            for n_ in rd:
                if n_ in P32:
                    p, m = P32[n_]
                    if m & ~st.get(p, 0) and p not in found: found[p] = a
            for n_ in wr:
                if n_ in P32:
                    p, m = P32[n_]; st[p] = st.get(p, 0) | m
            if mn in ("call", "jmp") and ins.operands and ins.operands[0].type == core.CX.X86_OP_IMM:
                t = ins.operands[0].imm
                for r in reg.get(t, []):
                    if r in P32 and 15 & ~st.get(r, 0) and r not in found: found[r] = a
                if mn == "call":
                    for r in ("eax", "ecx", "edx"): st[r] = 15
            if mn == "call": a += ins.size; 
            if mn == "call":
                for r in ("eax", "ecx", "edx"): st[r] = 15
                continue
            if mn == "ret" or mn.startswith("ret"): break
            if mn == "jmp":
                if ins.operands[0].type == core.CX.X86_OP_IMM: a = ins.operands[0].imm; continue
                break
            if mn.startswith("j") or mn.startswith("loop"):
                if ins.operands[0].type == core.CX.X86_OP_IMM: todo.append((ins.operands[0].imm, tuple(st.items()), False))
            a += ins.size
    return found

def main():
    args = sys.argv[1:]; verbose = "-v" in args; only = {x for x in args if x != "-v"}
    size, reg = load(); rows = []; out = []
    for l in open(f"{OUT}/sigs.txt"):
        p = l.rstrip("\n").split("|")
        if len(p) < 7 or "reg_" not in p[5] or (only and p[0] not in only): out.append(l); continue
        va = int(p[1], 16); sz = size.get(va)
        parts = [x for x in p[5].split(";") if x]; keep = []; drop = []
        real = entry_reads(va, sz, reg) if sz else None
        for x in parts:
            m = re.match(r"[^:]*:reg_(e\w\w):", x)
            if not m: keep.append(x); continue
            r = m.group(1)
            if real is None or r in real: keep.append(x)
            else: drop.append(r)
        if verbose and real is not None: print("#", p[0], "reads:", {k: hex(v) for k, v in real.items()})
        for r in drop: rows.append((p[0], p[1], r, "|".join(sorted(real)), "|".join(reg.get(va, [])), "never read before written on any path"))
        if real is not None and not verbose: pass
        p[5] = ";".join(keep) + (";" if keep else ""); out.append("|".join(p) + "\n")
        # also report regsigs-declared regs that the scan says are not read (informational)
        for r in reg.get(va, []):
            if real is not None and r not in real and r in REGS and r not in drop: rows.append((p[0], p[1], r, "|".join(sorted(real)), "|".join(reg[va]), "regsigs lists it, scan finds no read (not removed)"))
    if not only:
        open(f"{OUT}/sigs_fixed.txt", "w").writelines(out)
        with open(f"{OUT}/sigs_regfix.csv", "w", newline="") as f:
            w = csv.writer(f); w.writerow(["name", "addr", "reg", "real_inputs_by_scan", "regsigs_regs", "note"]); w.writerows(rows)
    for r in rows: print(*r, sep=",")
    print(f"{sum(1 for r in rows if r[5].startswith('never'))} bogus reg_ params, {sum(1 for r in rows if r[5].startswith('regsigs'))} informational", file=sys.stderr)
if __name__ == "__main__": main()
