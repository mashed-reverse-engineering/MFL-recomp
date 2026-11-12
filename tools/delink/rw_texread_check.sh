#!/bin/bash
# rw_texread_check.sh <mflrw.dll> [seeds=200] [steps=300] [va:byte ...]: texread group exports of the DLL (image / texture read layer, rw_texread.txt, librw-mfl
# capi/rwtexread.cpp) vs the original code of MFL.exe (mapped at its image base), both run natively under Wine in a fresh container (docker run --rm: no mfl_*
# container touched).  va:byte = mutate the original first (negative fixtures, must DIFF).  The group is integer only (no FPU control word runs).
# Exit 1 on DIFF, MISSING, TRAP, a TIMEOUT or a CRASH.  Output dir: $OUT (default rw_texread_check/ next to the DLL); $EXE overrides the MFL.exe path.
# Same mechanism as rw_d3d9texdic_check.sh.  Never while a gate holds the lock (run.py gatestate).
set -e
T=$(dirname "$(readlink -f "$0")"); DLL=$(readlink -f "$1"); O=${OUT:-$(dirname "$DLL")/rw_texread_check}; E=${EXE:-$T/../../game/MFL.exe}; mkdir -p $O; shift
i686-w64-mingw32-gcc -O1 -w -I$T -Wl,--image-base,0x400000 -Wl,--disable-dynamicbase -o $O/texread_check.exe $T/rw_texread_check.c
cp $DLL $O/mflrw.dll; cp $E $O/MFL.exe
docker run --rm -e XDG_RUNTIME_DIR=/tmp/xdg -e WINEDEBUG=-all -v $O:/t tianon/wine@sha256:878222fee186d28e06b6c34996590e786d5d073de24b8ee5ac4bcb9cddb18497 wine /t/texread_check.exe Z:\\t\\mflrw.dll Z:\\t\\MFL.exe ${1:-200} ${2:-300} "${@:3}" 2>&1 | grep -v -E "^error: XDG|fixme" | tee $O/replay.log
if grep -q -E "DIFF|MISSING|cannot|TRAP|TIMEOUT|CRASH|full" $O/replay.log; then exit 1; fi      # (not `! grep`: set -e ignores a negated command)
grep -q "^DONE" $O/replay.log
