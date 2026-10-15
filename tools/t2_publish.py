#!/usr/bin/env python3
"""t2_publish.py DIR (= t2_auto.py --publish DIR): the coordinator step that copies a finished isolated t2_auto run into the shared files (stdlib only, run once per run, then check_all.sh).
  results.csv                 -> output/t2_auto_results.csv (merge by name: rows of the run replace the old ones; PASS rows get the new an_hk number), summary.txt -> output/t2_auto_summary.txt
  candidates/an_hk_<n>.c      -> output/tier2_candidates2/an_hk_<next free>.c, names appended to names.txt (a name already in names.txt is skipped)
  overlay/MFL_extern_fix.txt  -> names/MFL_extern_fix.txt (missing lines appended)      overlay/aliases.txt -> src/aliases.txt (names not yet known appended)
  overlay/MFL_callee_protos_auto.txt -> names/MFL_callee_protos_auto.txt (merged by address)      failures/*.md -> output/t2_failures/      attempts/*.md -> output/attempts/ (missing lines appended)"""
import csv, os, re, shutil, sys
G = os.path.normpath(os.path.join(os.path.dirname(os.path.abspath(__file__)), ".."))
P = lambda *a: os.path.join(G, *a)
def lines(f): return open(f, errors="replace").read().split("\n") if os.path.exists(f) else []
def append_missing(dst, src, key=lambda l: l, skip=lambda l: not l.strip() or l.startswith("#")):
    have = {key(l) for l in lines(dst)}; new = [l for l in lines(src) if not skip(l) and key(l) not in have]
    if new:
        old = open(dst).read() if os.path.exists(dst) else ""
        open(dst, "a").write(("" if not old or old.endswith("\n") else "\n") + "\n".join(new) + "\n")
    return len(new)
def main(run):
    run = os.path.abspath(run); ov = os.path.join(run, "overlay"); rep = []
    if not os.path.exists(os.path.join(run, "results.csv")): sys.exit("%s: no results.csv (run unfinished?)" % run)
    # candidates: next free number
    cand, shared = os.path.join(run, "candidates"), P("output", "tier2_candidates2"); remap = {}
    names = [l for l in lines(os.path.join(cand, "names.txt")) if l]; sn = [l for l in lines(os.path.join(shared, "names.txt")) if l]
    nxt = max([int(m.group(1)) for f in os.listdir(shared) if (m := re.match(r"an_hk_(\d+)\.c$", f))] + [0]) + 1
    assert len(sn) == nxt - 1, "names.txt has %d lines, files go to %d" % (len(sn), nxt - 1)
    for i, n in enumerate(names):
        if n in sn: rep.append("skipped %s: already in names.txt" % n); continue
        new = "an_hk_%03d.c" % nxt; shutil.copy(os.path.join(cand, "an_hk_%03d.c" % (i + 1)), os.path.join(shared, new)); sn.append(n); remap[n] = new; nxt += 1
    if remap: open(os.path.join(shared, "names.txt"), "w").write("".join(n + "\n" for n in sn))
    # results merge
    rows = list(csv.DictReader(open(os.path.join(run, "results.csv")))); cols = list(rows[0]) if rows else ["name", "va", "bytes", "stage", "verdict", "detail", "cls", "file"]
    for r in rows:
        if r["name"] in remap: r["detail"] = remap[r["name"]]
        if r["file"] and not r["file"].startswith("output/"): r["file"] = ""      # a path into the run dir means nothing in the shared csv
    sp = P("output", "t2_auto_results.csv"); old = [r for r in csv.DictReader(open(sp)) if r["name"] not in {x["name"] for x in rows}] if os.path.exists(sp) else []
    with open(sp, "w", newline="") as f: w = csv.DictWriter(f, cols); w.writeheader(); w.writerows(sorted(old + rows, key=lambda r: -int(r["bytes"] or 0)))
    if os.path.exists(os.path.join(run, "summary.txt")): shutil.copy(os.path.join(run, "summary.txt"), P("output", "t2_auto_summary.txt"))
    # overlays
    a = append_missing(P("names", "MFL_extern_fix.txt"), os.path.join(ov, "MFL_extern_fix.txt"))
    al = P("src", "aliases.txt"); known = {l.split()[0] for l in lines(al) + lines(P("src", "symbols.txt")) if l.split()}
    b = append_missing(al, os.path.join(ov, "aliases.txt"), key=lambda l: l.split()[0] if l.split() else l, skip=lambda l: not l.split() or l.split()[0] in known or l.startswith("#"))
    c = append_missing(P("names", "MFL_callee_protos_auto.txt"), os.path.join(ov, "MFL_callee_protos_auto.txt"), key=lambda l: (re.match(r"/\*([0-9a-f]{8})\*/", l) or [0, l])[1])
    # failures / attempts
    nf = 0
    for sub, dst in (("failures", P("output", "t2_failures")), ("attempts", P("output", "attempts"))):
        d = os.path.join(run, sub)
        if not os.path.isdir(d): continue
        os.makedirs(dst, exist_ok=True)
        for f in sorted(os.listdir(d)):
            if sub == "failures": shutil.copy(os.path.join(d, f), os.path.join(dst, f)); nf += 1
            else: append_missing(os.path.join(dst, f), os.path.join(d, f), skip=lambda l: not l.strip())
    print("published %s: %d candidates (an_hk_%03d..%03d), %d result rows, %d extern lines, %d aliases, %d callee protos, %d failure files%s" % (
        run, len(remap), nxt - len(remap), nxt - 1, len(rows), a, b, c, nf, "".join("\n  " + x for x in rep)))
    return 0
if __name__ == "__main__": sys.exit(main(sys.argv[1]) if len(sys.argv) > 1 else sys.exit(__doc__))
