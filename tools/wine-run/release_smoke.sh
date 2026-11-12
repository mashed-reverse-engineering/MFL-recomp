#!/bin/bash
# release_smoke.sh <link dir> <out dir> [title|race]: boot smoke of a tools/delink/link_release.sh exe (no harness; joins the full gate in W6).
# title: intro -> title screen faded in (menu depth 1 at 0x69cbd0, fade alpha 0x69d1e4 = 0, both moved by the link's delta), screenshot.
# race: tools/delink/race_relinked.sh (title -> Quick Battle COURSE -> DRIVE_S s of driving).  Env: DENV="-e K=V ..." into the game
# container (play.sh, non-PERSIST path), WDBG = WINEDEBUG, BOOT_S = title timeout (default 90; 150 with the FFmpeg DLL: intro.mpg is 63.7 s).
# WDBG: leave it at the default (-all). Any setting that enables fixme (err, err+all) lets the d3d fixme "Unhandled usage flags" trip play_inner's crash watcher (greps wine.log for "Unhandled") and aborts the run at once.
# Prints "release_smoke <mode>: ok" or "release_smoke <mode>: FAIL <why>".
WR=$(dirname "$(readlink -f "$0")"); L=$(readlink -f "$1"); OUT=$(readlink -f "$2"); M=${3:-title}; D=$(cat $L/delta.txt); B=${BOOT_S:-90}
mkdir -p $OUT
[ "$M" = race ] && exec $WR/../delink/race_relinked.sh $L/game $OUT $D ${COURSE:-0}
printf 'waitmem 0x69cbd0 1 %s\nwaitmem 0x69d1e4 0 10\nwait 2\nshot title\n' $B | python3 $WR/addrmove.py $D > $OUT/script.txt
$WR/gameslot.sh timeout -k 10 $((B + 60)) $WR/play.sh $L/game $OUT/script.txt $OUT/run > $OUT/play.log 2>&1
cr=$(grep -a -i -E "Unhandled exception|unimplemented function" $OUT/run/wine.log 2>/dev/null | head -1)
if [ -n "$cr" ]; then echo "release_smoke title: FAIL $cr"; exit 1; fi
grep -q "==1 .*(value 1)" $OUT/run/waitmem.txt 2>/dev/null && grep -q "==0 .*(value 0)" $OUT/run/waitmem.txt && [ -f $OUT/run/title.png ] && { echo "release_smoke title: ok"; exit 0; }
echo "release_smoke title: FAIL title not reached ($(tail -1 $OUT/run/waitmem.txt 2>/dev/null))"; exit 1
