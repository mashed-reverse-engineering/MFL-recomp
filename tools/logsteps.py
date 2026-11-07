#!/usr/bin/env python3
"""logsteps.py <gate dir | gate.log | run.py/rungate log> [...]: per-step wall minutes of a gate.sh run (from the
`=== [HH:MM] step n/m: label` lines; the last step ends at the log's mtime, or now while running), the verdict, the
failing step with its failing lines, and the standalone link retry count (link2/link3/link4 dirs).  No grep/sed on logs.
logsteps.py --selftest: checks the parser on a synthetic log."""
import os, re, sys, time

STEP = re.compile(r"^=== \[(\d\d):(\d\d)\] step (\d+)/(\d+): (.*)")
FAILED = re.compile(r"^GATE FAILED at step (\d+)")
BAD = re.compile(r"FAIL|fail|not reached|crash|timed out|INVALID|Killed|GROWN|DIFF|FLIP|mismatch|above the control|no result|error|cannot|too high", re.I)
NOT_BAD = re.compile(r" 0 errors|DELTA mismatch: rerun|^GATE FAILED|^  next:")


def parse(lines, end_hm):
    """-> dict(steps=[(n, m, label, minutes, lines)], verdict, failed_step, out_dir)"""
    steps, verdict, failed, out_dir = [], "running / killed (no GATE line)", None, None
    for l in lines:
        l = l.rstrip("\n")
        m = STEP.match(l)
        if m:
            steps.append([int(m[3]), int(m[4]), m[5], int(m[1]) * 60 + int(m[2]), []])
            continue
        if l.startswith("gate ") and ", output " in l:
            out_dir = l.rsplit(", output ", 1)[1].strip()
        if FAILED.match(l):
            failed, verdict = int(FAILED.match(l)[1]), l
        elif re.match(r"^GATE \S+ PASSED", l) or l.startswith("load ") and "not starting" in l:
            verdict = l
        if steps:
            steps[-1][4].append(l)
    out = []
    for i, (n, m, label, t, body) in enumerate(steps):
        nxt = steps[i + 1][3] if i + 1 < len(steps) else end_hm
        out.append((n, m, label, (nxt - t) % 1440, body))      # % 1440: a gate across midnight
    return dict(steps=out, verdict=verdict, failed_step=failed, out_dir=out_dir)


def bad_lines(body, limit=16):
    hits = [l for l in body if BAD.search(l) and not NOT_BAD.search(l)]
    marks = [i for i, l in enumerate(body) if l.startswith("check_all failing check")]
    if marks:      # gate.sh prints the last 12 log lines (the failing check names itself last) after this marker
        hits = body[marks[0] + 1:]
        hits = [l for l in hits if not l.startswith(("GATE FAILED", "  next:"))]
    return hits[-limit:]


def report(arg):
    log = os.path.join(arg, "gate.log") if os.path.isdir(arg) else arg
    if not os.path.isfile(log):
        print("%s: no gate log" % arg)
        return 2
    lines = open(log, errors="replace").readlines()
    r = parse(lines, 0)
    finished = not r["verdict"].startswith("running")
    lt = time.localtime(os.path.getmtime(log) if finished else time.time())
    r = parse(lines, lt.tm_hour * 60 + lt.tm_min)
    gdir = arg if os.path.isdir(arg) else (r["out_dir"] if r["out_dir"] and os.path.isdir(r["out_dir"]) else os.path.dirname(log))
    print("log %s" % log)
    total = 0
    for n, m, label, mins, _ in r["steps"]:
        total += mins
        mark = "  <== FAILED" if n == r["failed_step"] else ""
        print("  step %d/%d %4d min  %s%s" % (n, m, mins, label[:100], mark))
    print("  total %d min (HH:MM resolution; the last step ends at %s)" % (total, "the log mtime" if finished else "now"))
    retries = [k for k in ("link2", "link3", "link4") if os.path.isdir(os.path.join(gdir, k))]
    if retries or os.path.exists(os.path.join(gdir, "link.log")):
        print("  link retries: %d (%s)" % (len(retries), " ".join(retries) or "first link stable"))
    print("verdict: " + r["verdict"])
    if r["failed_step"]:
        body = [s for s in r["steps"] if s[0] == r["failed_step"]][0][4]
        print("failing line(s) of step %d:" % r["failed_step"])
        for l in bad_lines(body) or body[-6:]:
            print("  " + l[:220])
    return 1 if r["failed_step"] else 0


def selftest():
    log = """=== [23:50] step 1/3: check_all
declcheck clean
link: 0 errors, 30 LNK4006
=== [23:58] step 2/3: MKRANGE link
DELTA mismatch: rerun with DELTA=14f000
=== [00:20] step 3/3: crash-only 15 courses
course 0: ok
course 14: race not reached (crash: aborted)
GATE FAILED at step 3 (crash-only 15 courses), exit 1; logs in /x
  next: groupsearch only on the failing course
""".splitlines(True)
    r = parse(log, 0 * 60 + 31)
    assert [s[3] for s in r["steps"]] == [8, 22, 11], r["steps"]
    assert r["failed_step"] == 3 and r["verdict"].startswith("GATE FAILED")
    assert bad_lines(r["steps"][2][4]) == ["course 14: race not reached (crash: aborted)"], bad_lines(r["steps"][2][4])
    assert bad_lines(r["steps"][1][4]) == [] and bad_lines(r["steps"][0][4]) == []
    ca = ["check_all failing check (last 12 log lines; x):", "FAIL declcheck: x.c", "GATE FAILED at step 1 (check_all), exit 1"]
    assert bad_lines(["noise fail"] + ca) == ["FAIL declcheck: x.c"]
    r = parse(["=== [10:00] step 1/1: x\n", "GATE tier1 PASSED (1 steps)\n"], 10 * 60 + 5)
    assert r["failed_step"] is None and r["steps"][0][3] == 5 and "PASSED" in r["verdict"]
    print("SELFTEST PASS")
    return 0


if __name__ == "__main__":
    a = sys.argv[1:]
    if not a or a[0] in ("-h", "--help"):
        print(__doc__)
        sys.exit(0)
    if a[0] == "--selftest":
        sys.exit(selftest())
    sys.exit(max(report(x) for x in a))
