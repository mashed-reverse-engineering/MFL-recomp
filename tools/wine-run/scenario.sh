#!/bin/bash
# scenario.sh <name>: run scenarios/<name>.sh (a game scenario script generator, see scenarios/lib.sh) on the control build and on the shipped overlay build
# (EXTRA_OVL, default " --automatch --automatch-near --bridges --clusters", plus --count and the harness), crash-check ovl against ctrl and print the functions executed.
# Output dir output/scenario/<name>/ (SCEN_OUT overrides the parent): out_ctrl/ out_ovl/ (screenshots, wine.log, counters.bin, gdbcov.txt), executed.txt (one name per line), result.txt.
#   - coverage = counters of --count (entry jmp of every replaced function) UNION one-shot gdb breakpoints (scen_inner.sh gdbcov) at the NEW address of every replaced function:
#     --count alone misses a replaced function that is only called by another replaced function (C -> C calls bypass the entry jmp), 581 "never executed" functions were mostly that.
#     gdb attaches ~1 s after the process starts: code that runs before that (CRT / engine init) is only seen by --count.
#   - verdict: a Wine "Unhandled" in out_ovl/wine.log = CRASH (the function is named by crashfn.py); the control must not crash; the final screenshots (taken by the script as `final`)
#     differ by imgdiff.py mean (< 30 = same scene; menus are deterministic, races are not: use SCEN_IMGTHRESH=n to change the limit, SCEN_IMGTHRESH=off to skip it for race scenarios).
# SCEN_EXE=<ours.exe> SCEN_DELTA=<hex> [SCEN_RWDLL=..] [NOCACHE=1]: run on a standalone link instead (see the SCEN_EXE block below); control cached in output/scenario/_ctrlcache/.
#   SCEN_PIN=1 [SCEN_PIN_DLL=<proxy build>]: both sides run with the d3d9 proxy and the determinism pins (scen_pin.sh); part of the control cache key.
# Env: NOCTRL=1 (skip the control run: coverage only), REBUILD=1 (rebuild the cached exes in output/scenario/_build), NEVER=file (list of names to report on, default output/never_exec.txt),
#      GDB_EXTRA="course_unload race_load_course_0042fad0" (also break on these original functions),
#      NOGDB=1 (no gdb breakpoints: crash / timing check without the debugger, coverage = counters only),
#      GDBCRASH=1 (gdb dumps registers + stack of the first SIGSEGVs to out_ovl/gdbfault.txt; use with NOGDB=1),
#      RUN_TIMEOUT (s, default 300, scaled by load/4), SLOT=n (local slot index, default 1; the control uses n+1).  Max 2 game processes per call, the global limit of 4 is enforced by gameslot.sh.
# leading NAME=VALUE arguments are exported (replaces an `env NAME=VALUE scenario.sh` prefix, which asks the user for confirmation): scenario.sh SCEN_TAG=x SCEN_MODE=arcade pause_all
while [[ "$1" =~ ^[A-Z][A-Z0-9_]*=.*$ ]]; do export "$1"; shift; done
T=$(dirname "$(readlink -f "$0")"); R=$(readlink -f $T/../..); N=$1; [ "$N" = --ctrl ] || [ -f "$T/scenarios/$N.sh" ] || { echo "usage: scenario.sh <name> (scenarios/*.sh: $(ls $T/scenarios | sed 's/\.sh$//' | tr '\n' ' '))"; exit 2; }
export MASHED_ROOT=${MASHED_ROOT:-$(readlink -f $R/..)}
EXTRA_OVL=${EXTRA_OVL- --automatch --automatch-near --bridges --clusters}
OVL_ARGS="--count$EXTRA_OVL"; SC=${SCEN_OUT:-$R/output/scenario}; B=$SC/_build; W=$SC/$N${SCEN_TAG:+_$SCEN_TAG}; mkdir -p $B; [ "$N" = --ctrl ] || mkdir -p $W
SLOT=${SLOT:-1}; NEVER=${NEVER:-output/never_exec.txt}
# ctrl.exe is (re)built when it is missing, REBUILD=1 is set, or its ctrl.exe.build.json source hashes are not the harness sources of the tree that runs
# this script (any SCEN_OUT).  W12 (harness review finding 1): the old rule kept every ctrl.exe under a SCEN_OUT, and sa_oracles.sh always sets SCEN_OUT,
# so gates ran ours against a control built from the old harness ("kept (lane build)" in 38 logs of gate 1116_0556, never in gate.log).
# A lane that runs a control built from other sources on purpose (a staged freeze.c) says so with SCEN_CTRL_KEEP=1: then it is kept with a loud
# "CTRL STALE" line; inside a gate (MFL_IN_GATE) that is refused.  `scenario.sh --ctrl` runs only this check (sa_oracles.sh calls it once, before its
# parallel scenarios, so the line reaches the gate log).  Built in a private dir and moved in, so parallel calls never run a half-written exe.
# Returns 1 = rebuilt, 0 = kept, 2 = build failed, 3 = stale and refused.
ctrl_srcs_equal() {      # 0 when every source in ctrl.exe.build.json has the tree's content (sha1, 12 hex, as build_overlay.py writes it)
  python3 - $B/ctrl.exe.build.json $R/src <<'PY'
import hashlib, json, os, sys
try: s = json.load(open(sys.argv[1])).get("sources") or {}
except (OSError, ValueError): sys.exit(1)
sys.exit(0 if s and all(os.path.isfile(os.path.join(sys.argv[2], k)) and hashlib.sha1(open(os.path.join(sys.argv[2], k), "rb").read()).hexdigest()[:12] == v for k, v in s.items()) else 1)
PY
}
ctrl_build() {
  if [ -f $B/ctrl.exe ] && [ -z "$REBUILD" ]; then
    ctrl_srcs_equal && return 0
    if [ "$SCEN_CTRL_KEEP" = 1 ]; then
      [ -n "$MFL_IN_GATE" ] && { echo "CTRL STALE: $B/ctrl.exe was not built from this tree's harness sources; SCEN_CTRL_KEEP=1 is refused in a gate" >&2; return 3; }
      echo "CTRL STALE: $B/ctrl.exe was not built from this tree's harness sources (ctrl.exe.build.json): KEPT because SCEN_CTRL_KEEP=1" >&2; return 0
    fi
    echo "ctrl.exe stale ($B/ctrl.exe.build.json differs from $R/src harness sources): rebuilding" >&2
  fi
  local t=$B/.ctrl.$$; rm -rf $t; mkdir -p $t
  python3 $R/tools/build_overlay.py $t/ctrl.exe --no-recomp --harness > $B/build_ctrl.log 2>&1 || { rm -rf $t; return 2; }
  for f in ctrl.exe.map ctrl.exe.build.json ctrl.exe; do [ -f $t/$f ] && mv -f $t/$f $B/$f; done; rm -rf $t; echo "ctrl.exe rebuilt ($B)" >&2; return 1
}
# ctrl_key <script_ctrl.txt>: 16 hex digits over the script, ctrl.exe, the run machinery and the env values that change a run (hx2).
ctrl_key() {
  { cat "$1"; md5sum < $B/ctrl.exe
    for f in scen_play.sh scen_inner.sh memwait.pl memread.pl probe.pl prepare.py gameslot.sh mkpfx.sh; do [ -f $T/$f ] && { echo "== $f"; cat $T/$f; }; done
    [ -n "$SCEN_PIN" ] && { cat $T/scen_pin.sh ${SCEN_PIN_DLL:-$T/../d3dtrace/d3d9.dll} $R/tools/statehash/data/state_pins.txt; }
    echo "env NOAUDIO=$NOAUDIO VBLANK=$VBLANK VIDEO_QUALITY=$VIDEO_QUALITY WDBG=$WDBG SCEN_PIN=$SCEN_PIN"; } | sha1sum | cut -c1-16; }
