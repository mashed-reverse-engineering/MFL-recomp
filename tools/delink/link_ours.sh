#!/bin/bash
# Link OUR recompiled C (the objects build_overlay.py compiles; tier 1 + agent matches + hand written, --automatch) together with the delinked
# image: no overlay section, no jmp patches by a script: mkobj --objs puts a REL32 `jmp` to our function at each original entry and defines every
# symbol our objects need at its original address, link.exe resolves the rest.  Usage: link_ours.sh <work dir> [base=0x400000] [build_overlay args]
# RUN=1: crash-only race on COURSE (default 0) afterwards.  PAD / MKOBJ_ARGS as in roundtrip.sh.
set -e
T=$(dirname "$(readlink -f "$0")"); G=$T/../..; W=$(readlink -f "$1"); B=${2:-0x400000}; shift 2 || shift $#
[ -z "$REUSE" ] && [ -n "$(ls -A $W 2>/dev/null)" ] && { echo "work dir $W is not empty: use a fresh one (REUSE=1 to override)"; exit 1; }
mkdir -p $W/o
# an_hk_975 (FUN_004acca0, DirectShow) writes the fs:0 pseudo global ExceptionList: it stays original in a linked build (the movie code is stubbed anyway)
export SKIP_FILES=an_hk_975.c${SKIP_FILES:+,$SKIP_FILES}
# LIB_ZLIB=src (A5): zlib 1.1.3 + libpng 1.0.12 (RenderWare rtpng.lib members, not the rtpng.obj wrapper), same mechanism.
# LIB_LUA=src LIB_QHULL=src (A4, MKRANGE/GEN_DATA builds): Lua 4.0 / qhull 2002.1 come from upstream source (output/libmatch2/build_libs.sh, cl 13.00 /G5 /O2 /Ob0), the exe's code range of the lib is dropped
# (tools/delink_sdk/plan_srclib.py, libs.cfg) and the lib's old data rows are not emitted (LIBSRC=1 for globgen/origbytes).  Without them the image keeps the original bytes of the lib.
LSRC=; for L in lua qhull zlib; do eval "v=\$LIB_$(echo $L | tr a-z A-Z)"; [ "$v" = src ] && LSRC="$LSRC $L"; done
if [ -n "$LSRC" ]; then
  export LIBSRC="${LSRC# }"; mkdir -p $W/lib; ITEMS=
  VC7_CONTAINER=libsrc $G/output/libmatch2/build_libs.sh $W/lib/src $LSRC > $W/lib/src.log 2>&1 || { tail -3 $W/lib/src.log; exit 1; }
  : > $W/lib/extra_objs.txt; : > $W/lib/members.txt
  for L in $LSRC; do python3 $G/tools/delink_sdk/plan_srclib.py $L $W/lib/src/$L/_o $W/lib/$L $ITEMS | tail -1; cat $W/lib/$L/extra_objs.txt >> $W/lib/extra_objs.txt; cat $W/lib/$L/members.txt >> $W/lib/members.txt; ITEMS=$W/lib/$L/items.csv; done
  cp $G/tools/delink_sdk/stubs.c $W/lib/; VC7_CONTAINER=ac $G/tools/vc7.sh $W/lib cl.exe /nologo /c /O2 stubs.c > /dev/null; echo "stub:stubs $W/lib/stubs.obj" >> $W/lib/extra_objs.txt      # stdio the exe does not link (dead in Lua / qhull), IID_IUnknown, errno
  EXTRA_OBJS=$W/lib/extra_objs.txt; MKOBJ_ARGS="$MKOBJ_ARGS --drop-members $W/lib/members.txt --drop-items $ITEMS"; export LIBSRC_ITEMS=$ITEMS
