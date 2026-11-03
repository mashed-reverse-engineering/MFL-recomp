#!/usr/bin/env python3
"""sasweep.py: scenario sweep on a standalone link (nightly runner; successor of the trapsweep run2.py).

  sasweep.py [--link DIR] [--jobs FILE] [--out DIR] [--cov NAMES] [--budget SEC] [--list]

--link   gate dir with link/ours.exe + delta.txt (default: newest output/gate/*_standalone whose gate.log says PASSED)
--jobs   job list, one per line "name scenario [VAR=VALUE ...]" ('#' comments); default: the built-in set (--list prints it)
--out    output dir (default output/scenario/sweep_<link>[_cov]): results.tsv (name, PASS|FLAKY|FAIL, per-try history),
         logs/<name>[_retry].log, scen/ (the scenario run dirs).  Names already in results.tsv are skipped: rerun to resume.
--cov    coverage mode: tools/wine-run/scen_cov.sh with this gdbcov names file ("name hexaddr", standalone_fns.py) instead of
         scenario.sh (no control); run dirs scen/<scenario>_cov_<name>/ for scen_union.py standalone.  Verdict: rc 0 and no Unhandled.
--budget start no new job after SEC seconds (running jobs finish; 0 = no limit).  Rerun to continue.
Every failure is retried once: PASS on the retry = FLAKY, appended to output/scenario/flaky.tsv (date, link, job, first-try reasons).
Parallel jobs = GAME_SLOTS (default 2) while the 1-min load (uptime) is below SWEEP_LOAD (default 12), else 1; gameslot.sh still
caps the game processes globally.  Defaults per job: SCEN_RWDLL=output/librw/mflrw.dll, SCEN_IMGTHRESH=100000 (job args override)."""
import argparse, glob, os, re, signal, subprocess, sys, time
T = os.path.dirname(os.path.abspath(__file__)); G = os.path.normpath(T + "/../.."); O = G + "/output"

def default_jobs():
    j = ["menus_main menus_main", "options_all options_all", "options options", "options_save options_save",
         "pause_cup pause_all SCEN_MODE=cup", "pause_training pause_all SCEN_MODE=training", "pause_quitgame pause_all SCEN_LAST=quitgame",
         "pause_multi pause_all SCEN_MODE=multi", "multi_std multi", "multi_team multi SCEN_TEAM=1", "exit_windows exit_windows",
         "tworaces tworaces", "cupwin cup SCEN_ROW=1 SCEN_DRIVE=41 SCEN_AFTER=win", "arcade_win arcade SCEN_DRIVE=41 SCEN_AFTER=win SCEN_SECS=25",
         "menus_anims menus_anims"]
    j += ["timetrial_r%d timetrial SCEN_ROW=%d SCEN_SECS=30" % (r, r) for r in (0, 3, 7, 11, 14)]
    j += ["training_l%d training SCEN_ROW=%d SCEN_SECS=40 SCEN_AFTER=quit" % (r, r) for r in range(8)]
    j += ["%s_r%dc%d %s SCEN_ROW=%d SCEN_RIGHT=%d SCEN_DRIVE=31" % (m, r, c, m, r, c) for m in ("arcade", "cup") for r in range(15) for c in range(3)]
    return j

def load1():
    m = re.search(r"load averages?: (\d+)[.,](\d+)", subprocess.run(["uptime"], capture_output=True, text=True).stdout)
    return float("%s.%s" % m.groups()) if m else 99.0

def newest_link():
    for d in sorted(glob.glob(O + "/gate/*_standalone"), reverse=True):
        if os.path.exists(d + "/gate.log") and "GATE standalone PASSED" in open(d + "/gate.log", errors="replace").read(): return d
    sys.exit("no passed standalone gate link in output/gate")

def reasons(log, cov):
    if cov:
        m = re.findall(r"^rc (\S+) breakpoints \S+ hits (\S+) unhandled (\S+)", log, re.M)
        if not m: return ["no result: " + (log.strip().splitlines() or ["(empty)"])[-1]]
        rc, hits, un = m[-1]
        return (["rc " + rc] if rc != "0" else []) + (["unhandled " + un] if un not in ("0", "") else [])
    m = re.findall(r"^RESULT: (.*)$", log, re.M)
    if not m: return ["no result: " + (log.strip().splitlines() or ["(empty)"])[-1]]
    if m[-1] == "PASS": return []
    return [x.strip() for x in re.sub(r"^FAIL \((.*)\)$", r"\1", m[-1]).split(";") if x.strip()]

