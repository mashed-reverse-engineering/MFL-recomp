#!/usr/bin/env python3
"""Win32 imports (the original calls them through the IAT: `call dword ptr ds:[0x5dd278]`, __stdcall, dllimport).  Ghidra's C calls `ShowWindow(a, b)` without a declaration, so cl emits a
cdecl call to a local thunk instead of `call [IAT slot]` and does not pop the arguments (found by the agent pilot: cursor_show, window_show, window_destroy).
Counts the pushes in front of every IAT call in the original (arguments are pushed directly before the call; the vote over all call sites wins) and writes
names/MFL_import_protos.txt: `extern __declspec(dllimport) int __stdcall NAME(int, int, ...);` (all argument types int: only the size matters for __stdcall) and appends `_imp__NAME <slot>` aliases
to src/aliases.txt.  tools/fix_decls.py adds the declaration to every chunk that calls the import without declaring it.  Usage: import_protos.py"""
import collections, os, re, subprocess
HERE = os.path.dirname(os.path.abspath(__file__))
EXE = os.path.join(HERE, "..", "..", "MashedGame", "FullyLoaded", "installed", "App_Executables", "MFL.exe")
slot = {}
for l in open(os.path.join(HERE, "..", "src", "symbols.txt")):
    m = re.match(r"PTR_(\w+?)_([0-9a-f]{8}) ([0-9a-f]{8})", l)
    if m and m.group(2) == m.group(3): slot[int(m.group(3), 16)] = m.group(1)
dis = subprocess.run(["objdump", "-d", "-M", "intel", "--no-show-raw-insn", "--start-address=0x401000", "--stop-address=0x5d7000", EXE], capture_output=True, text=True).stdout.splitlines()
ins = []
for l in dis:
    m = re.match(r"\s*([0-9a-f]+):\s+(\S+)\s*(.*)", l)
    if m: ins.append((m.group(2), m.group(3).strip()))
votes = collections.defaultdict(collections.Counter)
for i, (mn, op) in enumerate(ins):
    m = re.match(r"DWORD PTR ds:0x([0-9a-f]+)", op) if mn == "call" else None
    if not m or int(m.group(1), 16) not in slot: continue
    n = 0
    for mn2, op2 in reversed(ins[max(0, i - 14):i]):
        if mn2 in ("call", "ret", "jmp") or mn2.startswith("j") or (mn2 == "add" and op2.startswith("esp")) or (mn2 == "pop") or mn2 == "leave": break
        if mn2 == "push": n += 1
    votes[slot[int(m.group(1), 16)]][n] += 1
out = []
for name, c in sorted(votes.items()):
    n, k = c.most_common(1)[0]
    if k * 10 < sum(c.values()) * 6: continue                 # no clear majority
    out.append("extern __declspec(dllimport) int __stdcall %s(%s);" % (name, ", ".join(["int"] * n) if n else "void"))
open(os.path.join(HERE, "..", "names", "MFL_import_protos.txt"), "w").write("\n".join(out) + "\n")
al = os.path.join(HERE, "..", "src", "aliases.txt")
have = {l.split()[0] for l in open(al) if l.split()}
add = ["_imp__%s %08x\n" % (n, a) for a, n in sorted(slot.items()) if "_imp__" + n not in have]
open(al, "a").writelines(add)
print(len(votes), "imports with calls,", len(out), "prototypes,", len(add), "aliases added")
