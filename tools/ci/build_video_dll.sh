#!/bin/sh
# build_video_dll.sh <out dir>: CI step (build.yml job video), the SHIPPING movie set with the runner's msys2 mingw32 gcc:
# mfl_video_ffmpeg.dll (real miniaudio device) + the shared LGPL FFmpeg DLLs it imports, copied to <out dir>, sha256 printed.
# FFmpeg: tools/video/build_ffmpeg.sh win32 into $WORK/prefix/win32, skipped when that prefix is already there (actions/cache hit).
# miniaudio.h: fetched + sha256-checked by tools/audio/build_dll.sh (it also builds the audio DLL: ~30 s, unused here).
set -e
HERE=$(dirname "$(readlink -f "$0")")
OUT=${1:?out dir}
WORK=${WORK:-$OUT/video_work}
mkdir -p "$OUT"
if [ -d "$WORK/prefix/win32/bin" ]; then echo "FFmpeg from the cache: $WORK/prefix/win32"; else WORK=$WORK sh "$HERE/../video/build_ffmpeg.sh" win32; fi
WORK=$OUT/audio_work VARIANTS=real sh "$HERE/../audio/build_dll.sh"
WORK=$WORK MA_DIR=$OUT/audio_work sh "$HERE/../video/build_dll.sh"
cp "$WORK"/out/*.dll "$OUT/"
i686-w64-mingw32-gcc --version | head -1
sha256sum "$OUT"/*.dll
