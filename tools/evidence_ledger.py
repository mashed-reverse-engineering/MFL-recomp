#!/usr/bin/env python3
"""evidence_ledger.py: output/behaviour_evidence.csv = one row per replaced application function (VA < 0x4B0000) that is NOT byte identical, with the diffemu evidence behind it.
Sources, highest precedence first: cluster_sweep (clsweep ladder), weak_evidence tweak (hand spec), hs_sweep (HS seeds, ran with STDCALL_IND=1), weak_evidence osweep, tier_ledger (tier 2 diffemu),
cluster_pilot*/results.csv (cluster install runs, newest file wins).  verified = EQUAL, >= 20 cases, both coverages >= 0.8 (VERIFICATION.md).
The primary observation is the highest source that qualifies, else the highest source with any result.  A DIFF from the hs-seeded tools is not outvoted by a plain ledger/pilot EQUAL (flag CONFLICT, not verified).
flags: LOWCOV DIFF INCONCLUSIVE NOEVID CONFLICT STDIND (verified only by hs_sweep, STDCALL_IND=1 mis-pops batched cdecl cleanup: re-run without) LADDER (cluster_sweep via != hs) OVERRIDDEN_DIFF (output/ladder_overridden_diffs.csv)
usage: evidence_ledger.py [--check]   (progress.py imports evidence())"""
import csv, glob, os, re, sys
G = os.path.normpath(os.path.join(os.path.dirname(os.path.abspath(__file__)), ".."))
GAME_END = 0x4B0000
def find(n):
    for d in ("output", "data"):
        p = os.path.join(G, d, n)
        if os.path.exists(p): return p
def covs(s):
    try: return [float(x) for x in str(s).split("/") if x not in ("", "-")] or [0.0]
    except ValueError: return [0.0]
def obs(src, verdict, cases, cov, note=""):
    try: c = int(float(cases))
    except (ValueError, TypeError): c = 0
    return dict(src=src, verdict=(verdict or "").strip().upper(), cases=c, cov=min(covs(cov)), note=note)
