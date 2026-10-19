#!/usr/bin/env python3
"""ownership.py: who owns which bytes of MFL.exe (game | rw | crt | lua | qhull | ...), and the interface between the game and each library.
Config: tools/delink_sdk/libs.cfg.  Inputs: output/fnsizes.txt, rw_/crt_libmatch.csv (byte matched SDK / libcmt functions), names csv of source
matched libs (Lua, qhull), output/delink_sdk/sections.csv + symbols.csv (lib data placement of datasplit.py), output/delink_relocs.csv (absolute
pointer sites) and output/delink/ghidra_refs.csv (rel32 calls / jumps).
Writes output/delink_sdk/ownership.csv (lo,hi,section,owner,what): code by function (owner "<lib>?" = function between two functions of that lib
that did not match: unmatched library code, the split must replace or identify it), data by the anchored member sections and commons;
interface_<lib>.txt: every library symbol the game references (count, kind; NONSYMBOL = a static / interior target with no external name: blocker)
and the reverse direction (library code / data that references game addresses: callbacks, hooks).  ~5 s."""
import bisect, collections, configparser, csv, fnmatch, glob, os, struct, sys
H = os.path.dirname(os.path.abspath(__file__)); G = os.path.normpath(os.path.join(H, "..", "..")); O = os.path.join(G, "output")
cfg = configparser.ConfigParser(); cfg.read(os.path.join(H, "libs.cfg"))
OUT = os.path.join(O, "delink_sdk")
TEXT = (0x401000, 0x5dc000)          # .text + _rwcseg

# member key -> lib name (config section)
memlib = {}
for lib in cfg.sections():
    ex = cfg[lib].get("exclude", "").split()
    for f in cfg[lib].get("members", "").split():
        for l in open(os.path.join(G, f)):
            p = l.split(None, 1)
            if len(p) == 2 and not any(fnmatch.fnmatch(p[1].strip(), x) for x in ex): memlib[p[1].strip()] = lib

# ---- code: functions and their owner
fns = sorted((int(l.split()[0], 16), int(l.split()[2]), l.split()[1]) for l in open(os.path.join(O, "fnsizes.txt")))
cand = collections.defaultdict(set); lname = {}; lsize = {}; amb = set()
for f in ("rw_libmatch.csv", "crt_libmatch.csv"):
    for r in csv.reader(open(os.path.join(O, f))):
        a = int(r[0], 16)
        if r[2] in memlib:
            cand[a].add(memlib[r[2]]); lsize[a] = int(r[3])
            if r[4] == "": lname[(a, memlib[r[2]])] = (r[1], r[2])      # only a unique match names the member
            else: amb.add(a)                                            # identical code in several members: lib known, member not ("<lib>?")
# every byte matched lib function start is a function, even where Ghidra has one function over several (its size then ends at the next start)
have = {a for a, _, _ in fns}
fns = sorted([(a, max(n, lsize.get(a, 0)), nm) for a, n, nm in fns] + [(a, lsize[a], "lib_%x" % a) for a in lsize if a >= 0x4b0000 and a not in have])   # lib size incl. its switch tables
fns = [(a, min(n, fns[i + 1][0] - a) if i + 1 < len(fns) and fns[i + 1][0] > a else n, nm) for i, (a, n, nm) in enumerate(fns)]
srcnames = {}
for lib in cfg.sections():
    for pat in cfg[lib].get("names", "").split():
        for f in glob.glob(os.path.join(G, pat)):
            for r in csv.reader(open(f)):
                try: srcnames[int(r[0], 16)] = (lib, r[1])
                except (ValueError, IndexError): pass
def code_owner(i):
    a, n, nm = fns[i]
    if a in srcnames: return srcnames[a][0], "source match"
    c = cand.get(a, set())
    if len(c) == 1 and a >= 0x4b0000:      # byte matches below 0x4b0000 (game code) are short false positives
        lib = next(iter(c))
        if (a, lib) not in lname: return None, nm          # ambiguous: "<lib>?" only between two functions of that lib (below)
        return lib, lname[(a, lib)][1] + "|" + lname[(a, lib)][0]
    return None, nm
