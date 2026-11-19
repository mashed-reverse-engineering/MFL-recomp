# helpers for scenario scripts (sourced by scenario.sh; MAP = overlay map file). A scenario file defines `scenario()` that prints play.sh script lines (see play.sh header).
mapva() { grep "^$1 " $MAP | cut -d' ' -f2; }
CUR=0x69cbd0; MSG=0x69cfd0; PHASE=0x652f68; COLUMN=0x69d690; ROW=0x69d688
sel() { printf "0x%x" $((0x69d2e0 + $1 * 0x40 + 8)); }       # selected entry of the menu record of depth n
scenario_header() {   # fast menus (6x) like regress.sh fast mode; SCEN_DRIVE = scen_flags bit mask (freeze.c), SCEN_NOLIMIT bit mask of the phases without the 60 fps limiter (default 2: loading)
  echo "poke 0x$(mapva fast_menu_scale) 6"
  [ -n "$SCEN_DRIVE" ] && echo "poke 0x$(mapva scen_flags) $SCEN_DRIVE"
  echo "poke 0x$(mapva fast_nolimit) ${SCEN_NOLIMIT:-2}"
  [ -n "$SCEN_NOSHOW" ] && echo "poke 0x$(mapva fast_noshow) 1"
  [ -n "$SCEN_UNLOCK" ] && echo "poke 0x$(mapva unlock_all) 1"      # W15 UNL: freeze.c unlock_all (the game's 100 % state in every front-end frame, bonus menu included)
  echo "waitmem $CUR 1 30"; echo "waitmem 0x69d1e4 0 8"          # title screen faded in
}
scenario_footer() { echo "wait ${SCEN_END_WAIT:-1}"; echo "shot final"; CT=$(mapva count_table); CB=$(mapva count_bytes); echo "dump counters 0x$CT 0x$CB"; }
# title -> Autosave prompt -> main menu -> Single Player page (depth 3)
to_single() { echo "keywait Return $MSG 2 4"; echo "pressuntil Return $CUR 3 20 0.2"; echo "waitframes 0x$(mapva frames_total) 40"; }
# back to the main menu (depth 2)
to_main() { echo "keychange Escape $CUR 3"; echo "wait 1"; }
# n x key with a wait after each
keys() { local w=$1; shift; for k in "$@"; do echo "key $k"; echo "wait $w"; done; }
# W13 hx5: n x key, each followed by 4 game frames (frames_total) instead of a fixed time (slider / list moves inside a page)
keysf() { for k in "$@"; do echo "key $k"; echo "waitframes 0x$(mapva frames_total) 4"; done; }
# W13 hx5: a key that opens / closes a menu page: sent until the menu depth changes (keychange, one repeat), then 40 game frames for the
# page's fade-in, so a shot never samples the transition.  Only for keys that change the depth (probe of options_all / multi_pages, C3 notes).
pagekey() { echo "keychange $1 $CUR 3"; echo "waitframes 0x$(mapva frames_total) 40"; }
# W13 hx5: wait for the round to end (race phase leaves 6); a miss is a missed required event (tag end)
race_end() { echo "waitevent $PHASE !6 ${SCEN_RACECAP:-180} end"; echo "shot race_end"; }
# ---- race helpers
unlock_all() { echo "fill32 0x7eebc0 270 0x1"; echo "fill32 0x7ef598 68 0x01010101"; }      # all courses / events open in the live GameState (like COURSE=n of race_script.sh)
# from the loading / Press START screens to race phase 6 (racing): Return is harmless in all of them (Press START, tutorial boxes)
race_go() { echo "pressuntil Return $PHASE 6 ${1:-120} 0.7"; }
# pause menu "Transmission Interrupted": Continue / Options / Restart Race / Quit Race / Quit Game.  race_pause <downs> <yes 0|1>: Escape, Down x n, Return, [confirm]
race_pause() { echo "key Escape"; echo "wait 1.2"; echo "shot pause_d${1:-3}"; for i in $(seq 1 ${1:-3}); do echo "key Down"; echo "wait 0.5"; done; echo "key Return"; echo "wait 1.8"; [ "${2:-1}" = 1 ] && { echo "key Return"; echo "wait 4"; }; }
race_quit() { race_pause 3 1; echo "shot after_quit"; }
race_restart() { race_pause 2 1; echo "wait 8"; echo "shot after_restart"; }
# round end screens (Current Standings, Point Winner, next round): Return only while the race phase is above 6
# W13 hx5: end_screens <menu depth>: Return on the end screens (only while the phase is above 6) until the menu depth is back at <depth>
# (arcade: 6 = course page, cup: 5); was 8 / 10 x `wait 6` + keyabove.  A miss (cap 300 s) is a missed required event.
end_screens() { echo "returnloop $CUR ${1:-6} $PHASE 300 end"; echo "shot results_done"; }
# W14 A1: arcade Quick Battle end (probes agents/asm_w14 a1probe, a1probe2, a1probe3).  Round end phase 7 (Current Standings, "Continue")
# and battle end phase 11 (race camera, cars stopped) each wait for Return; 11 -> 9 -> 10 Race Results follows by itself.  returnloop:
# Return once a second unless phase 6 (a running round), until phase 10 = the required end event (tag end), as ss_arc_lose ends.
# Not used: the way from the results page back to the menus.  Its depth word moves 1 -> 2 -> 3 on Return (3 = race again); Escape at
# depth 1 and Down + Return at depth 2 left nothing within 30 s (a1probe3, both sides): a later probe item, not a guess here.
arc_results() { echo "returnloop $PHASE 10 $PHASE ${SCEN_RACECAP:-300} end"; echo "waitframes 0x$(mapva frames_total) 40"; echo "shot results_done"; }
# main menu (depth 2) -> last entry "Exit To Windows" -> Yes: runs the shutdown code, the process ends
exit_windows() { keys 0.7 Down Down Down; echo "shot exit_sel"; echo "key Return"; echo "wait 1.5"; echo "shot exit_box"; echo "key Return"; echo "wait 4"; }
# freeze both builds at the same race frame (harness freeze_target, like regress.sh) so the final screenshot and the car dump are comparable; the game does not advance afterwards
# W13 hx5 (finding 25): freeze.c sets frozen = 1 at the target frame, 2 when the round ended before it; both end the wait (waitmem.txt
# records the value).  Not a required `== 1` event: the DRIVE=31 sweep rows end their round before frame 300 on both sides (gate 1117_0021).
race_freeze() { echo "poke 0x$(mapva freeze_target) ${1:-300}"; echo "waitmem 0x$(mapva frozen) !0 $((30 + ${1:-300} / 20)) 8 0x$(mapva race_frames)"; echo "dump cars 0x8B06E0 0xD040"; }
