#!/usr/bin/env python3
"""Register inputs of the application functions of MFL.exe, from the disassembly (not from the decompiler text).
For every function (start addresses = '// ==== name @ addr' of output/named/MFL_app_named.c) the straight line entry block (up to the first jump / call /
ret) is scanned: a register read before it is written is an input (callee saved ebx/esi/edi/ebp pushed in the prologue are saves, not inputs; 'xor r,r' is a
write).  Also reports the 'ret N' (callee cleaned stack bytes) and whether the function returns a value in st(0) (fld before ret is not checked).
Usage: reginputs.py [out.csv]   -> name,addr,inputs,ret_n   (stdlib only; needs objdump)"""
import os, re, subprocess, sys
HERE = os.path.dirname(os.path.abspath(__file__))
EXE = os.path.join(HERE, "..", "..", "MashedGame", "FullyLoaded", "installed", "App_Executables", "MFL.exe")
NAMED = os.path.join(HERE, "..", "output", "named", "MFL_app_named.c")
REG32 = ["eax", "ecx", "edx", "ebx", "esi", "edi", "ebp", "esp"]
SUB = {"ax": "eax", "al": "eax", "ah": "eax", "cx": "ecx", "cl": "ecx", "ch": "ecx", "dx": "edx", "dl": "edx", "dh": "edx", "bx": "ebx", "bl": "ebx", "bh": "ebx",
       "si": "esi", "di": "edi", "bp": "ebp", "sp": "esp"}
CTRL = ("jmp", "ret", "call", "retn", "int3", "hlt") 


def regs_in(text):
    return {SUB.get(t, t) for t in re.findall(r"\b[a-z]{2,3}\b", text) if SUB.get(t, t) in REG32}


def analyse(insns, popped=frozenset()):
    inputs, written = [], set()
    saved = set()
    for op, args in insns:
        parts = [a.strip() for a in re.split(r",(?![^\[]*\])", args)] if args else []
        if op == "push" and parts and parts[0] in ("ebx", "esi", "edi", "ebp") and parts[0] not in written and parts[0] in popped:
            saved.add(parts[0]); continue        # prologue save of a callee saved register (restored by a pop in the same function)
        if op == "push" and parts and parts[0] in ("eax", "ecx", "edx") and insns.index((op, args)) < 2 and parts[0] not in written:
            continue        # "push ecx" as the first instruction reserves 4 bytes of stack
        if op == "call":
            written |= {"eax", "ecx", "edx"}
            continue
        dst = parts[0] if parts else ""
        dst_reg = SUB.get(dst, dst) if dst in REG32 or dst in SUB else None
        src_regs = set()
        nop_idiom = len(parts) == 2 and ((op == "mov" and parts[0] == parts[1]) or (op == "lea" and parts[1] == "[%s+0x0]" % parts[0]))   # mov edi,edi / lea ebx,[ebx+0x0] padding
        if nop_idiom:
            continue
        zero_idiom = (op in ("xor", "sub", "sbb") and len(parts) == 2 and parts[0] == parts[1]) or (op == "or" and len(parts) == 2 and parts[1] in ("0xffffffff", "-1"))
        for i, a in enumerate(parts):
            if zero_idiom:
                break
            if i == 0 and dst_reg and op in ("mov", "movzx", "movsx", "lea", "pop", "setne", "sete", "setl", "setg"):
                continue
            src_regs |= regs_in(a)
        if op in ("cdq",): src_regs.add("eax")
        if op in ("idiv", "div", "mul", "imul") and len(parts) == 1: src_regs |= {"eax"} | regs_in(parts[0])
        full = (op + " " + args).lower()
        if re.match(r"(rep\w* )?movs", full): src_regs |= {"ecx", "esi", "edi"} if full.startswith("rep") else {"esi", "edi"}
        elif re.match(r"(rep\w* )?stos", full): src_regs |= {"ecx", "edi", "eax"} if full.startswith("rep") else {"edi", "eax"}
        elif re.match(r"repn?z? cmps|repn?z? scas", full): src_regs |= {"ecx", "esi", "edi", "eax"}
        for r in sorted(src_regs):
            if r not in written and r not in inputs and r not in ("esp",) and not (r in saved):
                inputs.append(r)
        if dst_reg and op not in ("cmp", "test", "push") and not (op in ("mov", "movzx", "movsx", "lea", "pop") and False):
            written.add(dst_reg)
        if op in ("cdq",): written.add("edx")
        if op in ("idiv", "div", "mul") : written |= {"eax", "edx"}
    return inputs


