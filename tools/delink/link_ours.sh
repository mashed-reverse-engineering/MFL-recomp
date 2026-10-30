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
: > $W/objs_local.txt; n=0
while read -r src obj; do n=$((n+1)); cp $obj $W/o/$n.obj; echo "$src $W/o/$n.obj" >> $W/objs_local.txt; done < <(cat $W/objs.txt ${EXTRA_OBJS:-})      # EXTRA_OBJS=<file "src obj" per line>: more objects, e.g. SDK lib members (M2 lib step)
# GEN_DATA: initialised .data of the harness object would sit between the generated .data and .bss and shift the bss delta (the race scripts use ONE delta): give it a section of its own
[ -n "$GEN_DATA" ] && [ -n "$HARNESS" ] && while read -r src obj; do case $src in freeze.c) objcopy --rename-section .data=.hdat $obj;; esac; done < $W/objs_local.txt
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
# MKRANGE=1 (Brief G): the code of the image goes into one object per layer (mkrange.py: RenderWare.obj, app.obj, ...), blocks `.text$NNNNN` that link.exe sorts into address order; our own plain `.text`
# must sort behind them (.text$zz)
MFLOBJ=mfl.obj
[ -n "$MKRANGE" ] && { python3 $T/mkrange.py $W/mfl.obj $W/layers > $W/mkrange.log || { tail -3 $W/mkrange.log; exit 1; }; python3 $T/mkrange.py --rename-text $W/o/*.obj
  MFLOBJ="layers/rest.obj $(cd $W && ls layers/*.obj | grep -v rest.obj | tr '\n' ' ')"; }
(cd $W && ls o/*.obj) > $W/link.rsp
VC7_CONTAINER=ac $G/tools/vc7.sh $W link.exe /nologo /machine:ix86 /subsystem:windows /entry:mfl_entry /fixed /base:$B /nodefaultlib /force:multiple $LINKFLAGS /map:ours.map /out:ours.exe $MFLOBJ $GEN_OBJ $RESF @link.rsp $IMPLIBS_LNK $([ -s $W/mfl.obj.linkopts ] && echo @mfl.obj.linkopts) > $W/link.log 2>&1 || true
echo "link: $(grep -c -E "error" $W/link.log || true) errors, $(grep -c LNK4006 $W/link.log || true) LNK4006"; grep -E "error" $W/link.log | head -8
grep -q "^not found:" $W/mkobj.err && { grep -h "^not found:" $W/mkobj.err | head -5; echo "mkobj: references not found"; exit 1; }
[ -f $W/ours.exe ] || { echo "link failed"; exit 1; }
python3 $T/pefix.py $W/ours.exe $W/mfl_patched.exe >/dev/null
python3 $G/tools/wine-run/prepare.py $INST $W/game $W/ours.exe $W/ours.map >/dev/null      # movie stubs also at our C copies (found through the link map)
D=$(objdump -h $W/ours.exe | python3 -c "import sys; print('%x' % next(int(l.split()[3], 16) - 0x5fb000 for l in sys.stdin if len(l.split()) > 3 and l.split()[1] == '.data'))")
echo "linked: $W/ours.exe (.data moved by 0x$D: the script delta)"
[ -n "$HARNESS" ] && [ "$D" != "${DELTA:-b000}" ] && { echo "DELTA mismatch: rerun with DELTA=$D"; exit 2; }
[ -n "$RUN" ] && $T/race_relinked.sh $W/game $W/r ${D} ${COURSE:-0}
true
