#!/usr/bin/env python3
"""plan_split.py <lib> <member obj dir> <out dir> [member key filter (fnmatch, default *)]: which members of a library (libs.cfg) can be split out of the
delinked image WITH their data.  A member is split = its code and data bytes (ownership_items.csv) leave the image, the member object comes in whole.
Every reference from the remaining (live) image into split bytes must become a symbol reference: a function start whose name the member object defines
as external, or an external / common data symbol (+ offset).  Anything else (a static, an interior address, a static function) is a blocker: the target's
member is dropped from the split, and the fixpoint repeats (the dropped member's bytes are live again and may reference other members).
Writes <out>/members.txt (split members), <out>/extra_objs.txt (for link_ours.sh EXTRA_OBJS: "lib:<key> <obj>"), <out>/blockers.txt.  ~10 s."""
import bisect, collections, configparser, csv, fnmatch, os, re, struct, subprocess, sys
H = os.path.dirname(os.path.abspath(__file__)); G = os.path.normpath(os.path.join(H, "..", "..")); O = os.path.join(G, "output")
LIB, MDIR, OUT = sys.argv[1], sys.argv[2], sys.argv[3]; FILT = sys.argv[4] if len(sys.argv) > 4 else "*"
ONLY = {l.strip() for l in open(os.environ["ONLY"])} if os.environ.get("ONLY") else None      # bisecting: candidate members restricted to this file
os.makedirs(OUT, exist_ok=True)
cfg = configparser.ConfigParser(); cfg.read(os.path.join(H, "libs.cfg"))
ex = cfg[LIB].get("exclude", "").split()
objof = {}
for f in cfg[LIB]["members"].split():
    for l in open(os.path.join(G, f)):
        p = l.split(None, 1)
        if len(p) == 2 and not any(fnmatch.fnmatch(p[1].strip(), x) for x in ex) and fnmatch.fnmatch(p[1].strip(), FILT) and (ONLY is None or p[1].strip() in ONLY): objof[p[1].strip()] = os.path.join(MDIR, p[0])
def extnames(path, undef=False, d=None):    # external symbols defined (or referenced, undef=True) by an object (decorated)
    d = d or open(path, "rb").read(); n, so, ns = struct.unpack_from("<H", d, 2)[0], *struct.unpack_from("<II", d, 8); st = so + ns * 18; out = set(); i = 0
    while i < ns:
        o = so + i * 18; raw = d[o:o + 8]
        nm = d[st + struct.unpack_from("<I", raw, 4)[0]:d.index(b"\0", st + struct.unpack_from("<I", raw, 4)[0])].decode("latin1") if raw[:4] == b"\0\0\0\0" else raw.rstrip(b"\0").decode("latin1")
        val, sec, typ, cls, aux = struct.unpack_from("<IhHBB", d, o + 8)
        if cls == 2 and ((sec == 0 and val == 0) if undef else (sec > 0 or (sec == 0 and val > 0))): out.add(nm)
        i += 1 + aux
    return out
defs = {k: extnames(p) for k, p in objof.items()}
norm = lambda n: n.lstrip("_@").split("@")[0] if not n.startswith("?") else n
ndefs = {k: {norm(n): n for n in v} for k, v in defs.items()}
def decorated(name, mem):      # libmatch names: leading '_' / '@N' decoration differs
    return ndefs[mem].get(norm(name))
VAR = os.environ.get("VARIANTS") == "1"     # also split members whose variant functions (owner "<lib>~", other source revision) the SDK version replaces
items = [r for r in csv.DictReader(open(os.path.join(O, "delink_sdk", "ownership_items.csv"))) if (r["owner"] == LIB or (VAR and r["owner"] == LIB + "~")) and r["member"] in objof]
dsyms = collections.defaultdict(list)
for r in csv.DictReader(open(os.path.join(O, "delink_sdk", "symbols.csv"))):
    if r["addr"] and r["member"] in objof and r["binding"] in ("ext", "common"): dsyms[r["member"]].append((int(r["addr"], 16), max(1, int(r["size"] or 1)), r["symbol"]))
refs = []                      # site, target, how
for r in csv.DictReader(open(os.path.join(O, "delink_relocs.csv"))):
    if r["kind"] not in ("const", "header", "rsrc", "import"): refs.append((int(r["site"], 16), int(r["target"], 16), "abs"))
