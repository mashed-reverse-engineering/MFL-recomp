#!/usr/bin/env python3
"""rw_g0_check.py gen <vectors.bin> [N=100000] [--seed S] [--cw 0x7f] [--only name,name]   (diffemu venv python)
Test vectors for the G0 group of the librw DLL (tools/delink/rw_g0.txt + rw_g0_candidates.txt = the names to try): the ORIGINAL code of MFL.exe runs in Unicorn under the x87 control word (default 0x7f = the game's 24-bit precision) on N
random cases per function (random floats of mixed magnitude, a few specials; 4 buffers of 64 bytes the pointer arguments point into, aliasing where the function allows it); the file holds the arguments, the result (st0 bits / eax / which buffer is
returned) and the final buffer contents.  The original's table builder 4d5770 is run first (stub allocator), so the engine-instance tables and the vector-multiply pointers are the real ones.
tools/delink/rw_g0_replay.c (mingw, run under Wine: tools/delink/rw_g0_check.sh) loads mflrw.dll and compares bit for bit; it also checks that the control word stays 0x7f.  A function is G0 when it reports EQUAL for all cases."""
import os, random, struct, sys
try: import unicorn
except ImportError: os.execv(os.path.expanduser("~/.cache/mfl_diffemu_venv/bin/python"), [os.path.expanduser("~/.cache/mfl_diffemu_venv/bin/python")] + sys.argv)
import pefile
from unicorn import Uc, UC_ARCH_X86, UC_MODE_32
from unicorn.x86_const import UC_X86_REG_ESP
HERE = os.path.dirname(os.path.abspath(__file__)); G = os.path.normpath(os.path.join(HERE, "..", ".."))
EXE = os.path.join(G, "..", "MashedGame", "FullyLoaded", "installed", "App_Executables", "MFL.exe")
a = sys.argv[1:]; opt = {}
for k in ("--cw", "--seed", "--only", "--variant"):
    if k in a: i = a.index(k); opt[k] = a[i + 1]; del a[i:i + 2]
assert a and a[0] == "gen" and len(a) >= 2, __doc__
OUTF = a[1]; N = int(a[2]) if len(a) > 2 else 100000; CW = int(opt.get("--cw", "0x7f"), 16); random.seed(int(opt.get("--seed", "1")))
# name: (arg kinds, result kind)   p = pointer to one of the 4 buffers, i = int, n = count 0..4, o = RwOpCombineType 0..2, f = float;  result f = st0 float, p = pointer (buffer index), i = eax, v = none
SPEC = {"RwV3dNormalize": ("pp", "f"), "RwV3dLength": ("p", "f"), "RwV2dNormalize": ("pp", "f"), "RwV2dLength": ("p", "f"), "_rwSqrt": ("f", "f"), "_rwInvSqrt": ("f", "f"),
        "RwV3dTransformPoint": ("ppp", "p"), "RwV3dTransformPoints": ("ppnp", "p"), "RwV3dTransformVector": ("ppp", "p"), "RwV3dTransformVectors": ("ppnp", "p"),
        "RwMatrixUpdate": ("p", "p"), "RwMatrixTranslate": ("ppo", "p"), "RwMatrixScale": ("ppo", "p"), "RwMatrixMultiply": ("ppp", "p"), "RwMatrixInvert": ("pp", "p"),
        "RwMatrixRotate": ("ppfo", "p"), "RwMatrixTransform": ("ppo", "p"), "RtIntersectionLineTriangle": ("pppppp", "i"), "RtLineSphereIntersectionTest": ("ppp", "i"),
        "RtBaryGetTransform": ("ppppp", "i"), "RpRandomSeed": ("i", "v"), "RpRandom": ("", "i")}
