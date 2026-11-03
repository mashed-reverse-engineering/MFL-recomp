#!/bin/bash
# scenario.sh <name>: run scenarios/<name>.sh (a game scenario script generator, see scenarios/lib.sh) on the control build and on the shipped overlay build
# (EXTRA_OVL, default " --automatch --automatch-near --bridges --clusters", plus --count and the harness), crash-check ovl against ctrl and print the functions executed.
# Output dir output/scenario/<name>/ (SCEN_OUT overrides the parent): out_ctrl/ out_ovl/ (screenshots, wine.log, counters.bin, gdbcov.txt), executed.txt (one name per line), result.txt.
#   - coverage = counters of --count (entry jmp of every replaced function) UNION one-shot gdb breakpoints (scen_inner.sh gdbcov) at the NEW address of every replaced function:
#     --count alone misses a replaced function that is only called by another replaced function (C -> C calls bypass the entry jmp), 581 "never executed" functions were mostly that.
#     gdb attaches ~1 s after the process starts: code that runs before that (CRT / engine init) is only seen by --count.
#   - verdict: a Wine "Unhandled" in out_ovl/wine.log = CRASH (the function is named by crashfn.py); the control must not crash; the final screenshots (taken by the script as `final`)
#     differ by imgdiff.py mean (< 30 = same scene; menus are deterministic, races are not: use SCEN_IMGTHRESH=n to change the limit).
# SCEN_EXE=<ours.exe> SCEN_DELTA=<hex> [SCEN_RWDLL=..] [NOCACHE=1]: run on a standalone link instead (see the SCEN_EXE block below); control cached in output/scenario/_ctrlcache/.
# Env: NOCTRL=1 (skip the control run: coverage only), REBUILD=1 (rebuild the cached exes in output/scenario/_build), NEVER=file (list of names to report on, default output/never_exec.txt),
#      GDB_EXTRA="course_unload race_load_course_0042fad0" (also break on these original functions),
#      NOGDB=1 (no gdb breakpoints: crash / timing check without the debugger, coverage = counters only),
#      GDBCRASH=1 (gdb dumps registers + stack of the first SIGSEGVs to out_ovl/gdbfault.txt; use with NOGDB=1),
#      RUN_TIMEOUT (s, default 300, scaled by load/4), SLOT=n (local slot index, default 1; the control uses n+1).  Max 2 game processes per call, the global limit of 4 is enforced by gameslot.sh.
# leading NAME=VALUE arguments are exported (replaces an `env NAME=VALUE scenario.sh` prefix, which asks the user for confirmation): scenario.sh SCEN_TAG=x SCEN_MODE=arcade pause_all
while [[ "$1" =~ ^[A-Z][A-Z0-9_]*=.*$ ]]; do export "$1"; shift; done
T=$(dirname "$(readlink -f "$0")"); R=$(readlink -f $T/../..); N=$1; [ -f "$T/scenarios/$N.sh" ] || { echo "usage: scenario.sh <name> (scenarios/*.sh: $(ls $T/scenarios | sed 's/\.sh$//' | tr '\n' ' '))"; exit 2; }
export MASHED_ROOT=${MASHED_ROOT:-$(readlink -f $R/..)}
EXTRA_OVL=${EXTRA_OVL- --automatch --automatch-near --bridges --clusters}
OVL_ARGS="--count$EXTRA_OVL"; SC=${SCEN_OUT:-$R/output/scenario}; B=$SC/_build; W=$SC/$N${SCEN_TAG:+_$SCEN_TAG}; mkdir -p $B $W
SLOT=${SLOT:-1}; NEVER=${NEVER:-output/never_exec.txt}
# ---- SCEN_EXE mode: run the scenario on a standalone link (delink gate link/ours.exe) instead of the overlay build.
#   SCEN_EXE=<ours.exe> SCEN_DELTA=<hex> [SCEN_RWDLL=<mflrw.dll to copy next to the exe, default: the link's own game/App_Executables/mflrw.dll>] scenario.sh <name>
#   Script addresses: literals in 0x5dd000..0x9f9000 moved by DELTA (addrmove.py, like race_relinked.sh); harness symbols (scen_flags, fast_menu_scale, frames_total...) from <exe>.map minus DELTA
#   so that the same move gives their linked address.  Control = ctrl.exe (harness, original layout); its result (out_ctrl + rc) is cached in $SC/_ctrlcache/<sha of ctrl script+ctrl.exe>/.
#   No counters / gdbcov.  Verdict line "RESULT: PASS|FAIL ..." and exit status 0|1: FAIL on Wine Unhandled, rc != 0, title waitmem not reached (wrong DELTA), final screenshot vs control > SCEN_IMGTHRESH (30).
if [ -n "$SCEN_EXE" ]; then
  [ -n "$SCEN_DELTA" ] || { echo "SCEN_EXE needs SCEN_DELTA=<hex>"; exit 2; }
  EXE=$(readlink -f $SCEN_EXE); XMAP=${EXE%.exe}.map; [ -f "$EXE" ] && [ -f "$XMAP" ] || { echo "missing $EXE or $XMAP"; exit 2; }
  W=$SC/${N}_sa${SCEN_TAG:+_$SCEN_TAG}; rm -rf $W; mkdir -p $W $SC/_ctrlcache
  if [ ! -f $B/ctrl.exe ] || [ -n "$REBUILD" ]; then python3 $R/tools/build_overlay.py $B/ctrl.exe --no-recomp --harness > $B/build_ctrl.log 2>&1 || { echo "ctrl build failed"; exit 1; }; fi
  . $T/scenarios/lib.sh; . $T/scenarios/$N.sh
  # standalone mapva: link-map address minus DELTA (the common addrmove then moves it back); the counter dump of the default footer does not exist in a linked exe
  CMAP=$B/ctrl.exe.map; MAP=$CMAP; export MAP
  { scenario_header; scenario; scenario_footer; } > $W/script_ctrl.txt
  mapva() { local a=$(awk -v n="_$1" '$2==n{print $3; exit}' $XMAP); [ -n "$a" ] || { echo "harness symbol $1 not in $XMAP" >&2; echo 0; return; }; printf "%x" $((0x$a - 0x$SCEN_DELTA)); }
  declare -f scenario_footer | grep -q count_table && scenario_footer() { echo "wait ${SCEN_END_WAIT:-1}"; echo "shot final"; }
  { scenario_header; scenario; scenario_footer; } 2>$W/gen.err | python3 $T/addrmove.py $SCEN_DELTA > $W/script_ours.txt
  grep -q "not in" $W/gen.err && { cat $W/gen.err; echo "RESULT: FAIL (harness symbols missing)"; exit 1; }
  KEY=$( (cat $W/script_ctrl.txt; md5sum < $B/ctrl.exe) | sha1sum | cut -c1-16 ); CC=$SC/_ctrlcache/$N-$KEY
  INST=$R/../MashedGame/FullyLoaded/installed
  python3 $T/prepare.py $INST $W/run_ours $EXE $XMAP > /dev/null || exit 1
  RW=${SCEN_RWDLL:-$(dirname $EXE)/game/App_Executables/mflrw.dll}; [ -f "$RW" ] && cp -f $RW $W/run_ours/App_Executables/mflrw.dll
  rm -rf $W/out_ours; export LOADMAX=${LOADMAX:-40}
  LD=$(cut -d' ' -f1 /proc/loadavg); RT=$(awk -v t=${RUN_TIMEOUT:-300} -v l=$LD 'BEGIN{s=l/4; if(s<1)s=1; if(s>3)s=3; print int(t*s)}')
  runx() {  # runx <run dir> <script> <out dir> <slot> <rc file>
    PFX=wine_pfx_$4 PERSIST=slot$4 NOSHOW=${NOSHOW-} $T/gameslot.sh timeout -k 5 $RT $T/scen_play.sh $1 $2 $3 > /dev/null 2>&1; echo $? > $5; }
  $T/mkpfx.sh $SLOT
  if [ -f $CC/rc_ctrl ] && [ -z "$NOCACHE" ]; then echo "control: cached ($CC)"; CACHED=1
  else
    $T/mkpfx.sh $((SLOT+1)); python3 $T/prepare.py $INST $W/run_ctrl $B/ctrl.exe > /dev/null || exit 1; rm -rf $W/out_ctrl
    runx $W/run_ours $W/script_ours.txt $W/out_ours $SLOT $W/rc_ours & P1=$!
    runx $W/run_ctrl $W/script_ctrl.txt $W/out_ctrl $((SLOT+1)) $W/rc_ctrl & P2=$!; wait $P1 $P2
    CACHED=
  fi
  [ -d $W/out_ours ] || runx $W/run_ours $W/script_ours.txt $W/out_ours $SLOT $W/rc_ours
  if [ -n "$CACHED" ]; then rm -rf $W/out_ctrl; cp -a $CC/out_ctrl $W/out_ctrl; cp $CC/rc_ctrl $W/rc_ctrl; fi
  python3 - "$W" "$T" "$CC" "$CACHED" <<'PY' | tee $W/result.txt