for r in csv.reader(open(os.path.join(O, "delink", "ghidra_refs.csv"))):
    if len(r) > 2 and r[2] in ("UNCONDITIONAL_CALL", "UNCONDITIONAL_JUMP", "CONDITIONAL_JUMP"):
        try: s, t = int(r[0], 16), int(r[1], 16)
        except ValueError: continue
        if 0x401000 <= t < 0x5dc000: refs.append((s, t, "rel"))
# rel32 calls / jumps that Ghidra never decoded (code in undisassembled gaps): byte scan for E8 / E9 / 0F 8x whose target is a library function start
X = open(os.path.join(G, "..", "MashedGame", "FullyLoaded", "installed", "App_Executables", "MFL.exe"), "rb").read()
starts = {int(r["lo"], 16) for r in items if r["kind"] == "code"}; known = {(s_, t_) for s_, t_, _ in refs}
for i in range(0x1000, 0x1000 + 0x1db000 - 6):          # .text + _rwcseg raw = va - 0x400000
    b = X[i]
    if b in (0xE8, 0xE9) or (b == 0x0F and 0x80 <= X[i + 1] <= 0x8F):
        k = 1 if b != 0x0F else 2
        t = (0x400000 + i + k + 4 + struct.unpack_from("<i", X, i + k)[0]) & 0xFFFFFFFF
        if t in starts and (0x400000 + i, t) not in known: refs.append((0x400000 + i, t, "scan"))
# a member whose writable data has no known address in the exe (unanchored .data / .bss) while some of its functions may be in the exe as
# ambiguous (identical code) matches: splitting it would give those original functions the old data and the member's code a new copy
unanch = collections.defaultdict(set)      # member -> section numbers of writable data with no known address in the exe
for r in csv.DictReader(open(os.path.join(O, "delink_sdk", "sections.csv"))):
    if r["kind"] in ("data", "bss") and not r["comdat"] and not r["addr"] and int(r["size"]) > 0: unanch[r["member"]].add(int(r["secno"]))
ambn = collections.defaultdict(set)        # member -> function names that match the exe only ambiguously (identical code elsewhere)
for f in ("rw_libmatch.csv", "crt_libmatch.csv"):
    for r in csv.reader(open(os.path.join(O, f))):
        if r[4]: ambn[r[2]].add(r[1])
def users(path, secnos):                   # functions of the object that reference those sections
    d = open(path, "rb").read(); n, so, ns = struct.unpack_from("<H", d, 2)[0], *struct.unpack_from("<II", d, 8); opt = struct.unpack_from("<H", d, 16)[0]; st = so + ns * 18
    nm = lambda raw: d[st + struct.unpack_from("<I", raw, 4)[0]:d.index(b"\0", st + struct.unpack_from("<I", raw, 4)[0])].decode("latin1") if raw[:4] == b"\0\0\0\0" else raw.rstrip(b"\0").decode("latin1")
    sy = {}; i = 0
    while i < ns:
        o = so + i * 18; v, sn, ty, cl, ax = struct.unpack_from("<IhHBB", d, o + 8); sy[i] = (nm(d[o:o + 8]), sn, ty); i += 1 + ax
    fnof = {sn: n_ for n_, sn, ty in sy.values() if sn > 0 and (ty >> 4) == 2}
    out = set()
    for k in range(n):
        h = 20 + opt + k * 40; relptr, nrel = struct.unpack_from("<I", d, h + 24)[0], struct.unpack_from("<H", d, h + 32)[0]
        for j in range(nrel):
            _, si, _ = struct.unpack_from("<IIH", d, relptr + j * 10)
            if si in sy and sy[si][1] in secnos and (k + 1) in fnof: out.add(fnof[k + 1].lstrip("_"))
    return out
pre = []; open(os.path.join(OUT, "blockers.txt"), "w").close()
for m in sorted(objof):      # libs.cfg nosplit: members whose static data our C objects reference by address (g_* of src/automatch_agent) stay in the image
    if any(fnmatch.fnmatch(m, x) for x in cfg[LIB].get("nosplit", "").split()): pre.append(m); open(os.path.join(OUT, "blockers.txt"), "a").write("nosplit (libs.cfg): drop %s\n" % m)
