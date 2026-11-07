#!/bin/bash
# rw_pngread_check.sh <mflrw.dll> [variants=200] [neg]: rw37_RtPNGImageRead of the DLL (librw-mfl capi/rwpngread.cpp) vs RtPNGImageRead of MFL.exe on every
# PNG of the installed game + corrupt variants, both run natively under Wine in a fresh container (docker run --rm: no mfl_* container touched).
# Exit 1 on any DIFF, a TIMEOUT or a CRASH.  neg = the negative fixture (IHDR width byte flipped on the DLL side): must exit 1 (negtest.sh).
# Output dir: $OUT (default /tmp/rw_pngread_check); $EXE overrides the MFL.exe path; $GAME the installed game dir.
set -e
T=$(dirname "$(readlink -f "$0")"); DLL=$(readlink -f "$1"); V=${2:-200}; O=${OUT:-/tmp/rw_pngread_check}; E=${EXE:-$T/../../game/MFL.exe}
GAME=${GAME:-$T/../../../MashedGame/FullyLoaded/installed}; mkdir -p $O/png
i686-w64-mingw32-gcc -O1 -w -Wl,--image-base,0x400000 -Wl,--disable-dynamicbase -o $O/png_check.exe $T/rw_pngread_check.c
cp $DLL $O/mflrw.dll; cp $E $O/MFL.exe
i=0; : > $O/png/list.txt
while IFS= read -r f; do n=$(printf "%03d_%s" $i "$(basename "$f")"); cp "$f" "$O/png/$n"; echo "$n" >> $O/png/list.txt; i=$((i+1)); done < <(find "$GAME" -iname '*.png' | sort)
docker run --rm -e XDG_RUNTIME_DIR=/tmp/xdg -e WINEDEBUG=-all -v $O:/t tianon/wine:latest wine /t/png_check.exe Z:\\t\\mflrw.dll Z:\\t\\MFL.exe Z:\\t\\png $V $3 2>&1 | grep -v -E "^error: XDG|fixme" | tee $O/replay.log
if grep -q -E "DIFF|MISSING|cannot|TIMEOUT|CRASH" $O/replay.log; then exit 1; fi
grep -q "^DONE" $O/replay.log
