#!/usr/bin/env python3
"""datasplit.py [out dir]: SDK data split spike (static analysis only).
For every SDK / CRT lib member that has byte matched functions (output/rw_overlay_members.txt + crt_overlay_members.txt) this anchors the member's own
data sections (.data / .bss / .rdata, COMDAT or not) and commons at their ORIGINAL addresses in MFL.exe: the relocations of the member's uniquely matched
functions (output/rw_libmatch.csv / crt_libmatch.csv, rows without note) point at section symbols or symbols of the member, so section base = target - offset
(majority vote per (member, section), keyed by member: same-name statics in different members never mix).  Initialized anchors are verified by comparing the
section bytes (relocation fields masked) with the exe.
Writes <out>/sections.csv (a: per member section), symbols.csv (a: per data symbol), gaps.csv (b: exe data ranges no member claims), summary.txt (a-d numbers).
Default out dir: output/delink_sdk.  Runtime ~20 s."""
import bisect, collections, csv, os, re, struct, sys
H = os.path.dirname(os.path.abspath(__file__)); G = os.path.normpath(os.path.join(H, "..", ".."))
sys.path.insert(0, os.path.join(G, "tools"))
src = open(os.path.join(G, "tools", "libmatch_lib.py")).read().replace("\nmain()\n", "\n")
ns = {"__file__": os.path.join(G, "tools", "libmatch_lib.py"), "__name__": "libmatch_lib"}
exec(compile(src, "libmatch_lib", "exec"), ns)
members = ns["members"]
OUT = sys.argv[1] if len(sys.argv) > 1 else os.path.join(G, "output", "delink_sdk"); os.makedirs(OUT, exist_ok=True)
EXE = os.path.normpath(os.path.join(G, "..", "MashedGame", "FullyLoaded", "installed", "App_Executables", "MFL.exe"))
RWLIB = os.path.join(G, "toolchain", "rwsdk", "lib", "d3d9", "release")
VC7LIB = os.path.join(G, "toolchain", "vc7", "Vc7", "lib")

# ---- exe
X = open(EXE, "rb").read()
e = struct.unpack_from("<I", X, 0x3C)[0]; nsec = struct.unpack_from("<H", X, e + 6)[0]; osz = struct.unpack_from("<H", X, e + 20)[0]
BASE = struct.unpack_from("<I", X, e + 52)[0]
DIRS = [struct.unpack_from("<II", X, e + 24 + 96 + 8 * i) for i in range(16)]
XS = []
for i in range(nsec):
    o = e + 24 + osz + i * 40
    nm = X[o:o + 8].rstrip(b"\0").decode(); vs, va, rs, rp = struct.unpack_from("<IIII", X, o + 8); fl = struct.unpack_from("<I", X, o + 36)[0]
    XS.append((nm, BASE + va, vs, rs, rp, fl))
def xbytes(va, n):
    for nm, sva, vs, rs, rp, fl in XS:
        if sva <= va and va + n <= sva + vs:
            o = va - sva
            return X[rp + o:rp + min(o + n, rs)].ljust(n, b"\0") if o < rs else b"\0" * n
    return None
def xsec(va):
    for s in XS:
        if s[1] <= va < s[1] + s[2]: return s[0]
    return None

