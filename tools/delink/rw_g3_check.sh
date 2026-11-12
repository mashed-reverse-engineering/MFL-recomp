#!/bin/bash
# rw_g3_check.sh <mflrw.dll> [seeds=200] [steps=3000] [va:byte ...]: g3 exports of the DLL (RW 3.7 raster / texture / texdict layer, rw_g3.txt) vs the original code of MFL.exe (mapped at its image
# base), both run natively under Wine in a fresh container (docker run --rm: no mfl_* container touched).  va:byte = mutate the original first (must DIFF).  Exit 1 on DIFF.
# Output dir: $OUT (default /tmp/rw_g3_check); $EXE overrides the MFL.exe path.  Same mechanism as rw_g2_check.sh.
set -e
T=$(dirname "$(readlink -f "$0")"); DLL=$(readlink -f "$1"); O=${OUT:-/tmp/rw_g3_check}; E=${EXE:-$T/../../game/MFL.exe}; mkdir -p $O; shift
i686-w64-mingw32-gcc -O1 -mfpmath=387 -w -Wl,--image-base,0x400000 -Wl,--disable-dynamicbase -o $O/g3_check.exe $T/rw_g3_check.c
cp $DLL $O/mflrw.dll; cp $E $O/MFL.exe
docker run --rm -e XDG_RUNTIME_DIR=/tmp/xdg -e WINEDEBUG=-all -v $O:/t tianon/wine@sha256:878222fee186d28e06b6c34996590e786d5d073de24b8ee5ac4bcb9cddb18497 wine /t/g3_check.exe Z:\\t\\mflrw.dll Z:\\t\\MFL.exe ${1:-200} ${2:-3000} "${@:3}" 2>&1 | grep -v -E "^error: XDG|fixme" | tee $O/replay.log
if grep -q -E "DIFF|MISSING|cannot" $O/replay.log; then exit 1; fi      # (not `! grep`: set -e ignores a negated command)
grep -q "^DONE" $O/replay.log
