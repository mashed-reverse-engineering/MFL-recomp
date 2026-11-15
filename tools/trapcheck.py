#!/usr/bin/env python3
"""trapcheck.py: static check that no int3-filled original code is referenced by the installed C.
  trapcheck.py [W] [--objs LIST] [--list FILE]...   W = a link_ours.sh work dir (default: newest output/gate/*_standalone/link); exit 1 on any violation
  trapcheck.py --selftest [W]                       (W missing its link files = FAIL; no W = newest link, skipped if none) bad case built from W at run time (bad_selftest: one library function's defining object left out, the function listed: its C caller must fail; W9, replaces the
                                                    RtDictDestroy fixture rw_trap_bad.txt that G1's C port retired), tools/negtests/trap/rw_trap_good.txt must pass
The filled lists (`va size name|why`): tools/delink/rw_trap.txt (DROP_RWTRAP), rw_dead.txt (DROP_RW), dshow_dead.txt (DROP_DSHOW), dead_tables.txt (always), dinput_dead.txt (DROP_DINPUT).
Referrers = every object of W/objs_local.txt (or LIST, `src obj` per line, e.g. check_all's b4/objs.txt): clusters, automatch_near, tier1, hand files, bridges, harness, lib sources.
  name / VA:  an undefined symbol no scanned object defines (so it binds to the original bytes in mfl.obj) whose name is a listed name, or resolves (mfl.obj symbols, output/fnsizes.txt,
              src/aliases.txt + symbols.txt, W/gen/globals_layout.csv, `<name>_<hex va>` / func_0x<va>) to an address inside a listed range;
  data:       such a symbol that resolves to an original data row (W/gen/globals_layout.csv) whose pointer words (output/delink_relocs.csv sites + aligned .data/.rdata words of game/MFL.exe),
              followed through further rows, reach a listed range (vtables, dispatch / plugin tables): flagged with the row chain.
  const:      (W8 T-CA) an unrelocated instruction operand of a scanned object (objdump -d -r) inside a listed range: a call / push by constant address (`((void (*)(void))0x5cf230)()`).
Caller allow (W8 T-TCA): tools/delink/trap_caller_allow.txt (or --allow FILE), lines `<C symbol> <caller VA> <trap row name | VA> [<data VA>]  # reason`.  A violation of an object that defines
  <C symbol> against that trap row is not reported.  Each line is proved, else it is itself a violation: reason present, the trap row is still in a list (stale otherwise), <C symbol> resolves
  to <caller VA>, <caller VA> starts an output/fnsizes.txt row, and the original bytes of that row (objdump of game/MFL.exe) reference the trap row (call / jmp target or any operand);
  with <data VA>: the MFL.exe word there points into the trap row and the caller references the same globals row.  ponytail: per object, not per function (an object that defines
  <C symbol> may reach the row from another of its functions); objdump-per-function if that ever matters.
  trapcheck.py --selftest-allow OBJLIST [W]: the T-TCA / T-CA negatives on a strict build's object list (needs an_rwa_dxaudio_codec_output.c and an_hk_1367.c in it).
Pointers from original code into the lists are not checked here: rw_trap is by construction code the original live code can reach but no coverage run executed."""
import bisect, csv, glob, os, re, struct, subprocess, sys
from concurrent.futures import ThreadPoolExecutor
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

def consts(o):
    """unrelocated operand values >= TLO of object o (objdump -d -r, objscan.py's reading)"""
    out = subprocess.run(["objdump", "-d", "-r", "-M", "intel", o], capture_output=True, text=True).stdout.splitlines(); r = set()
    for i, l in enumerate(out):
        m = re.match(r"\s+([0-9a-f]+):\t([0-9a-f ]+)\t(.*)", l)
        if not m or i + 1 < len(out) and re.search(r"dir32|DISP32|IMAGE_REL|R_386", out[i + 1]) and not re.match(r"\s+[0-9a-f]+:\t", out[i + 1]): continue
        if m.group(3).split()[:1] not in (["push"], ["mov"], ["lea"], ["call"], ["jmp"], ["cmp"]): continue      # an address is pushed / stored / called / compared; `or eax,0x501513` is a colour (W8 G1: cl_frontend_draw.c, cl_menu_draw_team_select.c)
        r |= {int(v, 16) for v in re.findall(r"0x([0-9a-f]{6,8})\b", m.group(3)) if TLO <= int(v, 16) < THI}
    return r

