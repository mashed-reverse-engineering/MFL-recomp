#!/bin/sh
# build_dll.sh: mfl_audio_miniaudio.dll for the release exe (loaded by src/audio/mfl_audio_null.c -DMFL_AUDIO_DLL), two variants of the SAME name:
#   $WORK/out/null/  -DMFL_AUDIO_MA_NULL (miniaudio null device: no sound card, timer driven; evidence = MFL_AUDIO_DUMP)
#   $WORK/out/real/  real device backends
# Env: SRC=<file> backend source (default src/audio/mfl_audio_miniaudio.c; W5 WIP copy: agents/b1_w5/mfl_audio_miniaudio.c), WORK (default tools/audio/work).
# miniaudio.h is fetched into $WORK (URL + sha256 in SOURCES), checked on every run.
set -e
HERE=$(dirname "$(readlink -f "$0")")
WORK=${WORK:-$HERE/work}
INC=$(readlink -f "$HERE/../../src")          # mfl_audio.h
SRC=${SRC:-$INC/audio/mfl_audio_miniaudio.c}
URL=$(awk '$1=="URL"{print $2}' "$HERE/SOURCES")
SUM=$(awk '$1=="sha256"{print $2}' "$HERE/SOURCES")
mkdir -p "$WORK/out/null" "$WORK/out/real"
[ -f "$WORK/miniaudio.h" ] || curl -fsSL -o "$WORK/miniaudio.h" "$URL"
echo "$SUM  $WORK/miniaudio.h" | sha256sum -c - >/dev/null || { echo "miniaudio.h sha256 mismatch"; exit 1; }
for v in null real; do
  D=; [ $v = null ] && D=-DMFL_AUDIO_MA_NULL
  i686-w64-mingw32-gcc -O2 -Wall -shared -DMFL_AUDIO_BUILD_DLL $D -I"$INC" -I"$INC/audio" -I"$WORK" -o "$WORK/out/$v/mfl_audio_miniaudio.dll" "$SRC" \
    -static-libgcc -Wl,--no-insert-timestamp
  i686-w64-mingw32-strip "$WORK/out/$v/mfl_audio_miniaudio.dll"
  echo "== $v"; i686-w64-mingw32-objdump -p "$WORK/out/$v/mfl_audio_miniaudio.dll" | grep -E "DLL Name|mfl_audio_backend"
done
