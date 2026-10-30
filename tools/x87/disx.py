#!/usr/bin/env python3
"""dis.py VA [size]: objdump (intel) listing of an original MFL.exe function -> list of (addr, mnemonic, operands, bytes).  Branch / call targets are absolute addresses."""
import os, re, subprocess, sys, tempfile
sys.path.insert(0, os.path.join(os.path.dirname(os.path.abspath(__file__)), ".."))
import cmpfn

def dis(va, size=None):
    size = size or cmpfn.FNSZ.get(va)
    b = cmpfn.exe_bytes(va, size)
    with tempfile.NamedTemporaryFile(suffix=".bin") as t:
        t.write(b); t.flush()
        o = subprocess.run(["objdump", "-w", "-D", "-b", "binary", "-mi386", "-M", "intel", t.name], capture_output=True, text=True).stdout
    out = []
    for l in o.splitlines():
        p = l.split("\t")
        if len(p) < 3 or not re.match(r"\s*[0-9a-f]+:", p[0]): continue
        a = int(p[0].strip().rstrip(":"), 16)
        m = p[2].split(None, 1)
        if not m: continue
        ops = m[1].split("#")[0].strip() if len(m) > 1 else ""
        if (m[0].startswith("j") or m[0] == "call") and re.fullmatch(r"0x[0-9a-f]+", ops): ops = "0x%x" % ((int(ops, 16) + va) & 0xffffffff)
        out.append((va + a, m[0], ops, bytes.fromhex(p[1].replace(" ", ""))))
    return out

if __name__ == "__main__":
    for a, m, o, _ in dis(int(sys.argv[1], 16), int(sys.argv[2]) if len(sys.argv) > 2 else None): print("%08x %s %s" % (a, m, o))
