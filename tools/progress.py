#!/usr/bin/env python3
"""progress.py [--check] [--readme README.md]: progress numbers -> progress/progress.json and the table between the progress markers of the README.
Inputs (data/ in the public repo, output/ in the private tree): fnsizes.txt (va name size), tier_ledger.csv (tools/tier_ledger.py); src/manifest.txt.
--check: change nothing, exit 1 when progress.json or the README table would change.  Run `progress.py --self-test` for the check of the counting."""
import collections, csv, json, os, re, sys
G = os.path.normpath(os.path.join(os.path.dirname(os.path.abspath(__file__)), ".."))
GAME_END = 0x4B0000                                     # application code ends here, the rest is statically linked libraries / CRT
AUTO = re.compile(r"^(FUN_|thunk_FUN_|LAB_)")
START, END = "<!-- progress:start -->", "<!-- progress:end -->"

def find(name):
    for d in ("data", "output"):
        p = os.path.join(G, d, name)
        if os.path.exists(p): return p

def compute():
    fn = [(int(l.split()[0], 16), l.split()[1], int(l.split()[2])) for l in open(find("fnsizes.txt")) if len(l.split()) >= 3]
    size = {va: sz for va, _, sz in fn}
    va_of = {n: "%08x" % va for va, n, _ in fn}
    game = [f for f in fn if f[0] < GAME_END]
    named = lambda fs: sum(1 for _, n, _ in fs if not AUTO.match(n))
    man = collections.Counter(); by_mod = collections.Counter()
    for l in open(os.path.join(G, "src", "manifest.txt")):
        p = l.split()
        if not p or p[0] == "#" or p[0].startswith("#") or p[0] == "(none)": continue
        st = p[4] if len(p) > 4 else "MATCH"             # blank status = MATCH (cmpfn --manifest)
        man[st] += 1; by_mod[re.sub(r"(_np)?\.c$", "", p[0])] += 1
    tiers = collections.defaultdict(lambda: collections.Counter())
    p = p_led = find("tier_ledger.csv")
    if p:
        for r in csv.DictReader(open(p)):
            t = tiers[r["tier"]]; t["functions"] += 1; t["bytes"] += (size.get(int(r["va"], 16), 0) if re.fullmatch(r"[0-9a-fA-F]+", r["va"]) else 0)
            t["verified_bytes_identical"] += r["authority"] == "bytes-reloc"
    rep = {t: dict(c) for t, c in sorted(tiers.items())}
    cl_names = []
    cl = {"functions": 0, "bytes": 0}                    # register-argument clusters (natural-C body + generated entry thunk), behaviour verified
    p = find("clusters_enabled.txt") or os.path.join(G, "src", "clusters", "enabled.txt")
    if os.path.exists(p):
        by_name = {n: sz for _, n, sz in fn}
        for n in (l.strip() for l in open(p)):
            if not n: continue
            cf = next((os.path.join(x, "cl_%s.c" % n) for x in (os.path.join(G, "src", "clusters"), os.path.join(G, "src", "clusters")) if os.path.exists(os.path.join(x, "cl_%s.c" % n))), None)
            m = re.search(r"^// cluster-entries: (.*)$", open(cf, errors="replace").read(), re.M) if cf else None
            for e in (m.group(1).replace(",", " ").split() if m else [n]):          # a cluster file can hold several entry functions: count them all (enabled.txt lists roots only)
                cl_names.append(e); cl["functions"] += 1; cl["bytes"] += by_name.get(e, 0)
    pub = {"tier2_files": 0, "tier2_functions": 0, "cluster_files": 0, "cluster_functions": 0, "tier1_files": 0, "tier1_functions": 0, "automatch_files": 0, "automatch_functions": 0}     # published sources (public repo: src/tier1, src/automatch, src/tier2, src/clusters)
    vaof = lambda n: va_of.get(n, n)                                        # function name -> VA (unresolved names stay as they are)
    names = {"hand": set(), "tier1": set(), "automatch": set(), "tier2": set(), "cluster": set()}                   # function names per kind, to count a function once
    for l in open(os.path.join(G, "src", "manifest.txt")):
        q = l.split()
        if len(q) > 2 and not q[0].startswith("#") and q[0] != "(none)": names["hand"].add(q[2].lower().zfill(8))
    for sub, k in (("tier1", "tier1"), ("automatch", "automatch")):         # byte identical sources, one function per file ('{' in column 0 = function body)
        d = os.path.join(G, "src", sub)
        fs = [f for f in os.listdir(d) if f.endswith(".c")] if os.path.isdir(d) else []
        pub[k + "_files"] = len(fs)
        for f in fs:
            t = open(os.path.join(d, f), errors="replace").read().split("\n")
            for i, x in enumerate(t):
                if x == "{":
                    m = re.search(r"(\w+)\(", next((y for y in reversed(t[:i]) if "(" in y and not y[:1].isspace()), ""))
                    names[k].add(vaof(m.group(1) if m else f[:-2]))
    for sub, k in (("tier2", "tier2"), ("clusters", "cluster")):
        d = next((x for x in (os.path.join(G, "src", sub), os.path.join(G, "src", "automatch_near" if sub == "tier2" else "clusters")) if os.path.isdir(x)), None)
        fs = [f for f in os.listdir(d) if re.match(r"(an|cl)_.*\.c$", f)] if d else []
        pub[k + "_files"] = len(fs)
        if k == "tier2":
            nm = os.path.join(G, "src", "tier2", "names.txt")
            if os.path.exists(nm):
                for l in open(nm):
                    q = l.split()
                    if len(q) >= 3 and "an_" + q[2] in fs: names[k].add(q[0].lower().zfill(8))
        else:
            for f in fs:
                m = re.search(r"^// cluster-entries: (.*)$", open(os.path.join(d, f), errors="replace").read(), re.M)
                if m: names[k] |= {vaof(x) for x in m.group(1).replace(",", " ").split()}
    inrange = lambda v: not re.fullmatch(r"[0-9a-f]{8}", v) or int(v, 16) < GAME_END
    for k, nm_ in (("tier1", "tier1"), ("automatch", "automatch"), ("tier2", "tier2"), ("cluster", "cluster")): pub[k + "_functions"] = sum(1 for v in names[nm_] if inrange(v))
    pub["library_functions"] = len({v for k in names.values() for v in k if not inrange(v)})      # published sources of linked library code (VA >= 0x4B0000), not in the percentages
    pub["hand_functions"] = len(names["hand"])
    pub["unique_all_kinds"] = len(set().union(*names.values()))
    pub["unique_functions"] = len({v for k in names.values() for v in k if inrange(v)})                  # a name in two kinds (e.g. tier 2 draft and its cluster) counts once
    # whole pipeline, each function once: ledger rows (VA, or the VA of the name when the ledger has "?") plus enabled clusters, application range only
    led = {}
    if p_led:
        for r in csv.DictReader(open(p_led)):
            v = r["va"].lower() if re.fullmatch(r"[0-9a-fA-F]{8}", r["va"]) else va_of.get(r["name"])
            if v and int(v, 16) < GAME_END: led.setdefault(v, r)
    clv = {va_of[n] for n in cl_names if n in va_of and int(va_of[n], 16) < GAME_END}
    bi = {v for v, r in led.items() if r["authority"] == "bytes-reloc"}
    allv = set(led) | clv
    sz = {"%08x" % va: s_ for va, s_ in size.items()}
    ut = collections.defaultdict(collections.Counter)            # by tier, each function once (ledger row, or "cluster (not in ledger)")
    for v in allv:
        t = ut[led[v]["tier"] if v in led else "cluster (not in ledger)"]; t["functions"] += 1; t["bytes"] += sz.get(v, 0); t["verified_bytes_identical"] += v in bi
    uniq_tiers = {k: dict(c) for k, c in sorted(ut.items())}
    uniq = {"functions": len(allv), "bytes": sum(sz.get(v, 0) for v in allv), "byte_identical": len(bi), "byte_identical_bytes": sum(sz.get(v, 0) for v in bi),
            "clusters": len(clv), "clusters_not_in_ledger": len(clv - set(led)), "by_tier": uniq_tiers}
    # behaviour verified = rows with evidence (tools/evidence_ledger.py -> behaviour_evidence.csv: diffemu EQUAL, >= 20 cases, coverage >= 0.8, no unresolved DIFF), NOT "replaced minus byte identical"
    pe = find("behaviour_evidence.csv"); ev = {"rows": 0, "verified": 0, "verified_bytes": 0, "unverified": 0, "flags": {}}
    if pe:
        fc = collections.Counter()
        for r in csv.DictReader(open(pe)):
            ev["rows"] += 1; ok = r["verified"] == "1"; ev["verified"] += ok; ev["verified_bytes"] += ok * sz.get(r["va"], 0)
            for f in r["flags"].split(";"):
                if f: fc[f] += 1
        ev["unverified"] = ev["rows"] - ev["verified"]; ev["flags"] = dict(sorted(fc.items()))
        ev["unaccounted"] = len(allv - bi) - ev["rows"]
    uniq["behaviour_verified"] = ev["verified"]; uniq["behaviour_evidence"] = ev
    return {"exe_sha1": "d52ca4a5995875b6d052707188e2c6d065f41766",
            "functions": {"total": len(fn), "game": len(game), "named": named(fn), "game_named": named(game),
                          "game_bytes": sum(s for _, _, s in game)},
            "published_source": {"by_status": dict(man), "by_module": dict(sorted(by_mod.items()))},
            "replaced_in_pipeline": rep, "replaced_unique": uniq, "clusters": cl, "published_verified": pub}

