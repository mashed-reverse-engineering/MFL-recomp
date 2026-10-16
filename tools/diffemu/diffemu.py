#!/usr/bin/env python3
"""diffemu.py --cluster ROOT... [--clusters SPEC]   cluster mode (tools/diffemu/cluster.py), results in output/cluster_pilot/results.csv
diffemu.py <name ...> | --all-tier2 | --candidates | --names FILE | --src FILE.c   [--cases N] [--seeds K] [--jobs 3] [--budget SEC] [--force] [--verbose]
Differential emulation: our compiled function (src/automatch_near chunk, output/tier2_candidates, tier 1 / agent sources) against the ORIGINAL bytes of MFL.exe, both in Unicorn
with identical seeded memory and stubbed callees (README "diffemu").  Verdict per function: EQUAL | DIFF | INCONCLUSIVE; output/diffemu_results.csv, cache output/diffemu_cache.csv.
Exit code 1 when a DIFF is found."""
import random
import os, sys
try: import unicorn
except ImportError:      # the packages live in a venv: python3 -m venv ~/.cache/mfl_diffemu_venv; ~/.cache/mfl_diffemu_venv/bin/pip install unicorn==2.1.4 capstone pefile
    py = os.environ.get("DIFFEMU_PY") or os.path.expanduser("~/.cache/mfl_diffemu_venv/bin/python"); os.execv(py, [py] + sys.argv)
import csv, shutil, subprocess, tempfile, time
from collections import Counter
from concurrent.futures import ThreadPoolExecutor
from multiprocessing import Pool
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import core
from core import ROOT, T, sha

OUT = os.path.join(ROOT, "output")
DOUT = os.environ.get("DIFFEMU_OUT") or OUT      # DIFFEMU_OUT=<dir>: results / cache / diffs of this process go there (tools/t2_auto.py runs); the object cache (~/.cache, content addressed) stays shared
os.makedirs(DOUT, exist_ok=True)
RESULTS, CACHE, DIFFDIR = (os.path.join(DOUT, f) for f in ("diffemu_results.csv", "diffemu_cache.csv", "diffemu_diffs"))
OBJCACHE = os.environ.get("DIFFEMU_OBJ", os.path.expanduser("~/.cache/mfl_diffemu_obj"))
FLAGS = ["/nologo", "/c", "/Gy", "/O2", "/Ob0"]      # as build_overlay.py plus /Ob0: a callee defined in the same chunk is called (hooked at its original address like every callee), not inlined; the original calls it, and the call trace must be comparable
COLS = ["name", "va", "verdict", "cases", "coverage_orig", "coverage_ours", "note"]
TOOLHASH = sha(*[open(os.path.join(core.HERE, f), "rb").read() for f in ("core.py", "diffemu.py")], open(os.path.join(core.TOOLS, "cmpfn.py"), "rb").read())
MIN_CASES = 20


# ---------------------------------------------------------------------------------------------------------- sources
def index():
    """function name -> (C file, kind): tier 1 chunks < agent files < tier 2 candidates < installed tier 2 (later wins)"""
    tb = T(); idx = {}
    for kind, d, pre in (("tier1", "src/tier1", ""), ("tier1", "src/automatch", "am_"), ("agent", "src/automatch_agent", "ag_"), ("candidate", "output/tier2_candidates", "an_"), ("tier2", "src/automatch_near", "an_")):
        d = os.path.join(ROOT, d)
        for f in sorted(os.listdir(d)):
            if f.startswith(pre) and f.endswith(".c") and f != "an_shadow_rt.c":
                for n in core.defs(os.path.join(d, f)):
                    if n in tb.sym: idx[n] = (os.path.join(d, f), kind)
    return idx