own = [code_owner(i) for i in range(len(fns))]; own0 = list(own)
for i in range(len(fns)):            # unmatched functions between two functions of the same library: "<lib>?"
    if own[i][0] is None:
        j = i - 1
        while j >= 0 and own0[j][0] is None: j -= 1
        k = i + 1
        while k < len(fns) and own0[k][0] is None: k += 1
        lo = own0[j][0] if j >= 0 else None; hi = own0[k][0] if k < len(fns) else None
        if lo and lo == hi and not lo.endswith("?"): own[i] = (lo + "?", fns[i][2])
ranges = []                           # lo, hi, section, owner, what
XB = open(os.path.normpath(os.path.join(G, "..", "MashedGame", "FullyLoaded", "installed", "App_Executables", "MFL.exe")), "rb").read()
for i, ((a, n, nm), (o, w)) in enumerate(zip(fns, own)):
    hi = a + n
    ranges.append([a, hi, "code", o or "game", w])
    if i + 1 < len(fns) and fns[i + 1][0] > hi:          # gap to the next function: padding joins this one, anything else (a function Ghidra
        nx = fns[i + 1][0]                               # does not know, e.g. an unmatched lib function) is its own item, owner "<lib>?" / game
        gap = XB[hi - 0x400000:nx - 0x400000]
        if not gap.strip(b"\xcc\x90\x00"): ranges[-1][1] = nx
        elif (own[i + 1][0] or "game").rstrip("?") == (o or "game").rstrip("?"):
            k_ = len(gap) - len(gap.lstrip(b"\xcc\x90")); ranges[-1][1] = hi = hi + k_      # leading padding stays with this function
            ranges.append([hi, nx, "code", (o or "game").rstrip("?") + ("?" if o else ""), "gap_%x" % hi])

# ---- data: anchored member sections and commons
for r in csv.DictReader(open(os.path.join(OUT, "sections.csv"))):
    if r["addr"] and r["member"] in memlib:
        a = int(r["addr"], 16)
        # COMDAT constants (__real@, ??_C@ strings, selection any) are shared with the game objects: owner "shared", the game image keeps them
        ranges.append([a, a + int(r["size"]), r["kind"], "shared" if r["comdat"] else memlib[r["member"]], r["member"] + "|" + r["section"]])
dsym = collections.defaultdict(list)        # lib data symbols by address: (addr, size, name, binding, lib)
for r in csv.DictReader(open(os.path.join(OUT, "symbols.csv"))):
    if r["addr"] and r["member"] in memlib:
        a = int(r["addr"], 16); dsym[a].append((a, int(r["size"] or 0), r["symbol"], r["binding"], memlib[r["member"]]))
        if r["binding"] == "common": ranges.append([a, a + max(4, int(r["size"] or 4)), "common", memlib[r["member"]], r["member"] + "|" + r["symbol"]])
ranges.sort()
# neighbour inference: an unattributed piece of code ("<lib>?": ambiguous identical code, or a gap Ghidra has no function for) between functions of
# member M is tried against M's own functions (bytes, relocation fields masked) at its exact address; a match names it (rwErrorClose between
# rwErrorOpen and RwErrorSet, ...), so whole members become splittable
import tempfile
_ns = {"__file__": os.path.join(G, "tools", "libmatch_lib.py"), "__name__": "x"}
exec(compile(open(os.path.join(G, "tools", "libmatch_lib.py")).read().replace("\nmain()\n", "\n"), "libmatch_lib", "exec"), _ns)
_libdirs = {"rw": os.path.join(G, "toolchain", "rwsdk", "lib", "d3d9", "release"),
            "crt": os.path.join(G, "toolchain", "vc7", "Vc7", "lib")}