def orig_refs(va, size):
    """every operand value / branch target in the original bytes [va, va+size) of game/MFL.exe"""
    out = subprocess.run(["objdump", "-d", "-M", "intel", "--start-address=0x%x" % va, "--stop-address=0x%x" % (va + size), os.path.join(G, "game", "MFL.exe")], capture_output=True, text=True).stdout
    return {int(v, 16) for l in out.splitlines() if re.match(r"\s+[0-9a-f]+:\t", l) for v in re.findall(r"0x([0-9a-f]+)\b", l.split("\t")[-1])}

def load_allow(p):
    """-> [(line no, raw line, C symbol, caller VA, trap row text, data VA | None, problem | None)]"""
    r = []
    for i, l in enumerate(open(p), 1):
        body, hsh, why = l.partition("#")
        w = body.split()
        if not w: continue
        try:
            assert len(w) in (3, 4), "need <C symbol> <caller VA> <trap row> [<data VA>]"
            assert why.strip(), "reason (# ...) missing"
            r.append((i, l.strip(), w[0], int(w[1], 16), w[2], int(w[3], 16) if len(w) == 4 else None, None))
        except (AssertionError, ValueError) as e:
            r.append((i, l.strip(), w[0], 0, "", None, str(e)))
    return r

def check(W, lists, objlist=None, allow=None, scan_consts=False):
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
    words = []; secs = []
    for k in range(ns):
        h = e + 24 + so + 40 * k
        secs.append(struct.unpack_from("<IIII", d, h + 8))
        if d[h:h + 8].rstrip(b"\0") not in (b".data", b".rdata"): continue
        vs, sva, rs, rp = struct.unpack_from("<IIII", d, h + 8)
        words += [(0x400000 + sva + i, struct.unpack_from("<I", d, rp + i)[0], False) for i in range(0, min(vs, rs) - 3, 4)]
    words += [(int(r["site"], 16), int(r["target"], 16), True) for r in csv.DictReader(open(os.path.join(G, "output", "delink_relocs.csv"))) if r["kind"] not in ("const", "header", "rel32")]
    starts = {int(l.split()[0], 16) for l in open(os.path.join(G, "output", "fnsizes.txt"))} | set(tst)
    redge = {}
    def xedge(t):      # S w7: a pointer from another row into the middle of a coarse g_libspan row reaches only the part at that address (its sub-row, resolved in the walk), like a same-span pointer
        r = ridx(t)
        return ("p", t) if rows[r][2].startswith("g_libspan") and t != rows[r][0] else ("r", r)
    sys.path.insert(0, D); from dshow_drop import SLOT0, DEFTAB
    for s, t, isrel in words:
        r = ridx(s)
        if r is None or not TLO <= t < THI or SLOT0 <= s < DEFTAB + 0x11c: continue      # ponytail: the D3DX slot table / its default copy are skipped (runtime-installed, dshow_drop.py pool models them); a C reader of a slot is not checked
        if tr(t) and (isrel or t in starts): redge.setdefault(r, set()).add(("t", t))      # a raw data word counts as a code pointer only at a known start (floats / ints in byte spans hit code ranges by chance)
        elif ridx(t) is not None and ridx(t) != r: redge.setdefault(r, set()).add(xedge(t))
    # a C data symbol inside a coarse g_libspan row of W's layout (no C named it when W was linked; globgen gives it a row of its own once C does):
    # its own sub-row from its address to the next known symbol, so the rest of the span's pointers do not count (gate 1112_1838: 22 false hits)
    symaddr = sorted(set(sym.values())); subk = {}
    def subrow(a, r):
        lo, hi = rows[r][0], rows[r][0] + max(rows[r][1], 1)
        if a == lo or not rows[r][2].startswith("g_libspan"): return r
        if a in subk: return subk[a]
        i = bisect.bisect_right(symaddr, a); end = min(symaddr[i] if i < len(symaddr) else hi, hi)
        k = subk[a] = len(rows); rows.append((a, end - a, "%s@%08x" % (rows[r][2], a)))
        for s, t, isrel in words:
            if not a <= s < end or not TLO <= t < THI or SLOT0 <= s < DEFTAB + 0x11c: continue
            if tr(t) and (isrel or t in starts): redge.setdefault(k, set()).add(("t", t))
            elif ridx(t) == r: redge.setdefault(k, set()).add(("r", subrow(t, r)))      # into the same span: again only the part at that address
            elif ridx(t) is not None: redge.setdefault(k, set()).add(xedge(t))
        return k
    # objects
    lst = objlist or os.path.join(W, "objs_local.txt")
    objs = [(s, o.strip()) for s, o in (l.split(None, 1) for l in open(lst) if l.strip())]
    sy = syms([o for s, o in objs]); per = []; defined = set(); sdef = {}
    dsh = {int(l.split()[0], 16) for l in open(os.path.join(D, "dshow_dead.txt")) if l.strip() and not l.startswith("#")}
    for src, o in objs:
        dd, uu = sy[o]; vs = {va(n) for n in dd} - {None}
        if vs and vs <= dsh: continue      # dshow_drop.py filter: link_ours.sh (DROP_DSHOW) does not link C that only replaces dead movie code (matters for --objs of an overlay build)
        defined |= dd; per.append((src, uu, o)); sdef.setdefault(src, set()).update(dd)
    cdef = {va(n) for n in defined if not n.startswith("mfl_trap_")} - {None}      # (W8: an_hk_612's trap stub mfl_trap_<name>_<va> replaces nothing: func_0x005470e0 was missed) original addresses our objects define (replaced bodies, func_0x004ab230 of dinput_formats.c, DLL thunks): pointers to them bind to the C
    viol = []; rowseed = {}
    for src, uu, o in per:
        for n in sorted(uu - defined):
            a = va(n); x = byname.get(n) or (tr(a) if a is not None and a not in cdef else None)
            if x: viol.append((x, src, "symbol %s" % n)); continue
            r = ridx(a) if a is not None else None
            if r is not None: rowseed.setdefault(subrow(a, r), []).append((src, n))
    if scan_consts:      # T-CA: a trap row called / pushed by its constant address carries no symbol
        with ThreadPoolExecutor(4) as ex:
            for (src, uu, o), cs in zip(per, ex.map(consts, [o for s, u, o in per])):
                viol += [(tr(v), src, "const %08x" % v) for v in sorted(cs) if tr(v) and v not in cdef]
    # rows referenced by C: breadth-first through row pointers, report every listed target with its shortest chain
    seen = {}; work = []
    for r, refs in rowseed.items(): seen[r] = None; work.append(r)
    while work:
        nxt = []
        for r in work:
            for k, v in sorted(redge.get(r, ())):
                if k == "p": k, v = "r", subrow(v, ridx(v))
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
    if not allow: return out
    # T-TCA: prove each allow line, then drop the violations it covers
    fns = {}
    for l in open(os.path.join(G, "output", "fnsizes.txt")):
        w = l.split()
        if len(w) >= 3 and w[-1].isdigit(): fns.setdefault(int(w[0], 16), int(w[-1]))
    def word(a):
        for vs, sva, rs, rp in secs:
            if 0x400000 + sva <= a < 0x400000 + sva + min(vs, rs) - 3: return struct.unpack_from("<I", d, rp + a - 0x400000 - sva)[0]
    bad, ok = [], []
    for i, raw, sym_, cva, trow, dva, prob in load_allow(allow):
        hit = [x for x in trap if x[3] not in NONAME and (x[2] == trow or re.fullmatch(r"(0x)?[0-9a-fA-F]{6,8}", trow) and x[0] == int(trow, 16))] if not prob else []
        if not prob and not hit: prob = "stale: trap row %s is in no list" % trow
        sva_ = va(sym_) if va(sym_) is not None else va(norm(sym_)); sym_ = norm(sym_)      # object names are compared normalised (syms())
        if not prob and sva_ != cva: prob = "%s resolves to %s, not %08x" % (sym_, "%08x" % sva_ if sva_ is not None else "nothing", cva)
        if not prob and cva not in fns: prob = "caller %08x has no output/fnsizes.txt row" % cva
        if not prob:
            t = hit[0]; refs = orig_refs(cva, fns[cva])
            if dva is None:
                if not any(t[0] <= v < t[1] for v in refs): prob = "original %08x..%08x never references %s %08x" % (cva, cva + fns[cva], t[2], t[0])
            elif not (word(dva) is not None and t[0] <= word(dva) < t[1]): prob = "MFL.exe word at %08x does not point into %s" % (dva, t[2])
            elif ridx(dva) is None or not any(ridx(v) == ridx(dva) for v in refs if TLO <= v): prob = "original %08x never references the row holding %08x" % (cva, dva)
        if prob: bad.append(((0, 0, "allow line %d" % i, os.path.basename(allow)), raw[:60], prob))
        else: ok.append((sym_, hit[0][0]))
    out = [x for x in out if not any(x[0][0] == a and s in sdef.get(x[1], ()) for s, a in ok)]
    for s, a in ok:
        if not any(s in v for v in sdef.values()): print("trapcheck: allow %s -> %08x unused (no scanned object defines %s)" % (s, a, s))
    return out + bad