import os, shutil, subprocess, sys
W, T, CC, cached = sys.argv[1:5]
def crash(k):
    p = "%s/out_%s/wine.log" % (W, k)
    if not os.path.exists(p): return "no wine.log"
    if "Unhandled" not in open(p, errors="replace").read(): return None
    return subprocess.run(["python3", T + "/crashfn.py", W, "out_" + k], capture_output=True, text=True).stdout.strip() or "Unhandled exception"
def rc(k): return open("%s/rc_%s" % (W, k)).read().strip()
def title(k):
    p = "%s/out_%s/waitmem.txt" % (W, k)
    l = open(p).readline() if os.path.exists(p) else ""
    return "(value 1)" in l
co, cc = crash("ours"), crash("ctrl"); bad = []
print("scenario %s on standalone exe (control %s)" % (os.path.basename(W), "cached" if cached else "fresh"))
print("ctrl: %s  rc %s" % ("clean" if cc is None else "CRASH " + cc, rc("ctrl"))); print("ours: %s  rc %s" % ("clean" if co is None else "CRASH " + co, rc("ours")))
if co: bad.append("ours crashed")
if rc("ours") != "0": bad.append("ours rc %s" % rc("ours"))
if not title("ours"): bad.append("title waitmem not reached (wrong DELTA / harness address?)")
if cc or rc("ctrl") != "0" or not title("ctrl"): bad.append("control unhealthy (not cached)")
a, b = W + "/out_ctrl/final.png", W + "/out_ours/final.png"
if os.path.exists(a) and os.path.exists(b):
    d = subprocess.run(["python3", T + "/imgdiff.py", a, b], capture_output=True, text=True).stdout.strip(); lim = float(os.environ.get("SCEN_IMGTHRESH", "30"))
    print("final screenshot ctrl vs ours mean diff: %s (limit %g)" % (d, lim))
    try:
        if float(d.split()[0]) >= lim: bad.append("final screenshot differs")
    except Exception:
        if d or os.environ.get("SCEN_LAST") != "quitgame": bad.append("imgdiff failed: " + d)      # quitgame: the game has quit, final.png can be an empty shot