# ---- COFF
SCN_CODE, SCN_UNINIT, SCN_INFO, SCN_REMOVE, SCN_COMDAT, SCN_WRITE = 0x20, 0x80, 0x200, 0x800, 0x1000, 0x80000000
def coff(d):
    machine, n, ts, symoff, nsym, opt, ch = struct.unpack_from("<HHIIIHH", d, 0)
    strtab = symoff + nsym * 18
    def sname(raw):
        if raw[:4] == b"\0\0\0\0":
            o = struct.unpack_from("<I", raw, 4)[0]; return d[strtab + o:d.index(b"\0", strtab + o)].decode("latin1")
        return raw.rstrip(b"\0").decode("latin1")
    secs = []
    for i in range(n):
        o = 20 + opt + i * 40
        raw = d[o:o + 8].rstrip(b"\0").decode("latin1")
        if raw.startswith("/"): raw = d[strtab + int(raw[1:]):d.index(b"\0", strtab + int(raw[1:]))].decode("latin1")
        vs, va, rsz, rptr, relptr, lnptr, nrel, nln, fl = struct.unpack_from("<IIIIIIHHI", d, o + 8)
        rels = [struct.unpack_from("<IIH", d, relptr + k * 10) for k in range(nrel)]
        secs.append(dict(name=raw, size=rsz, data=d[rptr:rptr + rsz] if rptr and not fl & SCN_UNINIT else None, rels=rels, fl=fl, sel=0))
    syms = {}; i = 0
    while i < nsym:
        o = symoff + i * 18
        value, secnum, typ, cls, aux = struct.unpack_from("<IhHBB", d, o + 8)
        s = dict(name=sname(d[o:o + 8]), value=value, sec=secnum, typ=typ, cls=cls)
        if cls == 3 and value == 0 and aux and 0 < secnum <= n:          # section symbol: aux has the COMDAT selection
            secs[secnum - 1]["sel"] = d[o + 18 + 14]
            s["secsym"] = True
        syms[i] = s; i += 1 + aux
    return secs, syms
def is_data(s): return not s["fl"] & (SCN_CODE | SCN_INFO | SCN_REMOVE) and not s["name"].startswith((".debug", ".drectve")) and s["size"] > 0

# ---- the member set and all lib members (for the link closure)
want = {}
for f in ("rw_overlay_members.txt", "crt_overlay_members.txt"):
    for l in open(os.path.join(G, "output", f)):
        p = l.split(None, 1)
        if len(p) == 2: want[p[1].strip()] = 1
ALL = {}
for d_, libs in ((RWLIB, sorted(x for x in os.listdir(RWLIB) if x.endswith(".lib"))), (VC7LIB, ["libcmt.lib"])):
    for lib in libs:
        for mem, body in members(os.path.join(d_, lib)):
            key = lib + ":" + mem
            if key in ALL: key += "#2"
            try: ALL[key] = coff(body)
            except Exception: pass
SET = [k for k in ALL if k in want]
print(len(SET), "of", len(want), "wanted members parsed;", len(ALL), "members in all libs")

# ---- anchors from matched functions
match = collections.defaultdict(dict)       # member -> {fname: va}
lib_fn = []                                  # (va, va+size) of every matched lib function (for "game site" tests)
for f in ("rw_libmatch.csv", "crt_libmatch.csv"):
    for r in csv.reader(open(os.path.join(G, "output", f))):
        va = int(r[0], 16); lib_fn.append((va, va + int(r[3])))
        if r[4] == "": match[r[2]][r[1]] = va
# functions identified later by ownership.py (neighbour inference, near matches; not the "~" variants: other code, other relocation offsets)
# anchor more member sections: run datasplit -> ownership -> datasplit -> ownership
_oi = os.path.join(OUT, "ownership_items.csv"); nadd = 0
if os.path.exists(_oi) and not os.environ.get("NO_OWNERSHIP_ANCHORS"):
    for r in csv.DictReader(open(_oi)):
        if r["kind"] == "code" and r["owner"] in ("rw", "crt") and r["member"] and r["name"] and r["name"] not in match[r["member"]]:
            match[r["member"]][r["name"]] = int(r["lo"], 16); nadd += 1
print("anchors: %d more functions from ownership_items.csv" % nadd)
lib_fn.sort(); LFS = [a for a, b in lib_fn]
def in_lib_fn(va):
    i = bisect.bisect_right(LFS, va) - 1
    return i >= 0 and va < lib_fn[i][1]
