#!/usr/bin/env python3
"""Argument counts of cdecl functions from the original call sites: `call X` followed by `add esp,N` => X takes N/4 stack dwords.  Only functions that do NOT end in `ret N` (output/stdcall.txt)
and where at least 80 % of the call sites (with an `add esp`) agree and no site lacks the `add esp` are written to output/cdecl_args.txt: "<hex address> <dwords> <agreeing sites> <all sites>"."""
import collections, os, re, subprocess
HERE = os.path.dirname(os.path.abspath(__file__))
EXE = os.path.join(HERE, "..", "..", "MashedGame", "FullyLoaded", "installed", "App_Executables", "MFL.exe")
dis = subprocess.run(["objdump", "-d", "-M", "intel", "--start-address=0x401000", "--stop-address=0x5d7000", EXE], capture_output=True, text=True).stdout.split("\n")
ins = []
for l in dis:
    m = re.match(r"\s*([0-9a-f]+):\t[0-9a-f ]+\t(.*)$", l)
    if m: ins.append((int(m.group(1), 16), re.sub(r"\s+", " ", m.group(2)).strip()))
obs = collections.defaultdict(collections.Counter)
for i, (a, t) in enumerate(ins[:-1]):
    m = re.fullmatch(r"call 0x([0-9a-f]+)", t)
    if not m: continue
    n = re.fullmatch(r"add esp,0x([0-9a-f]+)", ins[i + 1][1])
    obs[int(m.group(1), 16)][int(n.group(1), 16) // 4 if n else -1] += 1          # -1: no add esp after this call
std = {int(l.split()[0], 16) for l in open(os.path.join(HERE, "..", "output", "stdcall.txt"))}
out = []
for a, c in sorted(obs.items()):
    if a in std: continue
    with_add = {k: v for k, v in c.items() if k >= 0}
    if not with_add: continue
    k, cnt = max(with_add.items(), key=lambda kv: kv[1])
    total = sum(with_add.values())
    if cnt >= 0.8 * total and -1 not in c: out.append((a, k, cnt, sum(c.values())))      # a site without `add esp` means register / stdcall / tail call convention: leave those alone
with open(os.path.join(HERE, "..", "output", "cdecl_args.txt"), "w") as o:
    for a, k, cnt, tot in out: o.write("%08x %d %d %d\n" % (a, k, cnt, tot))
print(len(out), "cdecl functions with a consistent argument count -> output/cdecl_args.txt")
