#!/usr/bin/env python3
"""d3dx_closure.py <link dir> [--dead FILE] [--selftest]: liveness proof of the dead data ranges (W14 A6 / L3 D7; was agents/l3_w13 deadscan.py,
nbcheck.py, incnames.py and agents/z6p_w13 closure2.py).  For every range of tools/delink/d3dx_dead.txt ("<va> <size> <name> <step>", original VAs):
  names    every label of src/symbols.txt + aliases.txt at a VA inside the range that src/**/*.c / .h / .inc names (an .inc bind table counts:
           D3D9IdentityMatrix 0x5ec460, an_hk_612_d3d9vsu.inc)                                                     -> FAIL
  closure  gen/ptrs.csv pointer graph over gen/globals_layout.csv rows: roots = rows whose kind is not lib / dead (a gap row inside a range is not a root); a range reached from a root -> FAIL
  absword  every 4-aligned word of the linked exe (any section) whose value lies inside the range's linked rows (map symbol + offset) at a site
           outside every dead range: a .text site counts only when the covering instruction names the value (objdump); a site inside a
           generated row that gen/ptrs.csv does not list is a value word globgen emitted as bytes (rule 51: printed, not a FAIL); other data -> FAIL
  front    objects in front (nbcheck): names within 512 B before the range that our C uses as an array / aggregate: printed, report only
Exit 0 = every range unreached; 1 = FAIL lines; 2 = input error.  --selftest: the real run, then the bad case built at run time (a copy of the dead
list with an extra range around a label an .inc file names) must FAIL by `names`."""
import bisect, csv, os, re, struct, subprocess, sys, tempfile
G = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
DEADF = os.path.join(G, "tools", "delink", "d3dx_dead.txt")


def ranges(f):
    return [(int(p[0], 16), int(p[0], 16) + int(p[1]), p[2]) for p in (l.split() for l in open(f) if l.strip() and l[0] != "#")]


def labels():
    names = {}
    for f in ("src/symbols.txt", "src/aliases.txt"):
        for l in open(os.path.join(G, f)):
            p = l.split()
            if len(p) >= 2 and l[0] != "#" and re.fullmatch(r"[A-Za-z_]\w*", p[0]):
                try: names.setdefault(p[0], int(p[1], 16))
                except ValueError: pass
    return names


def src_tokens():
    tok = {}
    for root, _, fs in os.walk(os.path.join(G, "src")):
        for f in fs:
            if f.endswith((".c", ".h", ".inc")):
                p = os.path.join(root, f)
                for t in set(re.findall(r"[A-Za-z_]\w*", open(p, errors="replace").read())): tok.setdefault(t, os.path.relpath(p, G))
    return tok