def qual(o): return o["verdict"] == "EQUAL" and o["cases"] >= 20 and o["cov"] >= 0.8
PREC = ["cluster_sweep", "weak_tweak", "hs_sweep", "weak_osweep", "ledger", "pilot"]
def evidence():
    fn = [l.split() for l in open(find("fnsizes.txt")) if len(l.split()) >= 3]
    va_of = {n: "%08x" % int(v, 16) for v, n, _ in fn}; name_of = {v: n for n, v in va_of.items()}
    inr = lambda v: int(v, 16) < GAME_END
    O = {}                                                    # va -> [obs]
    def add(name, va, o):
        v = (va or "").lower().zfill(8) if va and re.fullmatch(r"[0-9a-fA-F]{1,8}", va) else va_of.get(name)
        if v: O.setdefault(v, []).append(o)
    led = {}
    for r in csv.DictReader(open(find("tier_ledger.csv"))):
        v = r["va"].lower() if re.fullmatch(r"[0-9a-fA-F]{8}", r["va"]) else va_of.get(r["name"])
        if not v or not inr(v): continue
        led[v] = r
        m = re.search(r"diffemu-(EQUAL|DIFF|INCONCLUSIVE|STALE)", r["evidence"])
        if m and r["authority"] != "bytes-reloc": add(r["name"], v, obs("ledger", m.group(1), r["cases"], r["cov"]))
    for p in sorted(glob.glob(os.path.join(G, "output", "cluster_pilot*", "results.csv")), key=os.path.getmtime):   # newest last
        for r in csv.DictReader(open(p)):
            if r.get("verdict"): add(r["name"], r.get("va"), obs("pilot", r["verdict"], r["cases"], "%s/%s" % (r["coverage_orig"], r["coverage_ours"])))
    q = find("hs_sweep_2026-10-29.csv")
    if q:
        for r in csv.DictReader(open(q)): add(r["name"], None, obs("hs_sweep", r["verdict"], r["cases"], r["coverage"], r["note"]))
    q = find("weak_evidence.csv")
    if q:
        for r in csv.DictReader(open(q)):
            if r["tweak_verdict"]: add(r["name"], r["va"], obs("weak_tweak", r["tweak_verdict"], r["tweak_cases"], "%s/%s" % (r["tweak_cov_orig"], r["tweak_cov_ours"]), r["tweak_result"]))
            if r["osweep_verdict"]: add(r["name"], r["va"], obs("weak_osweep", r["osweep_verdict"], r["osweep_cases"], "%s/%s" % (r["osweep_cov_orig"], r["osweep_cov_ours"]), r["osweep_result"]))
    via = {}
    q = find("cluster_sweep_2026-10-29.csv")
    if q:
        for r in csv.DictReader(open(q)):
            via[r["root"]] = r["via"]; add(r["root"], None, obs("cluster_sweep", r["verdict"], r["cases"], r["coverage"], "via " + r["via"]))
    ovr = set()
    q = find("ladder_overridden_diffs.csv")
    if q: ovr = {r["root"] for r in csv.DictReader(open(q))}
    # population: ledger rows that are not byte identical + enabled cluster entries
    pop = {}
    for v, r in led.items():
        if r["authority"] != "bytes-reloc": pop[v] = (r["name"], r["tier"])
    for en in (os.path.join(G, "src", "clusters", "enabled.txt"), os.path.join(G, "src", "clusters", "enabled.txt")):
        if not os.path.exists(en): continue
        for n in (l.strip() for l in open(en) if l.strip() and not l.startswith("#")):
            cf = next((os.path.join(x, "cl_%s.c" % n) for x in (os.path.join(G, "src", "clusters"), os.path.join(G, "src", "clusters")) if os.path.exists(os.path.join(x, "cl_%s.c" % n))), None)
            m = re.search(r"^// cluster-entries: (.*)$", open(cf, errors="replace").read(), re.M) if cf else None
            for e in (m.group(1).replace(",", " ").split() if m else [n]):
                v = va_of.get(e)
                if v and inr(v) and not (v in led and led[v]["authority"] == "bytes-reloc"): pop.setdefault(v, (e, "cluster"))
        break
    rows = []
    for v, (name, tier) in sorted(pop.items()):
        os_ = sorted(O.get(v, []), key=lambda o: PREC.index(o["src"]) if o["src"] != "pilot" else 99)
        fl = []
        good = [o for o in os_ if qual(o)]
        prim = good[0] if good else (os_[0] if os_ else None)
        if prim is None: fl.append("NOEVID"); rows.append([name, v, tier, "-", "-", 0, 0, 0, ";".join(fl), "", led.get(v, {}).get("evidence", "")]); continue
        ok = qual(prim)
        hsd = [o for o in os_ if o["verdict"] == "DIFF" and o["src"] in ("hs_sweep", "weak_tweak", "cluster_sweep")]
        if ok and hsd and prim["src"] not in ("weak_tweak", "cluster_sweep"): ok = False; fl.append("CONFLICT")
        if prim["verdict"] in ("DIFF", "INCONCLUSIVE", "STALE"): fl.append(prim["verdict"])
        elif prim["verdict"] == "EQUAL" and not qual(prim): fl.append("LOWCOV" if prim["cov"] < 0.8 else "FEWCASES")
        if ok and prim["src"] == "hs_sweep" and not [o for o in good if o["src"] != "hs_sweep"]: fl.append("STDIND")
        if prim["src"] == "cluster_sweep" and via.get(name, "hs") != "hs": fl.append("LADDER")
        if name in ovr: fl.append("OVERRIDDEN_DIFF")
        rows.append([name, v, tier, prim["src"], prim["verdict"], prim["cases"], "%.2f" % prim["cov"], int(ok), ";".join(fl), "|".join("%s:%s" % (o["src"], o["verdict"]) for o in os_), led.get(v, {}).get("evidence", "")])
    return rows
HDR = ["name", "va", "tier", "source", "verdict", "cases", "cov_min", "verified", "flags", "all_sources", "ledger_evidence"]
if __name__ == "__main__":
    rows = evidence(); out = os.path.join(G, "output", "behaviour_evidence.csv")
    if "--check" not in sys.argv:
        w = csv.writer(open(out, "w", newline="")); w.writerow(HDR); w.writerows(rows)
    import collections
    c = collections.Counter(f for r in rows for f in r[8].split(";") if f)
    print(len(rows), "rows, verified", sum(r[7] for r in rows), dict(c))
