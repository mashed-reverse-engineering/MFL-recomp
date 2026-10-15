#!/usr/bin/env python3
"""Colour terminal view of a running regress.sh (stdlib only, ANSI).  Usage: live.py [regress work dir]  (default: ghidra/live/current)
Reads <work>/stage.txt, <work>/script_<exe>.txt and <work>/out_<exe>_<n>/steps.txt + waitmem.txt, refreshes twice a second."""
import glob, os, re, sys, time
HERE = os.path.dirname(os.path.abspath(__file__))
ARG = sys.argv[1] if len(sys.argv) > 1 else None
CUR = os.path.join(HERE, "..", "..", "live", "current")
def workdir():   # without an argument follow the regress started last: re-read every refresh, so a new regress is picked up
    if ARG: return ARG
    try: return open(CUR).read().strip()
    except OSError: return ""
W = workdir()
C = dict(r="\033[31m", g="\033[32m", y="\033[33m", b="\033[34m", m="\033[35m", c="\033[36m", w="\033[1;37m", d="\033[2m", x="\033[0m")
LABEL = {"0x69cbd0": "menu page", "0x69cfd0": "message box", "0x69d1e4": "title fade", "0x652f68": "race phase", "0x69d3a8": "menu selection"}
PHASE = {"1": "Press START", "3": "loading", "5": "countdown", "6": "racing"}
def read(p):
    try: return open(p, errors="replace").read()
    except OSError: return ""
def bar(frac, width=30, col="g"):
    n = int(frac * width + .5)
    return C[col] + "█" * n + C["d"] + "░" * (width - n) + C["x"]
def row(d):
    name = os.path.basename(d.rstrip("/"))
    m = re.match(r"out_(\w+?)_(\d+)$", name)
    script = os.path.join(W, "script_%s.txt" % m.group(1)) if m else ""
    if m and m.group(2) != "1" and os.path.exists(os.path.join(W, "script_%s_noshot.txt" % m.group(1))):   # runs 2.. take no screenshots
        script = os.path.join(W, "script_%s_noshot.txt" % m.group(1))
    total = max(1, len([l for l in read(script).splitlines() if l.strip()])) if m else 1
    lines_ = [l.split() for l in read(os.path.join(d, "steps.txt")).splitlines() if l.strip()]
    steps = [l for l in lines_ if re.match(r"^\d+(\.\d+)?$", l[0])]          # play_inner.sh appends "crash: <reason>" lines
    crash = " ".join(" ".join(l[1:]) for l in lines_ if l[0] == "crash:") or ("aborted" if os.path.exists(os.path.join(d, "crashed")) else "")
    wm = read(os.path.join(d, "waitmem.txt")).splitlines()
    if name.startswith("out_warm"):
        return "%s%-12s%s warm-up %s" % (C["d"], name[4:], C["x"], "done" if steps else "running")
    done = os.path.exists(os.path.join(d, "powerups.bin")) and len(steps) >= total
    el = float(steps[-1][0]) if steps else 0.0
    last = steps[-1] if steps else None
    what = "starting"
    if last:
        what = last[1] + (" " + LABEL.get(last[2], last[2]) if len(last) > 2 and last[1] != "dump" else (" " + last[2] if len(last) > 2 else ""))
    ph = ""
    pr = read(os.path.join(d, "progress.txt")).split()
    if pr and not done and len(pr) >= 6 and time.time() - float(pr[0]) < 3:     # a wait is in progress right now
        addr, want, now, to, waited = pr[1], pr[2], int(pr[3]), float(pr[4]), float(pr[5])
        if addr == "0x652f68" or addr == "0x00652f68":
            ph = " " + C["y"] + "%s (phase %d) %.0fs" % (PHASE.get(str(now), "phase %d" % now), now, waited) + C["x"]
        elif want.lstrip("!").isdigit() and int(want.lstrip("!")) >= 50 and not want.startswith("!"):
            ph = " " + C["y"] + "race frame %d/%s" % (now, want) + C["x"] + (C["r"] + " no progress %.0fs" % waited + C["x"] if waited > 8 and now < int(want) and False else "")
        else:
            ph = " " + C["d"] + "waiting %s=%s (now %d) %.0fs" % (LABEL.get(addr, addr), want, now, waited) + C["x"]
    stall = ""
    if wm and re.search(r"waited 6\d\.\d+s", wm[-1]): stall = " " + C["r"] + "STALLED (timeout)" + C["x"]
    if crash: stall = " " + C["r"] + "CRASHED (%s)" % crash + C["x"]
    col = "r" if stall else ("g" if done else "c")
    state = (C["r"] + "crash" if crash else C["g"] + "done " if done else C["c"] + "run  ") + C["x"]
    line1 = "%s%-12s%s %s %3d%% %s%5.1fs%s %s" % (C["w"], name[4:], C["x"], bar(min(1, len(steps) / total), 30, col), 100 * min(1, len(steps) / total), C["d"], el, C["x"], state)
    line2 = "   %s%s%s%s" % (C["d"] + "step: " + C["x"], what, ph, stall)
    return line1 + "\n" + line2