ALIAS_OK = {"RwV3dNormalize", "RwV2dNormalize", "RwV3dTransformPoint", "RwV3dTransformPoints", "RwV3dTransformVector", "RwV3dTransformVectors"}      # out == in allowed
names = [n for f in ("rw_g0.txt", "rw_g0_candidates.txt") if os.path.exists(os.path.join(HERE, f)) for n in (l.split("#")[0].split() for l in open(os.path.join(HERE, f))) if n for n in n[:1]]
if "--only" in opt: names = opt["--only"].split(",")
names = list(dict.fromkeys(names))
SYM = {}
for f_ in ("symbols.txt", "aliases.txt"):
    for l in open(os.path.join(G, "src", f_)):
        q = l.split()
        if len(q) >= 2 and q[0] in names: SYM.setdefault(q[0], int(q[1], 16))
P = lambda x: (x + 0xfff) & ~0xfff
SCR, WR, STK, VEC = 0x50000000, 0x50001000, 0x51000000, 0x52000000
u = Uc(UC_ARCH_X86, UC_MODE_32); u.mem_map(SCR, 0x2000); u.mem_map(STK, 0x10000); u.mem_map(VEC, 0x10000)
pe = pefile.PE(EXE); base = pe.OPTIONAL_HEADER.ImageBase
u.mem_map(base, P(pe.OPTIONAL_HEADER.SizeOfImage)); u.mem_write(base, pe.get_data(0, pe.OPTIONAL_HEADER.SizeOfHeaders))
for s in pe.sections:
    d = s.get_data()
    if d: u.mem_write(base + s.VirtualAddress, d)
ENG, B, HEAP = 0x20000000, 0x300, 0x21000000
u.mem_map(ENG, 0x10000); u.mem_map(HEAP, 0x10000)
u.mem_write(0x7d2128, struct.pack("<I", ENG)); u.mem_write(ENG + 0x108, struct.pack("<I", SCR + 0x100)); u.mem_write(ENG + 0x10c, struct.pack("<I", SCR + 0x200))
u.mem_write(SCR + 0xff8, struct.pack("<I", HEAP))      # alloc cursor
u.mem_write(SCR + 0x100, b"\xa1" + struct.pack("<I", SCR + 0xff8) + b"\x81\x05" + struct.pack("<II", SCR + 0xff8, 0x4000) + b"\xc3"); u.mem_write(SCR + 0x200, b"\xc3")      # alloc: mov eax,[cur]; add [cur],0x4000; ret
u.mem_write(0x4ea160, b"\xc3"); u.mem_write(0x53eac0, b"\x31\xc0\xc3")      # the RwError helpers of the error paths
def call(tgt, args):
    code = b"\x0f\xae\x15" + struct.pack("<I", SCR + 20) + b"\xdb\xe3\xd9\x2d" + struct.pack("<I", SCR) + b"\xff\x15" + struct.pack("<I", SCR + 8) + b"\xd9\x3d" + struct.pack("<I", SCR + 4) + b"\xd9\x1d" + struct.pack("<I", SCR + 12) + b"\xa3" + struct.pack("<I", SCR + 16) + b"\xf4"
    u.mem_write(WR, code); u.mem_write(SCR, struct.pack("<I", CW)); u.mem_write(SCR + 20, struct.pack("<I", 0x9fc0));      # MXCSR with FTZ, as the game sets it (4d85d7)
    u.mem_write(SCR + 8, struct.pack("<I", tgt))
    u.mem_write(STK + 0x8000, b"".join(struct.pack("<I", x) for x in args)); u.reg_write(UC_X86_REG_ESP, STK + 0x8000); u.emu_start(WR, WR + len(code) - 1)
    return struct.unpack("<II", u.mem_read(SCR + 12, 8)) + struct.unpack("<H", u.mem_read(SCR + 4, 2))      # st0 bits, eax, cw after
