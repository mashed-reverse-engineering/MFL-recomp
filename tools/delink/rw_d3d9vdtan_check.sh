#!/bin/bash
# rw_d3d9vdtan_check.sh <mflrw.dll> [seeds=200] [steps=3000] [va:byte ...]: the d3d9vdtan export of the DLL (tangent instancer FUN_00508600, rw_d3d9vdtan.txt) vs the original
# code of MFL.exe (mapped at its image base), both run natively under Wine in a fresh container (docker run --rm: no mfl_* container touched), at CW 0x7f and 0x27f.
# va:byte = mutate the original first (negative fixtures, must DIFF).  Exit 1 on DIFF, a TRAP, a TIMEOUT or a CRASH.  Output dir: $OUT (default rw_d3d9vdtan_check/ next
# to the DLL); $EXE overrides the MFL.exe path.  Same mechanism as rw_d3drs_check.sh.
set -e
T=$(dirname "$(readlink -f "$0")"); DLL=$(readlink -f "$1"); O=${OUT:-$(dirname "$DLL")/rw_d3d9vdtan_check}; E=${EXE:-$T/../../game/MFL.exe}; mkdir -p $O; shift
i686-w64-mingw32-gcc -O1 -mfpmath=387 -w -I$T -Wl,--image-base,0x400000 -Wl,--disable-dynamicbase -o $O/d3d9vdtan_check.exe $T/rw_d3d9vdtan_check.c
cp $DLL $O/mflrw.dll; cp $E $O/MFL.exe
docker run --rm -e XDG_RUNTIME_DIR=/tmp/xdg -e WINEDEBUG=-all -v $O:/t tianon/wine:latest wine /t/d3d9vdtan_check.exe Z:\\t\\mflrw.dll Z:\\t\\MFL.exe ${1:-200} ${2:-3000} "${@:3}" 2>&1 | grep -v -E "^error: XDG|fixme" | tee $O/replay.log
if grep -q -E "DIFF|MISSING|cannot|TRAP|TIMEOUT|CRASH" $O/replay.log; then exit 1; fi      # (not `! grep`: set -e ignores a negated command)
grep -q "^DONE" $O/replay.log
