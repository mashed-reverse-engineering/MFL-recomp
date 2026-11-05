#!/bin/bash
# rw_d3dx_check.sh <mflrw.dll> [N=100000]: D3DX math exports of the DLL vs the original bytes of MFL.exe, run natively under Wine in a fresh container (no mfl_* container touched).  Exit 1 on any DIFF.
set -e
T=$(dirname "$(readlink -f "$0")"); DLL=$(readlink -f "$1"); N=${2:-100000}; O=${OUT:-/tmp/rw_d3dx_check}; mkdir -p $O
i686-w64-mingw32-gcc -O1 -msse2 -w -o $O/d3dx_check.exe $T/rw_d3dx_check.c
cp $DLL $O/mflrw.dll; cp $T/../../game/MFL.exe $O/MFL.exe
docker run --rm -e XDG_RUNTIME_DIR=/tmp/xdg -e WINEDEBUG=-all -v $O:/t tianon/wine:latest wine /t/d3dx_check.exe Z:\\t\\mflrw.dll Z:\\t\\MFL.exe $N 2>&1 | grep -v -E "^error: XDG|fixme" | tee $O/replay.log
if grep -q -E "DIFF|MISSING" $O/replay.log; then exit 1; fi      # (not `! grep`: set -e ignores a negated command)