_bodies, _fncache = {}, {}
def coff_fns(d):
    """(functions [(name, code, masked, relocs[(off, target name, type)])], external names defined) of a COFF object"""
    nsec, symoff, nsym = struct.unpack_from("<H", d, 2)[0], *struct.unpack_from("<II", d, 8); opt = struct.unpack_from("<H", d, 16)[0]; st = symoff + nsym * 18
    nm_ = lambda raw: d[st + struct.unpack_from("<I", raw, 4)[0]:d.index(b"\0", st + struct.unpack_from("<I", raw, 4)[0])].decode("latin1") if raw[:4] == b"\0\0\0\0" else raw.rstrip(b"\0").decode("latin1")
    secs = [struct.unpack_from("<8sIIIIIIHHI", d, 20 + opt + i * 40) for i in range(nsec)]
    syms = {}; i = 0
    while i < nsym:
        o = symoff + i * 18; v, sn, ty, cl, ax = struct.unpack_from("<IhHBB", d, o + 8); syms[i] = (nm_(d[o:o + 8]), v, sn, ty, cl); i += 1 + ax
    fns, defs = [], {n for n, v, sn, ty, cl in syms.values() if cl == 2 and (sn > 0 or v > 0)}
    for n, v, sn, ty, cl in syms.values():
        if sn <= 0 or (ty >> 4) != 2 or cl not in (2, 3): continue
        _, _, _, rsz, rptr, relptr, _, nrel, _, fl = secs[sn - 1]
        if not fl & 0x20 or not rsz: continue
        code = d[rptr + v:rptr + rsz]; masked = set(); rl = []
        for k in range(nrel):
            va, si, rt = struct.unpack_from("<IIH", d, relptr + k * 10)
            if rt in (6, 0x14, 7) and va >= v: masked.update(range(va - v, va - v + 4)); rl.append((va - v, syms.get(si, ("?",))[0], rt))
        fns.append((n, code, masked, rl))
    return fns, defs
def member_fns(key):
    if key not in _fncache:
        lib, mem = key.split(":", 1)
        if lib not in _bodies:
            d_ = next((d for d in _libdirs.values() if os.path.exists(os.path.join(d, lib))), None)
            _bodies[lib] = dict(_ns["members"](os.path.join(d_, lib))) if d_ else {}
        b_ = _bodies[lib].get(mem)
        try: _fncache[key] = coff_fns(b_) if b_ else ([], set())
        except Exception: _fncache[key] = ([], set())
    return _fncache[key]
_rs = None
def item_at(t):          # attributed item (code or data) holding address t
    global _rs, _rl
    if _rs is None: _rs = sorted((r for r in ranges if "|" in r[4]), key=lambda r: r[0]); _rl = [r[0] for r in _rs]
    i = bisect.bisect_right(_rl, t) - 1
    while i >= 0 and _rs[i][0] <= t:
        if t < _rs[i][1]: return _rs[i]
        i -= 1
def relocs_agree(key, r0, code, rl):
    """every relocation target of the candidate must not land in another member's item unless that member defines the target name"""
    for off, tn, rt in rl:
        if off + 4 > len(code): continue
        add = struct.unpack_from("<i", code, off)[0]; v = struct.unpack_from("<i", XB, r0 + off - 0x400000)[0]
        t = (v - add) & 0xFFFFFFFF if rt == 6 else (r0 + off + 4 + v - add) & 0xFFFFFFFF if rt == 0x14 else None
        it = item_at(t) if t else None
        if it is None: continue
        other = it[4].split("|")[0]
        if other != key and tn not in member_fns(other)[1]: return False
    return True
