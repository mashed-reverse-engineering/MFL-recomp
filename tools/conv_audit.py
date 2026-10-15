#!/usr/bin/env python3
"""Calling convention audit: functions that the declarations (output/decls.h from the Ghidra export) mark __stdcall although the original code only returns with a plain `ret`, and the other way round
(found by the matching agents: course_open_and_select, find_texture_dict_b, game_open_piz, load_copter_model, lua_Rain*_impl were declared __stdcall but are __cdecl).  Looks at all `ret` / `ret N` instructions inside the
Ghidra function range.  Writes names/MFL_conv_fix.txt (`name cdecl|stdcall`); tools/fix_decls.py applies it to the extern declarations (rule 'conv').  Usage: conv_audit.py"""
import os, re, subprocess
HERE = os.path.dirname(os.path.abspath(__file__))
EXE = os.path.join(HERE, "..", "..", "MashedGame", "FullyLoaded", "installed", "App_Executables", "MFL.exe")
dis = subprocess.run(["objdump", "-d", "-M", "intel", "--no-show-raw-insn", "--start-address=0x401000", "--stop-address=0x5d7000", EXE], capture_output=True, text=True).stdout.splitlines()
rets = {}      # address -> (n plain rets, n rets with imm)
ins = []
for l in dis:
    m = re.match(r"\s*([0-9a-f]+):\s+(ret)\s*(\S*)", l)
    if m: ins.append((int(m.group(1), 16), m.group(3)))
import bisect
addrs = [a for a, _ in ins]
size = {}
for l in open(os.path.join(HERE, "..", "output", "fnsizes.txt")):
    a, n, s = l.split(); size[int(a, 16)] = (n, int(s))
import collections
cvotes = collections.Counter()      # direct calls followed by `add esp,N`: the callee does not pop its arguments (cdecl)
for i, l in enumerate(dis):
    m = re.match(r"\s*[0-9a-f]+:\s+call\s+0x([0-9a-f]+)", l)
    if m and i + 1 < len(dis) and re.match(r"\s*[0-9a-f]+:\s+add\s+esp,0x[0-9a-f]+", dis[i + 1]): cvotes[int(m.group(1), 16)] += 1
out = []
decl = {}
for l in open(os.path.join(HERE, "..", "output", "decls.h"), errors="replace"):
    m = re.match(r"/\*([0-9a-f]{8})\*/ extern [^;(]*?(__cdecl |__stdcall )?(\w+)\(", l)
    if m: decl[int(m.group(1), 16)] = (m.group(2) or "").strip(), m.group(3)
for a, (conv, name) in sorted(decl.items()):
    if a not in size: continue
    lo = bisect.bisect_left(addrs, a); hi = bisect.bisect_left(addrs, a + size[a][1])
    r = [x[1] for x in ins[lo:hi]]
    if not r and conv == "__stdcall" and cvotes[a] >= 1: out.append((name, "cdecl")); continue      # no ret at all (tail jump) but callers clean the stack
    if not r: continue
    plain = sum(1 for x in r if x == ""); imm = sum(1 for x in r if x not in ("",))
    want = "stdcall" if imm and not plain else "cdecl" if plain and not imm else None
    if want is None and conv == "__stdcall" and not imm and cvotes[a] >= 1: want = "cdecl"
    if want is None: continue
    if conv == "__stdcall" and want == "cdecl": out.append((name, "cdecl"))
    elif conv == "__cdecl" and want == "stdcall": out.append((name, "stdcall"))
P = os.path.join(HERE, "..", "names", "MFL_conv_fix.txt")
open(P, "w").write("# calling convention corrections (tools/conv_audit.py): name cdecl|stdcall\n" + "".join("%s %s\n" % x for x in sorted(set(out))))
print(len(out), "convention corrections ->", P)
