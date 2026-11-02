#!/usr/bin/env python3
"""A7: remove the statically linked DirectShow movie code (strmbase + Criterion RwaDS + the app side of the texture renderer) from a link.
  dshow_drop.py closure [exe]   writes dshow_dead.txt (`va size name`): the functions of the movie code pool that nothing outside the pool references
                                (call / jmp rel32, push / mov imm32 in code).  Data pointers (vtables, EH tables, the 12 XCU entries) do not count: globgen zeroes them.
  dshow_drop.py pool <work>     S3: writes rw_dead.txt = dead RenderWare/D3DX library pool code (reachability from the live code, see pool()); `explain <work> <fn>...` prints why a function is live.
                                Applied with DROP_RW=1 (mkobj fills, globgen zeroes pointers, default off).  <work> = a link_ours.sh dir with LIB_RW etc. set like the link that will use the list; DROP_DSHOW=1 in the env.
  dshow_drop.py filter <work>   drops the objects of <work>/objs_local.txt that define a dead function (their C replaces dead code); prints them.
mkobj.py (DROP_DSHOW=1) fills the dead functions with int3 (like dead_tables.txt), globgen.py (DROP_DSHOW=1) writes 0 for pointer words into them, link_ours.sh wires it up.
The pool: the RwaDS renderer class and its adjustor thunks (0x4a30b0-0x4a3420, 0x4a3780-0x4a37f0, 0x4a3910, 0x4a3d80), the movie filters 0x4ac4f0-0x4b2a60 (crt starts there), EH funclets / XCU
initialisers 0x5db530-0x5dbb20.  The movie functions the game still has (movie_renderer_create/poll/release/stop, sound_volume_to_directsound_db, video_play_loop) call mfl_video instead (src/video)."""
import bisect, collections, csv, os, re, struct, subprocess, sys, tempfile
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
# ---- S3: pool mode (RenderWare + D3DX library pool, tools/delink/rw_dead.txt) ----
RWDEAD = os.path.join(T, "rw_dead.txt")
POOL_LAYERS = ("RenderWare", "D3DX")
def pool(W, out=RWDEAD):
    """Reachability from the roots over a graph of functions (output/fnsizes.txt) and data rows (gen/globals_layout.csv).  Roots = every function outside the pool whose original bytes are still
    linked (not replaced by C / the DLL thunks: mfl.obj.dropapp.csv, not DirectShow-dead) + everything our C objects (o/*.obj, gen/*.obj) name.  Edges out of a function = rel32 branches / exact function start
    immediates in its bytes + output/delink_relocs.csv sites in it; a data row is live when a live function, C object or live row refers into it, and then ALL its pointer words keep their targets alive
    (the rule that protects dispatch / plugin / vtable rows).  Dead = pool functions never reached; bodies already replaced are not listed.  The D3DX slot table gets special handling (INSTALLERS)."""
    sys.path.insert(0, T)
    from layers import layer, item_at
    fn, st = fns(); N = len(fn)
    d = open(os.path.join(G, "game", "MFL.exe"), "rb").read()
    e = struct.unpack_from("<I", d, 0x3c)[0]; ns = struct.unpack_from("<H", d, e + 6)[0]; so = struct.unpack_from("<H", d, e + 20)[0]
    S = {}
    for k in range(ns):
        h = e + 24 + so + 40 * k; vs, va, rs, rp = struct.unpack_from("<IIII", d, h + 8); S[d[h:h + 8].rstrip(b"\0").decode()] = (0x400000 + va, vs, rp, rs)
    TLO, THI = 0x401000, 0x5dc451
    def sec_bytes(nm): va, vs, rp, rs = S[nm]; return va, d[rp:rp + min(vs, rs)]
    # data rows
    rows = sorted((int(r["va"], 16), int(r["size"]), r["name"]) for r in csv.DictReader(open(os.path.join(W, "gen", "globals_layout.csv"))))
    rst = [x[0] for x in rows]
    def ridx(a):
        i = bisect.bisect_right(rst, a) - 1
        if i >= 0 and a < rows[i][0] + max(rows[i][1], 1): return i
        j = bisect.bisect_right(rst, a + 16) - 1      # base - small offset (`[idx*4 + table - 4]`): the next row starts within 16 bytes
        return j if j >= 0 and rst[j] > a else None
    def fidx(a):
        i = bisect.bisect_right(st, a) - 1
        return i if i >= 0 and TLO <= a < THI else None
    def gapref(v):
        if not TLO <= v < THI: return None
        i = fidx(v)
        return i if i is not None and v >= fn[i][0] + max(fn[i][2], 1) else None
    # replaced / dead bodies: no outgoing edges
    gone = set(); FILL = []      # FILL = byte ranges of the exe that are int3 / replaced in the link: references sited in them do not count, the bytes behind a replaced function (undeclared code in its block) stay original and do
    for r in csv.DictReader(open(os.path.join(W, "mfl.obj.dropapp.csv"))):
        if r["status"] == "dropped": gone.add(int(r["va"], 16))
        if r["status"] in ("dropped", "partial"): FILL.append((int(r["va"], 16), int(r["va"], 16) + int(r["size"])))
    if os.environ.get("DROP_DSHOW"):
        gone |= set(dead_set())
        FILL += [(int(l.split()[0], 16), int(l.split()[0], 16) + int(l.split()[1])) for l in open(DEAD) if l.strip() and not l.startswith("#")]
    import glob      # source-built libs (LIB_LUA/QHULL/ZLIB=src) and split CRT members: their original bytes are filled (origbytes.py does the same)
    fr = []
    for f_ in glob.glob(os.path.join(W, "lib", "*", "items.csv")):
        if f_.endswith("/crt/items.csv"): continue
        fr += [(int(r["lo"], 16), int(r["hi"], 16)) for r in csv.DictReader(open(f_)) if r["member"].endswith(":src") and r["kind"] == "code"]
    if os.path.exists(os.path.join(W, "lib", "crt", "members.txt")):
        mem = {l.strip() for l in open(os.path.join(W, "lib", "crt", "members.txt"))}
        fr += [(int(r["lo"], 16), int(r["hi"], 16)) for r in csv.DictReader(open(os.path.join(W, "lib", "crt", "items.csv"))) if r["member"] in mem and r["kind"] == "code" and r["owner"] == "crt"]
    FILL += fr; FILL.sort(); fll = [x[0] for x in FILL]
    def filled(a):
        j = bisect.bisect_right(fll, a) - 1
        while j >= 0 and j > bisect.bisect_right(fll, a) - 4:      # a few overlapping ranges
            if a < FILL[j][1]: return True
            j -= 1
        return False
    fr.sort(); frl = [x[0] for x in fr]
    for a_, n_, z_ in fn:
        j = bisect.bisect_right(frl, a_) - 1
        if j >= 0 and a_ < fr[j][1]: gone.add(a_)
    inpool = [layer(a, *(lambda it: (it[3], it[4]) if it else ("", ""))(item_at(a))) in POOL_LAYERS for a, n, z in fn]
    fedge = [set() for _ in range(N)]; redge = [set() for _ in range(len(rows))]      # targets as ("f", i) / ("r", j)
    def add(src, t, rel=False):
        i = fidx(t)
        if i is not None: src.add(("f", i)); return
        j = ridx(t)
        if j is not None: src.add(("r", j))
    # byte scans
    starts = set(st); slotref = collections.defaultdict(set); cond = []
    for nm in (".text", "_rwcseg"):
        base, buf = sec_bytes(nm); end = base + len(buf)
        for i in range(len(buf) - 4):
            a = base + i; b1 = buf[i - 1] if i else 0
            if filled(a): continue
            if i >= 1 and (b1 in (0xe8, 0xe9) or (i >= 2 and buf[i - 2] == 0x0f and 0x80 <= b1 <= 0x8f)):
                t = a + 4 + struct.unpack_from("<i", buf, i)[0]
                if TLO <= t < THI:
                    f = fidx(a)
                    if f is not None and fidx(t) != f: fedge[f].add(("f", fidx(t)))
            v = struct.unpack_from("<I", buf, i)[0]
            if v in starts or gapref(v) is not None:      # a function start, or an address in the bytes between two listed functions (undeclared code / jump tables / constants of the block in front)
                f = fidx(a)
                if f is not None and fidx(v) != f: fedge[f].add(("f", fidx(v)))
            elif TLO <= v < 0x7ff000 and ridx(v) is not None:
                f = fidx(a)
                if f is not None:
                    fedge[f].add(("r", ridx(v)))
                    if SLOT0 <= v < SLOT0 + 0x120 and v % 4 == 0: slotref[f].add(v)
    for nm in (".rdata", ".data"):
        if nm not in S: continue
        base, buf = sec_bytes(nm)
        for i in range(0, len(buf) - 3, 4):
            v = struct.unpack_from("<I", buf, i)[0]; r_ = ridx(base + i)
            if r_ is None: continue
            if SLOT0 <= base + i < DEFTAB + 0x11c:      # the D3DX slot table and its default copy: conditional edges (cond), not row edges
                if v in starts and base + i < SLOT0 + 0x120: cond.append((base + i, fidx(v)))
                continue
            if v in starts or gapref(v) is not None: redge[r_].add(("f", fidx(v)))
            elif TLO <= v < 0x7ff000 and ridx(v) is not None and ridx(v) != r_: redge[r_].add(("r", ridx(v)))
    for r in csv.DictReader(open(os.path.join(G, "output", "delink_relocs.csv"))):
        if r["kind"] in ("const", "header", "rel32"): continue
        s_, t_ = int(r["site"], 16), int(r["target"], 16)
        f = fidx(s_)
        if f is not None:
            if not filled(s_): add(fedge[f], t_)
        elif not SLOT0 <= s_ < DEFTAB + 0x11c:
            r_ = ridx(s_)
            if r_ is not None: add(redge[r_], t_)
    # C objects: undefined symbols -> original VA (mfl.obj externals, layout names, `_<name>_<hex va>`)
    sym = {}
    for l in subprocess.run(["objdump", "-t", os.path.join(W, "mfl.obj")], capture_output=True, text=True).stdout.splitlines():
        m = re.match(r"^\[\s*\d+\]\(sec\s+(\d+)\).*0x([0-9a-f]{8}) (\S+)$", l)
        if m and m.group(1) == "1": sym[m.group(3).lstrip("_")] = 0x401000 + int(m.group(2), 16)
    for a, n, z in fn: sym.setdefault(n, a)
    for f_ in ("aliases.txt", "symbols.txt"):
        for l in open(os.path.join(G, "src", f_)):
            w = l.split()
            if len(w) == 2 and re.fullmatch(r"[0-9a-f]{8}", w[1]): sym.setdefault(w[0], int(w[1], 16))
    for a, z, n in rows: sym.setdefault(n, a)
    objs = [o for s, o in (l.split() for l in open(os.path.join(W, "objs_local.txt"))) if "thunks" not in o]
    objs += [os.path.join(W, "gen", "stubs.obj"), os.path.join(W, "lib", "stubs.obj")]      # gen/data|rdata|bss.obj hold every data row (all pointers): the rows are modelled by redge instead
    roots = set(); miss = collections.Counter()
    for o in objs:
        if not os.path.exists(o): continue
        out_ = subprocess.run(["objdump", "-t", o], capture_output=True, text=True).stdout
        for l in out_.splitlines():
            m = re.match(r"^\[\s*\d+\]\(sec\s+0\)\(fl 0x00\)\(ty\s+\d+\)\(scl\s+2\)\s+\(nx \d\) 0x[0-9a-f]+ (\S+)$", l)
            if not m: continue
            n = re.sub(r"@\d+$", "", m.group(1).lstrip("_@")); n = n[:-5] if n.endswith("_orig") else n
            n = re.sub(r"(__t)?__fwd$", "", n); a = sym.get(n)
            if a is None:
                m2 = re.search(r"_(?:0x)?([0-9a-f]{6,8})$", n); a = int(m2.group(1), 16) if m2 else None
            if a is None: miss[n] += 1; continue
            add(roots, a)
    # D3DX dispatch table (60+ slots at SLOT0, filled by D3DXCpuOptimizations from one *_InitFastTable): the InitFastTable of the CPU class this port targets is sse2 (x86-64 hosts always have SSE2; the 3DNow, SSE
    # and plain x86 variants are cut), and a slot's installed function stays alive only while live code outside the installer reads that slot.  The default table (0x62eda8, copied first) is an ordinary row.
    nidx = {n: i for i, (a, n, z) in enumerate(fn)}; inst = {}
    for nm in INSTALLERS:
        if nm not in nidx: continue
        i = nidx[nm]; a0 = fn[i][0]; ln = fn[i + 1][0] - a0
        with tempfile.NamedTemporaryFile() as tf:
            tf.write(d[S[".text"][2] + a0 - S[".text"][0]:][:ln]); tf.flush()
            txt = subprocess.run(["objdump", "-D", "-b", "binary", "-mi386", "-M", "intel", "--adjust-vma=0x%x" % a0, tf.name], capture_output=True, text=True).stdout
        reg, pairs = {}, []
        for l in txt.splitlines():
            m = re.search(r"\tmov\s+(e\w\w),0x([0-9a-f]+)$", l)
            if m: reg[m.group(1)] = int(m.group(2), 16); continue
            m = re.search(r"\tmov\s+DWORD PTR \[(e\w\w)(?:\+0x([0-9a-f]+))?\],(?:0x([0-9a-f]+)|(e\w\w))$", l)
            if m:
                v = int(m.group(3), 16) if m.group(3) else reg.get(m.group(4))
                if v in starts: pairs.append((SLOT0 + (int(m.group(2), 16) if m.group(2) else 0), fidx(v)))
        assert len(pairs) > 20 or "x86" in nm, (nm, len(pairs)); inst[i] = pairs
        fedge[i] -= {("f", t) for s_, t in pairs}
    cpu = nidx.get("D3DXCpuOptimizations")
    if cpu is not None:
        fedge[cpu] -= {("f", nidx[x]) for x in INSTALLERS if x in nidx and "sse2" not in x}
        dflt = ridx(DEFTAB)      # the default table (copied into the slots first) keeps its function words only for slots something reads
        fedge[cpu].discard(("r", dflt)); dpairs = []
        for k in range(0x47):
            v, = struct.unpack_from("<I", d, S[".data"][2] + DEFTAB - S[".data"][0] + 4 * k)
            if v in starts: dpairs.append((SLOT0 + 4 * k, fidx(v)))
        inst[cpu] = dpairs
    liveslots = set()
    live = set(); work = []
    def mark(x):
        if x not in live: live.add(x); work.append(x)
    for x in roots: mark(x)
    for i, (a, n, z) in enumerate(fn):
        if not inpool[i] and a not in gone: mark(("f", i))
    why = {}
    while True:
        while work:
            x = work.pop()
            if x[0] == "f":
                i = x[1]
                es = fedge[i]
                if i not in inst: liveslots |= slotref[i]
            else: es = redge[x[1]]
            for t in es:
                if t not in live: why[t] = x
                mark(t)
        for i, pairs in inst.items():
            if ("f", i) in live:
                for s_, t in pairs:
                    if s_ in liveslots and ("f", t) not in live: why[("f", t)] = ("f", i); mark(("f", t))
        for s_, t in cond:      # static initial slot values (equal to the default table): alive while the slot is read
            if s_ in liveslots and ("f", t) not in live: why[("f", t)] = ("r", ridx(SLOT0)); mark(("f", t))
        if not work: break
    cand = [i for i in range(N) if inpool[i] and ("f", i) not in live and fn[i][0] not in gone and fn[i][0] < 0x5dc000]      # _rwcseg (0x451 bytes) is not filled (mkobj fills .text only)
    end_ = lambda i: min(fn[i + 1][0] if i + 1 < N else THI, 0x5dc000 if fn[i][0] < 0x5dc000 else THI)
    open(out, "w").write("# va size name: dead RenderWare/D3DX pool code (dshow_drop.py pool), see ROADMAP T-S3\n" + "".join("%08x %d %s\n" % (fn[i][0], end_(i) - fn[i][0], fn[i][1]) for i in cand))
    tot = sum(end_(i) - fn[i][0] for i in cand); print("%d dead pool functions, %d bytes (pool %d functions); %d unresolved C-object symbols" % (len(cand), tot, sum(inpool), len(miss)))
    return fn, live, why, inpool, cand, end_, rows
SLOT0 = 0x62ec88; DEFTAB = 0x62eda8
INSTALLERS = ("sse2_D3DXInitFastTable", "sse_D3DXInitFastTable", "x3d_D3DXInitFastTable", "x86_D3DXInitFastTable")
def explain(W, names):
    fn, live, why, inpool, cand, end_, rows = pool(W, out=os.path.join(T, "..", "..", "output", "rw_dead_scratch.txt"))
    idx = {n: i for i, (a, n, z) in enumerate(fn)}
    for n in names:
        x = ("f", idx[n]); chain = []
        while x in why and len(chain) < 12: x = why[x]; chain.append("%s:%s" % (x[0], fn[x[1]][1] if x[0] == "f" else rows[x[1]][2]))
        print(n, "<-", " <- ".join(chain))
if __name__ == "__main__":
    if sys.argv[1] == "pool": pool(sys.argv[2])
    elif sys.argv[1] == "explain": explain(sys.argv[2], sys.argv[3:])
    elif sys.argv[1] == "closure": closure(sys.argv[2] if len(sys.argv) > 2 else os.path.join(G, "game", "MFL.exe"))
    else: filt(sys.argv[2])