for m in sorted(objof):
    if m in unanch:
        u = users(objof[m], unanch[m]) & {x.lstrip("_") for x in ambn.get(m, ())}
        if u: pre.append(m); open(os.path.join(OUT, "blockers.txt"), "a").write("unanchored writable data used by ambiguous functions %s: drop %s\n" % (" ".join(sorted(u)[:4]), m))
print("%d members blocked up front (writable data with unknown address, used by functions that are in the exe only as ambiguous matches)" % len(pre))
M = set(objof) - set(pre)
def ref_fix(M, tag):
    nsym = collections.Counter()
    for rnd in range(50):
        D = sorted(((int(r["lo"], 16), int(r["hi"], 16), r) for r in items if r["member"] in M), key=lambda x: x[:2]); DL = [d[0] for d in D]
        def hit(a):
            i = bisect.bisect_right(DL, a) - 1
            return D[i] if i >= 0 and a < D[i][1] else None
        bad = collections.defaultdict(list); nsym = collections.Counter()
        for s, t, how in refs:
            h = hit(t)
            if not h or hit(s): continue
            r = h[2]
            if r["kind"] == "code":
                if t == h[0] and decorated(r["name"], r["member"]): nsym["code " + how] += 1; continue
                why = "interior code %s+0x%x" % (r["name"], t - h[0]) if t != h[0] else "static function " + r["name"]
            else:
                if any(a <= t < a + n for a, n, _ in dsyms[r["member"]]): nsym["data " + how] += 1; continue
                why = "static/interior data in %s" % r["name"]
            bad[r["member"]].append("%08x -> %08x %s (%s)" % (s, t, why, how))
        if not bad: break
        M -= set(bad)
        with open(os.path.join(OUT, "blockers.txt"), "a") as f:
            for m, l in sorted(bad.items()): f.write(tag + "round %d drop %s (%d sites)\n%s\n" % (rnd, m, len(l), "".join("    %s\n" % x for x in l[:12])))
        print(tag + "round %d: %d members blocked, %d left" % (rnd, len(bad), len(M)))
    return M, nsym
M, nsym = ref_fix(M, "")
# ---- closure: every undefined external of a split member must resolve to (1) another split member, (2) a name in the remaining image
# (src/symbols.txt, fnsizes.txt; mkobj defines those), (3) an import of the exe, or (4) a lib member that is NOT in the exe (pulled by the linker,
# its data is new); a symbol only defined by a member that is in the exe but not split (blocked / excluded) blocks the member (no mixed copies).
src = open(os.path.join(G, "tools", "libmatch_lib.py")).read().replace("\nmain()\n", "\n"); ns_ = {"__file__": os.path.join(G, "tools", "libmatch_lib.py"), "__name__": "x"}
exec(compile(src, "libmatch_lib", "exec"), ns_)
RWLIB = os.path.join(G, "toolchain", "rwsdk", "lib", "d3d9", "release")
VC7LIB = os.path.join(G, "toolchain", "vc7", "Vc7", "lib")
inexe = set()
for lib_ in cfg.sections():
    for f in cfg[lib_].get("members", "").split():
        inexe |= {l.split(None, 1)[1].strip() for l in open(os.path.join(G, f)) if len(l.split(None, 1)) == 2}
libdef = {}; libund = {}; libdefs_of = {}; libbody = {}
for dd, libs in ((RWLIB, sorted(x for x in os.listdir(RWLIB) if x.endswith(".lib"))), (VC7LIB, ["libcmt.lib"])):
    for lb in libs:
        for mem, body in ns_["members"](os.path.join(dd, lb)):
            key = lb + ":" + mem
            try: df, un = extnames(None, False, body), extnames(None, True, body)
            except Exception: continue
            libdefs_of[key] = df; libund[key] = un; libbody[key] = body
            for n in df: libdef.setdefault(n, key)       # first definition in link order (rw libs alphabetical, then libcmt)
image = set(); libimage = {}
for f in ("src/symbols.txt", "output/fnsizes.txt"):
    for l in open(os.path.join(G, f)):
        q = l.split()
        if len(q) >= 2: image.add(q[0] if f.endswith("symbols.txt") else q[1])
