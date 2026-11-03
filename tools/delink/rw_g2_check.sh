#!/bin/bash
# rw_g2_check.sh <mflrw.dll> [N=100000] [name,name]: S4 g2 exports of the DLL (RtIntersectionSphereTriangle / BBoxTriangle, RwMatrixCreate / Destroy) vs the original code of MFL.exe
# (mapped at its image base), both run natively under Wine in a fresh container (docker run --rm: no mfl_* container touched).  x87 CW 0x7f.  Exit 1 on any finite DIFF.
# Output dir: $OUT (default /tmp/rw_g2_check); $EXE overrides the MFL.exe path.  Same mechanism as rw_g1_check.sh.
set -e
T=$(dirname "$(readlink -f "$0")"); DLL=$(readlink -f "$1"); N=${2:-100000}; O=${OUT:-/tmp/rw_g2_check}; E=${EXE:-$T/../../game/MFL.exe}; mkdir -p $O
i686-w64-mingw32-gcc -O1 -mfpmath=387 -w -Wl,--image-base,0x400000 -Wl,--disable-dynamicbase -o $O/g2_check.exe $T/rw_g2_check.c
cp $DLL $O/mflrw.dll; cp $E $O/MFL.exe
docker run --rm -e XDG_RUNTIME_DIR=/tmp/xdg -e WINEDEBUG=-all -v $O:/t tianon/wine:latest wine /t/g2_check.exe Z:\\t\\mflrw.dll Z:\\t\\MFL.exe $N $3 2>&1 | grep -v -E "^error: XDG|fixme" | tee $O/replay.log
! grep -q -E "DIFF|MISSING|cannot" $O/replay.log
grep -q "^DONE" $O/replay.log
