#!/bin/bash
# rw_rt2d_check.sh <mflrw.dll> [N=100000] [names|all] [neg|fld]: the rt2d group exports of the DLL (_rt2dCTMGet, Rt2dPath Empty / Moveto / Lineto / Close / Destroy /
# GetBBox, _rt2dFlatSubPathExtendBBox, Rt2dBrushCreate; librw-mfl capi/rwrt2d.cpp) vs the original code of MFL.exe (mapped at its image base), both run natively
# under Wine in a fresh container (docker run --rm: no mfl_* container touched).  x87 CW 0x7f.
# Exit 1 on any finite DIFF, a REGDIFF, a TIMEOUT or a CRASH.  neg = the negative fixture (one input changed on the DLL side): must exit 1 with DIFF on every entry;
# fld = the planted fixture (unbalanced fld1 in front of the DLL entry): REGDIFF on every entry (negtest.sh).  Output dir: $OUT (default /tmp/rw_rt2d_check);
# $EXE overrides the MFL.exe path.  Same mechanism as rw_rwmath_check.sh.
set -e
T=$(dirname "$(readlink -f "$0")"); DLL=$(readlink -f "$1"); N=${2:-100000}; O=${OUT:-/tmp/rw_rt2d_check}; E=${EXE:-$T/../../game/MFL.exe}; mkdir -p $O
i686-w64-mingw32-gcc -O1 -mfpmath=387 -w -Wl,--image-base,0x400000 -Wl,--disable-dynamicbase -o $O/rt2d_check.exe $T/rw_rt2d_check.c
cp $DLL $O/mflrw.dll; cp $E $O/MFL.exe
docker run --rm -e XDG_RUNTIME_DIR=/tmp/xdg -e WINEDEBUG=-all -v $O:/t tianon/wine:latest wine /t/rt2d_check.exe Z:\\t\\mflrw.dll Z:\\t\\MFL.exe $N ${3:-all} $4 2>&1 | grep -v -E "^error: XDG|fixme" | tee $O/replay.log
if grep -q -E "DIFF|MISSING|cannot|TIMEOUT|CRASH" $O/replay.log; then exit 1; fi      # (not `! grep`: set -e ignores a negated command, the exit would be 0)
grep -q "^DONE" $O/replay.log
