#!/usr/bin/env python3
"""Static comparison of functions that are NOT byte identical with the original (tier 2 / NEAR), no game run: for every function in <names file>
  - `ret N` (callee cleans N bytes: __stdcall / __thiscall) must be the same,
  - the multiset of call targets (thunks followed, our replacements mapped back to the original address) must be the same,
  - the multiset of (callee, `add esp,N` after the call) must be the same (argument counts / calling convention),
  - the multiset of data addresses used as operands must be the same (literals we placed ourselves are matched by content to original addresses).
Usage: staticcheck.py <overlay exe> <overlay exe .map> <names file> [<out reject file>]     prints the failing functions with the reason; the reject file has one name per line."""
import bisect, collections, os, re, struct, subprocess, sys, tempfile
HERE = os.path.dirname(os.path.abspath(__file__))
ORIG = os.path.join(HERE, "..", "..", "MashedGame", "FullyLoaded", "installed", "App_Executables", "MFL.exe")
exe_path, map_path, names_path = sys.argv[1:4]
out_path = sys.argv[4] if len(sys.argv) > 4 else None
BASE = 0x400000

def load(path):
    d = open(path, "rb").read()
    e = struct.unpack_from("<I", d, 0x3C)[0]; ns = struct.unpack_from("<H", d, e + 6)[0]; osz = struct.unpack_from("<H", d, e + 20)[0]
    secs = []
    for i in range(ns):
        o = e + 24 + osz + i * 40
        vs, va, rs, rp = struct.unpack_from("<IIII", d, o + 8)
        secs.append((va, max(vs, rs), rp, rs))
    return d, secs
def read(img, va, n):
    d, secs = img
    rva = va - BASE
    for sva, sz, rp, rs in secs:
        if sva <= rva < sva + sz:
            m = min(n, sva + sz - rva, max(0, rs - (rva - sva)))
            return d[rp + rva - sva:rp + rva - sva + m]
    return b""
ovl, orig = load(exe_path), load(ORIG)
newva = {}
for l in open(map_path):
    p = l.split()
    if len(p) == 2 and not p[0].startswith(("count", "count:")):
        try: newva[p[0]] = int(p[1], 16)
        except ValueError: pass
sym = {}
for l in open(os.path.join(HERE, "..", "src", "symbols.txt")):
    p = l.split()
    if len(p) == 2:
        try: sym.setdefault(p[0], int(p[1], 16))
        except ValueError: pass
for l in open(os.path.join(HERE, "..", "src", "aliases.txt")):
    p = l.split()
    if len(p) == 2: sym.setdefault(p[0], int(p[1], 16))
sizes = {int(l.split()[0], 16): int(l.split()[2]) for l in open(os.path.join(HERE, "..", "output", "fnsizes.txt")) if len(l.split()) == 3}
new_of = {v: k for k, v in newva.items()}
starts = sorted(newva.values())
ovl_lo = min(newva.values()) if newva else 0
text_lo, text_hi = 0x401000, 0x5d7000

def disasm(code, va):
    with tempfile.NamedTemporaryFile(suffix=".bin", delete=False) as f:
        f.write(code); p = f.name
    try:
        out = subprocess.run(["objdump", "-D", "-b", "binary", "-m", "i386", "-M", "intel", "--adjust-vma=0x%x" % va, p], capture_output=True, text=True).stdout
    finally:
        os.unlink(p)
    ins = []
    for l in out.split("\n"):
        m = re.match(r"\s*([0-9a-f]+):\t([0-9a-f ]+)\t(.*)$", l)
        if m: ins.append((int(m.group(1), 16), re.sub(r"\s+", " ", m.group(3).split("#")[0]).strip()))
    return ins

def final(img, va):                      # follow `jmp rel32` thunks
    for _ in range(4):
        b = read(img, va, 5)
        if len(b) == 5 and b[0] == 0xE9: va = (va + 5 + struct.unpack_from("<i", b, 1)[0]) & 0xFFFFFFFF
        else: break
    return va

_ad = {}; VIA = set()
def adapter_target(va):
    """a static F_r call adapter (tools/bridgegen.py gen_adapter: pushes / movs, ONE `call F`, ret) in our section -> the original function it calls, else None"""
    if va not in _ad:
        ins = disasm(read(ovl, va, 64), va); tg = None
        for a, t in ins:
            m = re.fullmatch(r"call 0x([0-9a-f]+)", t)
            if m: tg = None if tg is not None else int(m.group(1), 16)      # a second call: not an adapter
            if t.startswith("ret"): break
        else: tg = None
        _ad[va] = tg if tg is not None and text_lo <= tg < text_hi else None
    return _ad[va]