secvote = collections.defaultdict(collections.Counter)    # (member, secidx) -> base votes
gvote = collections.defaultdict(collections.Counter)      # external name -> address votes
for key in SET:
    secs, syms = ALL[key]
    fva = match.get(key, {})
    if not fva: continue
    for i, s in syms.items():           # function symbols with a unique match
        if s["sec"] <= 0 or (s["typ"] >> 4) != 2 or s["name"].lstrip("_") not in fva: continue      # libmatch csv names have the leading '_' stripped
        sec = secs[s["sec"] - 1]; va0 = fva[s["name"].lstrip("_")]
        for off, si, rt in sec["rels"]:
            if rt not in (6, 0x14) or off < s["value"]: continue
            t = syms.get(si)
            if t is None: continue
            got = xbytes(va0 + off - s["value"], 4)
            if got is None: continue
            v = struct.unpack_from("<i", got)[0]; add = struct.unpack_from("<i", sec["data"], off)[0]
            addr = (v - add) & 0xFFFFFFFF if rt == 6 else (va0 + off - s["value"] + 4 + v - add) & 0xFFFFFFFF
            if t["sec"] > 0 and is_data(secs[t["sec"] - 1]): secvote[(key, t["sec"] - 1)][(addr - t["value"]) & 0xFFFFFFFF] += 1
            if t["cls"] == 2: gvote[t["name"]][addr] += 1

# drop anchors outside the exe's data sections (immediates that only look like relocated values), then propagate through DIR32 relocations of anchored initialized sections
DSECS = [(s[1], s[1] + s[2]) for s in XS if s[0] in (".rdata", ".data", "_rwdseg")]
def in_data(a, n=1): return any(lo <= a and a + n <= hi for lo, hi in DSECS)
for k_ in list(secvote):
    for a in list(secvote[k_]):
        if not in_data(a, max(1, ALL[k_[0]][0][k_[1]]["size"])): del secvote[k_][a]
    if not secvote[k_]: del secvote[k_]
for it in range(4):
    added = 0
    for (key, k), c in list(secvote.items()):
        secs, syms = ALL[key]; sec = secs[k]
        if sec["data"] is None or not sec["rels"]: continue
        base = c.most_common(1)[0][0]
        for off, si, rt in sec["rels"]:
            t = syms.get(si)
            if rt != 6 or t is None or t["sec"] <= 0 or not is_data(secs[t["sec"] - 1]) or (key, t["sec"] - 1) in secvote: continue
            got = xbytes(base + off, 4)
            if got is None: continue
            addr = (struct.unpack_from("<i", got)[0] - struct.unpack_from("<i", sec["data"], off)[0]) & 0xFFFFFFFF
            if in_data(addr): secvote[(key, t["sec"] - 1)][(addr - t["value"]) & 0xFFFFFFFF] += 1; added += 1
    if not added: break
    print("propagation round", it, "added", added)

# ---- (a) sections and symbols
srows, yrows, claims = [], [], []      # claims: (lo, hi, member, sec)
verified = collections.Counter(); bytes_found = collections.Counter()
_ds = [s for s in XS if s[0] in (".rdata", ".data")]; DIMG = b"".join(X[s[4]:s[4] + min(s[2], s[3])] for s in _ds)
def DOFF(h):
    for s in _ds:
        n_ = min(s[2], s[3])
        if h < n_: return s[1] + h
        h -= n_