def main():
    ap = argparse.ArgumentParser(); ap.add_argument("--link"); ap.add_argument("--jobs"); ap.add_argument("--out"); ap.add_argument("--cov")
    ap.add_argument("--budget", type=float, default=0); ap.add_argument("--list", action="store_true"); a = ap.parse_args()
    if a.list: print("\n".join(default_jobs())); return
    L = os.path.abspath(a.link or newest_link()); delta = open(L + "/delta.txt").read().strip(); exe = L + "/link/ours.exe"
    S = os.path.abspath(a.out or "%s/scenario/sweep_%s%s" % (O, os.path.basename(L), "_cov" if a.cov else ""))
    SC = S + "/scen"; os.makedirs(SC, exist_ok=True); os.makedirs(S + "/logs", exist_ok=True)
    for x in ("_build", "_ctrlcache"):
        if not os.path.exists(SC + "/" + x): os.symlink(O + "/scenario/" + x, SC + "/" + x)
    lines = open(a.jobs).read().splitlines() if a.jobs else default_jobs()
    jobs = [l.split() for l in lines if l.split() and not l.lstrip().startswith("#")]
    res = S + "/results.tsv"
    done = {l.split("\t")[0] for l in open(res)} if os.path.exists(res) else set()
    queue = [(j, 1) for j in jobs if j[0] not in done]; hist = {}; run = []; t0 = time.time()
    os.environ.setdefault("GAME_SLOTS", "2")
    base = ["SCEN_RWDLL=" + O + "/librw/mflrw.dll", "SCEN_IMGTHRESH=100000"]
    print("sasweep: link %s delta %s, %d jobs to run, out %s" % (L, delta, len(queue), S), flush=True)

    def start(job, att):
        name, sc, args = job[0], job[1], job[2:]; tag = name + ("_retry" if att == 2 else "")
        if a.cov:
            env = dict(os.environ, SCEN_OUT=SC, SCEN_TAG=tag, RUN_TIMEOUT=os.environ.get("RUN_TIMEOUT", "150"))
            env.update(x.split("=", 1) for x in base + args)
            cmd = [T + "/scen_cov.sh", sc, exe, delta, os.path.abspath(a.cov)]
        else:
            env = None; cmd = [T + "/scenario.sh", "SCEN_EXE=" + exe, "SCEN_DELTA=" + delta, "SCEN_OUT=" + SC] + base + args + ["SCEN_TAG=" + tag, sc]
        lf = open("%s/logs/%s.log" % (S, tag), "w")
        return (subprocess.Popen(cmd, stdout=lf, stderr=subprocess.STDOUT, env=env, start_new_session=True), job, att, time.time(), lf)

    def stop(*_):      # killed sweep: take the running scenario process groups along (else they keep a game slot and write into a rerun's dirs)
        for r in run:
            try: os.killpg(r[0].pid, signal.SIGTERM)
            except OSError: pass
        sys.exit(1)
    signal.signal(signal.SIGTERM, stop); signal.signal(signal.SIGINT, stop); signal.signal(signal.SIGHUP, stop)
    while queue or run:
        for r in run[:]:
            p, job, att, ts, lf = r
            if p.poll() is None: continue
            run.remove(r); lf.close(); name = job[0]
            why = reasons(open(lf.name, errors="replace").read(), a.cov)
            hist.setdefault(name, []).append("try%d %ds: %s" % (att, time.time() - ts, "; ".join(why) or "PASS"))
            if why and att == 1: queue.insert(0, (job, 2)); continue
            v = "FAIL" if why else ("FLAKY" if att == 2 else "PASS")
            line = "%s\t%s\t%s\n" % (name, v, " | ".join(hist[name])); open(res, "a").write(line); print(line, end="", flush=True)
            if v == "FLAKY":
                fl = O + "/scenario/flaky.tsv"; new = not os.path.exists(fl)
                with open(fl, "a") as f:
                    if new: f.write("date\tlink\tjob\tfirst try\n")
                    f.write("%s\t%s\t%s\t%s\n" % (time.strftime("%Y-%m-%d %H:%M"), os.path.basename(L), " ".join(job), hist[name][0]))
        over = a.budget and time.time() - t0 > a.budget
        if over and not run: break
        n = int(os.environ["GAME_SLOTS"]) if load1() < float(os.environ.get("SWEEP_LOAD", "12")) else 1
        if queue and not over and len(run) < n: run.append(start(*queue.pop(0))); continue
        time.sleep(3)
    left = sum(1 for j in jobs if j[0] not in {l.split("\t")[0] for l in open(res)}) if os.path.exists(res) else len(jobs)
    print("SWEEP %s: %d jobs left (rerun to continue)" % ("DONE" if not left else "PAUSED", left), flush=True)

if __name__ == "__main__":
    main()
