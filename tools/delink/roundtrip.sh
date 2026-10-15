#!/bin/bash
# Delinker round trip: classify -> one COFF object of the whole image -> VC7 link.exe at the original base and at a shifted base -> loadable (pefix)
# -> byte check against the original -> optional Wine run of the shifted exe.
# Usage: roundtrip.sh <work dir> [base=0x1000000]   PAD=5f8000: instead of a shifted base, keep base 0x400000 and move the image up behind a no
#        access section, so every unrelocated pointer into the old range 0x401000-0x9f9000 faults at once (Wine shows the exact address)   (RUN=1: also prepare a game copy and run tools/delink/race.txt in it, needs no other Wine job)
set -e
T=$(dirname "$(readlink -f "$0")"); G=$T/../..; W=$(readlink -f "$1"); B=${2:-0x1000000}; mkdir -p $W
INST=$G/../MashedGame/FullyLoaded/installed
python3 $T/classify.py | head -1
python3 $G/tools/wine-run/prepare.py $INST $W/game >/dev/null      # movie patched copy: its exe is the input image
cp $W/game/App_Executables/MFL.exe $W/mfl_patched.exe
python3 $T/mkobj.py $W/mfl.obj --exe $W/mfl_patched.exe ${PAD:+--pad $PAD} $MKOBJ_ARGS | head -1     # MKOBJ_ARGS="--min-conf medium" etc. for bisecting
[ -n "$PAD" ] && B=0x400000
for b in $(echo 0x400000 $B | tr ' ' '\n' | sort -u); do
  VC7_CONTAINER=ac $G/tools/vc7.sh $W link.exe /nologo /machine:ix86 /subsystem:windows /entry:mfl_entry /fixed /base:$b /nodefaultlib /map:mfl_$b.map /out:mfl_$b.exe mfl.obj
  python3 $T/pefix.py $W/mfl_$b.exe $W/mfl_patched.exe >/dev/null
done
[ -z "$PAD" ] && echo "original base build vs input, differing bytes outside the PE header: $(cmp -l $W/mfl_0x400000.exe $W/mfl_patched.exe | awk '$1 > 4096' | wc -l)"
python3 $T/check_roundtrip.py $W/mfl_$B.exe
rm -f $W/game/App_Executables/MFL.exe; cp $W/mfl_$B.exe $W/game/App_Executables/MFL.exe
if [ -n "$RUN" ]; then
  timeout 300 $G/tools/wine-run/play.sh $W/game $T/race.txt $W/out | tail -1
  grep -a -E "Unhandled|page fault" $W/out/wine.log | head -3 || true
fi
