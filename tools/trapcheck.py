#!/usr/bin/env python3
"""trapcheck.py: static check that no int3-filled original code is referenced by the installed C.
  trapcheck.py [W] [--objs LIST] [--list FILE]...   W = a link_ours.sh work dir (default: newest output/gate/*_standalone/link); exit 1 on any violation
  trapcheck.py --selftest                           tools/negtests/trap: the old rw_trap.txt with RtDictDestroy (tworaces int3, cl_course_destroy_world.c) must fail, the current copy must pass
The filled lists (`va size name|why`): tools/delink/rw_trap.txt (DROP_RWTRAP), rw_dead.txt (DROP_RW), dshow_dead.txt (DROP_DSHOW), dead_tables.txt (always), dinput_dead.txt (DROP_DINPUT).
Referrers = every object of W/objs_local.txt (or LIST, `src obj` per line, e.g. check_all's b4/objs.txt): clusters, automatch_near, tier1, hand files, bridges, harness, lib sources.
  name / VA:  an undefined symbol no scanned object defines (so it binds to the original bytes in mfl.obj) whose name is a listed name, or resolves (mfl.obj symbols, output/fnsizes.txt,
              src/aliases.txt + symbols.txt, W/gen/globals_layout.csv, `<name>_<hex va>` / func_0x<va>) to an address inside a listed range;
  data:       such a symbol that resolves to an original data row (W/gen/globals_layout.csv) whose pointer words (output/delink_relocs.csv sites + aligned .data/.rdata words of game/MFL.exe),
              followed through further rows, reach a listed range (vtables, dispatch / plugin tables): flagged with the row chain.
Pointers from original code into the lists are not checked here: rw_trap is by construction code the original live code can reach but no coverage run executed."""
import bisect, csv, glob, os, re, struct, subprocess, sys
T = os.path.dirname(os.path.abspath(__file__)); G = os.path.dirname(T); D = os.path.join(T, "delink")
LISTS = ["rw_trap.txt", "rw_dead.txt", "dshow_dead.txt", "dead_tables.txt", "dinput_dead.txt"]
NONAME = {"dead_tables.txt", "dinput_dead.txt"}      # third column is a reason, not a function name
TLO, THI = 0x401000, 0x800000

def load_lists(paths):
    r = []
    for p in paths:
        for l in open(p):
            w = l.split(None, 2)
            if len(w) >= 2 and not l.startswith("#"):
                r.append((int(w[0], 16), int(w[0], 16) + max(int(w[1]), 1), w[2].strip() if len(w) > 2 else "", os.path.basename(p)))
    return sorted(r)

def norm(n):
    n = re.sub(r"@\d+$", "", n.lstrip("_@")); n = n[:-5] if n.endswith("_orig") else n
    return re.sub(r"(__t)?__fwd$", "", n)

def syms(objs):
    """{obj: (defined, undefined)} external names of COFF objects (objdump in batches)."""
    r = {o: (set(), set()) for o in objs}
    for i in range(0, len(objs), 400):
        cur = None
        for l in subprocess.run(["objdump", "-t"] + objs[i:i + 400], capture_output=True, text=True).stdout.splitlines():
            m = re.match(r"^(\S+):\s+file format", l)
            if m: cur = m.group(1); continue
            m = re.match(r"^\[\s*\d+\]\(sec\s+(-?\d+)\)\(fl 0x00\)\(ty\s+\w+\)\(scl\s+2\)\s+\(nx \d\) 0x[0-9a-f]+ (\S+)$", l)
            if m and cur in r and not m.group(2).startswith("__imp_"): r[cur][m.group(1) == "0"].add(norm(m.group(2)))
    return r

