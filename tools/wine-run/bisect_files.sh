#!/bin/bash
# Bisect which src/*.c file breaks the deterministic run.  Usage: bisect_files.sh <regress work dir> <skip list, comma separated names>
# CRIT (env, default "cars") = the dumps that decide OK/BAD: cars has zero control-vs-control noise, the others vary by a few bytes.
# OVL_EXTRA (env) is passed to build_overlay.py (e.g. "--replace f1,f2" to patch only those functions).
# Builds the overlay without the listed files (SKIP_FILES) and compares cars/vehicles/aibots/statsrace/powerups with the control dumps of the work dir.
W=$(readlink -f "$1"); SKIP="$2"
# params.env (written by regress.sh): the control dumps are only comparable with the same frame count / course / drive / flags
if [ -f $W/params.env ]; then
  . $W/params.env
  chk() { [ -z "$2" ] || [ "$2" = "$3" ] || { echo "ERROR (parameter mismatch: $1 is '$2' but the control in $W was made with '$3')"; exit 2; }; }
  chk FRAMES "$FRAMES" "$P_FR"; chk COURSE "$COURSE" "$P_COURSE"; chk DRIVE "$DRIVE" "$P_DRIVE"; chk NOLIMIT "$NOLIMIT" "$P_NOLIMIT"; chk OVL_ARGS "$OVL_ARGS" "$P_OVL_ARGS"
  : ${FRAMES:=$P_FR}; : ${COURSE:=$P_COURSE}; [ -n "$P_DRIVE" ] && : ${DRIVE:=$P_DRIVE}; [ -n "$P_NOLIMIT" ] && : ${NOLIMIT:=$P_NOLIMIT}; : ${OVL_ARGS:=$P_OVL_ARGS}; export FRAMES COURSE DRIVE NOLIMIT OVL_ARGS
fi
[ -n "$SLOT" ] && export VC7_PFX=wine_pfx_cc$SLOT      # every slot compiles with its own Wine prefix (concurrent wineservers on one prefix fail)
X=${SLOT:+_$SLOT}      # SLOT=n: an independent test (own run dir, container and Wine prefix), several can run at the same time
T=$(dirname "$(readlink -f "$0")")
mkdir -p $T/../../live; echo $W > $T/../../live/current      # live.sh / LIVE.md follow the group search too
# SAMELAYOUT=1 (set it when the control was built with it, e.g. by courses.sh): compile ALL files (same section size and heap layout as the control) and patch only
# the functions of the files that are not in the skip list; the control and b.exe must also agree on --count (OVL_ARGS)
SAMELAYOUT=${SAMELAYOUT-${P_SAMELAYOUT:-1}}      # default: what the regress run that made the control used (params.env)
if [ "$SAMELAYOUT" = 1 ]; then
  export SKIP_FILES=""
  case "$OVL_EXTRA" in *--replace*) ;; *) OVL_EXTRA="$OVL_EXTRA --replace $(python3 - "$T/../../src" "$SKIP" <<'PY'
import glob, os, re, sys
d, skip = sys.argv[1], set(sys.argv[2].split(","))
names = []
for f in sorted(glob.glob(d + "/*.c") + glob.glob(d + "/auto/ok_*.c")):
    if os.path.basename(f) in skip: continue
    names += re.findall(r"^(?:__declspec\(naked\)\s+)?[A-Za-z_][\w\s\*]*?\b(\w+)\([^;{]*\)\s*\n\{", open(f, errors="replace").read(), re.M)
print(",".join(sorted(set(names)) or ["__none__"]))
PY
)";; esac
  AUTO_FLAG="${AUTO_FLAG---no-auto}"      # chunks of src/auto are opt-in: AUTO_FLAG=--auto
