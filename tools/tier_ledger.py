#!/usr/bin/env python3
"""tier_ledger.py: one line per replaced function with the evidence behind it (idea from REA's obligation ledgers: a claim closes only with a verifier of comparable authority).
output/tier_ledger.csv: name, va, tier (hand / bridge / tier1 / agent / tier2), evidence list.
Evidence tokens: bytes (byte identical, relocation checked by overlay.py), manifest-<status>, shadow-OK / shadow-DIFF / shadow-CALLED / shadow-never (output/shadow_t2.txt, tier 2 only),
authority (bytes-reloc > diffemu > shadow > static > none) is the strongest evidence that is still valid; src_hash = sha1 of the definition, hdr_hash = ghidra_types.h copies + output/decls.h, harness_hash = diffemu tool files;
evidence recorded for another src_hash (output/ledger_state.csv) is STALE and the row drops to static; FAIL / DIFF / INCONCLUSIVE / unknown never count.  validated (tier 2) = diffemu EQUAL >= 20 cases, both coverages >= 0.8, shadow OK; or shadow CALLED + a passed infra/milestone gate.
--retag writes `/* TIER2: ... */` above each tier 2 function (a drift warning is printed otherwise); tools/status.sh summarises the csv.
sweep (all tiers run through the 15 course union sweep as a set; per function execution counts come from the coverage runs)."""
import csv, datetime, glob, hashlib, os, re, subprocess, sys
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__))); import shadow_cache
HERE = os.path.dirname(os.path.abspath(__file__)); R = os.path.join(HERE, "..", "src")
sym = {}
for l in open(os.path.join(R, "symbols.txt")):
    p = l.split()
    if len(p) == 2: sym.setdefault(p[0], p[1])
rows = {}
def add(n, tier, ev):
    rows[n] = (tier, ev)
mstat = {}      # cmpfn.py --manifest (cached): MATCH / EFFECTIVE / NEAR per function
try:
    out = subprocess.run([sys.executable, os.path.join(HERE, "cmpfn.py"), "--manifest", os.path.join(R, "manifest.txt")], capture_output=True, text=True, timeout=900).stdout
    for l in out.splitlines():
        q = l.split()
        if len(q) >= 2 and q[0] in ("MATCH", "EFFECTIVE", "NEAR"): mstat[q[1]] = q[0]
except Exception as e: print("manifest status unavailable:", e)
for l in open(os.path.join(R, "manifest.txt")):
    p = l.split()
    if len(p) > 3 and p[0] != "#" and p[0] != "(none)":
        add(p[1], "hand", [{"MATCH": "bytes", "EFFECTIVE": "registers-only", "NEAR": "near-regress-only"}.get(mstat.get(p[1]), "manifest-unchecked")])
for n in open(os.path.join(R, "bridges", "enabled.txt")):
    if n.strip(): add(n.strip(), "bridge", ["enabled-list (game + shadow test results not recorded per function)"])
def defs(path):
    out = []
    for f in glob.glob(path):
        out += re.findall(r"^[A-Za-z_][\w \*]*?\b(\w+)\([^;{]*\)\s*(?:/\*.*?\*/)?\s*\n\s*\{", open(f, errors="replace").read(), re.M)
    return out
for n in defs(os.path.join(R, "automatch", "*.c")) + defs(os.path.join(R, "tier1", "*.c")): rows.setdefault(n, ("tier1", ["bytes"]))
av = {}
ap = os.path.join(HERE, "..", "output", "agent_verify.txt")
if os.path.exists(ap):
    for l in open(ap):
        q = l.split()
        if len(q) == 3: av[q[0]] = (q[1], q[2])
for n in defs(os.path.join(R, "automatch_agent", "*.c")):
    if n in rows and rows[n][0] != "agent": rows[n + "@agent-duplicate"] = ("agent", ["DUPLICATE of " + rows[n][0]]); continue
    r_, h_ = av.get(n, ("unverified", ""))
    rows[n] = ("agent", ["bytes-standalone" if r_ == "MATCH" else "NOT-VERIFIED:" + r_, h_])
OUT = os.path.join(HERE, "..", "output"); NEAR = os.path.join(R, "automatch_near")
def sha(*b): return hashlib.sha1(b"".join(b)).hexdigest()[:12]
def rd(p): return open(p, "rb").read() if os.path.exists(p) else b""
hdr_hash = sha(*[rd(f) for f in sorted(glob.glob(os.path.join(R, "*", "ghidra_types.h")))], rd(os.path.join(OUT, "decls.h")))
harness_hash = sha(*[rd(os.path.join(HERE, "diffemu", f)) for f in ("core.py", "diffemu.py")]) if os.path.exists(os.path.join(HERE, "diffemu", "diffemu.py")) else "none"
srch = {}
for d in ("automatch", "tier1", "automatch_agent", "automatch_near"): srch.update(shadow_cache.hashes(os.path.join(R, d)))
# ---- recorded evidence: a result only counts for the source it was produced from
statep = os.path.join(OUT, "ledger_state.csv"); state = {}
if os.path.exists(statep):
    for r_ in csv.reader(open(statep)):
        if len(r_) == 4: state[(r_[0], r_[1])] = r_[2:]
nearsrc = {}      # name -> mtime of the file defining it
for f in glob.glob(os.path.join(NEAR, "*.c")):
    for n_ in set(re.findall(r"^[A-Za-z_][\w \*]*?\b(\w+)\(", open(f, errors="replace").read(), re.M)): nearsrc[n_] = max(nearsrc.get(n_, 0), os.path.getmtime(f))
