"""edge.py (W10 lane F, PLAN-NEW-TECHNIQUES phase 3b): seeded edge-value case source for diffemu, run over the HS rows (hand spec / hs_only).
usage (venv):  edge.py --list [names | @file]              rows and the float words the rule picks (no emulation)
               edge.py [CASES BUDGET] [names | @file]       run; default every installed HS row (hs_installed.py's set), 600 cases, 240 s
               EDGE_ONE=row:seed:k edge.py ...             rerun one case (prints all diff lines of it)
               edge.py --selftest                          layout parser + plans
               edge.py --negfix [CASES BUDGET]             negative fixture built at run time: the first rows (sorted) whose body stores a float
                                                           parameter unchanged (`x = p;`) get a mutant copy that flushes negative denormals of p;
                                                           it must DIFF with the edge plans and be EQUAL with EDGE_OFF (the HS base cases alone)
Installed W11 B6 from agents/f_w10/edge.py (run through `run.py edge`).
Rule (plan W10 v2 3.5 F): edge values go ONLY into words the signature or a struct layout types as float / double: float / double parameters,
and float fields of a struct (or float array) behind a pointer parameter, when that pointer points at the start of a case window.  Pointers,
indices, counts and every other word keep the HS case's value (integer scalars already get min / max / +-1 from core.bounds()).
Cases: the HS base cases of the row (hs.gen: generic seeding + the row's spec) in order, each with an edge plan: pass 1 one word x every edge
value, pass 2 every pair of edge values over the first two float arguments plus a == b / b == -a pairs, pass 3 seeded random plans (each
word an edge value with p 0.5).  Same seed as the base case (stub returns unchanged); repro key = row, base seed, plan index k.
Harness: DIFFEMU_SPECIALS off (this is the edge pass), hs.py's NOMAX / NOSPECIALS row opt-outs NOT applied (findings say so), seed.Guide off
(mutated copies would drop the plan), x87 control word = core.fpucw(fn) (0x7f, 0x27f for fpucw53.txt rows) pinned per side by core.side,
an invalid / unsupported instruction on either side (UC_ERR_INSN_INVALID) = failure 'UNSUPPORTED'.  All diffs are collected (cap 6 per row,
distinct (word, value) keys), not only the first.
Output: one line per row `EDGE <EQUAL|DIFF|UNSUP|NOWORDS|ERR> name cases N cw 0x.. words W [diff k=.. seed=.. plan=.. :: first difference]`."""
import os, sys, re, struct, random, itertools
os.environ["DIFFEMU_SPECIALS"] = ""      # before hs (setdefault): the edge table replaces the specials pass
HERE = os.path.dirname(os.path.abspath(__file__))
os.environ.setdefault("DXDIR", HERE)      # this tree's diffemu; DXDIR=<tools/diffemu of a tree> runs a copy of edge.py against that tree
TW = os.path.join(os.path.dirname(os.environ["DXDIR"]), "diffemu_audit", "tweak")
sys.path.insert(0, TW)
import hs
core, diffemu, seedmod = hs.core, hs.diffemu, hs.seed
from unicorn import UC_ERR_INSN_INVALID

F = lambda x: struct.unpack("<I", struct.pack("<f", x))[0]
EDGE_F = [0x00000000, 0x80000000, F(1.0), F(-1.0), 0x7fc00000, 0xffc00000, 0x7f800000, 0xff800000, 0x00000001, 0x80000001,
          0x007fffff, 0x00800000, 0x7f7fffff, 0xff7fffff, F(2.0 ** 31), F(-2.0 ** 31), F(0.5), F(1e-30)]
D = lambda x: struct.unpack("<Q", struct.pack("<d", x))[0]
EDGE_D = [0, 1 << 63, D(1.0), D(-1.0), 0x7ff8000000000000, 0xfff8000000000000, 0x7ff0000000000000, 0xfff0000000000000, 1, 0x000fffffffffffff,
          0x0010000000000000, 0x7fefffffffffffff, D(2.0 ** 31), D(-2.0 ** 31), D(0.5), D(1e-300), D(3.4028234663852886e38), D(1e39)]

