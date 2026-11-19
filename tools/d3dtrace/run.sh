#!/bin/bash
# run.sh <scenario> <out dir> [frames]: record a D3D9 draw trace of one scripted MFL.exe run (ctrl build = original code + the freeze harness) with the
# d3d9 proxy (d3d9.dll next to this script, built by `make`) in the wine-run harness.  ONE game process, through gameslot.sh (GAME_SLOTS).
# Scenarios: menu | title (FR frames_total frames of title/menu start, frame-locked) | cup | course:<N> (quick battle, course page moved down N times, see wine-run/race_script.sh) | script:<file> (own play.sh script, no freeze)
# Env: SEG (race|countdown), DRIVE (scen_flags mask, default 9 = accelerate + opponents frozen), SCEN_ROW/SCEN_RIGHT (cup), PHASE_MIN (first race phase logged, default 5 = countdown),
#      GEOM (1/0 hash vertex/index buffers), SLOT (local slot name, default 1: the global slot decides), CTRL_BUILD (dir with ctrl.exe + ctrl.exe.map),
#      STEP_US=<us> (virtual clock advance per main-thread clock call, d3d9.c step_us, default 20; 0 = the clock moves only by Sleep / vsnap),
#      ALOCK=1 (worker threads in lockstep, d3d9.c alock), SNDPIN=1 (with ALOCK=1: virtual dsound cursor snapped at race-phase changes + apoll/aio/vst,
#      control vs control 0 diff over a race, lane v2_w11), D3DP_DLL (proxy build to use instead of d3d9.dll here),
#      MFL_INSTALL (install dir with App_Executables), MAX_FRAMES, RUN_TIMEOUT (s, default 240), TMPDIR (scratch mounted into the containers),
#      AUDIORNG=1 (control only, test only: sound code draws from its own random stream, d3d9.c audiorng),
#      TRACE=1 (with STATEHASH=1: also record and normalise the draw trace; without it a statehash run has no trace.d3d.gz).
# SA_EXE=<link dir>/ours.exe SA_DELTA=<hex> (e.g. <link dir>/ours.exe, 112000): record the standalone link (game dir <link dir>/game, harness addresses from ours.map) instead of the ctrl build.
# Result: <out dir>/trace.raw (change stream), <out dir>/run.txt (params, exit code), screenshots (wine-run shots) when taken.  Then: normalise.py trace.raw.
T=$(dirname "$(readlink -f "$0")"); R=$(readlink -f "$T/../.."); WR=$(readlink -f "$T/../wine-run")
# N1: STATEHASH=1 -> statehash=1 in the cfg (sh_delta = SA_DELTA), result <out dir>/statehash.bin (tools/d3dtrace/statecmp.py); SH_POKE=<frame>:<va>:<xor> perturbs one dword
# gate exclusion: refuse while a gate holds the gate lock file (as run.py does for tools/wine-run)
. "$(dirname "$(readlink -f "$0")")/../wine-run/winelock.sh"      # W13 hx6: shared gate lock for the whole run (was: refuse while a gate runs); waits for a running gate
SC=${1:?scenario}; OUT=$(readlink -f "${2:?out dir}"); FR=${3:-120}
export TMPDIR=${TMPDIR:-/tmp/claude-$(id -u)}; export MASHED_ROOT=${MASHED_ROOT:-$(readlink -f "$R/..")}
MFL_INSTALL=${MFL_INSTALL:-$R/../MashedGame/FullyLoaded/installed}
CTRL_BUILD=${CTRL_BUILD:-$R/output/scenario/_build}
mkdir -p "$OUT"; W=$OUT/work; rm -rf "$W"; mkdir -p "$W"
if [ -n "$SA_EXE" ]; then :; elif [ ! -f $CTRL_BUILD/ctrl.exe ] || [ ! -f $CTRL_BUILD/ctrl.exe.map ]; then
  CTRL_BUILD=$TMPDIR/d3dtrace/_build; mkdir -p $CTRL_BUILD
  [ -f $CTRL_BUILD/ctrl.exe ] || python3 $R/tools/build_overlay.py $CTRL_BUILD/ctrl.exe --no-recomp --harness > $CTRL_BUILD/build.log 2>&1 || { echo "ctrl build failed"; exit 1; }
