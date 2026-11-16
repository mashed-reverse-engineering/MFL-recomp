#!/bin/bash
# one.sh <tag> <course> [frames]: one STATEHASH run -> $SH_WORK/runs/<tag>/statehash.bin (+ <tag>.log).
# Env: CTRL_BUILD (REQUIRED: dir with ctrl.exe + ctrl.exe.map, e.g. <gate>/state), SA_EXE/SA_DELTA (standalone link instead of the ctrl build),
#      SH_POKE=<frame>:<va>:<xor>, DRIVE (default 2079 = 31 | 2048 intro runs to its end), PINFILE (default data/state_pins_c<course>.txt, else data/state_pins.txt), SH_WORK (default <repo>/output/statehash).
# Starts Wine through tools/d3dtrace/run.sh (refuses while a gate holds the lock): main-session / wine-phase job.
T=$(dirname "$(readlink -f "$0")"); R=$(readlink -f "$T/../.."); W=${SH_WORK:-$R/output/statehash}
TAG=${1:?tag}; C=${2:?course}; FR=${3:-120}
[ -n "$SA_EXE" ] || : ${CTRL_BUILD:?set CTRL_BUILD=<dir with ctrl.exe + ctrl.exe.map> or SA_EXE=<link dir>/ours.exe}
mkdir -p "$W/runs"
# W12: the pin set is per course: data/state_pins_c<course>.txt when it exists (mkpinfile.sh <course>), else the course-0 set data/state_pins.txt
# (with a note: its menu residue values are course-0 values).  PINFILE=/dev/null = no pins (reference runs for mkpinfile.sh).
PC=$T/data/state_pins_c$C.txt; [ "$C" != 0 ] && [ ! -f "$PC" ] && [ -z "$PINFILE" ] && echo "note: no $PC, course $C uses the course-0 pins" >&2
[ -f "$PC" ] || PC=$T/data/state_pins.txt
# DRIVE default 31 + 2048 (freeze.c: no menu_select input in race phase 3, so the intro fly-over always runs to its natural end; REP ld7 W12)
export STATEHASH=1 DRIVE=${DRIVE:-2079} GEOM=0 PINFILE=${PINFILE:-$PC}
"$R/tools/d3dtrace/run.sh" course:$C "$W/runs/$TAG" $FR > "$W/runs/$TAG.log" 2>&1 || { echo "run failed"; tail -3 "$W/runs/$TAG.log"; exit 1; }
tail -2 "$W/runs/$TAG.log"
