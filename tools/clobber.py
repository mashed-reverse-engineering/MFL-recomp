#!/usr/bin/env python3
"""Register clobber check for the not byte identical (DIFFERENT) functions of src/manifest.txt.
The game was built with whole program optimisation, so callers may keep values in ECX/EDX across a call to a leaf function that never touches them.
For every DIFFERENT function this compares the sets of caller saved registers (ecx edx) that the compiled code and the original write; a register that only
our version writes is a risk (the sound_allowed_for_pair stall).  Usage: clobber.py [manifest]   (stdlib only; needs objdump)"""
import os, re, subprocess, sys, tempfile
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import cmpfn
import reginputs

W2 = {"cx": "ecx", "dx": "edx", "cl": "ecx", "ch": "ecx", "dl": "edx", "dh": "edx"}
WRITERS = ("mov", "movzx", "movsx", "lea", "xor", "add", "sub", "and", "or", "shl", "shr", "sar", "inc", "dec", "neg", "not", "pop", "imul", "sbb", "adc", "rol", "ror", "setl", "setg", "sete", "setne", "setle", "setge", "seta", "setb", "setae", "setbe", "xchg", "bswap")


def written(code):
    f = tempfile.NamedTemporaryFile(delete=False)
    f.write(code); f.close()
    out = subprocess.run(["objdump", "-D", "-b", "binary", "-mi386", "-M", "intel", f.name], capture_output=True, text=True).stdout
    os.unlink(f.name)
    regs = set()
    for l in out.splitlines():
        m = re.match(r"\s*[0-9a-f]+:\s+(?:[0-9a-f]{2} )+\s*(\w+)\s*(.*)", l)
        if not m:
            continue
        op, args = m.group(1), m.group(2).split(",")[0].strip()
        if op in WRITERS:
            r = args.lower()
            r = W2.get(r, r)
            if r in ("ecx", "edx"):
                regs.add(r)
        if op in ("cdq", "div", "idiv", "mul"):
            regs.add("edx")
        if op in ("cmpxchg", "rep", "repz", "repnz", "loop"):
            regs.add("ecx")
        if op == "call":
            regs.update(("ecx", "edx"))   # a callee may clobber them in both versions: treated as equal, see below
    return regs


def main():
    man = sys.argv[1] if len(sys.argv) > 1 else os.path.join(os.path.dirname(os.path.abspath(__file__)), "..", "src", "manifest.txt")
    base = os.path.dirname(os.path.abspath(man))
    groups = {}
    for line in open(man):
        line = line.strip()
        if not line or line.startswith("#"):
            continue
        f, sym, va, *fl = line.split()
        if f == "(none)" or "DIFFERENT" not in fl:
            continue
        fl = fl[:fl.index("DIFFERENT")]
        groups.setdefault((f, tuple(fl) or ("/O2",)), []).append((sym, int(va, 16)))
    risky = 0
    items_va = {sym: va for its in groups.values() for sym, va in its}
    for (f, fl), items in groups.items():
        for sym, (ok, det) in cmpfn.check(os.path.join(base, f), items, list(fl)).items():
            m = re.search(r"ours ([0-9a-f]+) orig ([0-9a-f]+)", det)
            if ok or not m:
                print("%-8s %s %s" % ("MATCH" if ok else "?", sym, "" if ok else det[:60]))
                continue
            ours, orig = written(bytes.fromhex(m.group(1))), written(bytes.fromhex(m.group(2)))
            extra, missing = ours - orig, orig - ours
            raw = cmpfn.exe_bytes(items_va[sym], len(m.group(1)) // 2 + 64)      # the original up to its real end (padding after the last ret)
            end = len(raw)
            for k in range(len(raw) - 3):
                if raw[k] in (0xC3, 0xC2) and all(b in (0x90, 0xCC) for b in raw[k + (1 if raw[k] == 0xC3 else 3):k + (1 if raw[k] == 0xC3 else 3) + 3]) and k > 4:
                    end = k + (1 if raw[k] == 0xC3 else 3); break
            i_ours, i_orig = reginputs.inputs_of(bytes.fromhex(m.group(1))), reginputs.inputs_of(raw[:end])
            if i_ours != i_orig:
                print("INPUT %-45s ours reads %-18s original reads %s   <-- the calling convention differs" % (sym, ",".join(sorted(i_ours)) or "-", ",".join(sorted(i_orig)) or "-"))
                risky += 1
            tag = "RISK" if extra else "ok"
            risky += bool(extra)
            print("%-5s %-45s ours writes %-12s original writes %-12s%s" % (tag, sym, ",".join(sorted(ours)) or "-", ",".join(sorted(orig)) or "-", "  <-- only ours: " + ",".join(sorted(extra)) if extra else ""))
    print("risky functions:", risky)


main()