for l in open(os.path.join(O, "delink_sdk", "lib_names.txt")):
    q = l.split(); libimage[q[0]] = int(q[1], 16)
imports = set(re.findall(r"\s\d+\s+(\w+)\s*$", subprocess.run(["objdump", "-p", os.path.join(G, "..", "MashedGame", "FullyLoaded", "installed", "App_Executables", "MFL.exe")], capture_output=True, text=True).stdout, re.M))
undn = lambda n: n[1:].split("@")[0] if n.startswith("@") else (n[1:] if n.startswith("_") else n).split("@")[0] if re.match(r"^_?\w+@\d+$", n) else (n[1:] if n.startswith("_") else n)
def mkhit(M):
    D = sorted(((int(r["lo"], 16), int(r["hi"], 16)) for r in items if r["member"] in M)); DL = [d[0] for d in D]
    def hit(a):
        i = bisect.bisect_right(DL, a) - 1
        return i >= 0 and a < D[i][1]
    return hit
STUBS = {"__fltused", "_IID_IUnknown", "_errno"}      # mkobj defines __fltused; the link adds a stub object for IID_IUnknown / errno
for rnd in range(50):
    M, _ = ref_fix(M, "closure %d: refs " % rnd)
    drop_names = {norm(r["name"]) for r in items if r["member"] in M}; hit = mkhit(M)       # names whose image copy is split out (only the member defines them then)
    have = set().union(*(defs[m] for m in M)) if M else set()
    pulled, blockedby, newimp, need = set(), collections.defaultdict(set), set(), [(m, n) for m in M for n in extnames(objof[m], True)]
    while need:
        m, n = need.pop()
        if n in have or n in STUBS: continue
        if n.startswith("__imp_"):
            if undn(n[6:]) not in imports: newimp.add(n); blockedby[m].add(n + " (import the exe does not have)")    # the linked exe keeps the original import table
            continue
        if {n, n[1:], undn(n)} & image and norm(n) not in drop_names: continue
        la = [libimage[x] for x in (n, n[1:], undn(n)) if x in libimage]
        if la and not hit(la[0]): continue
        k = libdef.get(n)
        if k and k not in inexe:
            if k not in pulled: pulled.add(k); have |= libdefs_of[k]; need += [(m, x) for x in libund[k]]
            continue
        blockedby[m].add(n + " (" + (k.split("/")[-1] if k else "undefined") + ")")
    if not blockedby: break
    M -= set(blockedby)
    with open(os.path.join(OUT, "blockers.txt"), "a") as f:
        for m, l in sorted(blockedby.items()): f.write("closure round %d drop %s: %s\n" % (rnd, m, " ".join(sorted(l)[:8])))
    print("closure round %d: %d members blocked (unresolvable undefined externals), %d left" % (rnd, len(blockedby), len(M)))
open(os.path.join(OUT, "pulled.txt"), "w").write("".join(k + "\n" for k in sorted(pulled)))
open(os.path.join(OUT, "new_imports.txt"), "w").write("".join(k + "\n" for k in sorted(newimp)))
print("closure: %d lib members pulled that are not in the exe, %d imports the exe does not have" % (len(pulled), len(newimp)))
open(os.path.join(OUT, "members.txt"), "w").write("".join(m + "\n" for m in sorted(M)))
os.makedirs(os.path.join(OUT, "pulled"), exist_ok=True)
for i, k in enumerate(sorted(pulled)): open(os.path.join(OUT, "pulled", "p%d.obj" % i), "wb").write(libbody[k])
open(os.path.join(OUT, "extra_objs.txt"), "w").write("".join("lib:%s %s\n" % (m.replace(" ", "_"), objof[m]) for m in sorted(M)) +
    "".join("pull:%s %s\n" % (k.replace(" ", "_"), os.path.join(OUT, "pulled", "p%d.obj" % i)) for i, k in enumerate(sorted(pulled))))
cb = sum(int(r["hi"], 16) - int(r["lo"], 16) for r in items if r["member"] in M and r["kind"] == "code")
db = sum(int(r["hi"], 16) - int(r["lo"], 16) for r in items if r["member"] in M and r["kind"] != "code")
print("%s: %d of %d members splittable (code %d bytes, data %d bytes leave the image); symbol references %s" % (LIB, len(M), len(objof), cb, db, dict(nsym)))