elif os.path.exists(a) != os.path.exists(b) and os.environ.get("SCEN_LAST") != "quitgame": bad.append("final.png only on one side")
os.environ["CMP_DELTA"] = os.environ.get("SCEN_DELTA", "")      # data pointers in the dumps moved by DELTA, heap pointers by the image size
gs = sorted(f for f in os.listdir(W + "/out_ctrl") if f.startswith("gs_") and f.endswith(".bin")) if os.path.isdir(W + "/out_ctrl") else []
if gs:      # GameState dumps (options_all, pause_all): byte compare with the control
    r = [(f[3:-4], subprocess.run(["python3", T + "/cmpdump.py", "gs", W + "/out_ctrl/" + f, W + "/out_ours/" + f], capture_output=True, text=True).stdout.strip()) for f in gs]
    nd = [x for x in r if x[1] != "0"]
    print("GameState dumps ctrl vs ours differing bytes: %d dumps, %d differ%s" % (len(r), len(nd), "".join("  %s=%s" % x for x in nd)))
    if nd: bad.append("GameState differs")
if not bad and not cached:
    os.makedirs(CC, exist_ok=True); shutil.rmtree(CC + "/out_ctrl", ignore_errors=True); shutil.copytree(W + "/out_ctrl", CC + "/out_ctrl"); shutil.copy(W + "/rc_ctrl", CC + "/rc_ctrl")
