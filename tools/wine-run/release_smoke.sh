#!/bin/bash
# release_smoke.sh <link dir> <out dir> [title|race|play]: boot smoke of a tools/delink/link_release.sh exe (no harness; joins the full gate in W6).
# title: intro -> title screen faded in (menu depth 1 at 0x69cbd0, fade alpha 0x69d1e4 = 0, both moved by the link's delta), screenshot.
# race: tools/delink/race_relinked.sh (title -> Quick Battle COURSE -> DRIVE_S s of driving).  Env: DENV="-e K=V ..." into the game
# container (play.sh, non-PERSIST path), WDBG = WINEDEBUG, BOOT_S = title timeout (default 90; 150 with the FFmpeg DLL: intro.mpg is 63.7 s).
# play (SMOKE W17; the real-user crashes were at the pause menu): the menu path of race_relinked.sh into Quick Battle course 0, race phase 6
# (g_race_phase 0x652f68), 1 s idle, still phase 6, Escape -> pause menu "Transmission Interrupted" = g_app_state 0x76e7f4 == 4 (frontend_init_menus:
# race 3 -> 5 on frontend_pause_input_poll, -> 4 when the page is up, probe smoke_w17; menu depth is NOT the pause state, hng2), held 2 s with the
# race clock g_race_clock_a 0x7ef7dc frozen, Return = Continue -> g_app_state 6 -> 3, 1 s idle, race clock advanced; then Escape, Down until the
# pause page row (g_menu_records[1].selected) is 4 = Quit Game, Return, Return on "quit the game?" (g_message_state 0x69cfd0 == 2) -> main menu
# (g_app_state 1, depth 2: the race teardown ran); the harness then ends the process (Quit Game leaves the race, not the program).
# (Short race on purpose: course 0's first point ends (phase 7, Current Standings) ~4 s of race clock after phase 6, idle or holding Up
# (smoke_w17 probes: g_race_clock_a 24750 at phase 6, phase 7 at 36050 / 36750); 1 s + 1 s of racing keeps both checks inside phase 6.)
# Every play verdict fails on a crash line in wine.log ("Unhandled", "page fault"), on warn:heap / err:heap lines (WDBG=fixme-all,warn+heap,
# the heap check run; plain warn+heap lets the d3d fixme "Unhandled usage flags" trip the crash watcher) and on mflng-crash.txt.
# WDBG: leave it at the default (-all) for title/race. Any setting that enables fixme (err, err+all) lets the d3d fixme "Unhandled usage flags" trip play_inner's crash watcher (greps wine.log for "Unhandled") and aborts the run at once.
# title and play also write <out dir>/run/loaded.maps (play_inner `maps`: the process's mappings once the title is up / the race runs, i.e. after
# the audio device and the intro movie were opened): tools/ci/smoke_release.py reads from it which folder the backend DLLs came from (W17 LAYOUT C).
# Prints "release_smoke <mode>: ok" or "release_smoke <mode>: FAIL <why>".
WR=$(dirname "$(readlink -f "$0")"); L=$(readlink -f "$1"); OUT=$(readlink -f "$2"); M=${3:-title}; D=$(cat $L/delta.txt); B=${BOOT_S:-90}
mkdir -p $OUT
[ "$M" = race ] && exec $WR/../delink/race_relinked.sh $L/game $OUT $D ${COURSE:-0}
va() { printf "0x%x" $(($1 + 0x$D)); }      # an original data address in this link
if [ "$M" = play ]; then
  PH=0x652f68; APP=0x76e7f4; CLK=0x7ef7dc; MSG=0x69cfd0; SEL1=0x69d328      # SEL1 = g_menu_records[1].selected (0x69d2e0 + 0x40 + 8)
  { COURSE=0 SHOTS=0 $WR/race_script.sh 0 0 0 /dev/null | sed '/^shot race_start/q'
    printf '%s\n' "probe 120 $PH 0x69cbd0 $APP $CLK $MSG $SEL1" "maps loaded" "wait 1" "waitmem $PH 6 1" \
      "key Escape" "waitmem $APP 4 5" "wait 1" "shot pause" "dump clk_p1 $CLK 4" "wait 2" "dump clk_p2 $CLK 4" "waitmem $APP 4 1" \
      "key Return" "waitmem $APP 3 10" "dump clk_r1 $CLK 4" "wait 1" "dump clk_r2 $CLK 4" "shot resumed" \
      "key Escape" "waitmem $APP 4 5" "pressuntil Down $SEL1 4 10 0.6" "waitmem $SEL1 4 1" "shot quit_row" "key Return" \
      "waitmem $MSG 2 5" "wait 1.5" "shot quit_box" "keywait Return $APP 1 10" "waitmem 0x69cbd0 2 10" "wait 2" "shot quit_menu"
  } | python3 $WR/addrmove.py $D > $OUT/script.txt
  $WR/gameslot.sh timeout -k 10 400 $WR/play.sh $L/game $OUT/script.txt $OUT/run > $OUT/play.log 2>&1
  W=$OUT/run/waitmem.txt; A=$(va $APP); R=$OUT/run
  u4() { od -An -tu4 $R/$1.bin 2>/dev/null | tr -d ' '; }
  cr=$(grep -a -E "Unhandled|page fault|warn:heap|err:heap" $R/wine.log 2>/dev/null | head -1)
  nh=$(grep -a -c -E "warn:heap|err:heap" $R/wine.log 2>/dev/null)
  if [ -n "$cr" ]; then why="wine.log ($nh heap lines): $cr"
  elif [ -f $L/game/App_Executables/mflng-crash.txt ]; then why="mflng-crash.txt: $(head -1 $L/game/App_Executables/mflng-crash.txt)"
  elif [ "$(grep "^waitmem $(va $PH)==6" $W 2>/dev/null | grep -c "(value 6)")" != 2 ]; then why="race not reached or not racing at the Escape ($(grep "^waitmem $(va $PH)==" $W 2>/dev/null | tail -1))"
  elif [ "$(grep "^waitmem $A==4" $W | head -2 | grep -c "(value 4)")" != 2 ]; then why="pause menu not reached or not held (g_app_state: $(grep "^waitmem $A==4" $W | head -2 | tr '\n' ' '))"
  elif [ -z "$(u4 clk_p2)" ] || [ "$(u4 clk_p2)" != "$(u4 clk_p1)" ]; then why="race clock ran while paused ($(u4 clk_p1) -> $(u4 clk_p2))"
  elif ! grep -q "^waitmem $A==3 .*(value 3)" $W; then why="race not resumed after Continue ($(grep "^waitmem $A==3" $W | tail -1))"
  elif [ -z "$(u4 clk_r2)" ] || [ "$(u4 clk_r2)" -le "$(u4 clk_r1)" ]; then why="race clock did not advance after resume ($(u4 clk_r1) -> $(u4 clk_r2))"
  elif ! grep -q "^waitmem $A==1 .*(value 1)" $W || ! grep -q "^waitmem $(va 0x69cbd0)==2 .*(value 2)" $W; then why="Quit Game: main menu not reached ($(tail -1 $W))"
  else echo "release_smoke play: ok (paused clock $(u4 clk_p1) -> $(u4 clk_p2), resumed $(u4 clk_r1) -> $(u4 clk_r2), Quit Game -> main menu)"; exit 0; fi
  echo "release_smoke play: FAIL $why"; exit 1
fi
printf 'waitmem 0x69cbd0 1 %s\nwaitmem 0x69d1e4 0 10\nwait 2\nshot title\nmaps loaded\n' $B | python3 $WR/addrmove.py $D > $OUT/script.txt
$WR/gameslot.sh timeout -k 10 $((B + 60)) $WR/play.sh $L/game $OUT/script.txt $OUT/run > $OUT/play.log 2>&1
cr=$(grep -a -i -E "Unhandled exception|unimplemented function" $OUT/run/wine.log 2>/dev/null | head -1)
if [ -n "$cr" ]; then echo "release_smoke title: FAIL $cr"; exit 1; fi
grep -q "==1 .*(value 1)" $OUT/run/waitmem.txt 2>/dev/null && grep -q "==0 .*(value 0)" $OUT/run/waitmem.txt && [ -f $OUT/run/title.png ] && { echo "release_smoke title: ok"; exit 0; }
echo "release_smoke title: FAIL title not reached ($(tail -1 $OUT/run/waitmem.txt 2>/dev/null))"; exit 1