# ------------------------------------------------------------------------------------------------ struct layouts (float fields)
SZ = {"char": 1, "short": 2, "int": 4, "long": 4, "float": 4, "double": 8, "RwReal": 4, "RwInt32": 4, "RwUInt32": 4, "RwBool": 4, "RwInt16": 2,
      "RwUInt16": 2, "RwInt8": 1, "RwUInt8": 1, "RwChar": 1, "undefined4": 4, "undefined2": 2, "undefined1": 1, "undefined": 1, "uint": 4, "ushort": 2,
      "byte": 1, "dword": 4, "BOOL": 4, "DWORD": 4, "WORD": 2, "BYTE": 1, "RwFixed": 4, "size_t": 4, "LONG": 4, "ULONG": 4, "UINT": 4}
FL = {"float": 4, "RwReal": 4, "double": 8}
_LAY = {}
def layouts(d, extra=()):
    """{struct / typedef name: (size, [(offset, 4|8)] of float fields)} from the headers of dir d (+ extra files); a struct with a member this
    small parser does not understand (union, bitfield, function pointer, unknown type) is left out (= no float words)"""
    key = (d,) + tuple(extra)
    if key in _LAY: return _LAY[key]
    txt = ""
    for f in sorted(os.listdir(d)):
        if f.endswith(".h"): txt += open(os.path.join(d, f), errors="replace").read() + "\n"
    for f in extra: txt += open(f, errors="replace").read()
    txt = re.sub(r"/\*.*?\*/|//[^\n]*", " ", txt, flags=re.S)
    alias = {}
    for a, b in re.findall(r"typedef\s+(?:struct|union)\s+(\w+)\s+(\w+)\s*;", txt): alias[b] = a
    for a, b in re.findall(r"typedef\s+struct\s+(\w+)\s+RWALIGN\(\s*(\w+)", txt): alias[b] = a
    for b, a in re.findall(r"typedef\s+(\w+)\s+(\w+)\s*;", txt):
        if b not in ("struct", "union", "enum"): alias.setdefault(a, b)
    bodies = {}
    for m in re.finditer(r"(typedef\s+)?struct\s+(\w*)\s*\{([^{}]*)\}\s*(\w*)\s*;", txt):
        if m.group(2): bodies[m.group(2)] = m.group(3)
        if m.group(4): bodies[m.group(4)] = m.group(3)
    out = {}
    def res(t, depth=0):
        while t in alias and t not in bodies and depth < 10: t = alias[t]; depth += 1
        return t
    def lay(n, stk=()):
        n = res(n)
        if n in out: return out[n]
        if n in stk or n not in bodies: return None
        off, fl, al = 0, [], 1
        for mem in bodies[n].split(";"):
            mem = " ".join(mem.split())
            if not mem: continue
            if ":" in mem or "(" in mem or "union" in mem: return None
            m = re.match(r"^(?:const\s+|volatile\s+|unsigned\s+|signed\s+|struct\s+|enum\s+)*([\w]+)\s*(.*)$", mem)
            if not m: return None
            base, decl = m.group(1), m.group(2)
            if mem.startswith(("unsigned", "signed")) and not decl.replace("*", "").strip(): base, decl = "int", decl
            for one in decl.split(","):
                one = one.strip(); ptr = one.startswith("*")
                dims = [int(x, 0) for x in re.findall(r"\[\s*(\w+)\s*\]", one) if re.fullmatch(r"0x[0-9a-fA-F]+|\d+", x)]
                if "[" in one and not dims: return None
                cnt = 1
                for x in dims: cnt *= x
                b = res(base)
                if ptr or mem.startswith("enum"): sz, a, sub = 4, 4, []
                elif b in SZ: sz, a = SZ[b], min(SZ[b], 8); sub = [(0, FL[b])] if b in FL else []
                elif base in SZ: sz, a = SZ[base], min(SZ[base], 8); sub = [(0, FL[base])] if base in FL else []
                else:
                    L = lay(b, stk + (n,))
                    if L is None: return None
                    sz, sub, a = L[0], L[1], L[2]
                off = (off + a - 1) // a * a; al = max(al, a)
                for j in range(cnt): fl += [(off + j * sz + o, w) for o, w in sub]
                off += sz * cnt
        size = (off + al - 1) // al * al
        out[n] = (size, fl, al); return out[n]
    named = {}
    for n in list(bodies) + list(alias):
        x = lay(n)
        if x: named[n] = x
    _LAY[key] = named; return named