print("RESULT: " + ("FAIL (%s)" % "; ".join(bad) if bad else "PASS")); sys.exit(1 if bad else 0)
PY
  exit ${PIPESTATUS[0]}
fi
if [ ! -f $B/ovl.exe ] || [ ! -f $B/ctrl.exe ] || [ -n "$REBUILD" ]; then
  python3 $R/tools/build_overlay.py $B/ctrl.exe --no-recomp --harness > $B/build_ctrl.log 2>&1 || { echo "ctrl build failed"; exit 1; }
  python3 $R/tools/build_overlay.py $B/ovl.exe --harness $OVL_ARGS > $B/build_ovl.log 2>&1 || { echo "ovl build failed"; exit 1; }
fi
cp -f $B/ctrl.exe $W/ctrl.exe; cp -f $B/ovl.exe $W/ovl.exe; cp -f $B/ctrl.exe.map $W/ctrl.exe.map; cp -f $B/ovl.exe.map $W/ovl.exe.map
MAP=$W/ovl.exe.map; export MAP
# gdb list: every counted function at its NEW address
grep '^count:' $MAP | sed 's/^count://' | cut -d' ' -f1 | sort -u > $W/.fnames
awk 'NR==FNR{n[$1]=1; next} ($1 in n){print $1, $2}' $W/.fnames <(grep -v '^count' $MAP) > $W/gdbcov_names.txt
for k in ctrl ovl; do rm -rf $W/run_$k $W/out_$k; python3 $T/prepare.py $R/../MashedGame/FullyLoaded/installed $W/run_$k $W/$k.exe > /dev/null || exit 1; done
for x in $GDB_EXTRA; do grep "^$x " $R/src/symbols.txt >> $W/gdbcov_names.txt; done      # GDB_EXTRA="name ...": extra ORIGINAL (not replaced) functions to watch, address from src/symbols.txt
cp $W/gdbcov_names.txt $W/run_ovl/gdbcov_names.txt
. $T/scenarios/lib.sh; . $T/scenarios/$N.sh
{ scenario_header; scenario; scenario_footer; } > $W/script_ctrl.txt
{ [ -n "$NOGDB" ] || echo "gdbcov gdbcov_names.txt"; [ -n "$GDBCRASH" ] && echo "gdbfault 8"; cat $W/script_ctrl.txt; } > $W/script_ovl.txt
LD=$(cut -d' ' -f1 /proc/loadavg); RT=$(awk -v t=${RUN_TIMEOUT:-300} -v l=$LD 'BEGIN{s=l/4; if(s<1)s=1; if(s>3)s=3; print int(t*s)}')
run() {  # run <k> <slot>
  PFX=wine_pfx_$2 PERSIST=slot$2 NOSHOW=${NOSHOW-} $T/gameslot.sh timeout -k 5 $RT $T/scen_play.sh $W/run_$1 $W/script_$1.txt $W/out_$1 > /dev/null 2>&1; echo $? > $W/rc_$1
}
$T/mkpfx.sh $SLOT; $T/mkpfx.sh $((SLOT+1))
run ovl $SLOT & P1=$!
if [ -z "$NOCTRL" ]; then run ctrl $((SLOT+1)) & P2=$!; fi
wait $P1 $P2
# ---- evaluation
python3 - "$W" "$NEVER" "$T" <<'PY' | tee $W/result.txt
import os, re, struct, subprocess, sys
W, NEVER, T = sys.argv[1:4]
def crash(k):
    p = "%s/out_%s/wine.log" % (W, k)
    if not os.path.exists(p): return "no wine.log"
    t = open(p, errors="replace").read()
    if "Unhandled" not in t: return None
    r = subprocess.run(["python3", T + "/crashfn.py", W, "out_" + k], capture_output=True, text=True).stdout.strip()
    return r or "Unhandled exception"
