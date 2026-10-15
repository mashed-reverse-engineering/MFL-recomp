#!/bin/bash
# State compare of the LINKED exe (link_ours.sh, HARNESS=1) against the original exe with the same harness (control): the regress.sh dumps (cars vehicles aibots
# statsrace powerups) at the same race frame, noise mask from two control runs, data addresses of the linked script moved by DELTA, data pointers in the dumps
# translated (cmpdump.py CMP_DELTA).  Usage: state_compare.sh <work dir> <linked exe> <delta hex> <course> [frames=90]
# env: DRIVE (scen_flags mask, e.g. 31), NC / NL = number of control / linked runs (2 / 2), SLOTS (two Wine slots, default "71 72"), LOADMAX (6)
W=$(readlink -f "$1"); EXE=$(readlink -f "$2"); D=$3; C=$4; FR=${5:-90}; NC=${NC:-2}; NL=${NL:-2}; SLOTS=${SLOTS:-71 72}
T=$(dirname "$(readlink -f "$0")"); WR=$T/../wine-run; G=$T/../..; INST=$G/../MashedGame/FullyLoaded/installed
mkdir -p $W; export NOSHOW=1 COURSE=$C SHOTS=0
[ -f $W/ctrl.exe ] || python3 $G/tools/build_overlay.py $W/ctrl.exe --no-recomp --harness > /dev/null || exit 1
for k in ctrl lnk; do [ $k = ctrl ] && x=$W/ctrl.exe || x=$EXE
  [ -d $W/snap_$k ] && continue; if [ $k = ctrl ]; then python3 $WR/prepare.py $INST $W/run_$k $x > /dev/null; else rm -rf $W/run_$k; $WR/clone.sh $(dirname $x)/game $W/run_$k; fi   # linked: link_ours.sh already prepared and swapped the exe
  $WR/clone.sh $W/run_$k $W/snap_$k; done
M=$W/ctrl.exe.map; va=$(grep '^freeze_target ' $M | cut -d' ' -f2); rf=$(grep '^race_frames ' $M | cut -d' ' -f2)
$WR/race_script.sh 0x$va 0x$rf $FR $M fast > $W/script_ctrl_c$C.txt
# linked: harness symbols from the link map (their addresses lie inside the shifted data range, so they go through placeholders), everything else moved by DELTA
LM=${EXE%.exe}.map; : > $W/fake.map; i=0
for n in freeze_target race_frames frozen scen_flags fast_menu_scale fast_nolimit fast_noshow frames_total; do
  r=$(awk -v n="_$n" '$2==n {print $3; exit}' $LM); echo "$n 7f$(printf %06x $((i*16))) $r" >> $W/fake.map; i=$((i+1)); done
awk '{print $1, $2}' $W/fake.map > $W/map_fake.txt
$WR/race_script.sh 0x7f000000 0x$(awk '$1=="race_frames"{print $2}' $W/fake.map) $FR $W/map_fake.txt fast | python3 -c "
import re, sys
d = int('$D', 16); real = {'0x' + l.split()[1]: '0x' + l.split()[2] for l in open('$W/fake.map')}
def mv(m):
    v = int(m.group(0), 16)
    if m.group(0).lower() in real: return real[m.group(0).lower()]
    return '0x%x' % (v + d) if 0x5dd000 <= v < 0x9f9000 else m.group(0)
for l in sys.stdin: sys.stdout.write(re.sub(r'0x[0-9a-fA-F]+', mv, l))" > $W/script_lnk_c$C.txt
grep -E "^(poke|waitmem 0x[0-9a-f]{8} 1 )" $W/script_lnk_c$C.txt | head -8
waitload() { while [ $(cut -d' ' -f1 /proc/loadavg | cut -d. -f1) -ge ${LOADMAX:-6} ]; do echo "load high, waiting" >&2; sleep 15; done; }
valid() { python3 -c "import struct,sys; d=open(sys.argv[1],'rb').read(); sys.exit(0 if len(d)>8 and struct.unpack_from('<I',d,4)[0]==1 else 1)" $1 2>/dev/null; }
job() {  # job <k> <run> <slot>
  local k=$1 r=$2 s=$3
  for try in 1 2; do
    waitload; rm -rf $W/run_${k}_c${C}_$r $W/out_${k}_c${C}_$r; $WR/clone.sh $W/snap_$k $W/run_${k}_c${C}_$r
    PFX=wine_pfx_$s PERSIST=slot$s $WR/gameslot.sh timeout -k 5 ${RUN_TIMEOUT:-120} $WR/play.sh $W/run_${k}_c${C}_$r $W/script_${k}_c$C.txt $W/out_${k}_c${C}_$r > /dev/null 2>&1 || echo "run $k $r timed out (try $try)"
    cr=$(grep -a -m1 Unhandled $W/out_${k}_c${C}_$r/wine.log 2>/dev/null); [ -n "$cr" ] && { echo "run $k $r CRASH: $cr"; break; }
    valid $W/out_${k}_c${C}_$r/cars.bin && break; echo "run $k $r invalid (try $try)"
  done
}
set -- $SLOTS; S1=$1; S2=$2; jobs_=(); for r in $(seq 1 $NC); do jobs_+=("ctrl $r"); done; for r in $(seq 1 $NL); do jobs_+=("lnk $r"); done
n=0; while [ $n -lt ${#jobs_[@]} ]; do
  job ${jobs_[$n]} $S1 & p1=$!; n=$((n+1))
  if [ $n -lt ${#jobs_[@]} ]; then sleep 3; job ${jobs_[$n]} $S2 & p2=$!; n=$((n+1)); else p2=; fi
  wait $p1 $p2
done
for k in ctrl lnk; do for d in $W/out_${k}_c${C}_*; do echo "$(basename $d): $(tail -1 $d/waitmem.txt 2>/dev/null)"; done; done
cd $W; OUT=$W/result_c$C.txt; : > $OUT
for f in cars vehicles aibots statsrace powerups; do
  cd1=$W/out_ctrl_c${C}_1/$f.bin; cd2=$W/out_ctrl_c${C}_2/$f.bin
  raw=$(python3 $WR/cmpdump.py $f $cd1 $W/out_lnk_c${C}_1/$f.bin)                         # no translation: what a plain compare says
  tr=$(CMP_DELTA=$D python3 $WR/cmpdump.py $f $cd1 $W/out_lnk_c${C}_1/$f.bin)
  nm=$(CMP_DELTA=$D python3 $WR/cmpdump.py $f $cd1 $W/out_lnk_c${C}_1/$f.bin $cd1 $cd2)
  l2=$(CMP_DELTA=$D python3 $WR/cmpdump.py $f $cd1 $W/out_lnk_c${C}_2/$f.bin $cd1 $cd2)
  ll=$(python3 $WR/cmpdump.py $f $W/out_lnk_c${C}_1/$f.bin $W/out_lnk_c${C}_2/$f.bin)
  echo "$f: ctrl1-ctrl2 $(python3 $WR/cmpdump.py $f $cd1 $cd2)  ctrl1-lnk1 raw $raw translated $tr noise-masked $nm  ctrl1-lnk2 noise-masked $l2  lnk1-lnk2 $ll" | tee -a $OUT
done
