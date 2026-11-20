#!/bin/sh
# build_dll.sh: mfl_video_ffmpeg.dll + harness.exe (i686 mingw, static libgcc inside the DLL), and the native harness.
# Needs build_ffmpeg.sh win32 first (native too for the native harness, skipped when $WORK/prefix/native is absent: CI),
# and miniaudio.h (movie sound) from tools/audio/build_dll.sh in $MA_DIR (default tools/audio/work).
# Output: $WORK/out (default tools/video/work/out) = the shipping set: mfl_video_ffmpeg.dll + the shared LGPL FFmpeg DLLs it imports
# (avformat-61 avcodec-61 avutil-59 swscale-8), all beside the exe. Binaries stay out of the repo.
# VCFLAGS=-DMFL_AUDIO_MA_NULL: test DLL with the miniaudio null device (Wine).
set -e
HERE=$(dirname "$(readlink -f "$0")")
WORK=${WORK:-$HERE/work}
INC=$(readlink -f "$HERE/../../src")          # mfl_video.h
W=$WORK/prefix/win32
N=$WORK/prefix/native
O=$WORK/out
MA=${MA_DIR:-$HERE/../audio/work}
[ -f "$MA/miniaudio.h" ] || { echo "no $MA/miniaudio.h: run tools/audio/build_dll.sh first"; exit 1; }
mkdir -p "$O"
LIBS="-lavformat -lavcodec -lswscale -lavutil"
i686-w64-mingw32-gcc -O2 -Wall -shared $VCFLAGS -o "$O/mfl_video_ffmpeg.dll" "$HERE/src/mfl_video_ffmpeg.c" -I"$INC" -I"$MA" -I"$W/include" -L"$W/lib" \
  $LIBS -lm -luser32 -lbcrypt -lole32 -static-libgcc -Wl,--no-insert-timestamp -Wl,--out-implib,"$O/libmfl_video_ffmpeg.dll.a"
i686-w64-mingw32-strip "$O/mfl_video_ffmpeg.dll"
cp "$W"/bin/*.dll "$O/"
i686-w64-mingw32-gcc -O2 -Wall -o "$O/harness.exe" "$HERE/src/harness.c" -I"$INC" -static-libgcc
[ ! -d "$N" ] || gcc -O2 -Wall -o "$O/harness_native" "$HERE/src/harness.c" "$HERE/src/mfl_video_ffmpeg.c" -I"$INC" -I"$MA" -I"$N/include" -L"$N/lib" $LIBS -lm -latomic -lpthread -ldl
i686-w64-mingw32-objdump -p "$O/mfl_video_ffmpeg.dll" | grep -E "DLL Name|^\s+\[ *[0-9]+\] mfl_" || true
ls -l "$O"