def bar(n, d, w=20):
    k = round(w * n / d) if d else 0
    return "`%s%s` %.0f%%" % ("█" * k, "░" * (w - k), 100 * n / d if d else 0)

def table(j):
    f, m = j["functions"], j["published_source"]["by_status"]
    pub = sum(m.values()); rep = j["replaced_in_pipeline"]; g = f["game"]
    out = ["**%s application functions** (VA < 0x4B0000, the rest is linked library code)\n" % format(g, ","),
           "| Stage | Functions | Progress |", "|---|---:|:---|",
           "| Named | %s | %s |" % (format(f["game_named"], ","), bar(f["game_named"], g))]
    u = j.get("replaced_unique")
    if u:
        out += ["| Replaced by recompiled C (all tiers, each function once) | %s | %s |" % (format(u["functions"], ","), bar(u["functions"], g)),
                "| ...verified byte identical | %s | %s |" % (format(u["byte_identical"], ","), bar(u["byte_identical"], g)),
                "| ...share of the code bytes that is byte identical | %s B | %s |" % (format(u["byte_identical_bytes"], ","), bar(u["byte_identical_bytes"], f["game_bytes"])),
                "| ...behaviour verified, not byte identical (a diffemu EQUAL run on file: >= 20 cases, coverage >= 0.8) | %s | %s |" % (format(u["behaviour_verified"], ","), bar(u["behaviour_verified"], g)),
                "| ...replaced but weakly verified or without evidence (low coverage, DIFF, inconclusive, no run on file; see `data/behaviour_evidence.csv`) | %s | %s |" % (format(u["functions"] - u["byte_identical"] - u["behaviour_verified"], ","), bar(u["functions"] - u["byte_identical"] - u["behaviour_verified"], g)),
                "| ...share of the code bytes | %s B | %s |" % (format(u["bytes"], ","), bar(u["bytes"], f["game_bytes"])),
                "| ...of these register-argument clusters | %s | %s |" % (format(u["clusters"], ","), bar(u["clusters"], g))]
    pv = j.get("published_verified")
    if pv:
        tp = pv["unique_functions"]; bi = m.get("MATCH", 0) + pv["tier1_functions"] + pv["automatch_functions"]
    else: tp = pub; bi = m.get("MATCH", 0)
    out.append("| **Published in this repo** (`src/`: tier 1 + automatch + hand-written + tier 2 + clusters, each function once) | %s | %s |" % (format(tp, ","), bar(tp, g)))
    out.append("| ...byte identical (tier 1, automatch, hand-written `MATCH`) | %s | %s |" % (format(bi, ","), bar(bi, g)))
    if pv:
        out.append("| ...tier 1, `src/tier1` (%d files) | %s | %s |" % (pv["tier1_files"], format(pv["tier1_functions"], ","), bar(pv["tier1_functions"], g)))
        out.append("| ...automatch, `src/automatch` (%d files) | %s | %s |" % (pv["automatch_files"], format(pv["automatch_functions"], ","), bar(pv["automatch_functions"], g)))
    out.append("| ...hand-written, `src/*.c` (`manifest.txt`) | %s | %s |" % (format(pub, ","), bar(pub, g)))
    if pv:
        out.append("| ...tier 2, `src/tier2` (%d files) | %s | %s |" % (pv["tier2_files"], format(pv["tier2_functions"], ","), bar(pv["tier2_functions"], g)))
        out.append("| ...clusters, `src/clusters` (%d files) | %s | %s |" % (pv["cluster_files"], format(pv["cluster_functions"], ","), bar(pv["cluster_functions"], g)))
        if pv.get("library_functions"): out.append("| ...plus linked library code (VA >= 0x4B0000), not in the percentages | %d | |" % pv["library_functions"])
        ov = sum(pv[k] for k in ("tier1_functions", "automatch_functions", "tier2_functions", "cluster_functions")) + pub - tp
        if ov: out.append("| ...functions present in two kinds (counted once above) | %d | |" % ov)
    out += ["", "**What the numbers mean.** A function is one VA below 0x4B0000; every row counts each function once, however many files or tiers mention it. A cluster file (`src/clusters/cl_*.c`) can hold several entry functions (`// cluster-entries:`), so *files* and *functions* differ (%d cluster files give %d functions). *Byte identical* = ledger authority `bytes-reloc`: the original compiler turns the C into the original bytes (relocations checked). *Behaviour verified* = not byte identical, but a differential run against the original code in an emulator (diffemu) ended EQUAL with at least 20 cases and coverage of at least 0.8 on both sides, no unresolved DIFF; the evidence per function is `data/behaviour_evidence.csv` (source, cases, coverage, flags). Rows with the flags `STDIND` (only verified by a sweep whose stack-pop heuristic is known to misbehave) or `LADDER` (a weaker harness setting was needed) are still counted, see the flag counts in `progress/progress.json`. It is NOT a statement that the whole game is verified, and the share of *bytes* that is byte identical is far lower than the share of *functions*." % ((pv or {}).get("cluster_files", 0), (pv or {}).get("cluster_functions", 0)), "",
           "**Layers.** All of the rows above are application code (VA < 0x4B0000). Not counted and still original bytes in the playable build: the C runtime (libcmt, about 81 KB, including 23.6 KB floating-point/exception code without source), RenderWare Rw/Rp/Rx/Rt (about 480 KB; to be replaced by a clean-room librw-mfl, not byte-matched), the RenderWare physics plugin Rwp (121 KB, no source), RtPNG/libpng/zlib, D3DX, Lua 4.0, qhull and DirectShow video code, plus the data, resources and the link without the original image. The D3D9, DirectSound, DirectInput and Win32 platform calls are made directly from the application C. See [docs/RW_LIBRARY_LAYER.md](docs/RW_LIBRARY_LAYER.md), [docs/CRT_LAYER.md](docs/CRT_LAYER.md) and [docs/VIDEO_INTERFACE.md](docs/VIDEO_INTERFACE.md).", "",
           "*Whole pipeline* rows above count everything the private working tree has replaced; *Published* rows are the part whose C source is in this repo (see [docs/VERIFICATION.md](docs/VERIFICATION.md)).", ""]
    bt = (u or {}).get("by_tier")
    if bt:
        out += ["", "<details><summary>By tier (whole pipeline, each function once)</summary>\n", "| Tier | Functions | Code bytes | Verified byte identical |", "|---|---:|---:|---:|"]
        out += ["| %s | %d | %s | %d |" % (k, t["functions"], format(t["bytes"], ","), t["verified_bytes_identical"]) for k, t in bt.items()]
        out.append("\n</details>")
    return "\n".join(out)