fi
# default build = the full gate build
FULL="--automatch --automatch-near --bridges --clusters"
INST=$G/../MashedGame/FullyLoaded/installed
# HARNESS=1: also link the test harness (src/harness/freeze.c: frame freeze, scripted driving, speed-ups) so the state compare of regress.sh can run on the linked exe.
# Its raw data addresses are shifted by DELTA (hex, the .data move printed at the end; rerun with the right value when it says "DELTA mismatch"), the hook list goes to mkobj.
if [ -n "$HARNESS" ]; then mkdir -p $W/harness; python3 - "$W/harness/freeze.c" $((0x${DELTA:-b000})) $G/src/harness/freeze.c <<'PY'
import re, sys
d = int(sys.argv[2])
s = open(sys.argv[3]).read()
s = re.sub(r"\b0x[0-9A-Fa-f]{6}\b", lambda m: "(0x%X + %d)" % (int(m.group(0), 16), d) if 0x5dd000 <= int(m.group(0), 16) < 0x9f9000 else m.group(0), s)
s = s.replace("extern void __stdcall Sleep", "__declspec(dllimport) void __stdcall Sleep")      # the linked exe has no thunk for it: use the IAT slot
open(sys.argv[1], "w").write(s)
PY
# the harness hooks patch call sites inside the ORIGINAL body of these cluster entries (src/harness/cluster_exclude.txt): leave those clusters out (build_overlay does this only for --harness)
export CLUSTER_ENABLED=$(python3 -c "
import sys
x = {l.split()[0] for l in open(sys.argv[2]) if l.strip() and not l.startswith('#')}
print(','.join(n for n in (l.strip() for l in open(sys.argv[1])) if n and not n.startswith('#') and n not in x))" $G/src/clusters/enabled.txt $G/src/harness/cluster_exclude.txt)
set -- "$@" ; [ $# -eq 0 ] && set -- $FULL; set -- "$@" --src=$W/harness/freeze.c; HMK="--hook-calls $G/src/harness/hooks.txt --dwords $G/src/harness/dwords.txt"; fi
JOBS=${JOBS:-4} OBJLIST=$W/objs.txt TMPDIR=$W python3 $G/tools/build_overlay.py $W/ovl.exe ${@:-$FULL} > $W/build_overlay.log 2>&1
python3 $G/tools/wine-run/prepare.py $INST $W/game >/dev/null
cp $W/game/App_Executables/MFL.exe $W/mfl_patched.exe
# LIB_RW=librw:<group> (L2, plan_librw_integration 2.3; needs IMPLIBS=1 and RW_DLL=<mflrw.dll>): the functions named in tools/delink/rw_<group>.txt (g0 = stateless math) come from the librw facade DLL.  A thunk object (gas) defines
# each under the exe function's own name (`_Name: jmp *__imp__Name`), so mkobj puts its usual 5 byte entry jmp at the original address like for any object of ours; mk_importlibs.py builds mflrw.lib from the DLL's exports.
# Without LIB_RW nothing changes (the A/B reference).  The DLL is copied next to ours.exe at the end.
if [ -n "$LIB_RW" ]; then
  RWG=${LIB_RW#librw:}; RWL=$T/rw_$RWG.txt
  [ "$RWG" != "$LIB_RW" ] && [ -f $RWL ] && [ -n "$IMPLIBS" ] && [ -f "$RW_DLL" ] || { echo "LIB_RW=librw:<group> needs $T/rw_<group>.txt, IMPLIBS=1 and RW_DLL=<mflrw.dll>"; exit 1; }
  mkdir -p $W/rw; export RW_DLL=$(readlink -f $RW_DLL)
  grep -v '^#' $RWL | awk 'NF{printf ".text\n.globl _%s\n_%s:\n\tjmp *__imp__%s\n", $1, $1, $1}' > $W/rw/thunks.s
  i686-w64-mingw32-as -o $W/rw/thunks.obj $W/rw/thunks.s || exit 1
  { [ -n "${EXTRA_OBJS:-}" ] && cat $EXTRA_OBJS; echo "rw:thunks $W/rw/thunks.obj"; } > $W/rw/extra_objs.txt; EXTRA_OBJS=$W/rw/extra_objs.txt
else unset RW_DLL; fi
: > $W/objs_local.txt; n=0
while read -r src obj; do n=$((n+1)); cp $obj $W/o/$n.obj; echo "$src $W/o/$n.obj" >> $W/objs_local.txt; done < <(cat $W/objs.txt ${EXTRA_OBJS:-})      # EXTRA_OBJS=<file "src obj" per line>: more objects, e.g. SDK lib members (M2 lib step)
# LITRELOC (T-E6): raw code addresses in the recompiled objects (tools/delink/code_literals.txt) become relocations against func_0x<va> (the copies in $W/o; the sources stay)
# DROP_DSHOW=1 (A7): the DirectShow movie code is dead (the game plays movies through src/video/mfl_video): its objects leave the link, mkobj fills the functions, globgen zeroes the pointers to them
[ -n "$DROP_DSHOW" ] && { python3 $T/dshow_drop.py filter $W; export DROP_DSHOW; }
[ -n "$MKRANGE" ] && python3 $T/litreloc.py $W/objs_local.txt
# GEN_DATA: initialised .data of the harness object would sit between the generated .data and .bss and shift the bss delta (the race scripts use ONE delta): give it a section of its own
[ -n "$GEN_DATA" ] && [ -n "$HARNESS" ] && while read -r src obj; do case $src in freeze.c) objcopy --rename-section .data=.hdat $obj;; esac; done < $W/objs_local.txt
# T-N13: the source-built libs' own initialised .data (zlib 0x14fc B, ...) would likewise sit between the generated .data and .bss and give the bss a bigger delta than .data (harness / race scripts use ONE delta: the
# game then ran with the harness patches at wrong bss addresses = fullscreen ChangeDisplaySettings, exit): own section behind everything
[ -n "$LSRC" ] && while read -r src obj; do case $src in lib:*|stub:*) objcopy --rename-section .data=.ldat $obj;; esac; done < $W/objs_local.txt
# IMPLIBS=1 (S2): the import table is built by the linker from generated import libs (tools/mk_importlibs.py): every import site references __imp__Name@N, the original IAT and import tables are dropped
IMPLIBS_ARG=; IMPLIBS_LNK=
[ -n "$IMPLIBS" ] && { OBJLIST=$W/objs_local.txt python3 $G/tools/mk_importlibs.py $W/imp > $W/mk_importlibs.log 2>&1; IMPLIBS_ARG="--imports-as-symbols $W/imp"; IMPLIBS_LNK="/libpath:imp $(cd $W && ls imp/*.lib | tr '\n' ' ')"; }
# GEN_DATA=1 (S5, needs IMPLIBS=1): the data layout comes from tools/globals/globgen.py (app data and library spans as generated C), mkobj --drop-app removes .rdata/.data/.bss and the replaced function bodies from the image object
GEN_ARG=; GEN_OBJ=
if [ -n "$GEN_DATA" ]; then
  mkdir -p $W/gen; python3 -I $G/tools/globals/globgen.py $W/gen > $W/gen/globgen.log 2>&1 || { tail -5 $W/gen/globgen.log; exit 1; }
  (cd $W/gen && VC7_CONTAINER=ac $G/tools/vc7.sh $W/gen cl.exe /nologo /c /Od /Zl rdata.c data.c bss.c > cl.log 2>&1; for x in rdata data bss; do objcopy $(tr '\n' ' ' < objcopy_$x.txt) $x.obj $x.o2 && mv $x.o2 $x.obj; done)
  [ -f $W/gen/bss.obj ] || { echo "gen compile failed"; tail -5 $W/gen/cl.log; exit 1; }
  GEN_ARG="--drop-app $W/gen"; GEN_OBJ="gen/rdata.obj gen/data.obj gen/bss.obj"
fi
# S3: the resources come from output/res/mfl.res (tools/res/build_res.sh) on the link line, pefix appends no .rsrc.  RES=<path> overrides, RES=0 keeps the old pefix .rsrc
RESF=; [ -z "$RES" ] && [ -n "$GEN_DATA" ] && [ -f $G/output/res/mfl.res ] && RES=$G/output/res/mfl.res
[ "$RES" = 0 ] && RES=; [ -n "$RES" ] && { cp $RES $W/mfl.res; RESF=mfl.res; export NO_RSRC=1; }
python3 $T/mkobj.py $W/mfl.obj --exe $W/mfl_patched.exe --objs $W/objs_local.txt ${PAD:+--pad $PAD} $HMK $IMPLIBS_ARG $GEN_ARG $MKOBJ_ARGS 2> $W/mkobj.err | tee $W/mkobj.log | head -12
[ -n "$LSRC" ] && python3 $G/tools/delink_sdk/undup.py $W/objs_local.txt      # A5: library functions our C defines as well get another name after mkobj (it resolves the references to dropped lib code by the lib objects' names), so no LNK4006
# MKRANGE=1 (Brief G): the code of the image goes into one object per layer (mkrange.py: RenderWare.obj, app.obj, ...), blocks `.text$NNNNN` that link.exe sorts into address order; our own plain `.text`
# must sort behind them (.text$zz)
MFLOBJ=mfl.obj
[ -n "$MKRANGE" ] && { python3 $T/mkrange.py $W/mfl.obj $W/layers > $W/mkrange.log || { tail -3 $W/mkrange.log; exit 1; }; python3 $T/mkrange.py --rename-text $W/o/*.obj
  MFLOBJ="layers/rest.obj $(cd $W && ls layers/*.obj | grep -v rest.obj | tr '\n' ' ')"; }
(cd $W && ls o/*.obj) > $W/link.rsp
# LIB_RW: the thunk object goes FIRST so its definition wins over our own C definition of the same function (link.exe keeps the first one; LNK4006 "second definition ignored" otherwise depends on the file name sort)
[ -n "$LIB_RW" ] && { T1=$(awk '$1=="rw:thunks"{print $2}' $W/objs_local.txt); T1=${T1#$W/}; { echo $T1; grep -vxF $T1 $W/link.rsp; } > $W/link.rsp.new && mv $W/link.rsp.new $W/link.rsp; }
VC7_CONTAINER=ac $G/tools/vc7.sh $W link.exe /nologo /machine:ix86 /subsystem:windows /entry:mfl_entry /fixed /base:$B /nodefaultlib /force:multiple $LINKFLAGS /map:ours.map /out:ours.exe $MFLOBJ $GEN_OBJ $RESF @link.rsp $IMPLIBS_LNK $([ -s $W/mfl.obj.linkopts ] && echo @mfl.obj.linkopts) > $W/link.log 2>&1 || true
echo "link: $(grep -c -E "error" $W/link.log || true) errors, $(grep -c LNK4006 $W/link.log || true) LNK4006"; grep -E "error" $W/link.log | head -8
grep -q "^not found:" $W/mkobj.err && { grep -h "^not found:" $W/mkobj.err | head -5; echo "mkobj: references not found"; exit 1; }
[ -f $W/ours.exe ] || { echo "link failed"; exit 1; }
# the generated .rdata (first symbol g_pespan_005dd000) must start 0 mod 16: SSE tables (_pow_pentium4 movapd) fault otherwise (T-N11)
[ -n "$GEN_DATA" ] && { A=$(awk '$2=="_g_pespan_005dd000"{print $3}' $W/ours.map); [ -n "$A" ] && [ $((0x$A % 16)) -eq 0 ] || { echo "FAIL: generated .rdata starts at 0x$A, not 0 mod 16"; exit 1; }; }
python3 $T/pefix.py $W/ours.exe $W/mfl_patched.exe >/dev/null
python3 $G/tools/wine-run/prepare.py $INST $W/game $W/ours.exe $W/ours.map >/dev/null      # movie stubs also at our C copies (found through the link map)
[ -n "$LIB_RW" ] && cp $RW_DLL $W/game/App_Executables/mflrw.dll      # next to ours.exe (Wine loads it from the exe's directory)
D=$(objdump -h $W/ours.exe | python3 -c "import sys; print('%x' % next(int(l.split()[3], 16) - 0x5fb000 for l in sys.stdin if len(l.split()) > 3 and l.split()[1] == '.data'))")
echo "linked: $W/ours.exe (.data moved by 0x$D: the script delta)"
[ -n "$HARNESS" ] && [ "$D" != "${DELTA:-b000}" ] && { echo "DELTA mismatch: rerun with DELTA=$D"; exit 2; }
[ -n "$RUN" ] && $T/race_relinked.sh $W/game $W/r ${D} ${COURSE:-0}
true
