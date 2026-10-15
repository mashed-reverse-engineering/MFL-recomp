#!/bin/bash
# Crash-only race check of a relinked exe (roundtrip.sh output, no harness): the state driven menu path of race_script.sh into a Quick Battle on
# COURSE (env), with every data address of the script moved by DELTA (PAD=5f8000 build: 0x5f8000, base 0x1000000 build: 0xc00000), then
# DRIVE_S seconds of holding Up in the race.  Prints "course N: ok" (race reached, no crash) or what failed.
# Usage: race_relinked.sh <game dir> <out dir> <delta hex> [course]   (one game process; hard timeout 300 s)
T=$(dirname "$(readlink -f "$0")"); WR=$T/../wine-run; GAME=$1; OUT=$2; D=$((0x$3)); C=${4:-0}
mkdir -p $OUT
COURSE=$C SHOTS=0 $WR/race_script.sh 0 0 0 /dev/null | sed '/^shot race_start/q' | python3 -c "
import re, sys
d = $D
mv = lambda m: '0x%x' % (int(m.group(0), 16) + d) if 0x5dd000 <= int(m.group(0), 16) < 0x9f9000 else m.group(0)
for l in sys.stdin: sys.stdout.write(re.sub(r'0x[0-9a-fA-F]+', mv, l))" > $OUT/script.txt
[ -n "$GDBCRASH" ] && sed -i "1i gdbcrash $GDBCRASH" $OUT/script.txt     # GDBCRASH=N: gdb dumps registers + stack at the next N faults (run/gdbcrash.txt)
printf 'hold Up %s\nshot race_mid\nwait 3\nshot race_end\n' ${DRIVE_S:-10} >> $OUT/script.txt
$WR/gameslot.sh timeout -k 10 300 $WR/play.sh $GAME $OUT/script.txt $OUT/run > $OUT/play.log 2>&1
cr=$(grep -a -E "Unhandled" $OUT/run/wine.log 2>/dev/null | head -1)
if [ -n "$cr" ]; then echo "course $C: CRASH $cr"
elif grep -q "==6 .*(value 6)" $OUT/run/waitmem.txt 2>/dev/null && [ -f $OUT/run/race_end.png ]; then echo "course $C: ok"
else echo "course $C: race not reached ($(grep -a -v '^[0-9.]* \(wait\|key\)' $OUT/run/steps.txt 2>/dev/null | tail -1))"; fi
