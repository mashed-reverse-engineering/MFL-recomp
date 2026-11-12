#!/bin/bash
# rw_resample_check.sh <mflrw.dll> [N=100000] [names|all] [neg]: the resample group exports of the DLL (RwImageResample, ImageResampleGetSpan; librw-mfl
# capi/rwresample.cpp) vs the original code of MFL.exe (mapped at its image base), both run natively under Wine in a fresh container (docker run --rm: no mfl_*
# container touched); x87 CW 0x7f / 0x27f.
# Exit 1 on any DIFF, a TIMEOUT or a CRASH.  neg = the negative fixture (the DLL gets two source pixels swapped): must exit 1 with DIFF on both entries
# (negtest.sh).  Output dir: $OUT (default /tmp/rw_resample_check); $EXE overrides the MFL.exe path.  Same mechanism as rw_image_check.sh.
set -e
T=$(dirname "$(readlink -f "$0")"); DLL=$(readlink -f "$1"); N=${2:-100000}; O=${OUT:-/tmp/rw_resample_check}; E=${EXE:-$T/../../game/MFL.exe}; mkdir -p $O
i686-w64-mingw32-gcc -O1 -mfpmath=387 -w -Wl,--image-base,0x400000 -Wl,--disable-dynamicbase -o $O/resample_check.exe $T/rw_resample_check.c
cp $DLL $O/mflrw.dll; cp $E $O/MFL.exe
docker run --rm -e XDG_RUNTIME_DIR=/tmp/xdg -e WINEDEBUG=-all -v $O:/t tianon/wine@sha256:878222fee186d28e06b6c34996590e786d5d073de24b8ee5ac4bcb9cddb18497 wine /t/resample_check.exe Z:\\t\\mflrw.dll Z:\\t\\MFL.exe $N ${3:-all} $4 2>&1 | grep -v -E "^error: XDG|fixme" | tee $O/replay.log
if grep -q -E "DIFF|MISSING|cannot|TIMEOUT|CRASH|exhausted" $O/replay.log; then exit 1; fi      # (not `! grep`: set -e ignores a negated command, the exit would be 0)
grep -q "^DONE" $O/replay.log