def profile(img, va, n, ours):
    code = read(img, va, n)
    ins = disasm(code, va)
    cut = len(ins)
    last_ret = max((i for i, (a, t) in enumerate(ins) if t.startswith("ret")), default=len(ins) - 1)
    ins = ins[:last_ret + 1] if not ours else ins[:last_ret + 1]
    retn = None
    calls, cleans, data = collections.Counter(), collections.Counter(), []
    fp = collections.Counter(re.match(r"(\w+)", t).group(1) for a, t in ins if re.match(r"f\w+", t) and not t.startswith("fn"))      # x87 mnemonics: `fild` where the original has `fld` means a float global typed as an integer
    for i, (a, t) in enumerate(ins):
        m = re.fullmatch(r"ret 0x([0-9a-f]+)", t)
        if m: retn = int(m.group(1), 16)
        m = re.fullmatch(r"call 0x([0-9a-f]+)", t)
        if m:
            tg = final(img, int(m.group(1), 16)); via = False
            if ours and tg >= ovl_lo and not (tg in new_of and new_of[tg] in sym) and adapter_target(tg): tg, via = final(orig, adapter_target(tg)), True      # call through an F_r adapter = a call of the original register function
            elif ours and tg in new_of and new_of[tg] in sym: tg = final(orig, sym[new_of[tg]])
            else: tg = tg if ours else final(orig, tg)
            calls[tg] += 1
            n2 = re.fullmatch(r"add esp,0x([0-9a-f]+)", ins[i + 1][1]) if i + 1 < len(ins) else None
            cleans[(tg, int(n2.group(1), 16) if n2 else 0)] += 1
            if via: VIA.add(tg)      # the adapter takes every parameter on the stack: its caller cleanup is not the original's, not compared
            continue
        if t.startswith(("j", "ret")): continue
        for h in re.findall(r"0x([0-9a-f]{6,8})\b", t):
            v = int(h, 16)
            if 0x401000 <= v < 0x1000000 or (ours and v >= ovl_lo and v < ovl_lo + 0x800000):
                if not (va <= v < va + n and not ours and 0): data.append(v)
    return retn, calls, cleans, data, fp

fails = []
names = [l.strip() for l in open(names_path) if l.strip()]
for nm in names:
    if nm not in newva or nm not in sym:
        continue
    nva, ova = newva[nm], sym[nm]
    i = bisect.bisect_right(starts, nva)
    n = (starts[i] - nva) if i < len(starts) else 512
    n = max(16, min(n, 4096))
    osz = sizes.get(ova, 256)
    r1, c1, k1, d1, f1 = profile(ovl, nva, n, True)
    r2, c2, k2, d2, f2 = profile(orig, ova, osz + 8, False)
    why = []
    if (r1 or 0) != (r2 or 0): why.append("ret %s vs original ret %s" % (hex(r1 or 0), hex(r2 or 0)))
    if c1 != c2: why.append("calls differ: ours-only %s original-only %s" % ([hex(x) for x in (c1 - c2)][:3], [hex(x) for x in (c2 - c1)][:3]))
    if f1 != f2: why.append("x87 instructions differ: ours-only %s original-only %s" % (dict(f1 - f2), dict(f2 - f1)))
    k1 = collections.Counter({k: v for k, v in k1.items() if k[0] not in VIA}); k2 = collections.Counter({k: v for k, v in k2.items() if k[0] not in VIA}); VIA.clear()
    if c1 == c2 and k1 != k2: why.append("stack cleanup after calls differs: %s" % [(hex(a), b) for (a, b) in list((k1 - k2).keys())[:3]])
    # data operands: ours in our own section is a literal; match by content with an unused original operand of equal content
    o_left = collections.Counter(d2); lits = []
    mine = collections.Counter()
    for v in d1:
        if v >= ovl_lo and v < ovl_lo + 0x800000:
            nmv = new_of.get(v)
            if nmv and nmv in sym: mine[sym[nmv]] += 1
            else: lits.append(v)
        else: mine[v] += 1
    for v, c in mine.items():
        if o_left[v] < c: why.append("data address %08x used %d times, original %d" % (v, c, o_left[v]))
        else: o_left[v] -= c
    for v in lits:
        content = read(ovl, v, 8)[:4]
        hit = next((a for a in o_left if o_left[a] > 0 and read(orig, a, 4) == content), None)
        if hit is None: why.append("literal %s at %08x has no original counterpart" % (content.hex(), v))
        else: o_left[hit] -= 1
    left = sum(c for c in o_left.values() if c > 0)
    if left: why.append("original uses %d more data operands" % left)
    strong = [w for w in why if w.startswith(("ret ", "calls differ", "stack cleanup", "x87"))]
    if strong or (why and not os.environ.get("STRONG")): fails.append((nm, "; ".join(strong or why)[:200]))      # STRONG=1: the data operand comparison is informational (struct member displacements are written differently by the two compilers' source shapes)
print("%d functions checked, %d fail" % (len(names), len(fails)))
for nm, w in fails: print("%-40s %s" % (nm, w))
if out_path:
    open(out_path, "w").write("\n".join(nm for nm, _ in fails) + "\n")