fi
if [ -n "$SA_EXE" ]; then   # standalone: placeholder map (0x7f0000xx) + translation of every script address by SA_DELTA, as tools/delink/state_compare.sh
  SA_EXE=$(readlink -f $SA_EXE); SA_DELTA=${SA_DELTA:?SA_DELTA}; LM=${SA_EXE%.exe}.map; : > $W/fake.map; i=0
  for n in freeze_target race_frames frozen scen_flags fast_menu_scale fast_nolimit fast_noshow frames_total count_table count_bytes; do
    r=$(awk -v n="_$n" '$2==n {print $3; exit}' $LM); [ -n "$r" ] && echo "$n 7f$(printf %06x $((i*16))) $r" >> $W/fake.map; i=$((i+1)); done
  awk '{print $1, $2}' $W/fake.map > $W/map_fake.txt; MAP=$W/map_fake.txt; export MAP
  satr() { python3 -I -c "
import re, sys
d = int('$SA_DELTA', 16); real = {'0x' + l.split()[1]: '0x' + l.split()[2] for l in open('$W/fake.map')}
def mv(m):
    v = int(m.group(0), 16)
    if m.group(0).lower() in real: return real[m.group(0).lower()]
    return '0x%x' % (v + d) if 0x5dd000 <= v < 0x9f9000 else m.group(0)
for l in sys.stdin: sys.stdout.write(re.sub(r'0x[0-9a-fA-F]+', mv, l))"; }
else cp $CTRL_BUILD/ctrl.exe $CTRL_BUILD/ctrl.exe.map $W/; MAP=$W/ctrl.exe.map; export MAP; satr() { cat; }; fi
mapva() { grep "^$1 " $MAP | cut -d' ' -f2; }
DLL=${D3DP_DLL:-$T/d3d9.dll}   # D3DP_DLL=<file>: a test build of the proxy instead of the shared one
[ -f $DLL ] || make -C $T >/dev/null || { echo "no d3d9.dll (make failed)"; exit 1; }
if [ -n "$SA_EXE" ]; then $WR/clone.sh $(dirname $SA_EXE)/game $W/run; [ -f $W/run/App_Executables/MFL.exe ] || exit 1; else python3 $WR/prepare.py $MFL_INSTALL $W/run $W/ctrl.exe >/dev/null || exit 1; fi
AE=$W/run/App_Executables
# play.sh hard-codes WINEDLLOVERRIDES (no d3d9=n,b possible without editing it, and Wine prefers its builtin d3d9 over a native file in the app dir):
# the import of the run copy of MFL.exe is renamed d3d9.dll -> d3dp.dll and the proxy is installed under that name (it loads the real d3d9 from system32)
python3 $T/patch_import.py $AE/MFL.exe d3dp.dll || exit 1
cp $DLL $AE/d3dp.dll
START=${PHASE_MIN:-5}; STOP=0; RF=0x$(mapva race_frames)
PHA=; [ -n "$SA_EXE" ] && { PHA=$(printf 0x%x $((0x652f68 + 0x$SA_DELTA))); RF=$(awk '$1=="race_frames"{print "0x"$3}' $W/fake.map); }
STOPPH=0; [ "$SC" = title ] && MAX_FRAMES=${MAX_FRAMES:-$FR}
case $SC in menu|title|mainmenu) START=0; RF=0;; *) STOP=$FR;; esac
# SEG=countdown: log only race phase 5 (loading is skipped, camera fly-over of the finished course: deterministic, byte-identical between runs);
# default SEG=race logs phases 5+6 up to race frame <frames> (the race itself diverges slightly between runs, see README: use diff.py --mask)
[ "${SEG:-race}" = countdown ] && { STOPPH=6; STOP=0; }
# SPLIT=1 = the audio/gameplay split acceptance form (REP 2026-11-16: control pairs ar0_3 / ar0_4 IDENTICAL only with STEP_US=0): the control gets
# AUDIORNG=1, ours (SA_EXE: a link built with MFL_FIX_DEFINES=/DMFL_FIX_DETERMINISTIC_AUDIO_RNG=1) no shim; both sides default STEP_US=0.
# AUDIORNG=1 alone also defaults STEP_US to 0.  An explicit STEP_US wins.
[ -n "$SPLIT" ] && [ -z "$SA_EXE" ] && AUDIORNG=1
[ -n "$SPLIT$AUDIORNG" ] && STEP_US=${STEP_US:-0}
{ echo "out=d3dtrace.log"; echo "start_phase=$START"; echo "racef_addr=$RF"; echo "stop_racef=$STOP"; echo "stop_phase=$STOPPH"; echo "geom=${GEOM:-1}"; [ -n "$PHA" ] && { echo "phase_addr=$PHA"; echo "anchor_phase=${ANCHOR:-3}"; }; [ -n "$MAX_FRAMES" ] && echo "max_frames=$MAX_FRAMES"; [ -n "$VDUMP" ] && echo "vdump=1"; [ -n "$STATEHASH" ] && { echo "statehash=1"; echo "sh_delta=${SA_DELTA:-0}"; echo "vsnap=${VSNAP:-2}"; [ "${SRAND:-1}" != 0 ] && echo "srand=3:$([ -n "$SA_EXE" ] && awk '$2=="_srand"{print $3; exit}' ${SA_EXE%.exe}.map || echo 5d365e):1"; for p in $PINS $([ -n "$PINFILE" ] && grep -v '^#' $PINFILE | cut -d' ' -f1); do echo "pin=$p"; done; }; [ -n "$SH_POKE" ] && echo "sh_poke=$SH_POKE"; [ -n "$ALOCK" ] && echo "alock=$ALOCK"; [ -n "$SNDPIN" ] && printf 'vds=1\napoll=1\naio=1\nvst=1\nvdssnap=1\n'; [ -n "$STEP_US" ] && echo "step_us=$STEP_US"; [ -n "$STATEHASH" ] && [ -z "$TRACE" ] && echo "trace=0"; } > $AE/d3dtrace.cfg
# W13 hx7 (harness review finding 16): a STATEHASH run without TRACE=1 records no draw / state lines (d3d9.c trace=0; the PRESENT lines, the
# ALOCK TIMEOUT lines and statehash.bin stay) and skips normalise.  Not max_frames=1: that also ends the state hash (d3d9.c records it only while active).
# RWTAP=1: also hook the RW layer (tools/rwtap, mfltap.dll + funcs.txt next to the exe), log -> $OUT/rwtap.log (standalone: the hooks get the link delta)
RT_=$(readlink -f $T/../rwtap); [ -n "$RWTAP" ] && { [ -f $RT_/mfltap.dll ] || make -C $RT_ >/dev/null || exit 1; cp $RT_/mfltap.dll $RT_/funcs.txt $AE/; echo "rwtap=funcs.txt" >> $AE/d3dtrace.cfg; [ -n "$SA_DELTA" ] && echo "rwtap_delta=$SA_DELTA" >> $AE/d3dtrace.cfg; }
# AUDIORNG=1 (TEST ONLY, control build only; output/determinism_2026-11-16.md): d3d9.c audiorng, the sound code's random draws go to the sound
# stream (in memory, sites from tools/statehash/data/audio_rng_sites.txt).  Ours gets the same split from MFL_FIX_DETERMINISTIC_AUDIO_RNG, not from here.
if [ -n "$AUDIORNG" ]; then
  [ -n "$SA_EXE" ] && { echo "run.sh: AUDIORNG is for the control build only (ours: build with MFL_FIX_DETERMINISTIC_AUDIO_RNG=1)"; exit 2; }
  echo "audiorng=1" >> $AE/d3dtrace.cfg; awk '/^[0-9a-f]{8} /{print "audiorng_site=" $1 ":" $2}' $R/tools/statehash/data/audio_rng_sites.txt >> $AE/d3dtrace.cfg