# ------------------------------------------------------------------------------------------------ float words of a row
def words(fn, src):
    """[('a', stack index, 4|8) | ('r', reg, 4) | ('p', ptr loc, offset, 4|8)]: the float words the rule picks"""
    sig = fn.sig; L = layouts(os.path.dirname(src), [src]); out = []; nreg = len(sig.regargs); si = 0
    regs = ("ecx", "edx")
    for j, (c, pt, txt) in enumerate(sig.params):
        loc = ("r", regs[j]) if j < nreg else ("a", si)
        n = 2 if c in ("d", "ll") else int(c[1:]) if c[:1] == "b" and c[1:].isdigit() else 1
        if c == "f": out.append(loc + (4,))
        elif c == "d" and loc[0] == "a": out.append(("a", si, 8))
        elif c == "p":
            base = re.sub(r"\b(const|volatile|struct)\b", "", pt).strip()
            if base in FL: fl = [(o, FL[base]) for o in range(0, 3 * FL[base], FL[base])]      # float * / RwReal *: the first 3 elements (a vector)
            else:
                lay = L.get(base); fl = lay[1] if lay else []
            for o, w in fl:
                if o + w <= 0x400: out.append(("p", loc, o, w))
        if j >= nreg: si += n
    return out

def getloc(c, loc):
    return c.regs.get(loc[1]) if loc[0] == "r" else c.args[loc[1]] if loc[1] < len(c.args) else None

def apply(c, plan):
    """plan [(word, bits)] -> the case with those words set (args / regs / window bytes); words whose pointer is not a window start are skipped"""
    args, regs, wins = list(c.args), dict(c.regs), [(a, bytearray(b)) for a, b in c.wins]; done = 0
    for w, v in plan:
        if w[0] == "a":
            if w[2] == 8 and w[1] + 1 < len(args): args[w[1]], args[w[1] + 1] = v & 0xffffffff, v >> 32; done += 1
            elif w[2] == 4 and w[1] < len(args): args[w[1]] = v; done += 1
        elif w[0] == "r": regs[w[1]] = v; done += 1
        else:
            p = getloc(c, w[1])
            for a, b in reversed(wins):      # the last window at an address is the one core writes last (specs append over generic windows)
                if p == a and w[2] + w[3] <= len(b): b[w[2]:w[2] + w[3]] = v.to_bytes(w[3], "little"); done += 1; break
    c2 = core.Case(c.seed, c.mode, c.fill, args, regs, [(a, bytes(b)) for a, b in wins]); c2.patch, c2.small = dict(c.patch), c.small
    return c2, done

def plans(ws, n, r):
    """pass 1 single words, pass 2 pairs over the first two float args (+ a == b, b == -a), pass 3 random; at most n plans"""
    tab = lambda w: EDGE_D if w[-1] == 8 else EDGE_F
    out = [[(w, v)] for w in ws for v in tab(w)]
    fa = [w for w in ws if w[0] in ("a", "r")][:2] or ws[:2]
    if len(fa) == 2:
        out += [[(fa[0], a), (fa[1], b)] for a, b in itertools.product(tab(fa[0]), tab(fa[1])) if (a, b) != (0, 0)]
        if fa[0][-1] == fa[1][-1] == 4:
            for _ in range(24):
                x = F(r.uniform(-1e4, 1e4)); out += [[(fa[0], x), (fa[1], x)], [(fa[0], x), (fa[1], x ^ 0x80000000)]]
    while len(out) < n and ws:
        p = [(w, r.choice(tab(w))) for w in ws if r.random() < 0.5]
        if p: out.append(p)
    return out[:n]