ex = set(); base = None; names = []
for l in open(W + "/ovl.exe.map"):
    p = l.split()
    if p[0].startswith("count:"): names.append((p[0][6:], int(p[1], 16)))
    elif p[0] == "count_table": base = int(p[1], 16)
cb = W + "/out_ovl/counters.bin"; ncnt = 0
if os.path.exists(cb):
    d = open(cb, "rb").read()
    for n, va in names:
        if va - base + 4 <= len(d) and struct.unpack_from("<I", d, va - base)[0] > 0: ex.add(n); ncnt += 1
ngdb = 0; gp = W + "/out_ovl/gdbcov.txt"
if os.path.exists(gp):
    for l in open(gp, errors="replace"):
        if l.startswith("HIT "): 
            n = l.split()[1]
            if n not in ex: ngdb += 1
            ex.add(n)
open(W + "/executed.txt", "w").write("\n".join(sorted(ex)) + "\n")
never = [l.strip() for l in open(NEVER) if l.strip()] if os.path.exists(NEVER) else []
co, cc = crash("ovl"), (crash("ctrl") if os.path.isdir(W + "/out_ctrl") else "skipped")
print("scenario %s: executed %d (counters %d, gdb adds %d) of %d replaced; of the %d never-executed list: %d" % (os.path.basename(W), len(ex), ncnt, ngdb, len(names), len(never), len(ex & set(never))))
print("ctrl: %s" % ("clean" if cc is None else cc)); print("ovl : %s" % ("clean" if co is None else "CRASH " + co))
sh = ""
if os.path.exists(W + "/out_ctrl/final.png") and os.path.exists(W + "/out_ovl/final.png"):
    sh = subprocess.run(["python3", T + "/imgdiff.py", W + "/out_ctrl/final.png", W + "/out_ovl/final.png"], capture_output=True, text=True).stdout.strip()
print("final screenshot ctrl vs ovl mean diff: %s (limit %s)" % (sh or "n/a", os.environ.get("SCEN_IMGTHRESH", "30")))
if os.path.exists(W + "/out_ctrl/cars.bin") and os.path.exists(W + "/out_ovl/cars.bin"):
    print("cars dump differing bytes (cmpdump, masked fields ignored): " + subprocess.run(["python3", T + "/cmpdump.py", "cars", W + "/out_ctrl/cars.bin", W + "/out_ovl/cars.bin"], capture_output=True, text=True).stdout.strip())
gs = sorted(f for f in os.listdir(W + "/out_ctrl") if f.startswith("gs_") and f.endswith(".bin")) if os.path.isdir(W + "/out_ctrl") else []
if gs:      # GameState dumps (options_all): byte compare ctrl vs ovl, no masks (the overlay section shifts nothing in .data)
    r = [(f[3:-4], subprocess.run(["python3", T + "/cmpdump.py", "gs", W + "/out_ctrl/" + f, W + "/out_ovl/" + f], capture_output=True, text=True).stdout.strip()) for f in gs]
    print("GameState dumps ctrl vs ovl differing bytes: %d dumps, %d differ%s" % (len(r), sum(1 for _, n in r if n not in ("0",)), "".join("  %s=%s" % x for x in r if x[1] != "0")))
print("rc ovl %s ctrl %s" % (open(W + "/rc_ovl").read().strip(), open(W + "/rc_ctrl").read().strip() if os.path.exists(W + "/rc_ctrl") else "-"))
PY
