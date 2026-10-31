#!/bin/bash
# Original side of the asset-parity oracle (S2-b): run the standalone harness exe to a frozen frame (menu = main menu, course N = race frame FR) and dump all rw memory
# regions of the live process (play_inner.sh `dumpall`) into <out>/mem.*.bin, then walk them into <out>/oracle.csv (walk.py) and delete the raw dump.
# Usage: scene.sh <link dir (link_ours.sh HARNESS=1 DELTA=..)> <delta hex> <menu|course N> <out dir> [frames=60]       one game at a time (gameslot.sh)
L=$(readlink -f "$1"); D=$2; SC=$3; N=$4; O=$(readlink -f "$5"); FR=${6:-60}
T=$(dirname "$(readlink -f "$0")"); WR=$T/../wine-run; EXE=$L/ours.exe; LM=${EXE%.exe}.map
[ "$SC" = menu ] && { O=$(readlink -f "$4"); FR=${5:-60}; }
mkdir -p $O; W=$O/w; mkdir -p $W
: > $W/fake.map; i=0
for n in freeze_target race_frames frozen scen_flags fast_menu_scale fast_nolimit fast_noshow frames_total; do
  r=$(awk -v n="_$n" '$2==n {print $3; exit}' $LM); echo "$n 7f$(printf %06x $((i*16))) $r" >> $W/fake.map; i=$((i+1)); done
awk '{print $1, $2}' $W/fake.map > $W/map_fake.txt
export NOSHOW=1 SHOTS=0; [ "$SC" = course ] && export COURSE=$N
$WR/race_script.sh 0x7f000000 0x$(awk '$1=="race_frames"{print $2}' $W/fake.map) $FR $W/map_fake.txt fast | python3 -c "
import re, sys
d = int('$D', 16); real = {'0x' + l.split()[1]: '0x' + l.split()[2] for l in open('$W/fake.map')}
def mv(m):
    v = int(m.group(0), 16)
    if m.group(0).lower() in real: return real[m.group(0).lower()]
    return '0x%x' % (v + d) if 0x5dd000 <= v < 0x9f9000 else m.group(0)
for l in sys.stdin: sys.stdout.write(re.sub(r'0x[0-9a-fA-F]+', mv, l))" > $W/script_full.txt
if [ "$SC" = menu ]; then awk '{print} /^pressuntil Return/ {exit}' $W/script_full.txt > $W/script.txt; echo "wait 2" >> $W/script.txt
else cp $W/script_full.txt $W/script.txt; fi
echo "dumpall mem" >> $W/script.txt
$WR/mkpfx.sh 73; rm -rf $W/run; $WR/clone.sh $L/game $W/run
PFX=wine_pfx_73 PERSIST=slot73 $WR/gameslot.sh timeout -k 5 ${RUN_TIMEOUT:-240} $WR/play.sh $W/run $W/script.txt $O/out > /dev/null 2>&1 || echo "run timed out"
grep -a -m1 Unhandled $O/out/wine.log && echo CRASH
tail -2 $O/out/waitmem.txt 2>/dev/null
python3 -I $T/walk.py $O/out mem $O/oracle.csv && { [ -n "$KEEP" ] || rm -f $O/out/mem.*.bin; }
