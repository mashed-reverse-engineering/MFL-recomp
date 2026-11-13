#!/bin/bash
# rw_d3d9vs_check.sh <mflrw.dll> [seeds=200] [steps=300] [rev] [va:byte ...]: d3d9vs group exports of the DLL (_rpD3D9VertexShaderCacheOpen / CachePurge,
# rpD3D9GetVertexShader, GetMaterialShaderCallBack; librw-mfl capi/d3d9vs.cpp) vs the original code of MFL.exe (mapped at its image base), both run natively under Wine
# in a fresh container (docker run --rm: no mfl_* container touched).  The shader generator / free are logging stand-ins; _memmove runs the real exe code.  Every call
# runs at x87 control word 0x7f, 0x27f or 0x37f.  rev = original first.  va:byte = mutate the original first (negative fixtures, must DIFF).  Exit 1 on DIFF, MISSING, a
# TIMEOUT or a CRASH.  Output dir: $OUT (default rw_d3d9vs_check/ next to the DLL); $EXE overrides the MFL.exe path.  Same mechanism as rw_d3d9ptank_check.sh.  Never
# while a gate holds the lock.
set -e
T=$(dirname "$(readlink -f "$0")"); DLL=$(readlink -f "$1"); O=${OUT:-$(dirname "$DLL")/rw_d3d9vs_check}; E=${EXE:-$T/../../game/MFL.exe}; mkdir -p $O; shift
i686-w64-mingw32-gcc -O1 -w -I$T -I$T/../../tools/delink -Wl,--image-base,0x400000 -Wl,--disable-dynamicbase -o $O/d3d9vs_check.exe $T/rw_d3d9vs_check.c -lm
cp $DLL $O/mflrw.dll; cp $E $O/MFL.exe
docker run --rm -e XDG_RUNTIME_DIR=/tmp/xdg -e WINEDEBUG=-all -v $O:/t tianon/wine@sha256:878222fee186d28e06b6c34996590e786d5d073de24b8ee5ac4bcb9cddb18497 wine /t/d3d9vs_check.exe Z:\\t\\mflrw.dll Z:\\t\\MFL.exe ${1:-200} ${2:-300} "${@:3}" 2>&1 | grep -v -E "^error: XDG|fixme" | tee $O/replay.log
if grep -q -E "DIFF|MISSING|cannot|TIMEOUT|CRASH|full|ABORT" $O/replay.log; then exit 1; fi      # (not `! grep`: set -e ignores a negated command)
grep -q "^DONE" $O/replay.log
