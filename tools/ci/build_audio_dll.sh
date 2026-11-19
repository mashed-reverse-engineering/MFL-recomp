#!/bin/sh
# build_audio_dll.sh <out dir>: CI step, builds the SHIPPING mfl_audio_miniaudio.dll (real-device variant) with the runner's
# msys2 mingw32 gcc and prints its sha256. miniaudio.h comes from tools/audio/SOURCES (curl + sha256 check inside build_dll.sh).
set -e
HERE=$(dirname "$(readlink -f "$0")")
OUT=${1:?out dir}
WORK=${WORK:-$OUT/audio_work}
mkdir -p "$OUT"
WORK=$WORK VARIANTS=real sh "$HERE/../audio/build_dll.sh"
cp "$WORK/out/real/mfl_audio_miniaudio.dll" "$OUT/"
i686-w64-mingw32-gcc --version | head -1
sha256sum "$OUT/mfl_audio_miniaudio.dll"
# the loader needs this exact data export (src/audio/mfl_audio_null.c k_export); fail early if it is missing
i686-w64-mingw32-objdump -p "$OUT/mfl_audio_miniaudio.dll" | grep -q mfl_audio_backend_miniaudio || { echo "export mfl_audio_backend_miniaudio missing"; exit 1; }