def kind(sec): return ("bss" if sec["fl"] & SCN_UNINIT else "data" if sec["fl"] & SCN_WRITE else "rdata")
unanch = collections.Counter(); unanch_sz = collections.Counter()
for key in SET:
    secs, syms = ALL[key]
    for k, sec in enumerate(secs):
        if not is_data(sec): continue
        c = secvote.get((key, k)); base = dis = n = None; ver = ""
        if not c and sec["data"] is not None and sec["size"] >= 64 and not sec["fl"] & SCN_COMDAT:     # no relocation anchor: unique byte search in the exe's data (relocation fields masked)
            mk = set(); [mk.update(range(o_, o_ + 4)) for o_, si, rt in sec["rels"]]
            pat = b"".join(b"." if i in mk else re.escape(bytes([b])) for i, b in enumerate(sec["data"]))
            hits = [m_.start() for m_ in re.finditer(pat, DIMG, re.S)][:2]
            bytes_found[len(hits)] += sec["size"]
            if len(hits) == 1: c = collections.Counter({DOFF(hits[0]): 0})
        if c:
            (base, n), dis = c.most_common(1)[0], sum(c.values()) - c.most_common(1)[0][1]
            if sec["data"] is not None:
                got = xbytes(base, sec["size"])
                if got is None: ver = "outside"
                else:
                    m = bytearray(sec["data"]); g = bytearray(got)
                    for off, si, rt in sec["rels"]: m[off:off + 4] = g[off:off + 4] = b"...."
                    ver = "ok" if m == g else "DIFF"
                verified[ver] += 1
            claims.append((base, base + sec["size"], key, sec["name"]))
        else:
            unanch[kind(sec)] += 1; unanch_sz[kind(sec)] += sec["size"]
        srows.append((key, k + 1, sec["name"], kind(sec), "comdat%d" % sec["sel"] if sec["fl"] & SCN_COMDAT else "", sec["size"],
                      "%08x" % base if base is not None else "", n or 0, dis or 0, ver, xsec(base) if base is not None else ""))
        # symbols of this section
        ss = sorted((s["value"], s["name"], s["cls"]) for s in syms.values() if s["sec"] == k + 1 and not s.get("secsym") and s["cls"] in (2, 3))
        for j, (v, nm, cl) in enumerate(ss):
            sz = (ss[j + 1][0] if j + 1 < len(ss) else sec["size"]) - v
            yrows.append((key, nm, ("ext" if cl == 2 else "static") + ("/comdat" if sec["fl"] & SCN_COMDAT else ""), sec["name"], kind(sec), v, sz, "%08x" % (base + v) if base is not None else ""))
    for s in syms.values():              # commons
        if s["sec"] == 0 and s["value"] > 0 and s["cls"] == 2:
            a = gvote.get(s["name"]); addr = a.most_common(1)[0][0] if a else None
            yrows.append((key, s["name"], "common", "", "bss", 0, s["value"], "%08x" % addr if addr is not None else ""))
            if addr is not None: claims.append((addr, addr + s["value"], key, "common:" + s["name"]))
with open(os.path.join(OUT, "sections.csv"), "w", newline="") as f:
    w = csv.writer(f); w.writerow(("member", "secno", "section", "kind", "comdat", "size", "addr", "votes", "disagree", "bytes_check", "exe_section")); w.writerows(srows)
with open(os.path.join(OUT, "symbols.csv"), "w", newline="") as f:
    w = csv.writer(f); w.writerow(("member", "symbol", "binding", "section", "kind", "offset", "size", "addr")); w.writerows(yrows)

# ---- (b) exe data ranges nobody claims
claims.sort()
nc = sorted(c for c in claims if not c[3].startswith("common:") and not any(r[0] == c[2] and r[2] == c[3] and r[4] and r[6] == "%08x" % c[0] for r in srows))
overl = [(nc[i - 1], nc[i]) for i in range(1, len(nc)) if nc[i][0] < nc[i - 1][1]]
ranges = [(s[1], s[1] + s[2], s[0]) for s in XS if s[0] in (".rdata", ".data", "_rwdseg")]
merged = []
for lo, hi, *_ in claims:
    if merged and lo <= merged[-1][1]: merged[-1][1] = max(merged[-1][1], hi)
    else: merged.append([lo, hi])
gaps, cov = [], collections.Counter()
for lo, hi, nm in ranges:
    p = lo
    for a, b in merged:
        a, b = max(a, lo), min(b, hi)
        if a >= b: continue
        if a > p: gaps.append((p, a, nm))
        cov[nm] += b - a; p = max(p, b)
    if p < hi: gaps.append((p, hi, nm))
raw_end = {s[0]: s[1] + s[3] for s in XS}
with open(os.path.join(OUT, "gaps.csv"), "w") as f:
    f.write("lo,hi,size,section,init\n")
    for lo, hi, nm in gaps: f.write("%08x,%08x,%d,%s,%s\n" % (lo, hi, hi - lo, nm, "bss" if lo >= raw_end[nm] else "init" if hi <= raw_end[nm] else "mixed"))