else export SKIP_FILES="$SKIP"; fi
python3 $T/../build_overlay.py $W/b$X.exe --harness ${AUTO_FLAG---no-auto} ${SAMELAYOUT:+$OVL_ARGS} $OVL_EXTRA >/dev/null 2>$W/b$X.err || { echo "BUILDFAIL"; exit 1; }
va=$(grep freeze_target $W/b$X.exe.map | cut -d' ' -f2)
[ -n "$SLOT" ] && $T/mkpfx.sh 9$SLOT
rm -rf $W/run_b$X $W/out_b$X $W/prep_b$X; $T/clone.sh $W/snap_ovl $W/run_b$X
python3 $T/prepare.py $T/../../../MashedGame/FullyLoaded/installed $W/prep_b$X $W/b$X.exe >/dev/null
cp $W/prep_b$X/App_Executables/MFL.exe $W/run_b$X/App_Executables/MFL.exe
rf=$(grep race_frames $W/b$X.exe.map | cut -d' ' -f2)
$T/race_script.sh 0x$va 0x$rf ${FRAMES:-150} $W/b$X.exe.map ${MODE:-fast} > $W/script_b$X.txt   # FRAMES / MODE must match the regress run that made the control dumps
PFX=${SLOT:+wine_pfx_9$SLOT} PERSIST=bis$SLOT ${PLAYT:+timeout -k 5 $PLAYT} $T/play.sh $W/run_b$X $W/script_b$X.txt $W/out_b$X >/dev/null 2>&1
# the game must have started at all (play_inner.sh logs "process up"): otherwise this is an infrastructure failure, not a result
grep -q "process up" $W/out_b$X/timing.txt 2>/dev/null || { echo "ERROR (the game did not start)"; exit 2; }
n=0
# CRIT=shot:<name> (e.g. shot:menu_single): BAD when that screenshot differs from the control run's by more than 10 (mean pixel difference, imgdiff.py); needs a regress dir with screenshots
# CRIT=exists:<name>: BAD when the run did not produce <name>.png (a scenario that hangs never gets to its last screenshot; use PLAYT=<seconds> to bound the run)
case "${CRIT:-cars}" in exists:*) [ -f $W/out_b$X/${CRIT#exists:}.png ] && echo "OK" || echo "BAD (no ${CRIT#exists:} screenshot)"; exit 0;; esac
# CRIT=moves:<a>:<b>: BAD when the screenshots <a> and <b> of the run are (nearly) the same: the game stands still (hang in a later round)
case "${CRIT:-cars}" in moves:*) IFS=: read -r _ ma mb <<< "$CRIT"; [ -f $W/out_b$X/$mb.png ] || { echo "BAD (no $mb screenshot)"; exit 0; }
  d=$(python3 $T/imgdiff.py $W/out_b$X/$ma.png $W/out_b$X/$mb.png); [ ${d%.*} -ge 1 ] && echo "OK diff=$d" || echo "BAD still diff=$d"; exit 0;; esac
case "${CRIT:-cars}" in shot:*)
  nm=${CRIT#shot:}; [ -f $W/out_b$X/$nm.png ] || { echo "BAD (no $nm screenshot)"; exit 1; }
  d=$(python3 $T/imgdiff.py $W/out_ctrl_1/$nm.png $W/out_b$X/$nm.png); [ ${d%.*} -gt 10 ] && echo "BAD diff=$d" || echo "OK diff=$d"; exit 0;; esac
for f in ${CRIT:-cars}; do
  if [ ! -f $W/out_b$X/$f.bin ]; then
    # no dump: tell a real crash / hang from an infrastructure failure (an early death of about 3 s is not a result and used to be cached as BAD)
    if grep -q "Unhandled\|page fault" $W/out_b$X/wine.log 2>/dev/null; then
      cf=$(python3 $T/crashfn.py $W out_b$X b$X.exe.map 2>/dev/null | tail -1 | cut -c1-80); echo "BAD (crash: $cf)"; exit 1
    fi
    last=$(grep -E '^[0-9]' $W/out_b$X/steps.txt 2>/dev/null | tail -1 | cut -d' ' -f1); lastn=$(grep -E '^[0-9]' $W/out_b$X/steps.txt 2>/dev/null | tail -1 | cut -d' ' -f2-3)
    if [ -n "$last" ] && [ ${last%.*} -ge 40 ]; then echo "BAD (stalled after ${last}s at: $lastn)"; exit 1; fi
    echo "ERROR (no $f dump after ${last:-0}s, no crash line: infrastructure failure, repeated)"; exit 2
  fi
  n=$((n + $(python3 $T/cmpdump.py $f $W/out_ctrl_1/$f.bin $W/out_b$X/$f.bin)))
done
# THRESH (env, default 0): differences up to this many bytes count as noise (courses with real-time driven animation differ by ~100 bytes between identical runs)
[ $n -le ${THRESH:-0} ] && echo "OK diff=$n" || echo "BAD diff=$n"
