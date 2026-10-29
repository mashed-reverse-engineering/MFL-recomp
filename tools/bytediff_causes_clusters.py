#!/usr/bin/env python3
"""bytediff_causes_clusters.py <out.csv> [--flags "/O2 /Ob0"] [--jobs 1] [--only a,b]: byte-diff census of the enabled clusters (src/clusters/cl_*.c).
Every cluster file is compiled standalone with the build flags of clusters (/O2 /Ob0, the cluster build has no /O2-only run) and every definition whose name is an original
function (entry of the cluster header `// cluster-entries:` or a name in src/symbols.txt; the root's natural C `ROOT_n` is compared with ROOT) is compared with the original bytes the
way tools/bytediff_causes.py does (cmpfn.check, relocations masked, RELCHECK targets).  Roots with register inputs / `ret N` get an entry thunk; their natural C `_n` takes the
inputs on the stack, so they differ in the prologue by construction (category `reginput_root`, not a candidate).
out.csv: cluster, name, va, size, bytes_diff_count, first_diff_offset, category.  Pass --pkl to keep the scan for tools/bytematch_rewrite.py-style variant searches."""
import collections, csv, os, pickle, re, shutil, sys
from concurrent.futures import ThreadPoolExecutor
HERE = os.path.dirname(os.path.abspath(__file__)); G = os.path.join(HERE, ".."); R = os.path.join(G, "src"); CL = os.path.join(R, "clusters")
sys.path.insert(0, HERE)
import cmpfn, bytediff_causes as bc, bytematch_rewrite as rw


def symbols():
    s = {}
    for l in open(os.path.join(R, "symbols.txt")):
        p = l.split()
        if len(p) == 2: s.setdefault(p[0], int(p[1], 16))
    return s


def members(cluster, text, sym):
    """[(function symbol in the file, original name, va)]"""
    ent = set(text.split("\n", 1)[0].split(":", 1)[1].split())
    out = []
    for m in rw.DEF.finditer(text):
        n = m.group(1)
        if re.match(r"^\s*(static|extern)\b", m.group(0)): continue
        o = n[:-2] if n.endswith("_n") and n[:-2] in ent else n
        if o in ent and o in sym: out.append((n, o, sym[o]))
    return out


def main():
    a = sys.argv[1:]
    def opt(k, d):
        if k in a: i = a.index(k); v = a[i + 1]; del a[i:i + 2]; return v
        return d
    flags = opt("--flags", "/O2 /Ob0").split(); jobs = int(opt("--jobs", "1")); only = opt("--only", None); pkl = opt("--pkl", None)
    work = opt("--work", os.path.join(os.environ.get("TMPDIR", "/tmp"), "bytediff_cl"))
    only = set(only.split(",")) if only else None
    out = a[0]
    os.makedirs(work, exist_ok=True)
    for h in os.listdir(R):
        if h.endswith(".h"): shutil.copy(os.path.join(R, h), work)
    sym = symbols()
    ri = {r["name"]: r for r in csv.DictReader(open(os.path.join(G, "output", "reginputs.csv")))}
    en = [l.strip() for l in open(os.path.join(CL, "enabled.txt")) if l.strip() and not l.startswith("#")]
    en = [c for c in en if only is None or c in only]
    def run(c):
        f = os.path.join(CL, "cl_%s.c" % c); t = open(f, errors="surrogateescape").read()
        mem = members(c, t, sym); w = os.path.join(work, "cl_%s.c" % c); shutil.copy(f, w)
        if not mem: return c, [], {}
        try: r = cmpfn.check(w, [(n, va) for n, o, va in mem], flags); last = {n: cmpfn.LAST.get(n) for n, _, _ in mem}
        except Exception as e: r = {n: (False, "compile error EXC %s" % e) for n, _, _ in mem}; last = {}
        return c, mem, {n: (r[n][0], r[n][1], last.get(n)) for n, _, _ in mem}
    rows = []; res = {}
    with ThreadPoolExecutor(jobs) as ex:
        for c, mem, r in ex.map(run, en):
            res[c] = (mem, r)
            for n, o, va in mem:
                ok, det, last = r[n]
                reg = o in ri and (ri[o]["reg_inputs"] or int(ri[o]["ret_n"])) and n.endswith("_n")
                if ok: row = (len(last[0]) if last else 0, 0, "", "MATCH")
                elif reg: row = (len(last[0]) if last else 0, "", "", "reginput_root")
                elif last is None: row = (0, "", "", "compile_error")
                elif det.startswith("RELOC"): row = (len(last[0]), "", "", "reloc_target")
                else: row = bc.classify(last[0], last[1], va)
                rows.append((c, o, "%x" % va) + row)
    if pkl: pickle.dump(res, open(pkl, "wb"))
    w = csv.writer(open(out, "w"), lineterminator="\n"); w.writerow("cluster name va size bytes_diff_count first_diff_offset category".split()); w.writerows(rows)
    cnt = collections.Counter(r[6] for r in rows)
    print("%-30s %5s %s" % ("category", "count", "with<=8B diff"))
    for k, v in cnt.most_common(): print("%-30s %5d %d" % (k, v, sum(1 for r in rows if r[6] == k and r[4] not in ("", 0) and r[4] <= 8)))
    print("total", len(rows), "in", len(en), "clusters")


if __name__ == "__main__":
    main()