def report(viol):
    for (lo, hi, name, lf), src, via in viol:
        print("VIOLATION %-16s %08x %-32s <- %s (%s)" % (lf, lo, name[:32], src, via))
    print("trapcheck: %d violation(s)" % len(viol))
    return 1 if viol else 0

def newest_link():
    c = sorted(p for p in glob.glob(os.path.join(G, "output", "gate", "*_standalone", "link")) if os.path.exists(os.path.join(p, "objs_local.txt")) and os.path.exists(os.path.join(p, "gen", "globals_layout.csv")))
    return c[-1] if c else None

def bad_selftest(W, good):
    """W9 (gate 1114_2129: the fixed fixture row RtDictDestroy became C in G1, so its negative stopped firing): the bad case is built from the link at run time,
    so no C port can retire it.  Pick the lowest-VA library function (fnsizes row >= 0x4b0000) that exactly one object B defines and another object A calls;
    leave B out of the object list and list the function as a trap row (good list + that row): A's reference now binds to the original address -> must be flagged.
    The same reduced object list with the good list alone must not flag it (the flag comes from the added row)."""
    import tempfile
    objs = [(s, o.strip()) for s, o in (l.split(None, 1) for l in open(os.path.join(W, "objs_local.txt")) if l.strip())]
    sy = syms([o for s, o in objs]); defs, users = {}, {}
    for s, o in objs:
        for n in sy[o][0]: defs.setdefault(n, []).append((s, o))
        for n in sy[o][1]: users.setdefault(n, set()).add(s)
    fns = {}
    for l in open(os.path.join(G, "output", "fnsizes.txt")):
        w = l.split()
        if len(w) >= 3 and w[-1].isdigit(): fns.setdefault(w[1], (int(w[0], 16), int(w[-1])))
    c = sorted((fns[n][0], n) for n in fns if fns[n][0] >= 0x4b0000 and len(defs.get(n, ())) == 1 and users.get(n, set()) - {defs[n][0][0]})
    if not c: print("selftest FAIL bad list: no library function defined by one object and called from another in %s" % W); return 1
    a, n = c[0]; bsrc, bobj = defs[n][0]
    ol = tempfile.mkstemp(suffix="_objs.txt")[1]; open(ol, "w").write("".join("%s %s\n" % so for so in objs if so[1] != bobj))
    bl = tempfile.mkstemp(suffix="_rw_trap_bad.txt")[1]; open(bl, "w").write(open(good).read() + "%08x %d %s\n" % (a, fns[n][1], n))
    try:
        hit = [x for x in check(W, [bl], objlist=ol, scan_consts=False) if x[0][2] == n]      # names only: the constant scan (objdump -d of every object) is --selftest-allow's
        ctl = [x for x in check(W, [good], objlist=ol, scan_consts=False) if x[0][2] == n]
    finally:
        os.remove(ol); os.remove(bl)
    if hit and not ctl: print("selftest ok   bad list: %s %08x (defined by %s, left out) <- %s flagged; good list alone: not flagged" % (n, a, bsrc, hit[0][1])); return 0
    print("selftest FAIL bad list: %s %08x not flagged (hits %d, control hits %d)" % (n, a, len(hit), len(ctl))); return 1

