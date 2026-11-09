"""disasm_va.py VA SIZE [VA SIZE ...]: disassemble MFL.exe bytes (capstone, venv) with symbol names for absolute operands.
Run: python3 tools/run.py venv tools/disasm_va.py 5b8630 69"""
import os, re, sys
H = os.path.dirname(os.path.abspath(__file__)); sys.path.insert(0, H)
import cmpfn
from capstone import Cs, CS_ARCH_X86, CS_MODE_32

R = os.path.join(H, "..", "src")
sym = {}
for f in ("symbols.txt", "aliases.txt"):
    for l in open(os.path.join(R, f)):
        p = l.split()
        if len(p) >= 2 and re.fullmatch(r"[0-9a-fA-F]{8}", p[1]): sym.setdefault(int(p[1], 16), p[0])
md = Cs(CS_ARCH_X86, CS_MODE_32)
a = sys.argv[1:]
for i in range(0, len(a), 2):
    va, n = int(a[i], 16), int(a[i + 1], 0)
    print("== %08x %s" % (va, sym.get(va, "")))
    for ins in md.disasm(cmpfn.exe_bytes(va, n), va):
        s = "%s %s" % (ins.mnemonic, ins.op_str)
        names = [sym[int(h, 16)] for h in re.findall(r"0x([0-9a-f]{6,8})", ins.op_str) if int(h, 16) in sym]
        print("  %08x  %-44s %s" % (ins.address, s, " ".join(names)))
