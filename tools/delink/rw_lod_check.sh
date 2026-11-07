#!/bin/bash
# rw_lod_check.sh <mflrw.dll | lrintf> [N=100000] [names|all] [neg|fld]: the lod group exports of the DLL (RpLODAtomicSetGeometry / SetRange, LODAtomicCallBackRender /
# SelectLOD; librw-mfl capi/rwlod.cpp) vs the original code of MFL.exe, both run natively under Wine in a fresh container (docker run --rm).  x87 CW 0x7f.
# Exit 1 on any finite DIFF, a REGDIFF, a TIMEOUT or a CRASH.  neg / fld: the input and planted-fld fixtures (DIFF / REGDIFF on every entry, negtest.sh).
# `lrintf` instead of a DLL: builds the fixture DLL from $LIBRW/capi/rwlod.cpp with -DMFL_FIXTURE_LOD_LRINTF (round to nearest instead of chop):
# LODAtomicCallBackSelectLOD must DIFF.  Output dir: $OUT (default /tmp/rw_lod_check); $EXE overrides the MFL.exe path.
set -e
T=$(dirname "$(readlink -f "$0")"); N=${2:-100000}; O=${OUT:-/tmp/rw_lod_check}; E=${EXE:-$T/../../game/MFL.exe}; L=${LIBRW:-$T/../../../librw-mfl}; mkdir -p $O
i686-w64-mingw32-gcc -O1 -mfpmath=387 -w -Wl,--image-base,0x400000 -Wl,--disable-dynamicbase -o $O/lod_check.exe $T/rw_lod_check.c
if [ "$1" = lrintf ]; then
  i686-w64-mingw32-g++ -std=c++11 -O2 -mfpmath=387 -ffp-contract=off -fno-exceptions -fno-rtti -w -DMFL_FIXTURE_LOD_LRINTF -I$L -I$L/src -shared -static-libgcc -static-libstdc++ \
    -o $O/mflrw.dll $T/rw_lod_fixture.def $L/capi/rwlod.cpp
else cp "$(readlink -f "$1")" $O/mflrw.dll; fi
cp $E $O/MFL.exe
docker run --rm -e XDG_RUNTIME_DIR=/tmp/xdg -e WINEDEBUG=-all -v $O:/t tianon/wine:latest wine /t/lod_check.exe Z:\\t\\mflrw.dll Z:\\t\\MFL.exe $N ${3:-all} $4 2>&1 | grep -v -E "^error: XDG|fixme" | tee $O/replay.log
if grep -q -E "DIFF|MISSING|cannot|TIMEOUT|CRASH" $O/replay.log; then exit 1; fi
grep -q "^DONE" $O/replay.log
