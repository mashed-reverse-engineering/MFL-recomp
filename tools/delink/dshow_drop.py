#!/usr/bin/env python3
"""A7: remove the statically linked DirectShow movie code (strmbase + Criterion RwaDS + the app side of the texture renderer) from a link.
  dshow_drop.py closure [exe]   writes dshow_dead.txt (`va size name`): the functions of the movie code pool that nothing outside the pool references
                                (call / jmp rel32, push / mov imm32 in code).  Data pointers (vtables, EH tables, the 12 XCU entries) do not count: globgen zeroes them.
  dshow_drop.py filter <work>   drops the objects of <work>/objs_local.txt that define a dead function (their C replaces dead code); prints them.
mkobj.py (DROP_DSHOW=1) fills the dead functions with int3 (like dead_tables.txt), globgen.py (DROP_DSHOW=1) writes 0 for pointer words into them, link_ours.sh wires it up.
The pool: the RwaDS renderer class and its adjustor thunks (0x4a30b0-0x4a3420, 0x4a3780-0x4a37f0, 0x4a3910, 0x4a3d80), the movie filters 0x4ac4f0-0x4b2a60 (crt starts there), EH funclets / XCU
initialisers 0x5db530-0x5dbb20.  The movie functions the game still has (movie_renderer_create/poll/release/stop, sound_volume_to_directsound_db, video_play_loop) call mfl_video instead (src/video)."""
import bisect, os, re, struct, subprocess, sys
T = os.path.dirname(os.path.abspath(__file__)); G = os.path.normpath(os.path.join(T, "..", ".."))
DEAD = os.path.join(T, "dshow_dead.txt")
POOL = [(0x4a30b0, 0x4a3420), (0x4a3780, 0x4a37f0), (0x4a3910, 0x4a3930), (0x4a3d80, 0x4a3fe0), (0x4ac4f0, 0x4b2a60), (0x5db530, 0x5dbb20)]
GONE = [(0x4a37f0, 0x4a3910), (0x4a3930, 0x4a3d80), (0x4a44a0, 0x4a4640)]      # app movie functions rewritten onto mfl_video: their old calls into the pool do not count
KEEP = {"movie_guid_compare"}      # also defined by the bridge file src/bridges/b004.c (a mixed object cannot be dropped); 78 bytes stay original
NOTCODE = {0x4a1eb1, 0x4a21b9}      # E9/E8 byte of an immediate (`mov [0x76e800],1` / `mov [0x76e858],1`), not a call into the pool (checked by hand)
def fns():
    r = sorted((int(a, 16), n, int(z)) for a, n, z in (l.split() for l in open(os.path.join(G, "output", "fnsizes.txt"))))
    return r, [x[0] for x in r]
def closure(exe):
    d = open(exe, "rb").read(); fn, st = fns()
    def fidx(a):
        i = bisect.bisect_right(st, a) - 1
        return i if i >= 0 and a < fn[i][0] + max(fn[i][2], 1) else None
    cand = {i for i, (a, n, z) in enumerate(fn) if any(lo <= a < hi for lo, hi in POOL) and n not in KEEP}
    e = struct.unpack_from("<I", d, 0x3c)[0]; ns = struct.unpack_from("<H", d, e + 6)[0]; so = struct.unpack_from("<H", d, e + 20)[0]
    refs = []
    for k in range(ns):
        h = e + 24 + so + 40 * k; nm = d[h:h + 8].rstrip(b"\0")
        if nm not in (b".text", b"_rwcseg"): continue
        vs, va, rs, rp = struct.unpack_from("<IIII", d, h + 8); base = 0x400000 + va; buf = d[rp:rp + vs]
        for i in range(1, len(buf) - 4):
            if buf[i - 1] in (0xe8, 0xe9):
                t = base + i + 4 + struct.unpack_from("<i", buf, i)[0]
                if 0x401000 <= t < 0x5dc451: refs.append((base + i, t))
            v = struct.unpack_from("<I", buf, i)[0]
            if 0x401000 <= v < 0x5dc451 and buf[i - 1] in (0x68, 0xb8, 0xb9, 0xba, 0xbb, 0xbd, 0xbe, 0xbf): refs.append((base + i, v))
    ch = True
    while ch:
        ch = False
        for s, t in refs:
            ti = fidx(t)
            if ti in cand and s not in NOTCODE and fidx(s) not in cand and not any(lo <= s < hi for lo, hi in POOL + GONE):
                cand.discard(ti); print("live: %08x %s <- %08x" % (fn[ti][0], fn[ti][1], s), file=sys.stderr); ch = True
    open(DEAD, "w").write("# va size name: dead movie code, see dshow_drop.py\n" + "".join("%08x %d %s\n" % (fn[i][0], min(fn[i+1][0], 0x5dbb20) - fn[i][0], fn[i][1]) for i in sorted(cand)))
    print("%d dead functions, %d bytes" % (len(cand), sum(min(fn[i+1][0], 0x5dbb20) - fn[i][0] for i in cand)))
def dead_set():
    return {int(l.split()[0], 16): l.split()[2] for l in open(DEAD) if l.strip() and not l.startswith("#")}
def filt(W):
    dead = dead_set(); fn, st = fns(); byname = {}
    for a, n, z in fn: byname.setdefault(n, []).append(a)
    loc = [l.split() for l in open(os.path.join(W, "objs_local.txt"))]
    out = subprocess.run(["objdump", "-t"] + [o for s, o in loc], capture_output=True, text=True).stdout
    cur, defs = None, {}
    for l in out.splitlines():
        m = re.match(r"^(\S+\.obj):", l)
        if m: cur = m.group(1); continue
        m = re.match(r"^\[\s*\d+\]\(sec\s+(\d+)\)\(fl 0x00\)\(ty\s+20\)\(scl\s+2\)\s+\(nx \d\) 0x[0-9a-f]+ (\S+)", l)
        if m and int(m.group(1)) > 0: defs.setdefault(cur, []).append(re.sub(r"@\d+$", "", m.group(2).lstrip("_@")))
    keep, drop = [], []
    for s, o in loc:
        vs = [a for n in defs.get(o, []) for a in byname.get(n, [])]
        nd = [a for a in vs if a in dead]
        if nd and len(nd) == len({a for a in vs}): drop.append(s); os.remove(o)      # link.rsp is `ls o/*.obj`: the object must leave the work dir too
        else:
            if nd: print("MIXED object %s defines dead and live functions: kept" % s, file=sys.stderr)
            keep.append((s, o))
    open(os.path.join(W, "objs_local.txt"), "w").write("".join("%s %s\n" % x for x in keep))
    print("dshow_drop: %d objects removed (%s ...)" % (len(drop), ",".join(drop[:4])))
if __name__ == "__main__":
    if sys.argv[1] == "closure": closure(sys.argv[2] if len(sys.argv) > 2 else os.path.join(G, "game", "MFL.exe"))
    else: filt(sys.argv[2])
