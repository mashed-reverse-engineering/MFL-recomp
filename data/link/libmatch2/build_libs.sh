#!/bin/bash
# Build Lua 4.0, qhull 2002.1 and zlib 1.1.3 from upstream source with the game's compiler (cl 13.00.9466) into
# lua40.lib / qhull2002.lib / zlib113.lib, one COMDAT per function (/Gy).  Usage: build_libs.sh <outdir below $TMPDIR (mounted into the compiler container)> [lua|qhull|zlib ...]
# CC=v143 (W16): build the same libraries with MSVC v143 /MD instead (CI variant; tools/run.py docker msvcw-libs <out dir> runs it under msvc-wine).
# Needs: ghidra/tools/vc7.sh (docker + wine image), VC7_CONTAINER set below (own container, never a mfl_slot*).
# MFL_REFS: directory with the upstream sources (lua/src40, qhull/blender240, zlib/src, libpng/src); default: upstream/ beside the ghidra tree.
set -e
OUT=$(readlink -f "${1:?outdir}"); shift
LIBS=${*:-lua qhull zlib}
H=$(dirname "$(readlink -f "$0")")
REF=$(readlink -f "${MFL_REFS:-$H/../../../upstream}")
[ -f "$REF/lua/src40/src/lapi.c" ] || { echo "build_libs.sh: upstream sources not found in $REF (set MFL_REFS)"; exit 1; }
VC7=$(readlink -f "$H/../../tools/vc7.sh")
export VC7_CONTAINER=${VC7_CONTAINER:-libmatch2}
cc() {  # cc <dir> <flags...> -- <files...>   (compile each file separately, then lib.exe)
  local d=$1; shift; local fl=(); while [ "$1" != "--" ]; do fl+=("$1"); shift; done; shift
  mkdir -p "$d/_o"
  for f in "$@"; do $VC7 "$d" cl.exe /nologo /c /Gy "${fl[@]}" '/Fo_o\' "$f" >/dev/null; done
}
mklib() { local d=$1 name=$2; shift 2; rm -f "$d/$name"; $VC7 "$d" lib.exe /nologo "/OUT:$name" "$@" >/dev/null; }
if [ "${CC:-}" = v143 ]; then  # W16 V143 (CI variant only; unset CC = the VC7 calls above, unchanged): MSVC v143 x86, /MD, x87 like the game files
  # (tools/msvcw/msvcw_compile.py BASE). MSVC_BIN = dir holding cl/lib (msvc-wine: /opt/msvc/bin/x86); empty = cl/lib from PATH (CI runner).
  # /G5 is gone in v143 (D9002); /Zl: the link layer names the CRT. One cl call per library (a cl start under Wine costs seconds).
  cc() {
    local d=$1; shift; local fl=(); while [ "$1" != "--" ]; do [ "$1" = /G5 ] || fl+=("$1"); shift; done; shift
    mkdir -p "$d/_o"
    (cd "$d" && "${MSVC_BIN:+$MSVC_BIN/}cl" /nologo /c /Gy "${fl[@]}" /arch:IA32 /GS- /MD /Zl /D_CRT_SECURE_NO_WARNINGS '/Fo_o\' "$@")
  }
  mklib() { local d=$1 name=$2; shift 2; rm -f "$d/$name"; (cd "$d" && "${MSVC_BIN:+$MSVC_BIN/}lib" /nologo "/OUT:$name" "$@"); }
fi
for L in $LIBS; do case $L in
lua)  # Lua 4.0 (NOT 4.0.1: ldo.c luaD_protectedparser matches only 4.0).  -Dstatic= : the original has no static-function register conventions
      # (cl 13 gives them to static functions, the original compiler did not); +105 -> +253 matched functions.  Std libs (base/str/io/db) are not linked in the game.
  D=$OUT/lua; rm -rf "$D"; mkdir -p "$D"
  cp $REF/lua/src40/src/*.[ch] $REF/lua/src40/include/*.h "$D/"; cp $REF/lua/src40/src/lib/lauxlib.c $REF/lua/src40/src/lib/lmathlib.c "$D/"
  FILES="lapi lauxlib lcode ldebug ldo lfunc lgc llex lmathlib lmem lobject lparser lstate lstring ltable ltm lundump lvm lzio"
  cc "$D" /G5 /O2 /Ob0 /DNDEBUG /I. /Dstatic= -- $(for f in $FILES; do echo $f.c; done)
  mklib "$D" lua40.lib $(for f in $FILES; do echo _o/$f.obj; done) ;;
qhull) # qhull 2002.1 (blender240 copy; "2002.1 2002/8/20" is the version string in the exe), user.h patched REALfloat 0 -> 1 (the game uses float): 46 -> 117 matched
  D=$OUT/qhull; rm -rf "$D"; mkdir -p "$D"
  cp $REF/qhull/blender240/*.[ch] "$D/"; rm -f "$D"/{qconvex,qdelaun,qhalf,qvoronoi,rbox,unix,user_eg,user_eg2}.c
  sed -i 's/^#define REALfloat 0/#define REALfloat 1/' "$D/user.h"
  FILES="geom geom2 global io mem merge poly poly2 qhull qset stat user"
  cc "$D" /G5 /O2 /Ob0 /DNDEBUG /I. -- $(for f in $FILES; do echo $f.c; done)
  mklib "$D" qhull2002.lib $(for f in $FILES; do echo _o/$f.obj; done) ;;
zlib) # zlib 1.1.3 + libpng 1.0.12 (A5, one unit "zlib/PNG": RenderWare's rtpng.lib members in the exe, RtPNG wrapper rtpng.obj stays RenderWare): only the files the exe contains
      # (no compress / uncompr / pngpread; gzio's gzputs / gztell are dead code), PNG_NO_STDIO (no png_init_io / default read-write in the exe), PNG_USER_MEM_SUPPORTED (png_create_*_struct_2, png_set_mem_fn)
  D=$OUT/zlib; rm -rf "$D"; mkdir -p "$D"
  cp $REF/zlib/src/*.h "$D/"; cp $REF/libpng/src/png.h $REF/libpng/src/pngconf.h "$D/"
  ZF="adler32 crc32 deflate infblock infcodes inffast inflate inftrees infutil trees zutil"
  PF="png pngerror pngget pngmem pngread pngrio pngrtran pngrutil pngset pngtrans pngwio pngwrite pngwtran pngwutil"
  for f in $ZF; do cp $REF/zlib/src/$f.c "$D/"; done; for f in $PF; do cp $REF/libpng/src/$f.c "$D/"; done
  cc "$D" /G5 /O2 /Ob0 /DNDEBUG /DPNG_NO_STDIO /DPNG_USER_MEM_SUPPORTED /I. /Dlocal= -- $(for f in $ZF $PF; do echo $f.c; done)
  mklib "$D" zlib113.lib $(for f in $ZF $PF; do echo _o/$f.obj; done) ;;
esac; done
ls -la "$OUT"/*/*.lib
