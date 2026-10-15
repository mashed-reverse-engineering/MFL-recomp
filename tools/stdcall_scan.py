#!/usr/bin/env python3
"""Find the functions of the original that clean their own arguments (`ret N` at the end): they are __stdcall (or __thiscall with stack arguments). The Ghidra export declares almost all of them
without __stdcall (`extern undefined f();`), so C that calls them adds an `add esp,N` the original does not have (stack corruption at run time, and a different byte sequence).
Writes output/stdcall.txt: <hex address> <bytes popped> per function.  Usage: stdcall_scan.py"""
import bisect, os, re, subprocess
HERE = os.path.dirname(os.path.abspath(__file__))
EXE = os.path.join(HERE, "..", "..", "MashedGame", "FullyLoaded", "installed", "App_Executables", "MFL.exe")
dis = subprocess.run(["objdump", "-d", "-M", "intel", "--start-address=0x401000", "--stop-address=0x5d7000", EXE], capture_output=True, text=True).stdout.split("\n")
fs = sorted((int(l.split()[0], 16), int(l.split()[2])) for l in open(os.path.join(HERE, "..", "output", "fnsizes.txt")) if len(l.split()) == 3)
starts = [f[0] for f in fs]
res = {}
for l in dis:
    m = re.match(r"\s*([0-9a-f]+):\t[0-9a-f ]+\tret\s+0x([0-9a-f]+)", l)
    if not m: continue
    a = int(m.group(1), 16); i = bisect.bisect_right(starts, a) - 1
    if i >= 0 and a < fs[i][0] + fs[i][1] + 16: res[fs[i][0]] = int(m.group(2), 16)
with open(os.path.join(HERE, "..", "output", "stdcall.txt"), "w") as o:
    for a, n in sorted(res.items()): o.write("%08x %d\n" % (a, n))
print(len(res), "functions with ret N -> output/stdcall.txt")