# ---- (c) link closure, undefined externals, collisions, special sections
defs = collections.defaultdict(list)          # external -> members defining it (non COMDAT / COMDAT flag)
for key, (secs, syms) in ALL.items():
    for s in syms.values():
        if s["cls"] == 2 and (s["sec"] > 0 or (s["sec"] == 0 and s["value"] > 0) or s["sec"] == -1):
            defs[s["name"]].append((key, s["sec"] > 0 and bool(secs[s["sec"] - 1]["fl"] & SCN_COMDAT)))
def undef_of(key):
    secs, syms = ALL[key]
    return {s["name"] for s in syms.values() if s["cls"] == 2 and s["sec"] == 0 and s["value"] == 0}
def defd_of(key):
    secs, syms = ALL[key]
    return {s["name"] for s in syms.values() if s["cls"] == 2 and (s["sec"] != 0 or s["value"] > 0)}
inset = set(SET); D_ = set().union(*(defd_of(k) for k in SET)); U_ = set().union(*(undef_of(k) for k in SET)) - D_
pulled, queue = set(), list(U_)
while queue:
    n_ = queue.pop()
    if n_ in D_ or n_.startswith("__imp_"): continue
    cand = [k for k, c in defs.get(n_, []) if k not in inset]
    if not cand: continue
    k = cand[0]; pulled.add(k); inset.add(k); dd = defd_of(k); D_ |= dd; queue += list(undef_of(k) - D_)
U_all = set().union(*(undef_of(k) for k in inset)) - D_
imps = {n for n in U_all if n.startswith("__imp_")}
resid = sorted(U_all - imps)
gaddr = {n: c.most_common(1)[0][0] for n, c in gvote.items()}
with open(os.path.join(OUT, "ext_votes.csv"), "w") as f:      # every external the matched functions reference, with its address in the exe (also data-only members: _fmode, nlsdata, ...)
    f.write("name,addr,votes,others\n")
    for n, c in sorted(gvote.items()):
        (a, v), rest = c.most_common(1)[0], sum(c.values()) - c.most_common(1)[0][1]
        f.write("%s,%08x,%d,%d\n" % (n, a, v, rest))
def tsec_of(n):
    a = gaddr.get(n); return ("code" if a is not None and 0x401000 <= a < 0x5dd000 else "data" if a is not None else "?")
pulled_data = sum(sec["size"] for k in pulled for sec in ALL[k][0] if is_data(sec))
pulled_by = collections.defaultdict(list)
for k in pulled: pulled_by[k.split(":")[0]].append((sum(sec["size"] for sec in ALL[k][0] if is_data(sec)), k.replace("\\", "/").split("/")[-1]))
errno_users = sorted(k.replace("\\", "/").split("/")[-1] for k in inset if "_errno" in undef_of(k))
# same-name statics (data) in different members of the set, and duplicate non-COMDAT externals
stat = collections.defaultdict(set); dupext = collections.defaultdict(set)
for key in inset:
    secs, syms = ALL[key]
    for s in syms.values():
        if s["sec"] > 0 and not s.get("secsym") and is_data(secs[s["sec"] - 1]) and not s["name"].startswith(("$", "??_C@", "__real@")):
            if s["cls"] == 3: stat[s["name"]].add(key)
            elif s["cls"] == 2 and not secs[s["sec"] - 1]["fl"] & SCN_COMDAT: dupext[s["name"]].add(key)