# unidentified pieces are split at padding runs before matching: a gap often holds several functions (each 16-byte aligned behind int3 / nop)
import re as _re
_segd = []
for r in ranges:
    if r[2] == "code" and r[3].endswith("?") and r[1] - r[0] > 32:
        cuts = [m.end() + r[0] for m in _re.finditer(rb"[\xcc\x90]{2,}", XB[r[0] - 0x400000:r[1] - 0x400000]) if (m.end() + r[0]) % 16 == 0 and m.end() + r[0] < r[1]]
        lo = r[0]
        for c in cuts: _segd.append([lo, c, "code", r[3], r[4] if lo == r[0] else "gap_%x" % lo]); lo = c
        _segd.append([lo, r[1], "code", r[3], r[4] if lo == r[0] else "gap_%x" % lo])
    else: _segd.append(r)
ranges[:] = sorted(_segd, key=lambda r: r[0])
ninf = nrej = ntol = 0; tolerant = []
_pool = {}
def pool(lib):           # every function of the config members of lib, indexed by its first two bytes (when not relocated)
    if lib not in _pool:
        idx = collections.defaultdict(list); wild = []; allf = []
        for key in sorted(k for k, l in memlib.items() if l == lib):
            for name, code, masked, rl in member_fns(key)[0]:
                if len(code) < 2: continue
                f = (key, name, code, masked, rl); allf.append(f)
                (wild if 0 in masked or 1 in masked else idx[code[:2]]).append(f)
        _pool[lib] = (idx, wild, allf)
    return _pool[lib]
