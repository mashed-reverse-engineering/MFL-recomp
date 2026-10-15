#!/usr/bin/env python3
"""interior_refs.py: static check behind OVERLAY_WIPE.  The sweeps only show that no TESTED path enters a replaced original function in the middle; this lists every direct branch (jmp / jcc / call) of the original code whose
target lies INSIDE another function (not at its start): shared tails, tail merged code, hand written asm.  A replaced function with such an incoming reference needs its body (or the referencing function must be replaced too).
Also lists data words (.data / .rdata) that point into the interior of a function from outside any switch table of that function.  Output: output/interior_refs.csv (kind, from, to, function containing the target, replaced?)."""
import bisect, csv, os, re, struct, subprocess
HERE = os.path.dirname(os.path.abspath(__file__)); EXE = os.path.join(HERE, "..", "..", "MashedGame", "FullyLoaded", "installed", "App_Executables", "MFL.exe")
fn = []
for l in open(os.path.join(HERE, "..", "output", "fnsizes.txt")):
    a, n, s = l.split(); fn.append((int(a, 16), n, int(s)))
fn.sort(); starts = [f[0] for f in fn]
led = {}
for r in csv.DictReader(open(os.path.join(HERE, "..", "output", "tier_ledger.csv"))): led[r["name"]] = r["tier"]
rw = {int(r[0], 16) for r in csv.reader(open(os.path.join(HERE, "..", "output", "rw_libmatch.csv"))) if len(r) >= 4}
crt = {int(r[0], 16) for r in csv.reader(open(os.path.join(HERE, "..", "output", "crt_libmatch.csv"))) if len(r) >= 4}
def replaced(a, n): return n in led or a in rw or a in crt
def containing(t):
    i = bisect.bisect_right(starts, t) - 1
    if i < 0: return None
    a, n, s = fn[i]
    return fn[i] if a <= t < a + s else None
dis = subprocess.run(["objdump", "-d", "-M", "intel", "--no-show-raw-insn", "--start-address=0x401000", "--stop-address=0x5dc000", EXE], capture_output=True, text=True).stdout.splitlines()
rows = []
for l in dis:
    m = re.match(r"\s*([0-9a-f]+):\s+(call|jmp|j[a-z]+)\s+0x([0-9a-f]+)\s*$", l)
    if not m: continue
    src, t = int(m.group(1), 16), int(m.group(3), 16)
    f = containing(t)
    if not f or t == f[0]: continue
    g = containing(src)
    if g and g[0] == f[0]: continue            # inside the same function
    rows.append(("branch", src, t, f[1], replaced(f[0], f[1]), g[1] if g else "?"))
data = open(EXE, "rb").read()
pe = struct.unpack_from("<I", data, 0x3c)[0]; nsec = struct.unpack_from("<H", data, pe + 6)[0]; opt = struct.unpack_from("<H", data, pe + 20)[0]; base = struct.unpack_from("<I", data, pe + 24 + 28)[0]
for i in range(nsec):
    n, vs, va, rs, ro = struct.unpack_from("<8sIIII", data, pe + 24 + opt + i * 40)
    if n.rstrip(b"\0") in (b".rdata", b".data"):
        for o in range(0, min(vs, rs) - 3, 4):
            v = struct.unpack_from("<I", data, ro + o)[0]
            if 0x401000 <= v < 0x5dc000:
                f = containing(v)
                if f and v != f[0]: rows.append(("data", base + va + o, v, f[1], replaced(f[0], f[1]), ""))
with open(os.path.join(HERE, "..", "output", "interior_refs.csv"), "w") as o:
    o.write("kind,from,to,target function,replaced,source function\n")
    for r in rows: o.write("%s,%08x,%08x,%s,%s,%s\n" % r)
rep = [r for r in rows if r[4]]
print(len(rows), "interior references,", len(rep), "into replaced functions;", len({r[3] for r in rep}), "distinct replaced functions; branch:", sum(1 for r in rep if r[0] == "branch"), "data:", sum(1 for r in rep if r[0] == "data"))
