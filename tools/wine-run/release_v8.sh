#!/bin/bash
# release_v8.sh <v143 link dir> <out dir>: V8 of the W17 plan (4.10, lane HEAP), a step of `gate.sh full`.
# The play smoke of the CI release exe of the TREE under Wine's heap checks: <v143 link dir> = the RET step's link (tools/gate/v143_ret.sh link:
# link_ci.sh CC=v143 FIXES=all AUDIO_DLL=1, LIB_CRT=v143md = the shipped MFLng.exe), run as tools/ci/smoke_release.py --mode play --heap
# (Quick Battle race, Escape pause, Continue, Quit Game; WINEDEBUG=fixme-all,warn+heap). V8_NOFIX=1 also links MFLng.nofix.exe (FIXES empty,
# +~5 min CPU inside the step) and smokes it.
# FAIL on any warn:heap / err:heap / Unhandled / page fault line, mflng-crash.txt or a non-ok play verdict (release_smoke.sh's rules).
# Plain Wine tolerates an interior free (r20261120-e2ddfb7 passed the play smoke without --heap): the heap channel makes V8 a detector.
# DLLs beside the exe: mflrw.dll = output/librw/mflrw.dll (the pinned build: link_ours.sh's RW_DLL default, the file V7 ships), miniaudio =
# tools/audio/work/out/null/mfl_audio_miniaudio.dll (V7's); no video DLLs (the CI zip carries none). Game dir: prepare.py of the install (hardlinks).
# Prints one "V8: PASS ..." / "V8: FAIL <why>" line last. Exit 0 PASS, 1 FAIL.
WR=$(dirname "$(readlink -f "$0")"); G=$WR/../..
[ $# = 2 ] || { sed -n 2,6p "$0"; exit 2; }
L=$(readlink -f "$1"); mkdir -p "$2"; O=$(readlink -f "$2")
[ -f $L/ours.exe ] && [ -f $L/ours.map ] || { echo "V8: FAIL no v143 link in $L (ours.exe / ours.map)"; exit 1; }
rm -rf $O/rel $O/maps $O/work; mkdir -p $O/rel $O/maps
cp $L/ours.exe $O/rel/MFLng.exe; cp $L/ours.map $O/maps/MFLng.map; X=MFLng.exe
if [ "${V8_NOFIX:-0}" = 1 ]; then
  CC=v143 AUDIO_DLL=1 $G/tools/delink/link_ci.sh $O/nofix > $O/link_nofix.log 2>&1 || { tail -5 $O/link_nofix.log; echo "V8: FAIL nofix link (log $O/link_nofix.log)"; exit 1; }
  cp $O/nofix/ours.exe $O/rel/MFLng.nofix.exe; cp $O/nofix/ours.map $O/maps/MFLng.nofix.map; X=$X,MFLng.nofix.exe
fi
cp $G/output/librw/mflrw.dll $G/tools/audio/work/out/null/mfl_audio_miniaudio.dll $O/rel/ || { echo "V8: FAIL DLLs missing (output/librw/mflrw.dll, tools/audio/work/out/null/mfl_audio_miniaudio.dll)"; exit 1; }
echo "cl gate-local v143 $(git -C $G rev-parse --short HEAD)" > $O/rel/BUILDINFO.txt      # smoke_release.py's smoke_ok line reads it
python3 $WR/prepare.py $G/../MashedGame/FullyLoaded/installed $O/game > $O/prepare.log 2>&1 || { tail -3 $O/prepare.log; echo "V8: FAIL prepare.py (log $O/prepare.log)"; exit 1; }
python3 $G/tools/ci/smoke_release.py $O/rel $O/maps $O/game $O/work --mode play --heap --exes $X > $O/smoke.log 2>&1; rc=$?; cat $O/smoke.log
[ $rc = 0 ] && { echo "V8: PASS ($X play smoke ok under warn+heap; dir $O)"; exit 0; }
echo "V8: FAIL play smoke ($(grep -c FAIL $O/smoke.log) failed of $X; log $O/smoke.log)"; exit 1
