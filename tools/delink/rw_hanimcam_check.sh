#!/bin/bash
# rw_hanimcam_check.sh <mflrw.dll> [N=100000] [names|all] [neg]: the hanimcam group exports of the DLL (CameraBuildPerspClipPlanes, CameraBuildParallelClipPlanes,
# RpHAnimHierarchyUpdateMatrices; librw-mfl capi/hanimcam.cpp) vs the original code of MFL.exe (mapped at its image base), both run natively under Wine in a fresh
# container (docker run --rm: no mfl_* container touched).  x87 CW 0x7f.
# Exit 1 on any finite DIFF, a TIMEOUT or a CRASH.  neg = the negative fixture (the DLL gets near <-> far and the first keyframe's quat x <-> y): must exit 1 with DIFF on
# all three (negtest.sh).  Output dir: $OUT (default /tmp/rw_hanimcam_check); $EXE overrides the MFL.exe path.  Same mechanism as rw_splanim_check.sh.
set -e
T=$(dirname "$(readlink -f "$0")"); DLL=$(readlink -f "$1"); N=${2:-100000}; O=${OUT:-/tmp/rw_hanimcam_check}; E=${EXE:-$T/../../game/MFL.exe}; mkdir -p $O
i686-w64-mingw32-gcc -O1 -mfpmath=387 -w -Wl,--image-base,0x400000 -Wl,--disable-dynamicbase -o $O/hanimcam_check.exe $T/rw_hanimcam_check.c
cp $DLL $O/mflrw.dll; cp $E $O/MFL.exe
docker run --rm -e XDG_RUNTIME_DIR=/tmp/xdg -e WINEDEBUG=-all -v $O:/t tianon/wine:latest wine /t/hanimcam_check.exe Z:\\t\\mflrw.dll Z:\\t\\MFL.exe $N ${3:-all} $4 2>&1 | grep -v -E "^error: XDG|fixme" | tee $O/replay.log
if grep -q -E "DIFF|MISSING|cannot|TIMEOUT|CRASH" $O/replay.log; then exit 1; fi      # (not `! grep`: set -e ignores a negated command)
grep -q "^DONE" $O/replay.log