call(0x4d5770, [0, B])
# the game's start-up (4d85f3) replaces the vector-multiply pointers by the variant its CPU flags select: A = SSE (4ec530..), B = 4ec310.., C = plain x87 (5dc000..); default = the ctor's 4d5010..
VAR = {"A": (0x4ec530, 0x4ec650, 0x4ec490, 0x4ec5c0), "B": (0x4ec310, 0x4ec420, 0x4ec280, 0x4ec3a0), "C": (0x5dc07f, 0x5dc160, 0x5dc000, 0x5dc0ef)}
if opt.get("--variant", "default") != "default": call(0x4d4ed0, list(VAR[opt["--variant"]]))
MODE = 0      # 0 = finite inputs only (the bit-exact bar), 1 = specials too: inf / NaN / random bit patterns (reported separately)
def fl():
    r = random.random()
    if MODE == 0: return random.choice([0, 0x80000000, 0x3f800000, 0xbf800000]) if r < 0.03 else struct.unpack("<I", struct.pack("<f", random.choice([-1, 1]) * random.random() * 2.0 ** random.randint(-12, 12)))[0]
    if r < 0.02: return random.choice([0, 0x80000000, 0x7f800000, 0xff800000, 0x7fc00000, 0x00000001, 0x007fffff, 0x7f7fffff, 0x3f800000])
    if r < 0.08: return random.getrandbits(32)
    return struct.unpack("<I", struct.pack("<f", random.choice([-1, 1]) * random.random() * 2.0 ** random.randint(-12, 12)))[0]
out = bytearray(); nf = 0
for n in names:
    if n not in SYM: print("skip (not in symbols.txt):", n); continue
    ak, rk = SPEC[n]; cases = bytearray(); nc = 0; skipped = 0; nfin = 0
    for i in range(N + N // 5):
        MODE = 0 if i < N else 1
        if i == N: nfin = nc
        bufs = [struct.pack("<16I", *[fl() for _ in range(16)]) for _ in range(4)]
        free = [0, 1, 2, 3]; random.shuffle(free); word = []
        for k, c in enumerate(ak):
            if c == "p":
                if n in ALIAS_OK and k == 1 and random.random() < 0.2: word.append(word[0]); continue
                b = free.pop() if free else random.randrange(4); word.append(b)
            elif c == "n": word.append(random.randint(0, 4))
            elif c == "o": word.append(random.randint(0, 2))
            elif c == "i": word.append(random.getrandbits(32) if n == "RpRandomSeed" else random.randrange(1 << 16))
            else: word.append(fl())
        for k, b in enumerate(bufs): u.mem_write(VEC + 0x100 * k, b)
        argv = [VEC + 0x100 * w if c == "p" else w for w, c in zip(word, ak)]
        try: st0, eax, cw = call(SYM[n], argv)
        except unicorn.UcError as e:
            skipped += 1
            if skipped == 1: print("  first emulator error %s: %s at eip %x" % (n, e, u.reg_read(__import__("unicorn.x86_const", fromlist=["x"]).UC_X86_REG_EIP)))
            continue
        if cw != CW: print("note: original changes the CW:", n, hex(cw))
        if rk == "f": res = st0
        elif rk == "p": res = 0 if eax == 0 else next((k + 1 for k in range(4) if eax == VEC + 0x100 * k), 0xffffffff)
        elif rk == "i": res = eax
        else: res = 0
        fin = b"".join(bytes(u.mem_read(VEC + 0x100 * k, 64)) for k in range(4))
        cases += struct.pack("<6I", *(word + [0] * 6)[:6]) + struct.pack("<I", res) + b"".join(bufs) + fin; nc += 1
    nf += 1; out += n.encode().ljust(32, b"\0") + struct.pack("<II", nc, nfin) + ak.encode().ljust(8, b"\0") + rk.encode().ljust(4, b"\0") + cases
    print("%-30s %6d cases (%d skipped by emulator errors)" % (n, nc, skipped))
open(OUTF, "wb").write(b"G0V1" + struct.pack("<II", CW, nf) + out)