PAD = b"\xcc\x90"
def find_at(p, hi, lib, near):
    """best member function at address p (exact masked match first, the longest; else a near match: same source, a few bytes differ)"""
    idx, wild, allf = pool(lib); x = XB[p - 0x400000:hi - 0x400000 + 16]
    hits = []
    for f in idx.get(x[:2], []) + wild:
        key, name, code, masked, rl = f
        if len(code) > len(x) or not all(i_ in masked or x[i_] == c_ for i_, c_ in enumerate(code)): continue
        if not relocs_agree(key, p, code, rl): continue
        hits.append(f)
    if hits:
        n_ = max(len(f[2]) for f in hits); top = [f for f in hits if len(f[2]) == n_]
        nb = [f for f in top if f[0] in near]
        # a neighbour member's function, or (anywhere in the lib) a function of >= 24 bytes whose bytes no other function shares
        if nb and len({f[1] for f in nb}) == 1: return nb[0], 0
        if n_ >= 24 and len({(f[0], f[1]) for f in top}) == 1: return top[0], 0
        return None, None
    if os.environ.get("NO_NEAR"): return None, None       # experiments: exact matches only
    lim = hi - p; cands = []
    for f in allf:                        # tolerant: the function must end at a boundary (padding or the item end) and differ in few bytes
        key, name, code, masked, rl = f
        n = len(code)
        if key not in near: continue          # near matches only from the members around it (false positives elsewhere)
        if n < 24 or n > lim or (p + n < hi and x[n] not in PAD): continue
        t = max(3, n // 25); d = 0
        for i_, c_ in enumerate(code):
            if i_ not in masked and x[i_] != c_:
                d += 1
                if d > t: break
        if d <= t: cands.append((d, -n, key not in near, f))
    if not cands: return None, None
    cands.sort(key=lambda c: c[:3])
    if len(cands) > 1 and cands[0][:2] == cands[1][:2] and cands[0][3][2] != cands[1][3][2]: return None, None     # tie between different code: unknown
    return cands[0][3], cands[0][0]
i = 0
while i < len(ranges):
    r = ranges[i]
    if r[2] != "code" or not r[3].endswith("?") or r[3] == "shared":
        i += 1; continue
    lib = r[3][:-1]
    near = set()                       # members of the nearest identified code on both sides (skipping other unidentified pieces)
    for step in (-1, 1):
        j = i + step
        while 0 <= j < len(ranges) and (ranges[j][2] != "code" or "|" not in ranges[j][4]) and abs(j - i) < 40: j += step
        if 0 <= j < len(ranges) and "|" in ranges[j][4]: near.add(ranges[j][4].split("|")[0])
    p, hi, out = r[0], r[1], []
    while p < hi:
        f, d = find_at(p, hi, lib, near)
        if f is None: break
        key, name, code, masked, rl = f
        n = len(code); nm = name if name.startswith("?") else name.lstrip("_")
        out.append([p, min(p + n, hi), "code", lib, key + "|" + nm]); ninf += d == 0; ntol += d != 0
        if d: tolerant.append((p, n, key, nm, d))
        p += n
        while p < hi and XB[p - 0x400000] in PAD: p += 1
        if out: out[-1][1] = p
    if out:
        if p < hi: out.append([p, hi, "code", r[3], "gap_%x" % p])
        ranges[i:i + 1] = out
        i += len(out)
    else: i += 1
# variants: unidentified library code that is clearly an SDK function of another source revision (byte similarity >= 0.65, length within 15 %,
# clear winner): owner "<lib>~".  plan_split.py / mkobj.py treat them as member code only with VARIANTS=1 (then the SDK version replaces the
# game's variant: behaviour can differ, the crash-only sweep does not see rendering differences)
import difflib
nvar = 0
for i, r in enumerate(ranges):
    if os.environ.get("NO_NEAR"): break
    if r[2] != "code" or not r[3].endswith("?") or r[1] - r[0] < 64: continue
    if not any(x_ and x_[2] == "code" and not x_[3].endswith("?") and x_[3] != "game" for x_ in (ranges[i - 1] if i else None, ranges[i + 1] if i + 1 < len(ranges) else None)): continue   # only next to identified lib code (speed: skips the Rwp block)
    lib = r[3][:-1]; n = r[1] - r[0]; x = XB[r[0] - 0x400000:r[1] - 0x400000]; best = []
    for key, name, code, masked, rl in pool(lib)[2]:
        if not 0.85 * n <= len(code) <= 1.15 * n: continue
        c = bytes(0 if j in masked else b for j, b in enumerate(code))
        sm = difflib.SequenceMatcher(None, x, c, autojunk=False)
        if sm.real_quick_ratio() < 0.45 or sm.quick_ratio() < 0.45: continue
        best.append((sm.ratio(), key, name))
    best.sort(reverse=True)
    near_ = {x[4].split("|")[0] for x in (ranges[i - 1] if i else None, ranges[i + 1] if i + 1 < len(ranges) else None) if x and "|" in x[4]}
    located = {x[4].split("|")[1] for x in ranges if "|" in x[4]}
    lim_ = 0.45 if best and best[0][1] in near_ and best[0][2].lstrip("_") not in located else 0.65    # a neighbour member's function not located elsewhere
    if best and best[0][0] >= lim_ and (len(best) == 1 or best[0][0] - best[1][0] >= 0.15 or best[1][2] == best[0][2]):
        nm = best[0][2] if best[0][2].startswith("?") else best[0][2].lstrip("_")
        r[3] = lib + "~"; r[4] = best[0][1] + "|" + nm; nvar += 1; tolerant.append((r[0], n, best[0][1], nm, "similarity %.2f" % best[0][0]))
print("variant functions (other source revision, owner <lib>~): %d" % nvar)
# jump tables: an unidentified piece whose pointer sites all target the code item in front of it belongs to that item (switch table)
_jt = collections.defaultdict(set)
for r_ in csv.DictReader(open(os.path.join(O, "delink_relocs.csv"))):
    if r_["kind"] in ("jumptable", "code_interior", "code"): _jt[int(r_["site"], 16)].add(int(r_["target"], 16))
_js = sorted(_jt); njt = 0
for i in range(len(ranges) - 1, 0, -1):
    r, pv = ranges[i], ranges[i - 1]
    if r[2] != "code" or not r[3].endswith("?") or pv[2] != "code" or pv[3].endswith("?") or pv[1] != r[0]: continue
    a_ = bisect.bisect_left(_js, r[0]); b_ = bisect.bisect_left(_js, r[1]); ts = [t for s_ in _js[a_:b_] for t in _jt[s_]]
    if ts and all(pv[0] <= t < pv[1] for t in ts) and (b_ - a_) * 4 >= (r[1] - r[0]) // 2:
        pv[1] = r[1]; del ranges[i]; njt += 1
print("jump tables joined to their function: %d" % njt)
with open(os.path.join(OUT, "tolerant.csv"), "w") as f:
    f.write("addr,size,member,name,differing bytes\n")
    for t_ in tolerant: f.write("%08x,%d,%s,%s,%s\n" % t_)
print("near matches (same source, a few bytes differ; output/delink_sdk/tolerant.csv): %d" % ntol)
print("neighbour inference named %d functions (%d byte matches rejected: relocation into another member)" % (ninf, nrej))
starts = [r[0] for r in ranges]
def owner_at(a):
    i = bisect.bisect_right(starts, a) - 1
    while i >= 0 and ranges[i][0] <= a:                 # overlapping data claims (duplicate members): first hit
        if a < ranges[i][1]: return ranges[i]
        i -= 1
    return None
with open(os.path.join(OUT, "ownership_items.csv"), "w") as f:      # unmerged: one row per function / member section / common (member|name), input of the split
    f.write("lo,hi,kind,owner,member,name\n")
    for lo, hi, k, o, w in ranges: f.write("%08x,%08x,%s,%s,%s\n" % (lo, hi, k, o, ",".join(w.split("|", 1)) if "|" in w else "," + w.replace(",", ";")))
merged = []
for r in ranges:
    if merged and merged[-1][3] == r[3] and merged[-1][2].split("/")[0] == r[2].split("/")[0] and r[0] <= merged[-1][1] + 16:
        merged[-1][1] = max(merged[-1][1], r[1]); merged[-1][5] += 1
    else: merged.append(r[:4] + [r[4].split("|")[0].split("/")[-1], 1])
with open(os.path.join(OUT, "ownership.csv"), "w") as f:
    f.write("lo,hi,kind,owner,first,count\n")
    for lo, hi, k, o, w, n in merged: f.write("%08x,%08x,%s,%s,%s,%d\n" % (lo, hi, k, o, w.replace(",", ";"), n))
tot = collections.Counter()
for lo, hi, k, o, w, n in merged: tot[(k.split("/")[0], o)] += hi - lo

# ---- interface: references between game-owned and library-owned bytes
def sym_of(t, r):
    if r[2] == "code":
        a = r[0]; nm = lname.get((a, r[3]), (r[4],))[0] if t == a else None
        return (nm, "code") if nm else ("%s+0x%x" % (lname.get((a, r[3]), (r[4],))[0], t - a), "NONSYMBOL code interior")
    for d in range(0, 0x10000, 1):
        for s in dsym.get(t - d, ()):
            if d == 0 or d < s[1]:
                b = s[3]
                return (s[2] + ("+0x%x" % d if d else ""), "data " + b if b in ("ext", "common") else "NONSYMBOL data " + b)
        if t - d < r[0]: break
    return ("%s+0x%x" % (r[4].split("/")[-1], t - r[0]), "NONSYMBOL data interior")
fwd = collections.defaultdict(collections.Counter); rev = collections.defaultdict(collections.Counter); kinds = {}
gnames = {}
for l in open(os.path.join(G, "src", "symbols.txt")):
    q = l.split()
    if len(q) == 2: gnames.setdefault(int(q[1], 16), q[0])
def sym_game(t): return gnames.get(t, "?")
def ref(site, t, how):
    so, to = owner_at(site), owner_at(t)
    sl = so[3] if so else "game"; tl = to[3] if to else "game"
    if sl == tl or sl.rstrip("?") == tl.rstrip("?") or "shared" in (sl, tl): return
    if tl != "game":
        nm, k = sym_of(t, to); fwd[tl][nm] += 1; kinds[(tl, nm)] = k + " " + how
    else:
        rev[sl.rstrip("?")]["%08x %s" % (t, sym_game(t))] += 1
for r in csv.DictReader(open(os.path.join(O, "delink_relocs.csv"))):
    if r["kind"] in ("const", "header", "rsrc", "import"): continue
    ref(int(r["site"], 16), int(r["target"], 16), "abs")
for r in csv.reader(open(os.path.join(O, "delink", "ghidra_refs.csv"))):
    if len(r) > 2 and r[2] in ("UNCONDITIONAL_CALL", "UNCONDITIONAL_JUMP", "CONDITIONAL_JUMP"):
        try: s, t = int(r[0], 16), int(r[1], 16)
        except ValueError: continue
        if TEXT[0] <= s < TEXT[1] and TEXT[0] <= t < TEXT[1]: ref(s, t, "rel")
for lib in cfg.sections():
    with open(os.path.join(OUT, "interface_%s.txt" % lib), "w") as f:
        f.write("# game -> %s: library symbols the game code / data references (sites, kind). NONSYMBOL = no external name: blocker for a symbolic split\n" % lib)
        for nm, c in sorted(fwd[lib].items(), key=lambda x: (-x[1], x[0])): f.write("%6d  %-50s %s\n" % (c, nm, kinds[(lib, nm)]))
        f.write("# %s -> game: game addresses referenced by library-owned code / data (callbacks, hooks; owner '%s?' code included)\n" % (lib, lib))
        for nm, c in sorted(rev[lib].items(), key=lambda x: -x[1]): f.write("%6d  %s\n" % (c, nm))
print("ownership (bytes):")
for (k, o), v in sorted(tot.items()): print("  %-8s %-8s %8d" % (k, o, v))
for lib in cfg.sections():
    ns = sum(1 for nm in fwd[lib] if kinds[(lib, nm)].startswith("NONSYMBOL"))
    print("interface %-6s game->lib %4d symbols (%d NONSYMBOL, %d sites), lib->game %d targets" % (lib, len(fwd[lib]), ns, sum(c for nm, c in fwd[lib].items() if kinds[(lib, nm)].startswith("NONSYMBOL")), len(rev[lib])))
# lib_names.txt: "name address" for every located library function (byte matched, unique lib) and external / common data symbol, names
# undecorated like src/symbols.txt (C: no leading '_'; C++ '?' names as they are): mkobj / plan_split resolve member references to the
# library code and data that stays in the image (members not split) by these names
with open(os.path.join(OUT, "lib_names.txt"), "w") as f:
    seen = set()
    for lo, hi, k, o, w in ranges:
        if "|" not in w or o == "shared" or o.endswith("?"): continue      # "<lib>~" variants keep their name: the game's variant is that function
        nm = w.split("|")[1]
        if k == "code" and nm not in seen: seen.add(nm); f.write("%s %08x\n" % (nm, lo))
    for r_ in csv.DictReader(open(os.path.join(OUT, "ext_votes.csv"))) if os.path.exists(os.path.join(OUT, "ext_votes.csv")) else []:
        u = r_["name"][1:] if r_["name"].startswith("_") else r_["name"]      # externals located through the relocations of matched functions
        if u not in seen and int(r_["votes"]) > int(r_["others"]) and 0x5dd000 <= int(r_["addr"], 16) < 0x94a008: seen.add(u); f.write("%s %s\n" % (u, r_["addr"]))
    for a, ss in sorted(dsym.items()):
        for _, _, nm, b, _ in ss:
            if b in ("ext", "common"):
                u = nm[1:] if nm.startswith("_") else nm
                if u not in seen: seen.add(u); f.write("%s %08x\n" % (u, a))
# libs.cfg `alias = name=addr ...`: game code that a source built library calls under a library name (the game's own fprintf replacement for qhull)
with open(os.path.join(OUT, "lib_names.txt"), "a") as f:
    for lib in cfg.sections():
        for a in cfg[lib].get("alias", "").split(): f.write("%s %s\n" % tuple(a.split("=")))