def compile_obj(src):
    """object of a whole C file (headers next to it), cached by source + headers + flags (like build_overlay.py)"""
    d = os.path.dirname(src); h = sha(*[open(os.path.join(d, x), "rb").read() for x in sorted(os.listdir(d)) if x.endswith(".h")])
    key = sha(open(src, "rb").read(), h, " ".join(FLAGS)); obj = os.path.join(OBJCACHE, key + ".obj")
    if os.path.exists(obj): return obj, key
    os.makedirs(OBJCACHE, exist_ok=True); os.makedirs(core.cmpfn.WORKROOT, exist_ok=True)
    w = tempfile.mkdtemp(prefix="dx_", dir=core.cmpfn.WORKROOT)
    try:
        for x in os.listdir(d):
            if x.endswith(".h"): shutil.copy(os.path.join(d, x), w)
        shutil.copy(src, os.path.join(w, "f.c"))
        r = subprocess.run([core.cmpfn.VC7, w, "cl.exe"] + FLAGS + ["f.c"], capture_output=True, text=True, timeout=300)
        if not os.path.exists(os.path.join(w, "f.obj")): raise RuntimeError("compile error: " + (r.stdout + r.stderr)[-300:])
        shutil.copy(os.path.join(w, "f.obj"), obj + ".tmp"); os.replace(obj + ".tmp", obj)      # atomic
    finally: shutil.rmtree(w, ignore_errors=True)
    return obj, key


# ---------------------------------------------------------------------------------------------------------- one function
def make_fn(name, src, obj):
    """-> (Fn, None) or (None, reason)"""
    tb = T(); d, m = core.defs(src)[name]
    sig, why = core.parse_sig(m, core.typedefs(os.path.dirname(src)))
    if sig is None: return None, why
    va = min(tb.sym[name])
    try: return core.Fn(name, va, obj, sig), None
    except core.Unresolved as e: return None, "unresolved relocation: %s" % e
    except KeyError as e: return None, str(e)

def evaluate(fn, cases, seeds, budget=90, first_diff_stop=True):
    """run the cases of fn; -> dict(verdict, cases, skipped Counter, diff (case, descriptions), coverage)"""
    t0 = time.time(); ok = 0; skip = Counter(); diff = None
    tg = core.cmp_targets(fn); core.RETWANT.clear(); core.RETWANT.update(core.ret_wants(fn))
    for c in core.gen_cases(fn.sig, cases, seeds):
        if tg and c.seed % 3 == 1 and not c.patch:      # compare-guided input: some globals the original tests against immediates take those values (both sides)
            r_ = random.Random(c.seed ^ 0x9e3779b9); c.patch = {a_: v_ for a_, v_ in r_.sample(tg, min(len(tg), r_.randint(1, 3)))}
        to = core.cmp_offsets(fn)
        if to and c.wins and c.seed % 3 != 0:      # structure fields behind pointer arguments the original compares with an immediate (`cmp [esi+0x18], 2`): written into the windows (both sides)
            r_ = random.Random(c.seed ^ 0x7f4a7c15); ws = [[a_, bytearray(b_)] for a_, b_ in c.wins]
            for d_, v_, sz_ in r_.sample(to, min(len(to), r_.randint(1, 3))):
                w_ = r_.choice(ws)
                if d_ + sz_ <= len(w_[1]): w_[1][d_:d_ + sz_] = v_.to_bytes(sz_, "little")
            c.wins = [(a_, bytes(b_)) for a_, b_ in ws]
        if time.time() - t0 > budget: skip["TIME_BUDGET"] += 1; break
        st, x = core.run_case(fn, c)
        if st == "equal": ok += 1
        elif st == "skip": skip[x] += 1
        else:
            diff = (c, x); break
    cov = lambda hit, lead: len(hit) / len(lead) if lead else 0.0
    v = "DIFF" if diff else "EQUAL" if ok >= MIN_CASES else "INCONCLUSIVE"
    return dict(verdict=v, cases=ok, skip=skip, diff=diff, cov_o=cov(fn.cov_o, fn.leaders_o), cov_u=cov(fn.cov_u, fn.leaders_u), secs=time.time() - t0)

def row_of(name, va, r, fn=None, extra=""):
    notes = list(fn.notes) if fn else []
    if r["diff"]:
        c, d = r["diff"]; notes.append("first diff %s: %s" % (c.describe()[:120], " | ".join(d)[:260]))
    if fn and getattr(fn, "patched", 0): notes.append("%d cases with loop-bound patch (large dwords the original reads set to 0/1/2/4 on both sides)" % fn.patched)
    if r["skip"]: notes.append("skipped %d: %s" % (sum(r["skip"].values()), ", ".join("%s %d" % kv for kv in r["skip"].most_common(4))))
    if r["verdict"] == "EQUAL" and r["cov_o"] < 0.6: notes.append("LOW COVERAGE of the original")
    if extra: notes.insert(0, extra)
    return dict(name=name, va="%08x" % va, verdict=r["verdict"], cases=r["cases"], coverage_orig="%.2f" % r["cov_o"], coverage_ours="%.2f" % r["cov_u"], note="; ".join(notes))