import shutil, re as _re
def clip(line):          # never wider than the terminal: a wrapped line makes the redraw jump
    w = shutil.get_terminal_size((100, 30)).columns - 1
    out, vis, i = "", 0, 0
    while i < len(line) and vis < w:
        m = _re.match(r"\x1b\[[0-9;?]*[A-Za-z]", line[i:])
        if m: out += m.group(0); i += len(m.group(0)); continue
        out += line[i]; vis += 1; i += 1
    return out + C["x"]


def write_md(W):
    """LIVE.md for the bisect / group search runs (out_b_N): regress.sh writes LIVE.md only for its own control / overlay runs.  Written when a b run changed in the last 2 minutes."""
    runs = [d for d in sorted(glob.glob(os.path.join(W, "out_b*/"))) if time.time() - os.path.getmtime(d) < 120]
    if not runs: return
    root = os.path.join(os.path.dirname(os.path.abspath(__file__)), "..", "..")
    ld = os.path.join(root, "live"); os.makedirs(ld, exist_ok=True)
    md = ["# Group search / bisect live view (%s)" % os.path.basename(W.rstrip("/")), "", "Updated %s" % time.strftime("%H:%M:%S"), "", "| run | last step | elapsed (s) | state |", "|---|---|---|---|"]
    imgs = []
    for d in runs:
        n = os.path.basename(d.rstrip("/"))
        lines = [l.split() for l in read(os.path.join(d, "steps.txt")).splitlines() if l.strip()]
        steps = [l for l in lines if re.match(r"^\d+(\.\d+)?$", l[0])]
        crash = any(l[0] == "crash:" for l in lines) or os.path.exists(os.path.join(d, "crashed"))
        last = steps[-1] if steps else None
        state = "**CRASHED**" if crash else ("done" if os.path.exists(os.path.join(d, "powerups.bin")) else "running")
        md.append("| %s | %s | %s | %s |" % (n, " ".join(last[1:3]) if last else "", last[0] if last else "", state))
        shots = [l[2] for l in steps if len(l) > 2 and l[1] == "shot"][-8:]
        row = []
        for sh in shots:
            src = os.path.join(d, sh + ".png")
            if os.path.exists(src):
                dst = os.path.join(ld, "%s_%s.png" % (n, sh))
                try: shutil.copy2(src, dst)
                except OSError: continue
                row.append('<td><img src="live/%s_%s.png" width="150"><br><sub>%s</sub></td>' % (n, sh, sh))
        if row: imgs.append("<b>%s</b><table><tr>%s</tr></table>" % (n, "".join(row)))
    md += [""] + imgs
    try:
        open(os.path.join(root, "LIVE.md.tmp"), "w").write("\n".join(md) + "\n"); os.replace(os.path.join(root, "LIVE.md.tmp"), os.path.join(root, "LIVE.md"))
    except OSError: pass

def main():
    global W
    sys.stdout.write("\033[?25l\033[2J")
    try:
        while True:
            W = workdir()
            if not os.path.isdir(W):
                sys.stdout.write("\033[H\n waiting for a regress (tools/wine-run/regress.sh) ...\033[K\n\033[J"); sys.stdout.flush(); time.sleep(1); continue
            write_md(W)
            stage = read(os.path.join(W, "stage.txt")).strip() or "?"
            out = ["%s MFL regress %s %s%s" % (C["w"], C["x"], C["d"] + W + C["x"], ""), ""]
            sc = C["g"] if "finished" in stage else (C["y"] if stage in ("building", "runs") else C["c"])
            out.append(" stage  %s%s%s" % (sc, stage, C["x"]))
            out.append("")
            for d in sorted(glob.glob(os.path.join(W, "out_*_*/")), key=lambda p: (("warm" in p), p)):
                out.append(" " + row(d) if "\n" not in row(d) else " " + row(d).replace("\n", "\n "))
            res = read(os.path.join(W, "result.txt")).strip()
            if res:
                out += ["", " " + C["w"] + "result" + C["x"]]
                for l in [x for x in res.splitlines() if re.match(r"(screenshot|cars|vehicles|aibots|statsrace|powerups|WARNING)", x)]:   # no tracebacks / noise
                    bad = re.search(r"cars: .*ctrl1-ovl\d [1-9]", l) or "WARNING" in l
                    out.append("  " + (C["r"] if bad else C["g"] if l.startswith("cars") else "") + l[:110] + C["x"])
            out += ["", C["d"] + " " + time.strftime("%H:%M:%S") + "   screenshots: ghidra/LIVE.md   Ctrl-C quits" + C["x"]]
            out = [clip(l) for x in out for l in x.split("\n")]
            sys.stdout.write("\033[H" + "\n".join(x + "\033[K" for x in out) + "\n\033[J")
            sys.stdout.flush(); time.sleep(.5)
    except KeyboardInterrupt:
        pass
    finally:
        sys.stdout.write("\033[?25h\n")
main()