coll = {n: m for n, m in stat.items() if len(m) > 1}; dupx = {n: m for n, m in dupext.items() if len(m) > 1}
special = collections.defaultdict(set); comdat_data = collections.Counter(); comdat_bytes = collections.Counter()
for key in inset:
    secs, syms = ALL[key]
    for sec in secs:
        if sec["name"].startswith((".CRT$", ".tls", "_rwdseg", "_rwcseg")) or sec["name"] not in (".text", ".data", ".bss", ".rdata", ".drectve", ".debug$S", ".debug$T", ".debug$F", ".rdata$r") and not sec["name"].startswith((".text", ".debug")):
            special[sec["name"]].add(key)
        if is_data(sec) and sec["fl"] & SCN_COMDAT: comdat_data[sec["sel"]] += 1; comdat_bytes[sec["sel"]] += sec["size"]
    for s in syms.values():
        if s["name"] in ("__except_list", "__tls_used", "__tls_index", "__tls_array", "___onexitbegin", "___onexitend", "__encode_pointer", "___security_cookie") and s["sec"] != 0:
            special["sym:" + s["name"]].add(key)
# game references into lib data (sites outside matched lib functions): must become symbol references after the split
rel = list(csv.DictReader(open(os.path.join(G, "output", "delink_relocs.csv"))))
cl_lo = [c[0] for c in claims]
def claim_of(a):
    i = bisect.bisect_right(cl_lo, a) - 1
    while i >= 0 and claims[i][0] <= a:
        if a < claims[i][1]: return claims[i]
        i -= 1
    return None
symat = {}
for y in yrows:
    if y[7]: symat.setdefault(int(y[7], 16), y)
gref = collections.Counter(); gtargets = collections.defaultdict(set); gsites = collections.defaultdict(set)
for r in rel:
    site, tgt = int(r["site"], 16), int(r["target"], 16)
    c = claim_of(tgt)
    if not c or r["kind"] in ("const",): continue
    side = "lib" if (r["src"] == "code" and in_lib_fn(site)) or (r["src"] == "data" and claim_of(site)) else "game"
    y = symat.get(tgt)
    cat = y[2] if y else "interior/unnamed"
    gref[(side, r["src"], cat)] += 1; gtargets[(side, cat)].add(tgt)
    if side == "game": gsites[(tgt, y[1] if y else c[3], y[0] if y else c[2])].add(r["site"])

# ---- summary
L = []
p = L.append
p("SDK data split spike (tools/delink_sdk/datasplit.py), %d members (RW %d, CRT %d) with byte matched functions" % (len(SET), sum(1 for k in SET if not k.startswith("libcmt")), sum(1 for k in SET if k.startswith("libcmt"))))
tot = collections.Counter(); totn = collections.Counter(); anch = collections.Counter(); anchn = collections.Counter()
for r in srows:
    kk = r[3] + ("/comdat" if r[4] else ""); tot[kk] += r[5]; totn[kk] += 1
    if r[6]: anch[kk] += r[5]; anchn[kk] += 1
p("(a) data sections of the set: kind: n sections / bytes, anchored n / bytes")
for kk in sorted(tot): p("    %-14s %5d / %8d   anchored %5d / %8d" % (kk, totn[kk], tot[kk], anchn[kk], anch[kk]))
com = [y for y in yrows if y[2] == "common"]
p("    commons: %d / %d bytes, anchored %d / %d bytes" % (len(com), sum(y[6] for y in com), sum(1 for y in com if y[7]), sum(y[6] for y in com if y[7])))
p("    byte check of anchored initialized sections: %s;  anchors with disagreeing votes: %d;  overlapping non-COMDAT claims: %d %s" % (dict(verified), sum(1 for r in srows if r[8]), len(overl), overl[:3]))
p("    byte search for unanchored non-COMDAT sections >= 64 bytes: {hits: bytes} %s (1 = anchored by bytes, 0 = data not in the exe)" % dict(bytes_found))
p("    symbols: %d (ext %d, static %d, common %d)" % (len(yrows), sum(1 for y in yrows if y[2].startswith("ext")), sum(1 for y in yrows if y[2].startswith("static")), len(com)))
for nm in (".rdata", ".data", "_rwdseg"):
    sva = [s for s in XS if s[0] == nm][0]
    p("(b) %-8s %08x-%08x (%d bytes, init %d): lib members claim %d bytes, unclaimed (game or unmatched lib) %d bytes" % (nm, sva[1], sva[1] + sva[2], sva[2], min(sva[2], sva[3]), cov[nm], sva[2] - cov[nm]))