def job(a):
    name, src, obj, cases, seeds, budget = a
    va = min(T().sym[name]); t0 = time.time()
    try:
        fn, why = make_fn(name, src, obj)
        if fn is None: return dict(name=name, va="%08x" % va, verdict="INCONCLUSIVE", cases=0, coverage_orig="0.00", coverage_ours="0.00", note=why)
        r = evaluate(fn, cases, seeds, budget)
        row = row_of(name, va, r, fn)
        if r["diff"]:
            os.makedirs(DIFFDIR, exist_ok=True)
            open(os.path.join(DIFFDIR, name + ".txt"), "w").write("%s %08x\nfirst diverging case: %s\n%s\n" % (name, va, r["diff"][0].describe(), "\n".join(r["diff"][1])))
        elif os.path.exists(os.path.join(DIFFDIR, name + ".txt")): os.remove(os.path.join(DIFFDIR, name + ".txt"))      # no stale diff text of an earlier verdict
        row["secs"] = "%.1f" % (time.time() - t0); return row
    except Exception as e:
        import traceback; return dict(name=name, va="%08x" % va, verdict="INCONCLUSIVE", cases=0, coverage_orig="0.00", coverage_ours="0.00", note="harness error: %s %s" % (type(e).__name__, str(e)[:150]))


# ---------------------------------------------------------------------------------------------------------- cache / driver
def load_cache():
    if not os.path.exists(CACHE): return {}
    return {r["name"]: r for r in csv.DictReader(open(CACHE))}

def save(cache):
    rows = sorted(cache.values(), key=lambda r: r["va"])
    for path, cols in ((CACHE, ["key"] + COLS), (RESULTS, COLS)):
        with open(path + ".tmp", "w", newline="") as f:
            w = csv.DictWriter(f, cols, extrasaction="ignore"); w.writeheader(); w.writerows(rows)
        os.replace(path + ".tmp", path)

def cluster_main(a):
    """--cluster ROOT [--clusters SPEC] [--cases N] [--seeds K]: cluster mode (cluster.py), verdict per entry point, output/cluster_pilot/results.csv"""
    import cluster
    def opt(n, d, t=int):
        if n in a: i = a.index(n); v = t(a[i + 1]); del a[i:i + 2]; return v
        return d
    cases, seeds, budget, specf = opt("--cases", 100), opt("--seeds", 2), opt("--budget", 90), opt("--clusters", cluster.SPEC, str)
    roots = [x for x in a if not x.startswith("--") and x != "cluster"]; a.clear(); spec = cluster.load_spec(specf); rows = []; bad = 0
    for root in (roots or list(spec)):
        sp = spec[root]; obj, _ = compile_obj(sp["file"]); td = core.typedefs(os.path.dirname(sp["file"])); dfs = core.defs(sp["file"])
        cl = cluster.Cluster(sp, obj, dfs); t0 = time.time(); res = []
        for n in sp["entries"]:
            sig, why = core.parse_sig(dfs[n][1], td)
            if sig is None: res.append((n, dict(verdict="INCONCLUSIVE", cases=0, cov_o=0, cov_u=0, skip=Counter(), diff=None), why)); continue
            fn = cl.entry(n, sig); r = evaluate(fn, cases, seeds, budget); res.append((n, r, ""))
            if r["diff"]:
                os.makedirs(os.path.join(os.path.dirname(sp["file"]), "diffs"), exist_ok=True)
                open(os.path.join(os.path.dirname(sp["file"]), "diffs", n + ".txt"), "w").write("%s (cluster %s)\nfirst diverging case: %s\n%s\n" % (n, root, r["diff"][0].describe(), "\n".join(r["diff"][1])))
            else:
                old = os.path.join(os.path.dirname(sp["file"]), "diffs", n + ".txt")
                if os.path.exists(old): os.remove(old)      # no stale diff text of an earlier verdict
        for n, r, why in res:
            row = row_of(n, cl.vas[n], r, None, why); row["cluster"] = root; rows.append(row); bad += r["verdict"] == "DIFF"
            print("%-12s %-12s %-34s cases %3s cov %s/%s %s" % (root, r["verdict"], n, r["cases"], row["coverage_orig"], row["coverage_ours"], row["note"][:200] if r["verdict"] != "EQUAL" else ""))
        print("cluster %s: %s  (%.0fs)" % (root, ", ".join("%s %s" % (n, r["verdict"]) for n, r, _ in res), time.time() - t0)); sys.stdout.flush()
    out = os.path.join(os.path.dirname(specf), "results.csv")
    with open(out, "w", newline="") as f:
        w = csv.DictWriter(f, ["cluster"] + COLS, extrasaction="ignore"); w.writeheader(); w.writerows(rows)
    return 1 if bad else 0