def check(W, lists, objlist=None):
    trap = load_lists(lists); tst = [x[0] for x in trap]
    def tr(a):
        i = bisect.bisect_right(tst, a) - 1
        return trap[i] if i >= 0 and a < trap[i][1] else None
    byname = {x[2]: x for x in trap if x[3] not in NONAME and x[2]}
    rows = sorted((int(r["va"], 16), int(r["size"]), r["name"]) for r in csv.DictReader(open(os.path.join(W, "gen", "globals_layout.csv"))))
    rst = [x[0] for x in rows]
    def ridx(a):
        i = bisect.bisect_right(rst, a) - 1
        return i if i >= 0 and a < rows[i][0] + max(rows[i][1], 1) else None
    # name -> original VA
    sym = {}
    mo = os.path.join(W, "mfl.obj")
    if os.path.exists(mo):
        for l in subprocess.run(["objdump", "-t", mo], capture_output=True, text=True).stdout.splitlines():
            m = re.match(r"^\[\s*\d+\]\(sec\s+1\).*0x([0-9a-f]{8}) (\S+)$", l)
            if m: sym[norm(m.group(2))] = 0x401000 + int(m.group(1), 16)
    for l in open(os.path.join(G, "output", "fnsizes.txt")):
        w = l.split(); sym.setdefault(w[1], int(w[0], 16))
    for f in ("aliases.txt", "symbols.txt"):
        for l in open(os.path.join(G, "src", f)):
            w = l.split()
            if len(w) == 2 and re.fullmatch(r"[0-9a-f]{8}", w[1]): sym.setdefault(w[0], int(w[1], 16))
    for a, z, n in rows: sym.setdefault(n, a)
    def va(n):
        if n in sym: return sym[n]
        m = re.search(r"(?:_|^func_0x|^FUN_|^DAT_)(?:0x)?([0-9a-f]{6,8})$", n, re.I)
        return int(m.group(1), 16) if m and TLO <= int(m.group(1), 16) < THI else None
    # row -> pointer targets (rows / listed ranges)
    d = open(os.path.join(G, "game", "MFL.exe"), "rb").read()
    e = struct.unpack_from("<I", d, 0x3c)[0]; ns = struct.unpack_from("<H", d, e + 6)[0]; so = struct.unpack_from("<H", d, e + 20)[0]
    words = []
    for k in range(ns):
        h = e + 24 + so + 40 * k
        if d[h:h + 8].rstrip(b"\0") not in (b".data", b".rdata"): continue
        vs, sva, rs, rp = struct.unpack_from("<IIII", d, h + 8)
        words += [(0x400000 + sva + i, struct.unpack_from("<I", d, rp + i)[0], False) for i in range(0, min(vs, rs) - 3, 4)]
    words += [(int(r["site"], 16), int(r["target"], 16), True) for r in csv.DictReader(open(os.path.join(G, "output", "delink_relocs.csv"))) if r["kind"] not in ("const", "header", "rel32")]
    starts = {int(l.split()[0], 16) for l in open(os.path.join(G, "output", "fnsizes.txt"))} | set(tst)
    redge = {}
    sys.path.insert(0, D); from dshow_drop import SLOT0, DEFTAB
    for s, t, isrel in words:
        r = ridx(s)
        if r is None or not TLO <= t < THI or SLOT0 <= s < DEFTAB + 0x11c: continue      # ponytail: the D3DX slot table / its default copy are skipped (runtime-installed, dshow_drop.py pool models them); a C reader of a slot is not checked
        if tr(t) and (isrel or t in starts): redge.setdefault(r, set()).add(("t", t))      # a raw data word counts as a code pointer only at a known start (floats / ints in byte spans hit code ranges by chance)
        elif ridx(t) is not None and ridx(t) != r: redge.setdefault(r, set()).add(("r", ridx(t)))
    # objects
    lst = objlist or os.path.join(W, "objs_local.txt")
    objs = [(s, o.strip()) for s, o in (l.split(None, 1) for l in open(lst) if l.strip())]
    sy = syms([o for s, o in objs]); per = []; defined = set()
    dsh = {int(l.split()[0], 16) for l in open(os.path.join(D, "dshow_dead.txt")) if l.strip() and not l.startswith("#")}
    for src, o in objs:
        dd, uu = sy[o]; vs = {va(n) for n in dd} - {None}
        if vs and vs <= dsh: continue      # dshow_drop.py filter: link_ours.sh (DROP_DSHOW) does not link C that only replaces dead movie code (matters for --objs of an overlay build)
        defined |= dd; per.append((src, uu))
    cdef = {va(n) for n in defined} - {None}      # original addresses our objects define (replaced bodies, func_0x004ab230 of dinput_formats.c, DLL thunks): pointers to them bind to the C
    viol = []; rowseed = {}
    for src, uu in per:
        for n in sorted(uu - defined):
            a = va(n); x = byname.get(n) or (tr(a) if a is not None and a not in cdef else None)
            if x: viol.append((x, src, "symbol %s" % n)); continue
            r = ridx(a) if a is not None else None
            if r is not None: rowseed.setdefault(r, []).append((src, n))
    # rows referenced by C: breadth-first through row pointers, report every listed target with its shortest chain
    seen = {}; work = []
    for r, refs in rowseed.items(): seen[r] = None; work.append(r)
    while work:
        nxt = []
        for r in work:
            for k, v in sorted(redge.get(r, ())):
                if k == "r" and v not in seen: seen[v] = r; nxt.append(v)
                elif k == "t" and v not in cdef:
                    ch = [r]
                    while seen[ch[-1]] is not None: ch.append(seen[ch[-1]])
                    src, n = rowseed[ch[-1]][0]
                    viol.append((tr(v), src, "data %s%s -> %08x" % ("" if n == rows[ch[-1]][2] else n + ": ", " -> ".join(rows[c][2] for c in reversed(ch)), v)))
        work = nxt
    out, seen_ = [], set()      # one line per (listed entry, referrer object)
    for x in viol:
        if (x[0], x[1]) not in seen_: seen_.add((x[0], x[1])); out.append(x)
    return out