fi
# ---- the play.sh script
DRIVE=${DRIVE-9}
case $SC in
  course:*) COURSE=${SC#course:} DRIVE=$DRIVE NOSHOW=${NOSHOW-0} SHOTS=${SHOTS-0} $WR/race_script.sh 0x$(mapva freeze_target) 0x$(mapva race_frames) $FR $MAP fast | satr > $W/script.txt ;;
  cup)  . $WR/scenarios/lib.sh; . $WR/scenarios/cup.sh; SCEN_DRIVE=$DRIVE
        { scenario_header; SCEN_FRAMES=$FR scenario; echo "wait 1"; echo "shot final"; } | satr > $W/script.txt ;;
  title) . $WR/scenarios/lib.sh; { scenario_header; echo "waitframes 0x$(mapva frames_total) $((FR+40)) 90"; echo "shot final"; } | satr > $W/script.txt ;;   # title screen, frozen by frame count (frames_total + FR): comparable between runs
  menu) . $WR/scenarios/lib.sh; . $WR/scenarios/menus_main.sh
        { scenario_header; scenario; echo "shot final"; } | satr > $W/script.txt ;;
  mainmenu) . $WR/scenarios/lib.sh; { scenario_header; to_single; to_main; echo "wait 3"; echo "shot final"; } | satr > $W/script.txt ;;   # main menu at rest (Game Type Select)
  script:*) cp ${SC#script:} $W/script.txt ;;
  *) echo "unknown scenario $SC"; exit 2 ;;