lib_cl = [c for c in claims]
if lib_cl:
    for nm in (".rdata", ".data"):
        cs = [c for c in claims if xsec(c[0]) == nm]
        if cs:
            init = [c for c in cs if c[0] < raw_end[nm]]; bss = [c for c in cs if c[0] >= raw_end[nm]]
            for lab, cc in (("init", init), ("bss", bss)):
                if cc:
                    lo, hi = min(c[0] for c in cc), max(c[1] for c in cc); inner = sum(b - a for a, b, nm2 in gaps if lo <= a and b <= hi)
                    p("    %s %s lib claims span %08x-%08x (%d bytes), unclaimed holes inside that span %d bytes" % (nm, lab, lo, hi, hi - lo, inner))
big = sorted(gaps, key=lambda g: g[0] - g[1])[:8]
p("    largest unclaimed ranges: " + ", ".join("%08x+%x" % (a, b - a) for a, b, n_ in big))
p("(c) link closure: %d undefined externals of the set before closure; closure pulls %d more lib members (%d data bytes not in the matched set)" % (len(U_), len(pulled), pulled_data))
p("    after closure: %d unresolved externals: %d imports (__imp_*), %d others (must come from the game / our C / other libs):" % (len(U_all), len(imps), len(resid)))
p("      " + " ".join("%s[%s]" % (n, tsec_of(n)) for n in resid))
p("    pulled members per lib (n, data bytes, largest): " + "; ".join("%s %d %d %s" % (l_, len(v), sum(x[0] for x in v), max(v)[1]) for l_, v in sorted(pulled_by.items(), key=lambda kv: -sum(x[0] for x in kv[1]))))
p("    _errno (data, libc single thread) referenced by: " + " ".join(errno_users))
p("    same-name data statics in >1 member: %d names (%s)" % (len(coll), ", ".join(sorted(coll)[:40])))
p("    duplicate non-COMDAT data externals: %d (%s)" % (len(dupx), ", ".join("%s:%s" % (n, "|".join(sorted(m))) for n, m in sorted(dupx.items())[:10])))
p("    data COMDAT sections by selection (2=any,1=nodup,3=same size,4=exact,5=assoc,6=largest): %s bytes %s" % (dict(comdat_data), dict(comdat_bytes)))
for k_ in sorted(special): p("    special %-14s in %d members: %s" % (k_, len(special[k_]), " ".join(sorted(m.split(":", 1)[1].replace("\\", "/").split("/")[-1] for m in special[k_])[:12])))
p("    references into claimed lib data (delink_relocs.csv): " + ", ".join("%s/%s/%s %d" % (k[0], k[1], k[2], v) for k, v in sorted(gref.items())))
p("    distinct targets: " + ", ".join("%s/%s %d" % (k[0], k[1], len(v)) for k, v in sorted(gtargets.items())))
p("    unanchored data sections: %s bytes %s" % (dict(unanch), dict(unanch_sz)))
with open(os.path.join(OUT, "game_refs.csv"), "w") as f:
    f.write("target,symbol,member,nsites,sites\n")
    for (t, n_, m_), ss in sorted(gsites.items()): f.write("%08x,%s,%s,%d,%s\n" % (t, n_, m_, len(ss), " ".join(sorted(ss)[:8])))
open(os.path.join(OUT, "summary.txt"), "w").write("\n".join(L) + "\n")
with open(os.path.join(OUT, "unresolved.txt"), "w") as f:
    for n in resid: f.write("%s %s %s\n" % (n, tsec_of(n), "%08x" % gaddr[n] if n in gaddr else "-"))
with open(os.path.join(OUT, "static_collisions.txt"), "w") as f:
    for n, m in sorted(coll.items()): f.write("%s %s\n" % (n, " ".join(sorted(m))))
print("\n".join(L))
