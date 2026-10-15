#!/bin/bash
# Test one subset of autocompile chunks against a finished regress.sh work dir.  Usage: bisect.sh <regress work dir> <ok_NNN.c>...
# Prints OK when the overlay run reaches the freeze point and cars/vehicles/aibots dumps equal the control, else BAD (+ differing bytes).
W=$(readlink -f "$1"); shift
T=$(dirname "$(readlink -f "$0")")
A=""; for f in "$@"; do A="$A --src=$f"; done
python3 $T/../build_overlay.py $W/b.exe --harness --no-auto $A >/dev/null 2>$W/b.err || { echo "BUILDFAIL"; exit 1; }
va=$(grep freeze_target $W/b.exe.map | cut -d' ' -f2)
rm -rf $W/run_b $W/out_b; $T/clone.sh $W/snap_ovl $W/run_b
rm -rf $W/prep_b; python3 $T/prepare.py $T/../../../MashedGame/FullyLoaded/installed $W/prep_b $W/b.exe >/dev/null
cp $W/prep_b/App_Executables/MFL.exe $W/run_b/App_Executables/MFL.exe
sed "s/poke 0x[0-9a-f]* /poke 0x$va /" $W/script_ovl.txt > $W/script_b.txt
PERSIST=bis $T/play.sh $W/run_b $W/script_b.txt $W/out_b >/dev/null 2>&1
n=0
for f in cars vehicles aibots statsrace powerups; do
  [ -f $W/out_b/$f.bin ] || { echo "BAD (no $f dump)"; exit 1; }
  n=$((n + $(cmp -l $W/out_ctrl_1/$f.bin $W/out_b/$f.bin | wc -l)))
done
[ $n = 0 ] && echo OK || echo "BAD diff=$n"
