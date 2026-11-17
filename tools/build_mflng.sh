#!/bin/bash
# build_mflng.sh <path to your MFL.exe> <out dir>: drop-in builds of Mashed: Fully Loaded from this repository's C.
#   <out dir>/MFLng.exe        every MFL_FIX_* switch at its default (off) = original behaviour; the release link the regression gate checks
#   <out dir>/MFLng_fixes.exe  every MFL_FIX_* switch on (src/mfl_fix.h); NOT gated
# Install: rename the game's MFL.exe to MFL_original.exe (the build reads its resources from it at run time) and copy MFLng.exe beside it;
# copy mflrw.dll from <out dir> beside it as well.  The copied library tables still come from your MFL.exe (README).
# The script holds no source file list: it calls tools/delink/link_release.sh (the gate's release form), twice.
# Exit 2: wrong or missing MFL.exe, or a missing prerequisite (one line per missing item).
# DRAFT (W13 BLD) against the private tree layout; the export layout (src/, data/) needs the export stage of the link inputs first.
set -e
T=$(dirname "$(readlink -f "$0")"); G=$(readlink -f "$T/..")
EXE=${1:?usage: build_mflng.sh <MFL.exe> <out dir>}; OUT=${2:?usage: build_mflng.sh <MFL.exe> <out dir>}
[ -f "$EXE" ] || { echo "build_mflng: $EXE: no such file"; exit 2; }
python3 "$T/check_exe.py" "$EXE" > /dev/null || { echo "build_mflng: $EXE is not the supported MFL.exe (sha1 differs, tools/check_exe.py)"; exit 2; }

# ---- prerequisites (BUILDING.md lists where each comes from); every missing item is printed, then exit 2
miss=0
need() { [ -e "$2" ] || { echo "MISSING $1: $2"; miss=1; }; }
cmd() { command -v "$1" > /dev/null || { echo "MISSING command $1 ($2)"; miss=1; }; }
cmd python3 "Python 3"; cmd docker "Docker"; cmd i686-w64-mingw32-gcc "mingw-w64 (dinput tables, RenderWare thunks)"; cmd i686-w64-mingw32-as "mingw-w64 binutils"
cmd objcopy "binutils"; cmd objdump "binutils"; cmd nm "binutils"
VC7=$(readlink -f "${VC7_ROOT:-$G/toolchain/vc7}")
need "Visual C++ .NET 2002 compiler (cl 13.00.9466)" "$VC7/Vc7/bin/cl.exe"
need "Visual C++ .NET 2002 linker" "$VC7/Vc7/bin/link.exe"
need "VC7 libcmt.lib (C runtime members linked from it)" "$VC7/Vc7/lib/libcmt.lib"
INST=$G/../MashedGame/FullyLoaded/installed
need "the game install the link reads (MFL.exe at the tree's install path)" "$INST/App_Executables/MFL.exe"
REF=${MFL_REFS:-$G/../references}
need "Lua 4.0 source" "$REF/lua/src40/src/lapi.c"
need "qhull 2002.1 source" "$REF/qhull/blender240/qhull.c"
need "zlib 1.1.3 source" "$REF/zlib/src/inflate.c"
need "libpng 1.0.12 source" "$REF/libpng/src/png.c"
RWD=${RW_DLL:-$G/output/librw/mflrw.dll}
need "mflrw.dll (librw-mfl: librw-mfl/tools/build_mflrw.sh)" "$RWD"
if command -v docker > /dev/null && ! docker image inspect tianon/wine@sha256:878222fee186d28e06b6c34996590e786d5d073de24b8ee5ac4bcb9cddb18497 > /dev/null 2>&1; then
  echo "MISSING docker image tianon/wine@sha256:878222fe... (docker pull tianon/wine@sha256:878222fee186d28e06b6c34996590e786d5d073de24b8ee5ac4bcb9cddb18497)"; miss=1; fi
[ -f "$INST/App_Executables/MFL.exe" ] && ! cmp -s "$EXE" "$INST/App_Executables/MFL.exe" && { echo "build_mflng: $EXE differs from $INST/App_Executables/MFL.exe"; exit 2; }
[ $miss = 0 ] || { echo "build_mflng: prerequisites missing (see BUILDING.md)"; exit 2; }

# ---- switch list from src/mfl_fix.h (one place; a pair marked "excludes" there ends the fixes build)
FIXES=$(sed -n 's/^#define \(MFL_FIX_[A-Z0-9_]*\) 0$/\1/p' "$G/src/mfl_fix.h")
[ -n "$FIXES" ] || { echo "build_mflng: no MFL_FIX_* switch found in src/mfl_fix.h"; exit 2; }
if grep -q "excludes MFL_FIX_" "$G/src/mfl_fix.h"; then echo "build_mflng: src/mfl_fix.h marks switches that exclude each other; the all-on build is not defined"; exit 2; fi
DEFS=$(for f in $FIXES; do printf '/D%s=1 ' "$f"; done)

mkdir -p "$OUT"; OUT=$(readlink -f "$OUT"); W=$OUT/work
[ -e "$W" ] && { echo "build_mflng: $W exists: use a fresh out dir"; exit 2; }
mkdir -p "$W"      # link_release.sh resolves its work dir with readlink -f: the parent must exist
# 1. default build: the gate's release form
RW_DLL=$RWD bash "$T/delink/link_release.sh" "$W/default" > "$OUT/default.log" 2>&1 || { tail -5 "$OUT/default.log"; echo "build_mflng: default link failed ($OUT/default.log)"; exit 1; }
cp "$W/default/game/App_Executables/MFL.exe" "$OUT/MFLng.exe"; cp "$W/default/game/App_Executables/MFL.map" "$OUT/MFLng.map"
cp "$RWD" "$OUT/mflrw.dll"
# 2. fixes build: every switch on; the switched call sites differ by design (RELCHECK=warn)
MFL_FIX_DEFINES="$DEFS" RELCHECK=warn RW_DLL=$RWD bash "$T/delink/link_release.sh" "$W/fixes" > "$OUT/fixes.log" 2>&1 || { tail -5 "$OUT/fixes.log"; echo "build_mflng: fixes link failed ($OUT/fixes.log)"; exit 1; }
cp "$W/fixes/game/App_Executables/MFL.exe" "$OUT/MFLng_fixes.exe"; cp "$W/fixes/game/App_Executables/MFL.map" "$OUT/MFLng_fixes.map"
echo "build_mflng: $OUT/MFLng.exe (switches off) and $OUT/MFLng_fixes.exe (on: $FIXES; not gated); copy them and mflrw.dll beside MFL_original.exe"
