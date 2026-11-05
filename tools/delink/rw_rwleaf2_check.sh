#!/bin/bash
# rw_rwleaf2_check.sh <mflrw.dll> [N=100000] [names|all] [neg]: the rwleaf2 group exports of the DLL (Rt2dBrushSetRGBA, BMPImageSetSpan, PVSCalculateSectorBounds,
# RpLightGetConeAngle, keyframe -> matrix FUN_00542fe0; librw-mfl capi/rwleaf2.cpp) vs the original code of MFL.exe (mapped at its image base), both run natively
# under Wine in a fresh container (docker run --rm: no mfl_* container touched).  Exit 1 on any DIFF, a TIMEOUT or a CRASH.  neg = the negative fixture (the DLL
# gets a changed input): must exit 1 with DIFF on every entry (negtest.sh).  Output dir: $OUT (default /tmp/rw_rwleaf2_check); $EXE overrides the MFL.exe path.
# Same mechanism as rw_rwleaf_check.sh.
set -e
T=$(dirname "$(readlink -f "$0")"); DLL=$(readlink -f "$1"); N=${2:-100000}; O=${OUT:-/tmp/rw_rwleaf2_check}; E=${EXE:-$T/../../game/MFL.exe}; mkdir -p $O
i686-w64-mingw32-gcc -O1 -w -Wl,--image-base,0x400000 -Wl,--disable-dynamicbase -o $O/rwleaf2_check.exe $T/rw_rwleaf2_check.c
cp $DLL $O/mflrw.dll; cp $E $O/MFL.exe
docker run --rm -e XDG_RUNTIME_DIR=/tmp/xdg -e WINEDEBUG=-all -v $O:/t tianon/wine:latest wine /t/rwleaf2_check.exe Z:\\t\\mflrw.dll Z:\\t\\MFL.exe $N ${3:-all} $4 2>&1 | grep -v -E "^error: XDG|fixme" | tee $O/replay.log
if grep -q -E "DIFF|MISSING|cannot|TIMEOUT|CRASH|exhausted" $O/replay.log; then exit 1; fi      # (not `! grep`: set -e ignores a negated command, the exit would be 0)
grep -q "^DONE" $O/replay.log