def fresh(n, kind, digest, resfile):
    """True when the recorded result belongs to the current source of n; records it the first time / after a re-run (changed digest) when the source is not newer than the result file"""
    cur = srch.get(n, "-"); rec = state.get((n, kind))
    if rec and rec[0] == cur: state[(n, kind)] = [cur, digest]; return True
    if (not rec or rec[1] != digest) and nearsrc.get(n, 0) <= os.path.getmtime(resfile): state[(n, kind)] = [cur, digest]; return True
    return False
dfe = {}; dres = os.path.join(OUT, "diffemu_results.csv"); dkeys = {}
if os.path.exists(dres):
    for r_ in csv.DictReader(open(dres)): dfe[r_["name"]] = r_
    dc = os.path.join(OUT, "diffemu_cache.csv")
    if os.path.exists(dc):
        for r_ in csv.DictReader(open(dc)): dkeys[r_["name"]] = r_["key"]
shadow = {}; sp = os.path.join(OUT, "shadow_t2.txt")
if os.path.exists(sp):
    for l in open(sp):
        p = l.split()
        if len(p) > 1: shadow[p[0]] = p[1]
scache = shadow_cache.load()      # name -> (src hash, result); carries its own hash
sweep_clean = any("PASSED" in open(f, errors="replace").read() for f in glob.glob(os.path.join(OUT, "gate", "*_infra", "gate.log")) + glob.glob(os.path.join(OUT, "gate", "*_milestone", "gate.log")))
auth = {}      # name -> (authority, cases, cov, stale, validated)
for l in open(os.path.join(R, "automatch_near_names.txt")):
    p_ = l.split(); n = p_[1] if len(p_) >= 2 else l.strip()      # automatch_near_names.txt: va name chunk
    if not n or n in rows: continue
    ev = ["compiled"]; a = "static"; cases = 0; cov = 0.0; stale = 0; dok = sok = scalled = False
    d = dfe.get(n)
    if d:
        good = d["verdict"] == "EQUAL"
        if fresh(n, "diffemu", dkeys.get(n, d["verdict"] + d["cases"]), dres):
            ev.append("diffemu-" + d["verdict"]); cases = int(d["cases"] or 0); cov = min(float(d["coverage_orig"] or 0), float(d["coverage_ours"] or 0))
            if good and cases >= 20: a = "diffemu"; dok = cov >= 0.8
        elif good or d["verdict"] == "DIFF": stale = 1; ev.append("diffemu-STALE")
    sc = scache.get(n)
    if sc: sres = sc[1]; sfresh = sc[0] == srch.get(n)
    elif shadow.get(n, "UNTESTED") != "UNTESTED": sres = shadow[n]; sfresh = fresh(n, "shadow", sres, sp)
    else: sres = None; sfresh = False
    if sres:
        if sfresh:
            ev.append("shadow-" + sres); sok = sres == "OK"; scalled = sres == "CALLED"
            if a == "static" and sres in ("OK", "CALLED"): a = "shadow"
        else: stale = 1; ev.append("shadow-STALE")
    else: ev.append("shadow-never")
    rows[n] = ("tier2", ev); auth[n] = (a, cases, cov, stale, (a == "diffemu" and dok and sok) or (scalled and sweep_clean))
with open(statep, "w") as o:
    for (n, k), v in sorted(state.items()): o.write("%s,%s,%s,%s\n" % (n, k, v[0], v[1]))
def authority(n, t, ev):
    if n in auth: return auth[n]
    return ("bytes-reloc" if any(e in ("bytes", "bytes-standalone") for e in ev) else "none" if t in ("bridge", "agent") else "static", 0, 0.0, 0, False)
TAG = re.compile(r"^/\* TIER2: (\w+) (\S+) authority=(\S+) \*/$")
def retag(write):
    """tier 2 functions get `/* TIER2: unvalidated|validated YYYY-MM-DD authority=... */` above the signature (date kept while status + authority are unchanged); -> number of drifted tags"""
    drift = 0; today = datetime.date.today().isoformat()
    for f in sorted(glob.glob(os.path.join(NEAR, "an_*.c"))):
        L = open(f, errors="surrogateescape").read().split("\n"); out = []; ch = False
        for i, l in enumerate(L):
            m = shadow_cache.DEF.match(l)
            if m and i + 1 < len(L) and L[i + 1].startswith("{") and m.group(2) in auth:
                a = auth[m.group(2)]; st = "validated" if a[4] else "unvalidated"
                old = TAG.match(out[-1]) if out else None
                if not (old and old.group(1) == st and old.group(3) == a[0]):
                    drift += 1; ch = True; tag = "/* TIER2: %s %s authority=%s */" % (st, today, a[0])
                    if old: out[-1] = tag
                    else: out.append(tag)
            out.append(l)
        if ch and write: open(f, "w", errors="surrogateescape").write("\n".join(out))
    return drift
if "--retag" in sys.argv: print("retag: %d tag(s) written" % retag(True))
else:
    d = retag(False)
    if d: print("WARNING tier 2 source tags drifted: %d (python3 tools/tier_ledger.py --retag)" % d)
with open(os.path.join(OUT, "tier_ledger.csv"), "w") as o:
    o.write("name,va,tier,evidence,authority,src_hash,hdr_hash,harness_hash,cases,cov,stale,validated\n")
    for n, (t, ev) in sorted(rows.items()):
        a, c, cv, st, v = authority(n, t, ev)
        o.write("%s,%s,%s,%s,%s,%s,%s,%s,%d,%.2f,%d,%d\n" % (n, sym.get(n, "?"), t, "+".join(ev), a, srch.get(n.split("@")[0], "-"), hdr_hash, harness_hash, c, cv, st, int(v)))
import collections
print(dict(collections.Counter(t for t, _ in rows.values())), "->", "output/tier_ledger.csv")