# ------------------------------------------------------------------------------------------------ harness hooks
_UNSUP = []
_mrun = core.Machine.run
def _run_rec(m, *a, **k):
    r = _mrun(m, *a, **k)
    if r.get("err") is not None and r["err"].errno == UC_ERR_INSN_INVALID: _UNSUP.append(("ours" if m.ours else "orig", r["eip"]))
    return r
core.Machine.run = _run_rec
seedmod.Guide.prep = lambda g, c: c      # Guide off: a mutated copy would drop the edge plan
ST = {}      # per row: plans, diffs, counters
def _gen(sig, n, k, seed0=0):
    base = hs.gen(sig, n, k, seed0); fn = hs._cur["fn"]; ws = ST["words"]
    if os.environ.get("EDGE_OFF"): return base      # control: the HS base cases alone (no edge plans, specials off)
    r = random.Random(sum(map(ord, fn.name)))      # seeded by the row name (PYTHONHASHSEED-free)
    pl = plans(ws, n, r); out = []
    for i, p in enumerate(pl):
        c = base[i % len(base)]; c2 = core.Case(c.seed, c.mode, c.fill, list(c.args), dict(c.regs), list(c.wins)); c2.patch, c2.small = dict(c.patch), c.small
        c2.edge = (i, p); out.append(c2)
    one = ST.get("one")
    if one: out = [c for c in out if c.edge[0] == one[1] and c.seed == one[0]]
    return out
core.gen_cases = _gen
_rc = hs._rc0      # stock core.run_case (hs's NOMAX wrapper NOT applied)
def _run_case(fn, c):
    pl = getattr(c, "edge", None)
    if pl is None: return _rc(fn, c)
    c2, done = apply(c, pl[1])
    if not done: ST["noapply"] += 1; return "skip", "EDGE_NOAPPLY"
    del _UNSUP[:]
    st, x = _rc(fn, c2)
    if _UNSUP: st, x = "diff", ["UNSUPPORTED instruction (%s eip %08x)" % _UNSUP[0]]
    if st == "equal": ST["eq"] += 1
    elif st == "skip": ST["skip"][x] = ST["skip"].get(x, 0) + 1
    else:
        key = tuple((w, v) for w, v in pl[1])
        ST["ndiff"] += 1
        if len(ST["diffs"]) < 6 and key not in ST["dkeys"]:
            ST["dkeys"].add(key); ST["diffs"].append((pl[0], c.seed, pl[1], x))
    return "equal", None      # evaluate() keeps going: every plan runs, diffs are collected here
core.run_case = _run_case

def fmt_plan(p): return ",".join("%s=%0*x" % ("a%d" % w[1] if w[0] == "a" else w[1] if w[0] == "r" else "%s+%x" % ("a%d" % w[1][1] if w[1][0] == "a" else w[1][1], w[2]), 2 * w[-1], v) for w, v in p)

def rows():
    hs.load_specs(); idx = hs.diffemu.index()
    if os.environ.get("EDGE_SRC"):      # negative fixture: EDGE_SRC=<mutant .c next to copies of its headers> replaces the source of the rows it defines
        f = os.path.abspath(os.environ["EDGE_SRC"]); idx.update({n: (f, "tier2") for n in core.defs(f) if n in idx})
    hs._IDX = idx      # hs.run reads the same index
    ho = {l.split()[0] for l in open(os.path.join(os.path.dirname(TW), "hs_only.txt")) if l.strip() and not l.startswith("#")}
    return sorted((set(hs.SPECS) | set(hs.ENVS) | set(hs.INDS) | ho) & {n for n, (s, k) in idx.items() if k == "tier2"}), idx