if [ "$N" = --ctrl ]; then ctrl_build; r=$?; case $r in 2) echo "ctrl build failed ($B/build_ctrl.log)" >&2; exit 1;; 3) exit 1;; esac
  echo "control: $B/ctrl.exe sources $(grep -o '"[^"]*\.[ch]": "[0-9a-f]*"' $B/ctrl.exe.build.json | tr -d '"' | tr '\n' ' ')" >&2; exit 0; fi
# ---- SCEN_EXE mode: run the scenario on a standalone link (delink gate link/ours.exe) instead of the overlay build.
#   SCEN_EXE=<ours.exe> SCEN_DELTA=<hex> [SCEN_RWDLL=<mflrw.dll to copy next to the exe, default: the link's own game/App_Executables/mflrw.dll>] scenario.sh <name>
#   Script addresses: literals in 0x5dd000..0x9f9000 moved by DELTA (addrmove.py, like race_relinked.sh); harness symbols (scen_flags, fast_menu_scale, frames_total...) from <exe>.map minus DELTA
#   so that the same move gives their linked address.  Control = ctrl.exe (harness, original layout); its result (out_ctrl + rc) is cached in $SC/_ctrlcache/<sha of ctrl script+ctrl.exe>/.
#   No counters / gdbcov.  Verdict line "RESULT: PASS|FAIL ..." and exit status 0|1: FAIL on Wine Unhandled, rc != 0, title waitmem not reached (wrong DELTA), final screenshot vs control > SCEN_IMGTHRESH (30).
if [ -n "$SCEN_EXE" ]; then
  [ -n "$SCEN_DELTA" ] || { echo "SCEN_EXE needs SCEN_DELTA=<hex>"; exit 2; }
  EXE=$(readlink -f $SCEN_EXE); XMAP=${EXE%.exe}.map; [ -f "$EXE" ] && [ -f "$XMAP" ] || { echo "missing $EXE or $XMAP"; exit 2; }
  W=$SC/${N}_sa${SCEN_TAG:+_$SCEN_TAG}; rm -rf $W; mkdir -p $W $SC/_ctrlcache
  ctrl_build; [ $? = 2 ] && { echo "ctrl build failed"; exit 1; }
  . $T/scenarios/lib.sh; . $T/scenarios/$N.sh
  # standalone mapva: link-map address minus DELTA (the common addrmove then moves it back); the counter dump of the default footer does not exist in a linked exe
  CMAP=$B/ctrl.exe.map; MAP=$CMAP; export MAP
  { scenario_header; scenario; scenario_footer; } > $W/script_ctrl.txt
  mapva() { local a=$(awk -v n="_$1" '$2==n{print $3; exit}' $XMAP); [ -n "$a" ] || { echo "harness symbol $1 not in $XMAP" >&2; echo 0; return; }; printf "%x" $((0x$a - 0x$SCEN_DELTA)); }
  declare -f scenario_footer | grep -q count_table && scenario_footer() { echo "wait ${SCEN_END_WAIT:-1}"; echo "shot final"; }
  { scenario_header; scenario; scenario_footer; } 2>$W/gen.err | python3 $T/addrmove.py $SCEN_DELTA > $W/script_ours.txt
  grep -q "not in" $W/gen.err && { cat $W/gen.err; echo "RESULT: FAIL (harness symbols missing)"; exit 1; }
  # W12 hx2 (harness review finding 6): the control's cached result depends on everything that makes the control run, not only its script and exe:
  # the driver scripts, the memory helpers, the game-dir preparer, the Wine image (named in scen_play.sh), the pins and the run env.  Key format v2:
  # every store of the older format is ignored from now on (they stay on disk unused; the "-v2-" in the dir name is the explicit invalidation).
  CC=$SC/_ctrlcache/$N-v2-$(ctrl_key $W/script_ctrl.txt)
  INST=$R/../MashedGame/FullyLoaded/installed
  # SCEN_PIN=1 (TS W11): both runs get the d3d9 proxy with V2's determinism pins (wine-run/scen_pin.sh: alock, sound cursor snap, srand, state pins)
  python3 $T/prepare.py $INST $W/run_ours $EXE $XMAP > /dev/null || exit 1
  RW=${SCEN_RWDLL:-$(dirname $EXE)/game/App_Executables/mflrw.dll}; [ -f "$RW" ] && cp -f $RW $W/run_ours/App_Executables/mflrw.dll
  # W15 PREF: SCEN_PREFS=<file> = the MFL_FIX_PREFS settings file of ours (beside its exe); the control never reads one
  [ -n "$SCEN_PREFS" ] && { cp -f $SCEN_PREFS $W/run_ours/App_Executables/mflng_settings.cfg || exit 1; }
  # (ASM review: after the mflrw.dll copy, so scen_pin.sh also renames the DLL's own d3d9 import; before it, the DLL created the device without the proxy)
  [ -n "$SCEN_PIN" ] && { $T/scen_pin.sh $W/run_ours/App_Executables $SCEN_DELTA $(awk '$2=="_srand"{print $3; exit}' $XMAP) || exit 1; }
  rm -rf $W/out_ours; export LOADMAX=${LOADMAX:-40}
  # W13 C4 (finding 9): outer timeout = the script's own limits (the longer of ours / control), load-scaled by the one rule (loadscale.sh), + 90 s
  # for start and shutdown; RUN_TIMEOUT is only a floor now.  It can no longer end a run whose waits are still inside their limits.
  . $T/loadscale.sh; SL=$(script_limit $W/script_ours.txt); SC_=$(script_limit $W/script_ctrl.txt); [ $SC_ -gt $SL ] && SL=$SC_; [ ${RUN_TIMEOUT:-0} -gt $SL ] && SL=$RUN_TIMEOUT
  RT=$(( $(lscale $SL) + 90 )); echo "run limit: ${RT}s (script limits ${SL}s, load factor from $(loadnow))"
  runx() {  # runx <run dir> <script> <out dir> <slot> <rc file>
    PFX=wine_pfx_$4 PERSIST=slot$4 NOSHOW=${NOSHOW-} $T/gameslot.sh timeout -k 5 $RT $T/scen_play.sh $1 $2 $3 > /dev/null 2>&1; echo $? > $5
    # W13 hx7 (finding 17): SCEN_PIN runs log `ALOCK TIMEOUT` in the proxy log; into missed.txt, scen_judge says UNJUDGED for that run
    L=$1/App_Executables/d3dtrace.log; grep -q "^ALOCK TIMEOUT" $L 2>/dev/null && { mkdir -p $3; grep "^ALOCK TIMEOUT" $L >> $3/missed.txt; }; }
  $T/mkpfx.sh $SLOT
  # a cached control of a race_freeze scenario without cars.bin is a stalled run stored by an older script: ignore it
  if [ -f $CC/rc_ctrl ] && [ -z "$NOCACHE" ] && { ! grep -q "^dump cars" $W/script_ctrl.txt || [ -f $CC/out_ctrl/cars.bin ]; }; then echo "control: cached ($CC)"; CACHED=1
  else
    $T/mkpfx.sh $((SLOT+1)); python3 $T/prepare.py $INST $W/run_ctrl $B/ctrl.exe > /dev/null || exit 1; rm -rf $W/out_ctrl
    [ -n "$SCEN_PIN" ] && { $T/scen_pin.sh $W/run_ctrl/App_Executables 0 5d365e || exit 1; }
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
for k_ in ("ours", "ctrl"):      # W13 hx5 (rule 29): a required end event (tag end) that timed out is a MISS: ours FAIL, a control that missed it is no control
    m_ = "%s/out_%s/missed.txt" % (W, k_)
    for l_ in (open(m_, errors="replace").read().splitlines() if os.path.exists(m_) else []):
        if l_.startswith("MISS end"): bad.append("%s missed its end event (%s)" % ("ours" if k_ == "ours" else "control", l_[9:]))
if "dump cars" in open(W + "/script_ctrl.txt").read():      # race_freeze scenario: each side must have reached the freeze (cars.bin), else a stalled run (lost keys) passes
    for k in ("ctrl", "ours"):
        if not os.path.exists("%s/out_%s/cars.bin" % (W, k)): bad.append("%s did not reach the race freeze (no cars.bin, stalled)" % k)
a, b = W + "/out_ctrl/final.png", W + "/out_ours/final.png"
if os.environ.get("SCEN_IMGTHRESH") == "off": print("final screenshot: not judged (SCEN_IMGTHRESH=off)")      # race scenarios: the race picture differs run to run (render oracle covers it)
elif os.path.exists(a) and os.path.exists(b):
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
print("RESULT: " + ("FAIL (%s)" % "; ".join(bad) if bad else "PASS")); sys.stdout.flush()      # W12 hx2 (finding 7): the verdict first, the store cannot cost it
if not bad and not cached and not os.environ.get("NOCACHE"):      # NOCACHE runs (scenset _a/_b pairs) never store
    tmp = "%s.tmp.%d" % (CC, os.getpid())
    try:
        os.makedirs(tmp); shutil.copytree(W + "/out_ctrl", tmp + "/out_ctrl"); shutil.copy(W + "/rc_ctrl", tmp + "/rc_ctrl")
        os.rename(tmp, CC)      # atomic; a store by a parallel run of the same key already there -> keep that one
    except OSError as e:
        print("control store skipped (%s)" % e.__class__.__name__)
    finally:
        shutil.rmtree(tmp, ignore_errors=True)
sys.exit(1 if bad else 0)
PY
  exit ${PIPESTATUS[0]}
fi
ctrl_build; cb=$?; [ $cb = 2 ] && { echo "ctrl build failed"; exit 1; }
if [ ! -f $B/ovl.exe ] || [ $cb = 1 ] || [ -n "$REBUILD" ]; then
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
. $T/loadscale.sh; SL=$(script_limit $W/script_ovl.txt); [ ${RUN_TIMEOUT:-0} -gt $SL ] && SL=$RUN_TIMEOUT; RT=$(( $(lscale $SL) + 90 ))      # W13 C4: as in the standalone path
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
if os.environ.get("SCEN_IMGTHRESH") != "off" and os.path.exists(W + "/out_ctrl/final.png") and os.path.exists(W + "/out_ovl/final.png"):
    sh =subprocess.run(["python3", T + "/imgdiff.py", W + "/out_ctrl/final.png", W + "/out_ovl/final.png"], capture_output=True, text=True).stdout.strip()
print("final screenshot ctrl vs ovl mean diff: %s (limit %s)" % (sh or "n/a", os.environ.get("SCEN_IMGTHRESH", "30")))
if os.path.exists(W + "/out_ctrl/cars.bin") and os.path.exists(W + "/out_ovl/cars.bin"):
    print("cars dump differing bytes (cmpdump, masked fields ignored): " + subprocess.run(["python3", T + "/cmpdump.py", "cars", W + "/out_ctrl/cars.bin", W + "/out_ovl/cars.bin"], capture_output=True, text=True).stdout.strip())
gs = sorted(f for f in os.listdir(W + "/out_ctrl") if f.startswith("gs_") and f.endswith(".bin")) if os.path.isdir(W + "/out_ctrl") else []
if gs:      # GameState dumps (options_all): byte compare ctrl vs ovl, no masks (the overlay section shifts nothing in .data)
    r = [(f[3:-4], subprocess.run(["python3", T + "/cmpdump.py", "gs", W + "/out_ctrl/" + f, W + "/out_ovl/" + f], capture_output=True, text=True).stdout.strip()) for f in gs]
    print("GameState dumps ctrl vs ovl differing bytes: %d dumps, %d differ%s" % (len(r), sum(1 for _, n in r if n not in ("0",)), "".join("  %s=%s" % x for x in r if x[1] != "0")))
print("rc ovl %s ctrl %s" % (open(W + "/rc_ovl").read().strip(), open(W + "/rc_ctrl").read().strip() if os.path.exists(W + "/rc_ctrl") else "-"))
PY