def run(check, readme):
    j = compute(); js = json.dumps(j, indent=1, sort_keys=True) + "\n"; jp = os.path.join(G, "progress", "progress.json")
    new = table(j); text = open(readme).read() if readme and os.path.exists(readme) else None
    if text and START in text:
        nt = re.sub(re.escape(START) + ".*?" + re.escape(END), lambda m: START + "\n" + new + "\n" + END, text, flags=re.S)
    else: nt = text
    stale = (not os.path.exists(jp) or open(jp).read() != js) or (nt != text)
    if check: return 1 if stale else 0
    os.makedirs(os.path.dirname(jp), exist_ok=True); open(jp, "w").write(js)
    if nt and nt != text: open(readme, "w").write(nt)
    print(new); return 0

def self_test():
    assert AUTO.match("FUN_00401000") and AUTO.match("thunk_FUN_1") and not AUTO.match("car_update")
    assert bar(5, 10) == "`" + "█" * 10 + "░" * 10 + "` 50%" and bar(0, 0).endswith("0%")
    j = {"functions": {"game": 10, "game_named": 5, "game_bytes": 100}, "published_source": {"by_status": {"MATCH": 2, "NEAR": 1}}, "replaced_in_pipeline": {}}
    assert "| 3 |" in table(j) and "30%" in table(j); print("self-test ok")

if __name__ == "__main__":
    a = sys.argv[1:]
    if a == ["--self-test"]: self_test(); sys.exit(0)
    sys.exit(run("--check" in a, a[a.index("--readme") + 1] if "--readme" in a else os.path.join(G, "README.md")))