def one_row(name, idx, cases, budget, listonly):
    src, kind = idx[name]
    obj, _ = diffemu.compile_obj(src); fn, why = diffemu.make_fn(name, src, obj)
    if fn is None: return "EDGE ERR      %s %s" % (name, why)
    ws = words(fn, src)
    if listonly: return "words %-44s %-10s %2d %s" % (name, fn.sig.kind(), len(ws), " ".join(fmt_plan([(w, 0)]).split("=")[0] for w in ws)[:200])
    if not ws: return "EDGE NOWORDS  %-44s (signature / layouts type no word as float)" % name
    ST.clear(); ST.update(words=ws, eq=0, skip={}, ndiff=0, diffs=[], dkeys=set(), noapply=0)
    if os.environ.get("EDGE_ONE"):
        _, s_, k_ = os.environ["EDGE_ONE"].split(":"); ST["one"] = (int(s_), int(k_))
    r = hs.run((name, cases, budget))
    if r[1] == "ERR": return "EDGE ERR      %s %s" % (name, r[2])
    if os.environ.get("EDGE_OFF"): return "EDGE %-8s %-44s cases %4s (EDGE_OFF: HS base cases only) %s" % (r[1], name, r[2], r[-1][:300])
    unsup = [d for d in ST["diffs"] if d[3][0].startswith("UNSUPPORTED")]
    v = "UNSUP" if unsup else "DIFF" if ST["diffs"] else "EQUAL" if ST["eq"] >= 20 else "INCONC"
    sk = " skip %s" % ",".join("%s:%d" % kv for kv in sorted(ST["skip"].items(), key=lambda kv: -kv[1])[:3]) if ST["skip"] else ""
    s = "EDGE %-8s %-44s cases %4d diffs %3d cw %s words %d cov %s/%s%s%s" % (v, name, ST["eq"], ST["ndiff"], hex(core.fpucw(fn) or 0), len(ws), r[3], r[4], sk,
                                                                     " noapply %d" % ST["noapply"] if ST["noapply"] else "")
    for k, sd, p, x in ST["diffs"] if not os.environ.get("EDGE_ONE") else ST["diffs"][:1]:
        s += "\n   diff k=%d seed=%d plan=%s :: %s" % (k, sd, fmt_plan(p), (" | ".join(x) if os.environ.get("EDGE_ONE") else x[0])[:600 if not os.environ.get("EDGE_ONE") else 4000])
    return s

G = {}
def _job(n):
    try: return one_row(n, G["idx"], G["cases"], G["budget"], G["listonly"])
    except Exception as e: return "EDGE ERR      %s %s: %s" % (n, type(e).__name__, str(e)[:200])

def selftest():
    import tempfile
    d = tempfile.mkdtemp()
    open(d + "/a.h", "w").write("typedef float RwReal;\ntypedef struct V { RwReal x, y, z; } V;\nstruct M { V r; int f; V u; char c; double q; };\ntypedef struct M M;\n"
                                "typedef struct U { union { int a; float b; } u; } U;\n")
    L = layouts(d); ok = L["V"][1] == [(0, 4), (4, 4), (8, 4)] and L["M"][1] == [(0, 4), (4, 4), (8, 4), (16, 4), (20, 4), (24, 4), (32, 8)] and L["M"][0] == 40 and "U" not in L
    os.remove(d + "/a.h"); os.rmdir(d)
    p = plans([("a", 0, 4), ("a", 1, 4)], 10000, random.Random(1))
    ok2 = len(p) >= 2 * len(EDGE_F) + len(EDGE_F) ** 2 - 1 and all(w[0] == "a" for q in p for w, _ in q)
    print("selftest layouts %s plans %s" % ("ok" if ok else "FAIL %r" % L, "ok" if ok2 else "FAIL")); return 0 if ok and ok2 else 1

