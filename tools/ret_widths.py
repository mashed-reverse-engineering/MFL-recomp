#!/usr/bin/env python3
"""Callees that Ghidra declares with the 1 byte return type `undefined` but whose result the original reads as a whole register (found by the matching agent pilot: `neg eax`, `test eax,eax` after the call
where C compiled `neg al` / `test al,al`).  Looks at the first instructions after every direct `call ADDR` in the original: the first one that READS eax / ax / al decides (wide = eax or ax, byte = al).
Writes names/MFL_ret_wide.txt: functions with at least one wide read and no byte read; tools/fix_decls.py turns their `extern undefined [conv] f(...)` into `int`.  Usage: ret_widths.py"""
import collections, os, re, subprocess
HERE = os.path.dirname(os.path.abspath(__file__))
EXE = os.path.join(HERE, "..", "..", "MashedGame", "FullyLoaded", "installed", "App_Executables", "MFL.exe")
dis = subprocess.run(["objdump", "-d", "-M", "intel", "--no-show-raw-insn", "--start-address=0x401000", "--stop-address=0x5d7000", EXE], capture_output=True, text=True).stdout.splitlines()
ins = []
for l in dis:
    m = re.match(r"\s*([0-9a-f]+):\s+(\S+)\s*(.*)", l)
    if m: ins.append((int(m.group(1), 16), m.group(2), m.group(3).split("  ")[0].strip()))
A = {"eax": "w", "ax": "w", "al": "b", "ah": "b"}
def regs_in(s): return [A[r] for r in re.findall(r"\b(eax|ax|al|ah)\b", s)]
votes = collections.defaultdict(collections.Counter)
for i, (a, mn, op) in enumerate(ins):
    m = re.match(r"0x([0-9a-f]+)$", op) if mn == "call" else None
    if not m: continue
    tgt = int(m.group(1), 16)
    for a2, mn2, op2 in ins[i + 1:i + 7]:
        if mn2 in ("call", "ret", "jmp", "leave") or mn2.startswith("j") or (mn2 == "add" and op2.startswith("esp")) or mn2 in ("pop", "push", "nop"):
            if mn2 in ("add", "pop", "nop"): continue
            if mn2 == "push" and regs_in(op2): votes[tgt][regs_in(op2)[0]] += 1
            if mn2 == "push": continue
            break
        parts = op2.split(",", 1)
        dst, src = parts[0], parts[1] if len(parts) > 1 else ""
        if mn2 in ("xor", "sub") and len(parts) > 1 and dst == src: break          # zeroing idiom: no read
        reads = regs_in(src)
        if mn2 not in ("mov", "lea", "movzx", "movsx", "pop", "setne", "sete", "setl", "setg", "setle", "setge", "seta", "setb", "setae", "setbe") : reads += regs_in(dst)
        if reads: votes[tgt][reads[0]] += 1; break
        if regs_in(dst) and mn2 in ("mov", "lea", "movzx", "movsx"): break           # eax overwritten before any read: result unused
decl = {}
for l in open(os.path.join(HERE, "..", "output", "decls.h"), errors="replace"):
    m = re.match(r"/\*([0-9a-f]{8})\*/ extern undefined (?:__\w+ )?(\w+)\(", l)
    if m: decl[int(m.group(1), 16)] = m.group(2)
out = sorted(decl[a] for a, c in votes.items() if a in decl and c["w"] and not c["b"])
P = os.path.join(HERE, "..", "names", "MFL_ret_wide.txt")
open(P, "w").write("# functions declared `undefined` (1 byte) whose callers read eax / ax (tools/ret_widths.py); fix_decls.py declares them int\n" + "\n".join(out) + "\n")
print(len(decl), "functions declared undefined,", len(out), "read as a whole register ->", P)