def report(viol):
    for (lo, hi, name, lf), src, via in viol:
        print("VIOLATION %-16s %08x %-32s <- %s (%s)" % (lf, lo, name[:32], src, via))
    print("trapcheck: %d violation(s)" % len(viol))
    return 1 if viol else 0

def newest_link():
    c = sorted(p for p in glob.glob(os.path.join(G, "output", "gate", "*_standalone", "link")) if os.path.exists(os.path.join(p, "objs_local.txt")) and os.path.exists(os.path.join(p, "gen", "globals_layout.csv")))
    return c[-1] if c else None

def selftest():
    W = newest_link()
    if not W: print("trapcheck selftest: skipped (no standalone link under output/gate)"); return 0
    N = os.path.join(T, "negtests", "trap"); rc = 0
    bad = check(W, [os.path.join(N, "rw_trap_bad.txt")])
    if any(x[0][2] == "RtDictDestroy" and x[1] == "cl_course_destroy_world.c" for x in bad): print("selftest ok   bad list: RtDictDestroy <- cl_course_destroy_world.c flagged")
    else: print("selftest FAIL bad list: RtDictDestroy not flagged"); rc = 1
    good = check(W, [os.path.join(N, "rw_trap_good.txt")])
    if not good: print("selftest ok   good list: 0 violations")
    else: report(good); print("selftest FAIL good list flagged"); rc = 1
    print("trapcheck selftest: %s (%s)" % ("FAIL" if rc else "ok", os.path.relpath(W, G)))
    return rc

if __name__ == "__main__":
    a = sys.argv[1:]
    if a[:1] == ["--selftest"]: sys.exit(selftest())
    objlist = None; lists = []; W = None
    while a:
        x = a.pop(0)
        if x == "--objs": objlist = a.pop(0)
        elif x == "--list": lists.append(a.pop(0))
        else: W = x
    W = W or newest_link()
    print("trapcheck: link %s, objects %s" % (W, objlist or "objs_local.txt"))
    sys.exit(report(check(W, lists or [os.path.join(D, f) for f in LISTS], objlist)))