def negfix(cases, budget, tries=3):
    """W11 B6 negative fixture, built at run time (names no row): mutate the first rows whose body has `<lvalue> = <float param>;` so the stored
    value is 0 for a negative denormal parameter (the HS base cases rarely hold one, the edge table does).  PASS = some mutant DIFFs with the edge
    plans and is EQUAL with EDGE_OFF; a mutant the edge pass does not reach is reported and the next one is tried (at most `tries`)."""
    import shutil, tempfile
    allr, idx = rows(); n = 0
    for name in allr:
        src = idx[name][0]; txt = open(src, errors="replace").read()
        m = re.search(r"\b%s\s*\(([^)]*)\)\s*\{" % re.escape(name), txt)
        if not m: continue
        end = txt.find("\n}", m.end()); body = txt[m.end():end]
        for p in re.findall(r"\b(?:float|RwReal)\s+(\w+)\s*(?:,|$)", m.group(1)):
            s = re.search(r"^(\s*)([^;=\n{}]+?)\s*=\s*%s\s*;" % p, body, re.M)
            if s: break
        else: continue
        d = tempfile.mkdtemp()
        for f in os.listdir(os.path.dirname(src)):
            if f.endswith(".h"): shutil.copy(os.path.join(os.path.dirname(src), f), d)
        mut = "%s%s = (%s < 0.0f && %s > -1e-37f) ? 0.0f : %s;      /* negfix mutant */" % (s.group(1), s.group(2), p, p, p)
        open(os.path.join(d, os.path.basename(src)), "w").write(txt[:m.end() + s.start()] + mut + txt[m.end() + s.end():])
        os.environ["EDGE_SRC"] = os.path.join(d, os.path.basename(src)); allr2, idx2 = rows()
        os.environ.pop("EDGE_OFF", None); on = one_row(name, idx2, cases, budget, False)
        os.environ["EDGE_OFF"] = "1"; off = one_row(name, idx2, cases, budget, False); del os.environ["EDGE_OFF"]; del os.environ["EDGE_SRC"]
        print("negfix %s (param %s): edge %s | EDGE_OFF %s" % (name, p, on.split("\n")[0].split()[1], off.split()[1]), flush=True)
        if on.split()[1] == "DIFF" and off.split()[1] == "EQUAL": print("negfix: ok"); return 0
        n += 1
        if n >= tries: break
    print("negfix: FAIL (no mutant DIFFed with edges and stayed EQUAL without)"); return 1

if __name__ == "__main__":
    a = sys.argv[1:]
    if a == ["--selftest"]: sys.exit(selftest())
    if a[:1] == ["--negfix"]: sys.exit(negfix(*(int(x) for x in a[1:3])) if len(a) >= 3 else negfix(600, 240))
    listonly = a[:1] == ["--list"]; a = a[1:] if listonly else a
    cases, budget = (int(a[0]), int(a[1])) if len(a) >= 2 and a[0].isdigit() else (600, 240)
    a = a[2:] if len(a) >= 2 and a[0].isdigit() else a
    allr, idx = rows()
    names = [l.strip() for x in a for l in (open(x[1:]) if x.startswith("@") else [x]) if l.strip()] or allr
    print("edge: %d rows, %d plans max, budget %d s" % (len(names), cases, budget), flush=True)
    tot = {}; G.update(idx=idx, cases=cases, budget=budget, listonly=listonly)
    from multiprocessing import Pool
    with Pool(1 if listonly or len(names) < 2 else 2) as p:      # 2 workers as hs_installed.py; ST is per process
        for s in p.imap(_job, names):
            print(s, flush=True); v = "words>0" if listonly and len(s.split()) > 3 and s.split()[3] != "0" else "words=0" if listonly else s.split()[1]; tot[v] = tot.get(v, 0) + 1
    print("edge summary: " + ", ".join("%s %d" % kv for kv in sorted(tot.items())))