def selftest(W=None):
    if W:      # an explicit link (gate.sh: its own $D/link; negtest.sh: the baseline link) must be usable: missing = error, never a skip
        miss = [f for f in ("objs_local.txt", os.path.join("gen", "globals_layout.csv")) if not os.path.exists(os.path.join(W, f))]
        if miss: print("trapcheck selftest: FAIL, link %s has no %s" % (W, ", ".join(miss))); return 1
    else:      # manual run without W: newest link, skip when there is none
        W = newest_link()
        if not W: print("trapcheck selftest: skipped (no standalone link under output/gate)"); return 0
    N = os.path.join(T, "negtests", "trap"); rc = 0
    rc |= bad_selftest(W, os.path.join(N, "rw_trap_good.txt"))
    # W8 G1 (gate 1114_0950): the good list is a copy of rw_trap.txt, so the T-TCA pairs (trap_caller_allow.txt) are violations there unless the allow file
    # is applied, like the gate form does; allow lines whose trap row the fixture does not list are "stale" only relative to the fixture: ignored here
    al = ALLOW if os.path.exists(ALLOW) else None
    raw = check(W, [os.path.join(N, "rw_trap_good.txt")], scan_consts=False)
    good = [x for x in check(W, [os.path.join(N, "rw_trap_good.txt")], allow=al, scan_consts=False) if not (al and x[0][3] == os.path.basename(al) and x[2].startswith("stale"))]
    if not good: print("selftest ok   good list: 0 violations (allow file removes %d T-TCA pair(s))" % len(raw))
    else: report(good); print("selftest FAIL good list flagged"); rc = 1
    if al and raw:      # negative: the same check with the allow line of one flagged pair removed must flag that pair again
        import tempfile
        x0 = raw[0]
        def names_row(l):      # the line's trap row field is x0's row (by name or VA)
            w = l.split("#")[0].split()
            return len(w) >= 3 and (w[2] == x0[0][2] or re.fullmatch(r"(0x)?[0-9a-fA-F]{6,8}", w[2]) is not None and int(w[2], 16) == x0[0][0])
        keep = [l for l in open(al) if not names_row(l)]
        p = tempfile.mkstemp(suffix="_allow.txt")[1]; open(p, "w").write("".join(keep))
        neg = [x for x in check(W, [os.path.join(N, "rw_trap_good.txt")], allow=p, scan_consts=False) if x[0][3] != os.path.basename(p)]
        os.remove(p)
        if any(x[0][0] == x0[0][0] for x in neg): print("selftest ok   allow line for %s removed: flagged again" % x0[0][2])
        else: print("selftest FAIL allow line for %s removed but not flagged" % x0[0][2]); rc = 1
    print("trapcheck selftest: %s (%s)" % ("FAIL" if rc else "ok", os.path.relpath(W, G)))
    return rc

