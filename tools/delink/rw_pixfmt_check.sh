#!/bin/bash
# rw_pixfmt_check.sh <mflrw.dll> [N=100000] [names|all] [neg]: the pixfmt group exports of the DLL (_rwD3D9RGBToPixel, _rwD3D9PixelToRGB, D3D9ConvR8G8B8A8To*;
# librw-mfl capi/pixfmt.cpp) vs the original code of MFL.exe (mapped at its image base), both run natively under Wine in a fresh container (docker run --rm: no mfl_*
# container touched).  Exit 1 on any DIFF or a CRASH.  neg = the negative fixture (the DLL gets input bytes 0 <-> 2 swapped): must exit 1 with DIFF on every entry
# (negtest.sh).  Output dir: $OUT (default /tmp/rw_pixfmt_check); $EXE overrides the MFL.exe path.  Same mechanism as rw_rwmath_check.sh.
set -e
T=$(dirname "$(readlink -f "$0")"); DLL=$(readlink -f "$1"); N=${2:-100000}; O=${OUT:-/tmp/rw_pixfmt_check}; E=${EXE:-$T/../../game/MFL.exe}; mkdir -p $O
i686-w64-mingw32-gcc -O1 -w -Wl,--image-base,0x400000 -Wl,--disable-dynamicbase -o $O/pixfmt_check.exe $T/rw_pixfmt_check.c
cp $DLL $O/mflrw.dll; cp $E $O/MFL.exe
docker run --rm -e XDG_RUNTIME_DIR=/tmp/xdg -e WINEDEBUG=-all -v $O:/t tianon/wine@sha256:878222fee186d28e06b6c34996590e786d5d073de24b8ee5ac4bcb9cddb18497 wine /t/pixfmt_check.exe Z:\\t\\mflrw.dll Z:\\t\\MFL.exe $N ${3:-all} $4 2>&1 | grep -v -E "^error: XDG|fixme" | tee $O/replay.log
if grep -q -E "DIFF|MISSING|cannot|TIMEOUT|CRASH" $O/replay.log; then exit 1; fi      # (not `! grep`: set -e ignores a negated command)
grep -q "^DONE" $O/replay.log