def main():
    a = sys.argv[1:]
    if "--cluster" in a: a.remove("--cluster"); return cluster_main(a)
    def opt(n, d, t=int):
        if n in a: i = a.index(n); v = t(a[i + 1]); del a[i:i + 2]; return v
        return d
    cases, seeds, jobs, budget = opt("--cases", 100), opt("--seeds", 2), opt("--jobs", 3), opt("--budget", 90)
    names_file, src_file = opt("--names", None, str), opt("--src", None, str)
    force = "--force" in a; verbose = "--verbose" in a; allt2 = "--all-tier2" in a; cand = "--candidates" in a
    names = [x for x in a if not x.startswith("--")]
    idx = index()
    if allt2: names += [l.split()[1] for l in open(os.path.join(ROOT, "src", "automatch_near_names.txt")) if len(l.split()) >= 2]
    if cand: names += [n for n, (p, k) in idx.items() if k == "candidate"]
    if names_file: names += open(names_file).read().split()
    if src_file: names += list(core.defs(src_file).keys()); idx.update({n: (os.path.abspath(src_file), "file") for n in core.defs(src_file) if n in T().sym})
    names = list(dict.fromkeys(names))
    if not names: print(__doc__); return 2
    unk = [n for n in names if n not in idx]
    if unk: print("no source found for: " + " ".join(unk)); names = [n for n in names if n in idx]
    cache = load_cache(); todo = []; t0 = time.time(); srcs = {}
    for n in names: srcs.setdefault(idx[n][0], None)
    with ThreadPoolExecutor(3) as ex:      # one compile per chunk (the object cache makes reruns free)
        futs = {s: ex.submit(compile_obj, s) for s in srcs}
        for s, f in futs.items():
            try: srcs[s] = f.result()
            except Exception as e: srcs[s] = e
    res = {}
    for n in names:
        o = srcs[idx[n][0]]
        if isinstance(o, Exception):
            res[n] = dict(name=n, va="%08x" % min(T().sym[n]), verdict="INCONCLUSIVE", cases=0, coverage_orig="0.00", coverage_ours="0.00", note=str(o)[:200]); continue
        key = sha(o[1], n, cases, seeds, TOOLHASH, T().hash)
        if not force and n in cache and cache[n]["key"] == key: res[n] = cache[n]; res[n]["cached"] = 1
        else: todo.append((n, idx[n][0], o[0], cases, seeds, budget)); cache.pop(n, None); res.setdefault(n, None); keys = None
    keyof = {n: sha(srcs[idx[n][0]][1], n, cases, seeds, TOOLHASH, T().hash) for n in names if not isinstance(srcs[idx[n][0]], Exception)}
    print("%d functions, %d to run (%d cached), compile %.0fs" % (len(names), len(todo), len(names) - len(todo), time.time() - t0)); sys.stdout.flush()
    with Pool(min(jobs, max(1, len(todo)))) as p:
        for row in p.imap_unordered(job, todo):
            res[row["name"]] = row
            if row["name"] in keyof: row["key"] = keyof[row["name"]]; cache[row["name"]] = row; save(cache)
            print("%-9s %-40s cases %3s cov %s/%s %s%s" % (row["verdict"], row["name"], row["cases"], row["coverage_orig"], row["coverage_ours"], row["note"][:160] if row["verdict"] != "EQUAL" else "", "" if row["verdict"] != "DIFF" or not verbose else "\n" + open(os.path.join(DIFFDIR, row["name"] + ".txt")).read()))
            sys.stdout.flush()
    for n in names:
        if res.get(n) and n not in cache and n in keyof: cache[n] = dict(res[n], key=keyof[n])
    save(cache)
    c = Counter(res[n]["verdict"] for n in names if res.get(n))
    print("summary: " + ", ".join("%s %d" % kv for kv in sorted(c.items())) + "  (%.0fs wall)" % (time.time() - t0))
    return 1 if c.get("DIFF") else 0


if __name__ == "__main__":
    sys.exit(main())