ALLOW = os.path.join(D, "trap_caller_allow.txt")

def selftest_allow(objlist, W=None):
    """T-TCA / T-CA behaviour test on a real strict build: the two W7 constant-address callers must be flagged without their allow lines and pass with them;
    an unlisted pair, a stale trap row, a caller whose original never references the row, a wrong caller VA and a line without reason must each FAIL."""
    import tempfile
    W = W or newest_link(); L = [os.path.join(D, f) for f in LISTS[:4]]; rc = 0
    tmp = tempfile.mkdtemp(prefix="tca_")
    def run(name, text):
        p = os.path.join(tmp, name); open(p, "w").write(text)
        return [x for x in check(W, L, objlist, p, scan_consts=True) if x[1] in ("an_rwa_dxaudio_codec_output.c", "an_hk_1367.c") or x[0][3] == name]
    def want(label, got, cond):
        nonlocal rc
        print("selftest %-4s %s" % ("ok" if cond else "FAIL", label))
        if not cond: rc = 1; report(got)
    good = "".join(l for l in open(ALLOW) if re.match(r"(rwa_dxaudio_codec_fill_packet_005cee90|gap_5d01c0) ", l))
    v = run("none.txt", "")
    want("T-CA: constant-address calls flagged without allow lines (5cf230 5cf2b0 5cf300 5d06c0 5cfc80)",
         v, {x[0][0] for x in v if x[2].startswith("const ")} >= {0x5cf230, 0x5cf2b0, 0x5cf300, 0x5d06c0, 0x5cfc80})
    v = run("good.txt", good); want("listed pairs pass", v, not v)
    v = run("part.txt", "".join(l for l in good.splitlines(True) if "5d06c0" not in l)); want("unlisted pair still FAILs", v, [x[0][0] for x in v] == [0x5d06c0])
    v = run("stale.txt", good + "gap_5d01c0 5d01c0 FUN_00000000_gone  # stale\n"); want("stale trap row FAILs", v, len(v) == 1 and "stale" in v[0][2])
    v = run("noref.txt", good + "gap_5d01c0 5d01c0 FUN_005cf300  # 5d01c0 does not call 5cf300\n"); want("never-referenced pair FAILs (5cf300 from 5d01c0)", v, len(v) == 1 and "never references" in v[0][2])
    v = run("wrongva.txt", good + "gap_5d01c0 5cee90 FUN_005cf230  # wrong caller VA\n"); want("symbol / caller VA mismatch FAILs", v, len(v) == 1 and "resolves to" in v[0][2])
    v = run("noreason.txt", good + "gap_5d01c0 5d01c0 FUN_005cfc80\n"); want("line without reason FAILs", v, len(v) == 1 and "reason" in v[0][2])
    print("trapcheck selftest-allow: %s" % ("FAIL" if rc else "ok"))
    return rc

if __name__ == "__main__":
    a = sys.argv[1:]
    if a[:1] == ["--selftest"]: sys.exit(selftest(a[1] if len(a) > 1 else None))
    if a[:1] == ["--selftest-allow"]: sys.exit(selftest_allow(a[1], a[2] if len(a) > 2 else None))
    objlist = None; lists = []; W = None; allow = ALLOW if os.path.exists(ALLOW) else None; cs = True
    while a:
        x = a.pop(0)
        if x == "--objs": objlist = a.pop(0)
        elif x == "--list": lists.append(a.pop(0))
        elif x == "--allow": allow = a.pop(0)
        elif x == "--no-consts": cs = False      # T-CA constant scan: on by default since W8 G1 (the existing constant-address users have allow lines)
        else: W = x
    W = W or newest_link()
    print("trapcheck: link %s, objects %s, allow %s%s" % (W, objlist or "objs_local.txt", allow, ", constant scan" if cs else ""))
    sys.exit(report(check(W, lists or [os.path.join(D, f) for f in LISTS], objlist, allow, cs)))