def main():
    out = subprocess.run(["objdump", "-d", "-M", "intel", "--no-show-raw-insn", "--start-address=0x401000", "--stop-address=0x5dc000", EXE], capture_output=True, text=True).stdout
    insn = {}
    order = []
    for l in out.splitlines():
        m = re.match(r"\s*([0-9a-f]{6,8}):\s+(\w+)\s*(.*)", l)
        if m:
            a = int(m.group(1), 16); insn[a] = (m.group(2), m.group(3).split("#")[0].split("<")[0].strip()); order.append(a)
    funcs = sorted({(int(a, 16), n) for n, a in re.findall(r"^// ==== (\w+) @ ([0-9a-f]{8})", open(NAMED, errors="replace").read(), re.M)})
    nxt = {a: funcs[i + 1][0] if i + 1 < len(funcs) else 0x5dc000 for i, (a, _) in enumerate(funcs)}
    pos = {a: i for i, a in enumerate(order)}
    rows = []
    for a, n in funcs:
        if a not in pos: continue
        seq = []
        for k in range(pos[a], min(pos[a] + 150, len(order))):
            ia = order[k]
            if ia >= nxt[a]: break
            op, args = insn[ia]
            if op in ("ret", "retn", "jmp", "int3", "hlt"): break
            if op.startswith("j"): continue          # conditional jump: follow the fall through path
            seq.append((op, args))
        popped = set()
        for k in range(pos[a], min(pos[a] + 4000, len(order))):
            ia = order[k]
            if ia >= nxt[a]: break
            op, args = insn[ia]
            if op == "pop": popped.add(args.strip())
        inputs = analyse(seq, popped)
        retn = 0
        for k in range(pos[a], min(pos[a] + 4000, len(order))):
            ia = order[k]
            if ia >= nxt[a]: break
            op, args = insn[ia]
            if op == "ret" and args:
                retn = int(args, 16); break
            if op == "ret": break
        rows.append((n, a, [r for r in inputs], retn))
    dst = sys.argv[1] if len(sys.argv) > 1 else None
    if dst:
        with open(dst, "w") as f:
            f.write("name,addr,reg_inputs,ret_n\n")
            for n, a, i, r in rows: f.write("%s,%08x,%s,%d\n" % (n, a, "|".join(i), r))
    cnt = {}
    for n, a, i, r in rows:
        k = "|".join(x for x in i if x in ("eax", "ecx", "edx")) or "-"
        cnt[k] = cnt.get(k, 0) + 1
    print("functions", len(rows), " with callee saved register inputs (esi/edi/ebx/ebp):", sum(1 for n, a, i, r in rows if set(i) & {"ebx", "esi", "edi", "ebp"}))
    for k, v in sorted(cnt.items(), key=lambda x: -x[1]): print("  inputs among eax/ecx/edx: %-12s %d" % (k, v))


if __name__ == "__main__":
    main()


def inputs_of(code):
    """register inputs (eax/ecx/edx/ebx/esi/edi/ebp read before written along the fall through path) of raw function bytes"""
    import tempfile
    f = tempfile.NamedTemporaryFile(delete=False); f.write(code); f.close()
    out = subprocess.run(["objdump", "-D", "-b", "binary", "-mi386", "-M", "intel", "--no-show-raw-insn", f.name], capture_output=True, text=True).stdout
    os.unlink(f.name)
    seq, popped, stop = [], set(), False
    for l in out.splitlines():
        m = re.match(r"\s*[0-9a-f]+:\s+(\w+)\s*(.*)", l)
        if not m: continue
        op, args = m.group(1), m.group(2).split("#")[0].split("<")[0].strip()
        if op == "pop": popped.add(args)
        if stop: continue
        if op in ("ret", "retn", "jmp", "int3", "hlt"): stop = True; continue
        if op.startswith("j"): continue
        seq.append((op, args))
    return set(analyse(seq, popped)) - {"esp"}
