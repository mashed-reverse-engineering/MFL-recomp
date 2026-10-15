"""written registers of raw function bytes (shared by bridgegen.py and clobber.py)"""
import os, re, subprocess, tempfile

W2 = {"ax": "eax", "al": "eax", "ah": "eax", "cx": "ecx", "cl": "ecx", "ch": "ecx", "dx": "edx", "dl": "edx", "dh": "edx"}
WRITERS = ("mov", "movzx", "movsx", "lea", "xor", "add", "sub", "and", "or", "shl", "shr", "sar", "inc", "dec", "neg", "not", "pop", "imul", "sbb", "adc", "rol", "ror",
           "setl", "setg", "sete", "setne", "setle", "setge", "seta", "setb", "setae", "setbe", "xchg", "bswap")


def written(code):
    f = tempfile.NamedTemporaryFile(delete=False)
    f.write(code); f.close()
    out = subprocess.run(["objdump", "-D", "-b", "binary", "-mi386", "-M", "intel", f.name], capture_output=True, text=True).stdout
    os.unlink(f.name)
    regs = set()
    for l in out.splitlines():
        m = re.match(r"\s*[0-9a-f]+:\s+(?:[0-9a-f]{2} )+\s*(\w+)\s*(.*)", l)
        if not m: continue
        op, args = m.group(1), m.group(2).split(",")[0].strip()
        if op in WRITERS:
            r = W2.get(args.lower(), args.lower())
            if r in ("eax", "ecx", "edx"): regs.add(r)
        if op in ("cdq", "div", "idiv", "mul", "cwde", "lodsd", "lodsb", "in", "fnstsw"): regs.update(("eax",) + (("edx",) if op in ("cdq", "div", "idiv", "mul") else ()))
        if op in ("rep", "repz", "repnz", "loop", "movs", "stos", "cmps", "scas", "cmpxchg"): regs.add("ecx")
        if op == "call": regs.update(("eax", "ecx", "edx"))
    return regs
