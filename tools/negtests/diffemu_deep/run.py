"""run.py [--work DIR] [--cases N] [name...]: negative fixtures of diffemu deep-callee mode (DIFFEMU_DEEP, tools/diffemu/core.py).
Each fixture = an old, known-bad C text (git history) of an installed function.  It is compiled with the headers of its home directory and run by diffemu.py --src
with DIFFEMU_DEEP=0 and =1 (results in a temp dir, never in output/).  Exit 1 when a fixture marked `must` is not DIFF with DEEP=1.
Needs the diffemu venv (~/.cache/mfl_diffemu_venv); negtest.sh skips it without."""
import csv, os, shutil, subprocess, sys, tempfile
G = os.path.abspath(os.path.join(os.path.dirname(os.path.abspath(__file__)), "..", "..", "..")); HERE = os.path.dirname(os.path.abspath(__file__))
# file, function, header directory, DEEP=1 must DIFF, note
FIX = [("tp19_timer_ticks.c", "timer_ticks", "src/automatch_near", False, "T-P19: observable only as edx:eax of a void-declared function; DEEP cannot see it (see README.pending)"),
       ("rwp_585950_res16.c", "rwp_aggregate_00585950", "src/automatch_near", True, "callee 586f20 writes 0xec bytes into float res[16]"),
       ("menu_page_enter_novolatile.c", "menu_page_enter", "src/clusters", True, "naked _r wrapper: MSVC drops the next_slot stores, the real callee reads the stale slot")]

def run(fx, deep, work, cases):
    f, name, hd, _, _ = fx; d = tempfile.mkdtemp(prefix="dd_%s_%d_" % (name, deep), dir=work)
    for h in os.listdir(os.path.join(G, hd)):
        if h.endswith(".h"): shutil.copy(os.path.join(G, hd, h), d)
    shutil.copy(os.path.join(HERE, f), os.path.join(d, f))
    env = dict(os.environ, DIFFEMU_DEEP=str(deep), DIFFEMU_OUT=d)
    r = subprocess.run([sys.executable, os.path.join(G, "tools", "diffemu", "diffemu.py"), "--src", os.path.join(d, f), "--force", "--jobs", "1", "--cases", str(cases), "--budget", "120"], env=env, capture_output=True, text=True, cwd=G)
    rows = {x["name"]: x for x in csv.DictReader(open(os.path.join(d, "diffemu_results.csv")))} if os.path.exists(os.path.join(d, "diffemu_results.csv")) else {}
    x = rows.get(name)
    return (x["verdict"], x["note"][:300]) if x else ("ERROR", (r.stdout + r.stderr)[-300:])

def main():
    a = sys.argv[1:]; work = os.path.join(tempfile.gettempdir(), "diffemu_deep_neg"); cases = 100
    if "--work" in a: i = a.index("--work"); work = a[i + 1]; del a[i:i + 2]
    if "--cases" in a: i = a.index("--cases"); cases = int(a[i + 1]); del a[i:i + 2]
    os.makedirs(work, exist_ok=True); rc = 0
    for fx in FIX:
        if a and fx[1] not in a: continue
        v0, n0 = run(fx, 0, work, cases); v1, n1 = run(fx, 1, work, cases)
        ok = v1 == "DIFF" or not fx[3]
        print("diffemu_deep %-4s %-24s DEEP=0 %-12s DEEP=1 %-12s %s | deep note: %s" % ("ok" if ok else "FAIL", fx[1], v0, v1, fx[4], n1), flush=True)
        rc |= not ok
    return rc

if __name__ == "__main__":
    sys.exit(main())
