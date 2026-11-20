#!/bin/sh
# build_ffmpeg.sh <win32|native>: minimal LGPL FFmpeg (mpegps/mpegvideo demux, mpeg1video + mp2 decode, swscale).
# win32 = SHARED (W16 VIDCI, the shipping layout): avformat-61.dll avcodec-61.dll avutil-59.dll swscale-8.dll in $WORK/prefix/win32/bin,
# loaded by mfl_video_ffmpeg.dll through their import libs (LGPL 2.1 section 6(b): the user can replace them); no version resource
# (--windres=none: no_original.py wants resource-free DLLs). native = static (offline test harness only, never shipped).
# Fetches + verifies the upstream tarball (see SOURCES), builds out of tree in $WORK/build/<target>, installs to $WORK/prefix/<target>.
# $WORK defaults to tools/video/work (not tracked, never commit it). No asm (no nasm needed; C paths give the same bits on both targets).
# CI (build.yml job video) caches $WORK/prefix/win32 under the hash of this file: VER, SHA256 and the configure line all live here.
set -e
HERE=$(dirname "$(readlink -f "$0")")
WORK=${WORK:-$HERE/work}
VER=7.1
URL=https://ffmpeg.org/releases/ffmpeg-$VER.tar.xz
SHA256=40973d44970dbc83ef302b0609f2e74982be2d85916dd2ee7472d30678a7abe6
T=$1
case "$T" in
  win32)  EXTRA="--enable-cross-compile --arch=x86 --target-os=mingw32 --cross-prefix=i686-w64-mingw32- --enable-shared --disable-static
                 --windres=none --extra-ldflags=-static-libgcc --extra-ldflags=-Wl,--no-insert-timestamp" ;;
  native) EXTRA="--enable-pic --enable-static --disable-shared" ;;
  *) echo "usage: $0 win32|native"; exit 2 ;;
esac
mkdir -p "$WORK/upstream"
TAR=$WORK/upstream/ffmpeg-$VER.tar.xz
[ -f "$TAR" ] || curl -fL -o "$TAR" "$URL"
echo "$SHA256  $TAR" | sha256sum -c - || { echo "sha256 mismatch: delete $TAR and re-fetch"; exit 1; }
SRC=$WORK/upstream/ffmpeg-$VER
[ -d "$SRC" ] || tar -C "$WORK/upstream" -xf "$TAR"
COMMON="--disable-everything --disable-programs --disable-doc --disable-network --disable-autodetect --disable-asm
 --disable-debug --disable-avdevice --disable-avfilter --disable-swresample --disable-postproc
 --disable-pthreads --disable-w32threads --disable-os2threads
 --enable-protocol=file --enable-demuxer=mpegps,mpegvideo --enable-parser=mpegvideo,mpegaudio
 --enable-decoder=mpeg1video,mp2 --enable-swscale"
mkdir -p "$WORK/build/$T"
cd "$WORK/build/$T"
"$SRC/configure" --prefix="$WORK/prefix/$T" $COMMON $EXTRA > configure.log 2>&1 || { tail -30 configure.log; tail -30 ffbuild/config.log; exit 1; }
grep -q "License: LGPL version 2.1 or later" configure.log || { echo "configure did not report LGPL-only: refusing"; exit 1; }
make -j8 > make.log 2>&1 || { tail -40 make.log; exit 1; }
make install > install.log 2>&1
echo "OK $T -> $WORK/prefix/$T"
