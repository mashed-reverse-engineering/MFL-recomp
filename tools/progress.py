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
    game = [f for f in fn if f[0] < GAME_END]
    named = lambda fs: sum(1 for _, n, _ in fs if not AUTO.match(n))
    man = collections.Counter(); by_mod = collections.Counter()
    for l in open(os.path.join(G, "src", "manifest.txt")):
        p = l.split()
        if not p or p[0] == "#" or p[0].startswith("#") or p[0] == "(none)": continue
        st = p[4] if len(p) > 4 else "MATCH"             # blank status = MATCH (cmpfn --manifest)
        man[st] += 1; by_mod[re.sub(r"(_np)?\.c$", "", p[0])] += 1
    tiers = collections.defaultdict(lambda: collections.Counter())
    p = find("tier_ledger.csv")
    if p:
        for r in csv.DictReader(open(p)):
            t = tiers[r["tier"]]; t["functions"] += 1; t["bytes"] += size.get(int(r["va"], 16), 0)
            t["verified_bytes_identical"] += r["authority"] == "bytes-reloc"
    rep = {t: dict(c) for t, c in sorted(tiers.items())}
    return {"exe_sha1": "d52ca4a5995875b6d052707188e2c6d065f41766",
            "functions": {"total": len(fn), "game": len(game), "named": named(fn), "game_named": named(game),
                          "game_bytes": sum(s for _, _, s in game)},
            "published_source": {"by_status": dict(man), "by_module": dict(sorted(by_mod.items()))},
            "replaced_in_pipeline": rep}

def bar(n, d, w=20):
    k = round(w * n / d) if d else 0
    return "`%s%s` %.0f%%" % ("█" * k, "░" * (w - k), 100 * n / d if d else 0)

def table(j):
    f, m = j["functions"], j["published_source"]["by_status"]
    pub = sum(m.values()); rep = j["replaced_in_pipeline"]; g = f["game"]
    out = ["**%s application functions** (VA < 0x4B0000, the rest is linked library code)\n" % format(g, ","),
           "| Stage | Functions | Progress |", "|---|---:|:---|",
           "| Named | %s | %s |" % (format(f["game_named"], ","), bar(f["game_named"], g))]
    if rep:
        tot = sum(t["functions"] for t in rep.values()); ver = sum(t["verified_bytes_identical"] for t in rep.values())
        rb = sum(t["bytes"] for t in rep.values())
        out += ["| Replaced by recompiled C (all tiers) | %s | %s |" % (format(tot, ","), bar(tot, g)),
                "| ...verified byte identical | %s | %s |" % (format(ver, ","), bar(ver, g)),
                "| ...share of the code bytes | %s B | %s |" % (format(rb, ","), bar(rb, f["game_bytes"]))]
    out.append("| Published in `src/` | %s | %s |" % (format(pub, ","), bar(pub, g)))
    out.append("| ...byte identical (`MATCH`) | %s | %s |" % (format(m.get("MATCH", 0), ","), bar(m.get("MATCH", 0), g)))
    if rep:
        out += ["", "<details><summary>By tier (whole pipeline)</summary>\n", "| Tier | Functions | Code bytes | Verified byte identical |", "|---|---:|---:|---:|"]
        out += ["| %s | %d | %s | %d |" % (k, t["functions"], format(t["bytes"], ","), t["verified_bytes_identical"]) for k, t in rep.items()]
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
