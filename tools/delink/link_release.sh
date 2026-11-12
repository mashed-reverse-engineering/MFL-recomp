#!/bin/bash
# link_release.sh <work dir>: release exe = the gate's standalone link (gate.sh standalone_link target env) WITHOUT the test harness.
# Result: <work dir>/game/App_Executables/MFL.exe (+ mflrw.dll, MFL.map, DLLS), <work dir>/delta.txt (.data move, hex, for addrmove.py).
# Same A/B switches as gate.sh (LIB_*=orig, DROP_*=0).  Stage 2: AUDIO_DLL=1 (mfl_audio_null.c with -DMFL_AUDIO_DLL: loads
# mfl_audio_miniaudio.dll beside the exe), TRAP=1 (src/release/mfl_trap.c, TRAP_SRC=<file> overrides; entry mfl_release_entry),
# DLLS="<file> ..." copied next to the exe.  REL_CLUSTERS=harness: leave out the clusters the harness link leaves out (fallback).
set -e
T=$(dirname "$(readlink -f "$0")"); G=$T/../..; W=$(readlink -f "$1"); S=$W.src
# ponytail: the LIB_RW group list is read from gate.sh's standalone_link default so the two cannot drift; breaks if that line is reformatted (then move the list into a shared file)
RWDEF=$(grep -o 'lr=${LIB_RW-librw:[^}]*' $T/../gate.sh | head -1); RWDEF=${RWDEF#*LIB_RW-}
[ -n "$RWDEF" ] || { echo "LIB_RW default not found in tools/gate.sh"; exit 1; }
: ${LIB_LUA:=src} ${LIB_QHULL:=src} ${LIB_ZLIB:=src} ${LIB_CRT:=libcmt} ${DROP_DSHOW:=1} ${DROP_RW:=1} ${DROP_RWTRAP:=1} ${DROP_DINPUT:=1}
LIB_RW=${LIB_RW-$RWDEF}
for v in DROP_DSHOW DROP_RW DROP_RWTRAP DROP_DINPUT; do [ "${!v}" = 0 ] && unset $v; done      # link_ours.sh treats any non-empty value as on
export LIB_LUA LIB_QHULL LIB_ZLIB LIB_CRT LIB_RW DROP_DSHOW DROP_RW DROP_RWTRAP DROP_DINPUT MKRANGE=1 GEN_DATA=1 IMPLIBS=1
unset HARNESS RUN DELTA
ARGS="--automatch --automatch-near --bridges --clusters"
mkdir -p $S
if [ -n "$AUDIO_DLL" ]; then { echo "#define MFL_AUDIO_DLL"; cat $G/src/audio/mfl_audio_null.c; } > $S/mfl_audio_rel.c
  ARGS="$ARGS --src=$S/mfl_audio_rel.c"; export SKIP_FILES=mfl_audio_null.c${SKIP_FILES:+,$SKIP_FILES}; fi
[ -n "$TRAP" ] && { ARGS="$ARGS --src=${TRAP_SRC:-$G/src/release/mfl_trap.c}"; export LINKFLAGS="$LINKFLAGS /entry:mfl_release_entry"; }
[ "$REL_CLUSTERS" = harness ] && export CLUSTER_ENABLED=$(grep -v '^#' $G/src/clusters/enabled.txt | awk 'NF{print $1}' | grep -v -x -F -f <(grep -v '^#' $G/src/harness/cluster_exclude.txt | awk 'NF{print $1}' | grep -v -x -e app_main_loop -e message_box_update) | paste -sd,)
$T/link_ours.sh $W 0x400000 $ARGS
A=$W/game/App_Executables
objdump -h $W/ours.exe | python3 -c "import sys; print('%x' % next(int(l.split()[3], 16) - 0x5fb000 for l in sys.stdin if len(l.split()) > 3 and l.split()[1] == '.data'))" > $W/delta.txt
cp $W/ours.map $A/MFL.map
for f in $DLLS; do cp $f $A/; done
cmp -s $W/ours.exe $A/MFL.exe && echo "release exe = ours.exe (prepare.py patched nothing)" || echo "WARNING: prepare.py patched the exe copy (an original movie function is still linked): list it in the notes"
[ -n "$TRAP" ] && { objdump -f $W/ours.exe | grep start; awk '$2=="_mfl_release_entry"{print "map", $3}' $W/ours.map; }
echo "release: $A (delta $(cat $W/delta.txt))"
