#!/bin/bash
# rw_g0_check.sh <mflrw.dll> [N=100000]: bit-exact check of the librw facade DLL's G0 functions (tools/delink/rw_g0.txt + rw_g0_candidates.txt) against the original code of MFL.exe at CW 0x7f.
# Unicorn (original) writes the vectors, the mingw replay exe runs the DLL under Wine (docker run --rm, a fresh container: no mfl_* container is touched).  Exit 1 on any DIFF.
set -e
T=$(dirname "$(readlink -f "$0")"); DLL=$(readlink -f "$1"); N=${2:-100000}; O=${OUT:-/tmp/rw_g0_check}; mkdir -p $O
python3 $T/rw_g0_check.py gen $O/vec.bin $N | tail -40
i686-w64-mingw32-gcc -O1 -mfpmath=387 -w -o $O/g0_replay.exe $T/rw_g0_replay.c
cp $DLL $O/mflrw.dll
docker run --rm -e XDG_RUNTIME_DIR=/tmp/xdg -e WINEDEBUG=-all -v $O:/t tianon/wine:latest wine /t/g0_replay.exe Z:\\t\\mflrw.dll Z:\\t\\vec.bin 2>&1 | grep -v -E "^error: XDG|fixme" | tee $O/replay.log
! grep -q -E "DIFF|MISSING" $O/replay.log