esac
# FRAMED=1 (course:N): race_script.sh prints the menu path as mkbd= / mstep= lines for the proxy (d3d9.c mstep_frame): they go to the cfg, not to play.sh
grep -E '^(mkbd|mstep)=' $W/script.txt >> $AE/d3dtrace.cfg; sed -i -E '/^(mkbd|mstep)=/d' $W/script.txt
case $SC in menu|title|mainmenu) ;; *) echo "dump course $(printf 0x%x $((0x69cfb0 + 0x${SA_DELTA:-0}))) 4" >> $W/script.txt ;; esac     # g_menu_event_course_index -> game/course.bin (little endian dword)
# every run starts from the same snapshot (the game rewrites its saves): warm prefix marker as in regress.sh
[ -f $HOME/.cache/mfl_warm_done ] || { printf 'wait 14\nkey Return\nwait 2\n' > $W/warm.txt; PERSIST=slot${SLOT:-1} $WR/gameslot.sh timeout -k 5 90 $WR/play.sh $W/run $W/warm.txt $W/out_warm >/dev/null 2>&1; touch $HOME/.cache/mfl_warm_done; }
$WR/mkpfx.sh ${SLOT:-1}
. $WR/loadscale.sh; RT=$(lscale ${RUN_TIMEOUT:-240}); echo "run limit: ${RT}s (RUN_TIMEOUT ${RUN_TIMEOUT:-240}s, load $(loadnow), cap ${LOADCAP:-3})"      # W14 A2: the one load rule (wine-run/loadscale.sh), was its own awk copy (load1 only)
T0=$(date +%s)
PFX=wine_pfx_${SLOT:-1} PERSIST=slot${SLOT:-1} $WR/gameslot.sh timeout -k 5 $RT $WR/play.sh $W/run $W/script.txt $OUT/game >/dev/null 2>&1; RC=$?
cp $AE/d3dtrace.log $OUT/trace.raw 2>/dev/null; [ -n "$STATEHASH" ] && mv $AE/statehash.bin $OUT/statehash.bin;[ -n "$RWTAP" ] && cp $AE/rwtap.log $OUT/rwtap.log 2>/dev/null
{ echo "scenario=$SC frames=$FR drive=$DRIVE rc=$RC seconds=$(( $(date +%s) - T0 )) dll=$(md5sum < $DLL | cut -c1-12) exe=$(md5sum < ${SA_EXE:-$W/ctrl.exe} | cut -c1-12)"; grep -c . $OUT/trace.raw 2>/dev/null; } | tee $OUT/run.txt
[ -s $OUT/trace.raw ] || { echo "NO TRACE (override not active or game failed): see $OUT/game/wine.log"; exit 1; }
grep "^ALOCK TIMEOUT" $OUT/trace.raw | head -3      # W13 hx7 (finding 17): lockstep lost in this run; statecmp.py says UNJUDGED for it
[ -n "$STATEHASH" ] && [ -z "$TRACE" ] && exit 0
python3 $T/normalise.py $OUT/trace.raw $OUT/trace.d3d.gz
