#!/bin/bash
# Link OUR recompiled C (the objects build_overlay.py compiles; tier 1 + agent matches + hand written, --automatch) together with the delinked
# image: no overlay section, no jmp patches by a script: mkobj --objs puts a REL32 `jmp` to our function at each original entry and defines every
# symbol our objects need at its original address, link.exe resolves the rest.  Usage: link_ours.sh <work dir> [base=0x400000] [build_overlay args]
# RUN=1: crash-only race on COURSE (default 0) afterwards.  PAD / MKOBJ_ARGS as in roundtrip.sh.
set -e
T=$(dirname "$(readlink -f "$0")"); G=$T/../..; W=$(readlink -f "$1"); B=${2:-0x400000}; shift 2 || shift $#; mkdir -p $W/o
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
set -- "$@" ; [ $# -eq 0 ] && set -- --automatch; set -- "$@" --src=$W/harness/freeze.c; HMK="--hook-calls $G/src/harness/hooks.txt --dwords $G/src/harness/dwords.txt"; fi
JOBS=${JOBS:-4} OBJLIST=$W/objs.txt TMPDIR=$W python3 $G/tools/build_overlay.py $W/ovl.exe ${@:---automatch} > $W/build_overlay.log 2>&1
python3 $G/tools/wine-run/prepare.py $INST $W/game >/dev/null
cp $W/game/App_Executables/MFL.exe $W/mfl_patched.exe
: > $W/objs_local.txt; n=0
while read -r src obj; do n=$((n+1)); cp $obj $W/o/$n.obj; echo "$src $W/o/$n.obj" >> $W/objs_local.txt; done < <(cat $W/objs.txt ${EXTRA_OBJS:-})      # EXTRA_OBJS=<file "src obj" per line>: more objects, e.g. SDK lib members (M2 lib step)
python3 $T/mkobj.py $W/mfl.obj --exe $W/mfl_patched.exe --objs $W/objs_local.txt ${PAD:+--pad $PAD} $HMK $MKOBJ_ARGS 2> $W/mkobj.err | tee $W/mkobj.log | head -12
(cd $W && ls o/*.obj) > $W/link.rsp
VC7_CONTAINER=ac $G/tools/vc7.sh $W link.exe /nologo /machine:ix86 /subsystem:windows /entry:mfl_entry /fixed /base:$B /nodefaultlib /force:multiple $LINKFLAGS /map:ours.map /out:ours.exe mfl.obj @link.rsp $([ -s $W/mfl.obj.linkopts ] && echo @mfl.obj.linkopts) > $W/link.log 2>&1 || true
grep -c -E "error|warning" $W/link.log || true; grep -E "error" $W/link.log | head -8
[ -f $W/ours.exe ] || { echo "link failed"; exit 1; }
python3 $T/pefix.py $W/ours.exe $W/mfl_patched.exe >/dev/null
rm -f $W/game/App_Executables/MFL.exe; cp $W/ours.exe $W/game/App_Executables/MFL.exe
D=$(objdump -h $W/ours.exe | python3 -c "import sys; print('%x' % next(int(l.split()[3], 16) - 0x5fb000 for l in sys.stdin if len(l.split()) > 3 and l.split()[1] == '.data'))")
echo "linked: $W/ours.exe (.data moved by 0x$D: the script delta)"
[ -n "$HARNESS" ] && [ "$D" != "${DELTA:-b000}" ] && { echo "DELTA mismatch: rerun with DELTA=$D"; exit 2; }
[ -n "$RUN" ] && $T/race_relinked.sh $W/game $W/r ${D} ${COURSE:-0}
true