def check(link, deadf, out=sys.stdout):
    R = ranges(deadf); fails = 0
    pr = lambda s: print(s, file=out)
    names, tok = labels(), src_tokens()
    inr = lambda v: any(a <= v < e for a, e, _ in R)
    lay = [r for r in csv.DictReader(open(os.path.join(link, "gen", "globals_layout.csv")))]
    for r in lay: r["lo"] = int(r["va"], 16); r["hi"] = r["lo"] + int(r["size"])
    lay.sort(key=lambda r: r["lo"]); lv = [r["lo"] for r in lay]
    def row(a):
        i = bisect.bisect_right(lv, a) - 1
        return i if i >= 0 and a < lay[i]["hi"] else None
    # closure
    edges = {}
    for p in csv.DictReader(open(os.path.join(link, "gen", "ptrs.csv"))):
        s, t = row(int(p["site"], 16)), row(int(p["orig"], 16))
        if s is not None and t is not None and s != t: edges.setdefault(s, set()).add(t)
    roots = [i for i, r in enumerate(lay) if r["kind"] not in ("lib", "dead") and not (r["kind"] == "gap" and any(a <= r["lo"] and r["hi"] <= e for a, e, _ in R))]      # W14 L3 D5: a gap row inside a range is a candidate, not a root (app rows stay roots)
    seen, par, st = set(roots), {}, list(roots)
    while st:
        a = st.pop()
        for b in edges.get(a, ()):
            if b not in seen: seen.add(b); par[b] = a; st.append(b)
    # linked addresses of the rows (map)
    sym = {}
    for l in open(os.path.join(link, "ours.map")):
        m = re.match(r"\s*[0-9a-f]{4}:[0-9a-f]{8}\s+(\S+)\s+([0-9a-f]{8})\s", l)
        if m: sym.setdefault(m.group(1), int(m.group(2), 16))
    pub = sorted((v, n) for n, v in sym.items()); pv = [x[0] for x in pub]
    lk = []      # linked lo, hi, range index
    for k, (a, e, _) in enumerate(R):
        for r in lay:
            if r["lo"] < e and a < r["hi"]:
                if "_" + r["name"] not in sym: pr("FAIL input: row %s (%08x) not in ours.map" % (r["name"], r["lo"])); fails += 1; continue
                b = sym["_" + r["name"]] - r["lo"]; lk.append((max(a, r["lo"]) + b, min(e, r["hi"]) + b, k))
    lk.sort(); ll = [x[0] for x in lk]
    gl = sorted((sym["_" + r["name"]], sym["_" + r["name"]] + r["hi"] - r["lo"], r["lo"]) for r in lay if "_" + r["name"] in sym); gll = [x[0] for x in gl]
    relocated = {int(p["site"], 16) for p in csv.DictReader(open(os.path.join(link, "gen", "ptrs.csv")))}
    def gen_value(s):      # a site inside a generated row whose original site globgen did not emit as a pointer (not in ptrs.csv): a value (rule 51)
        i = bisect.bisect_right(gll, s) - 1
        return i >= 0 and s < gl[i][1] and gl[i][2] + s - gl[i][0] not in relocated
    def lkr(v):
        i = bisect.bisect_right(ll, v) - 1
        return lk[i][2] if i >= 0 and v < lk[i][1] else None
    exe = os.path.join(link, "ours.exe"); b = open(exe, "rb").read()
    pe = struct.unpack_from("<I", b, 0x3c)[0]; nsec = struct.unpack_from("<H", b, pe + 6)[0]; oh = pe + 24 + struct.unpack_from("<H", b, pe + 20)[0]
    hits = {}
    for i in range(nsec):
        o = oh + 40 * i; nm = b[o:o + 8].rstrip(b"\0").decode(errors="replace"); vs, va, rs, rp = struct.unpack_from("<IIII", b, o + 8)
        ch = struct.unpack_from("<I", b, o + 36)[0]; raw = b[rp:rp + min(rs, vs)]
        for j, (w,) in enumerate(struct.iter_unpack("<I", raw[:len(raw) // 4 * 4])):
            k = lkr(w)
            if k is None: continue
            s = 0x400000 + va + 4 * j
            if lkr(s) is not None: continue      # a site inside a dead range
            hits.setdefault(k, []).append((s, nm, bool(ch & 0x20), w))
    def code_real(s, w):
        p = bisect.bisect_right(pv, s) - 1
        st_ = pub[p][0] if p >= 0 and s - pub[p][0] < 0x4000 else s - 64
        o = subprocess.run(["objdump", "-d", "-M", "intel", "--start-address=0x%x" % st_, "--stop-address=0x%x" % (s + 8), exe], capture_output=True, text=True).stdout
        for l in o.splitlines():
            m = re.match(r"\s*([0-9a-f]+):\s+((?:[0-9a-f]{2} )+)\s*(.*)", l)
            if m and int(m.group(1), 16) <= s and s + 4 <= int(m.group(1), 16) + len(m.group(2).split()): return ("0x%x" % w) in m.group(3)
        return False
    for k, (a, e, nm) in enumerate(R):
        bad = []
        for n, v in names.items():
            if a <= v < e and n in tok: bad.append("names: %s %08x in %s" % (n, v, tok[n]))
        ps = [i for i, r in enumerate(lay) if r["lo"] >= a and r["hi"] <= e]
        for i in ps:
            if i in seen:
                ch, c = [i], i
                while c in par and len(ch) < 6: c = par[c]; ch.append(c)
                bad.append("closure: reached " + " <- ".join("%08x:%s:%s" % (lay[x]["lo"], lay[x]["kind"], lay[x]["name"]) for x in ch))
        nf = 0; vals = []
        for s, sn, code, w in hits.get(k, []):
            if code and not code_real(s, w): nf += 1; continue
            if not code and gen_value(s): vals.append("%08x=%08x" % (s, w)); continue
            p = bisect.bisect_right(pv, s) - 1
            bad.append("absword: site %08x %s %s+%x -> %08x" % (s, sn, pub[p][1] if p >= 0 else "?", s - pub[p][0] if p >= 0 else 0, w))
        front = sorted(n for n, v in names.items() if a - 512 <= v < a and n in tok and not inr(v))
        pr("%s %08x %6d %-34s rows %d%s%s%s" % ("FAIL" if bad else "ok  ", a, e - a, nm, len(ps), " (%d code false hits)" % nf if nf else "",
           " (value words, not relocated: %s)" % " ".join(vals) if vals else "", "  front: " + " ".join(front[:6]) if front else ""))
        for x in bad[:8]: pr("     " + x)
        fails += bool(bad)
    pr("d3dx_closure: %d of %d ranges FAIL (%s)" % (fails, len(R), deadf))
    return 1 if fails else 0


def selftest(link):
    rc = check(link, DEADF)
    names, tok = labels(), src_tokens()
    inc = sorted((v, n) for n, v in names.items() if tok.get(n, "").endswith(".inc") and 0x5e2000 <= v < 0x64d000)
    if not inc: print("SELFTEST FAIL: no .inc-named data label to build the bad case"); return 1
    v, n = inc[0]
    with tempfile.NamedTemporaryFile("w", suffix=".txt", delete=False) as t:
        t.write(open(DEADF).read() + "%08x 4 selftest_%s 9\n" % (v, n)); tn = t.name
    with open(os.devnull, "w") as null: r2 = check(link, tn, out=null)
    os.remove(tn)
    print("selftest bad case (%s %08x as a dead range, built at run time): %s" % (n, v, "FAIL as required" if r2 else "NOT caught"))
    return rc if r2 else 1


if __name__ == "__main__":
    a = sys.argv[1:]
    if not a: print(__doc__); sys.exit(2)
    if "--selftest" in a: sys.exit(selftest(a[0]))
    sys.exit(check(a[0], a[a.index("--dead") + 1] if "--dead" in a else DEADF))
