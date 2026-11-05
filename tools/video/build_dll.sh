#!/bin/sh
# build_dll.sh: mfl_video_ffmpeg.dll + harness.exe (i686 mingw, static FFmpeg + static libgcc inside the DLL), and the native harness.
# Needs build_ffmpeg.sh win32 and native first. Output: $WORK/out (default tools/video/work/out). Binaries stay out of the repo.
set -e
HERE=$(dirname "$(readlink -f "$0")")
WORK=${WORK:-$HERE/work}
INC=$(readlink -f "$HERE/../../src")          # mfl_video.h
W=$WORK/prefix/win32
N=$WORK/prefix/native
O=$WORK/out
mkdir -p "$O"
LIBS="-lavformat -lavcodec -lswscale -lavutil"
i686-w64-mingw32-gcc -O2 -Wall -shared -o "$O/mfl_video_ffmpeg.dll" "$HERE/src/mfl_video_ffmpeg.c" -I"$INC" -I"$W/include" -L"$W/lib" \
  $LIBS -lm -luser32 -lbcrypt -lole32 -static-libgcc -Wl,--no-insert-timestamp -Wl,--out-implib,"$O/libmfl_video_ffmpeg.dll.a"
i686-w64-mingw32-strip "$O/mfl_video_ffmpeg.dll"
i686-w64-mingw32-gcc -O2 -Wall -o "$O/harness.exe" "$HERE/src/harness.c" -I"$INC" -static-libgcc
gcc -O2 -Wall -o "$O/harness_native" "$HERE/src/harness.c" "$HERE/src/mfl_video_ffmpeg.c" -I"$INC" -I"$N/include" -L"$N/lib" $LIBS -lm -latomic
i686-w64-mingw32-objdump -p "$O/mfl_video_ffmpeg.dll" | grep -E "DLL Name|^\s+\[ *[0-9]+\] mfl_" || true
ls -l "$O"
