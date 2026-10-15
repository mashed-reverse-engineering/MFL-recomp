#!/bin/bash
# Prints a play.sh script that drives a fresh (warmed up) MFL copy into a quick battle and freezes it after N race frames.
# Usage: race_script.sh <freeze_target VA> <race_frames VA> <frames> [<harness map file> [fast]]
# With a map file and the word "fast" the harness speed-up of src/harness/freeze.c is switched on before the title screen (fast_menu_scale 6,
# menus and message boxes about 6x faster); the keys are the same.
# State driven: every key press waits for the game state it must produce (keywait: press, wait for the memory value, press once more if
# it did not arrive).  Game variables (found with probe.pl, see README "Running the game"):
#   g_menu_current 0x69CBD0   menu stack depth: 1 title, 2 main menu ("Game Type Select"; also while the two message boxes are shown),
#                             3 single player, 4 arcade modes, 5 quick battle, 6 colour select, 7 game mode page (page id 24); one more
#                             Return opens the loading screen (phase 1, 'Press START button'), another one starts loading (phase 3)
#   g_message_state 0x69CFD0  0 none, 2 message box shown, 3 box closing (Autosave prompt, then "Save Successful", after the title)
#   g_menu_records[n].selected = 0x69D2E0 + n*0x40 + 8,   g_race_phase 0x652F68 (1 start, 3 loading, 5 countdown, 6 racing)
# Timings seen: popup appears 1.3 s after the title key, closes ~1.5 s after its key, a menu key is accepted ~0.7 s after the box has closed
# (an earlier fixed 1.5 s script lost exactly this key).
FT=$1; RF=$2; FR=$3; MAP=$4; FAST=$5
mapva() { grep "^$1 " $MAP | cut -d' ' -f2; }
if [ "$FAST" = fast ]; then
  echo "poke 0x$(mapva fast_menu_scale) 6"
  [ -n "$DRIVE" ] && echo "poke 0x$(mapva scen_flags) $DRIVE"   # DRIVE = scripted driving bit mask (see freeze.c scen_flags): 1 accelerate, 2 steer, 4 weapons, 8 opponents frozen, 16 countdown throttle
  [ "${NOLIMIT:-2}" != 0 ] && echo "poke 0x$(mapva fast_nolimit) ${NOLIMIT:-2}"   # NOLIMIT = bit mask of the phases without the 60 fps frame limiter: 1 menus/Press START, 2 loading, 4 race (see freeze.c); default 2 = only the loading phase is unlimited (menus + loading together made identical runs differ on course 3, either alone does not)
  [ "$NOSHOW" = 1 ] && echo "poke 0x$(mapva fast_noshow) 1"
fi
CUR=0x69cbd0; MSG=0x69cfd0
sel() { printf "0x%x" $((0x69d2e0 + $1 * 0x40 + 8)); }
echo "waitmem $CUR 1 20"
echo "waitmem 0x69d1e4 0 5"             # title screen fade-in finished (g_menu_fade_alpha): the first key before this was lost
[ "$SHOTS" != 0 ] && echo "shot title"
echo "keywait Return $MSG 2 4"          # title -> Autosave prompt
[ "$SHOTS" != 0 ] && echo "shot msgbox"
# the two message boxes (Autosave prompt, "Save Successful") and then the main menu swallow the Returns: press every 0.6 s until the menu
# stack is at depth 3 (Single Player page); a sped-up box ignores keys for a moment after it appears, so single keywaits are not reliable here
if [ -n "$MULTI" ]; then
  # MULTI=1: Multi Player > Standard Play with two human players (needs DRIVE with bit 128: harness controller_present / player_color_id, see freeze.c); the keys of the page
  # sequence are timed (the colour select page has no stable depth values): Single Player is opened once to get out of the message boxes, Escape goes back to the main menu
  echo "pressuntil Return $CUR 3 20 0.2"; echo "keychange Escape $CUR 3"; echo "wait 1"
  [ "$SHOTS" != 0 ] && echo "shot menu_single"
  echo "key Down"; echo "wait 0.8"
  echo "key Return"; echo "wait 1.5"       # Multi Player page (Standard Play / Team Play)
  [ "$SHOTS" != 0 ] && echo "shot menu_multi"
  for n in colour ability course mode; do
    echo "key Return"; echo "wait 2"; [ "$SHOTS" != 0 ] && echo "shot menu_$n"
  done
else
  echo "pressuntil Return $CUR 3 20 0.2"  # boxes closed, main menu -> Single Player
  # COURSE=n (default 0): scenario with another course.  All courses are opened in the live GameState (progress[270] +0x0 = 1, enabled[270] +0x9D8 = 1),
  # and the course page (menu depth 6) is moved down n times.
  if [ "${COURSE:-0}" != 0 ]; then echo "fill32 0x7eebc0 270 0x1"; echo "fill32 0x7ef598 68 0x01010101"; fi
  [ "$SHOTS" != 0 ] && echo "waitframes 0x$(mapva frames_total) 40" && echo "shot menu_single"
  echo "keywait Down $(sel 3) 1 3"
  echo "keywait Down $(sel 3) 2 3"        # Arcade Modes
  echo "keywait Return $CUR 4 3"
  n=(x x x x x menu_colour menu_course menu_mode)   # depth 5 player colour select, 6 course select (Challenge Select, three medals per course), 7 Game Mode page
  for d in 5 6 7; do
    echo "keywait Return $CUR $d 3"; [ "$SHOTS" != 0 ] && echo "waitframes 0x$(mapva frames_total) 40" && echo "shot ${n[$d]}"
    if [ $d = 6 ]; then for c in $(seq 1 ${COURSE:-0}); do echo "keychange Down 0x69d688 2"; done; fi   # g_career_row (0x69D688, selected row 0..14; rows that are not selectable are skipped)
  done   # Quick Battle, course select (menu_colour = "Challenge Select"), game mode
  # VARIANT="mode=1,pu=2,skill=4,len=2,mk=1": the choices of the Game Mode page, written just before the race is started (power-up set 0..2 @0x69CF90,
  # markers 0/1 @0x69CF94, bot skill 0..4 @0x69CF98, race length 0..2 @0x69CF9C, game mode 0..2 @0x69CFA4; mode 2 only exists for courses with that mode unlocked)
  for kv in $(echo "${VARIANT:-}" | tr ',' ' '); do
    case ${kv%%=*} in pu) a=0x69cf90;; mk) a=0x69cf94;; skill) a=0x69cf98;; len) a=0x69cf9c;; mode) a=0x69cfa4;; *) continue;; esac
    echo "poke $a ${kv#*=}"
  done
fi
echo "keywait Return 0x652f68 !0 5"     # game mode page -> loading screen ("Press START button", phase 1)
echo "shot press_start"
echo "keywait Return 0x652f68 !1 6"     # Press START -> loading (phase 3)
[ "$SHOTS" != 0 ] && echo "shot loading"
[ "$SHOTS" != 0 ] && echo "waitmem 0x652f68 5 30" && echo "shot countdown"
echo "waitmem 0x652f68 6 30"
echo "shot race_start"
SE=$(mapva shadow_enabled); [ -n "$SE" ] && echo "poke 0x$SE 1"      # shadow build: start the tests when the race runs
if [ -n "$QUIT" ]; then
  # QUIT=quit|restart: leave the race through the pause menu (Escape: Continue / Options / Restart Race / Quit Race / Quit Game); use DRIVE=8 so the race is still running (opponents frozen).
  # quit runs the teardown code (course, effect, replay buffers) and returns to the menus, restart runs race_restart; the counters are dumped afterwards (no state dumps).
  echo "probe 40 0x652f68 0x69cbd0 0x69cfd0"      # race phase, menu depth, message state every 0.25 s -> probe.txt
  echo "wait ${QUIT_WAIT:-3}"; echo "shot pause_before"
  echo "key Escape"; echo "wait 1"; echo "shot pause_menu"
  n=3; [ "$QUIT" = restart ] && n=2
  for i in $(seq 1 $n); do echo "key Down"; echo "wait 0.5"; done
  echo "key Return"; echo "wait 2"; echo "shot pause_after_select"
  # the confirmation box ("Are you sure ...", Return = Yes) ignores keys for a moment after it appears (the race phase is not a usable signal: it is the same during the pause)
  echo "wait 1.5"; echo "key Return"; echo "wait 3"; echo "shot pause_after_confirm"
  [ "$QUIT" = restart ] && echo "wait 8" && echo "shot restarted"
  echo "wait 6"; echo "shot pause_end"
  CT=$(mapva count_table); CB=$(mapva count_bytes)
  [ -n "$CT" ] && echo "dump counters 0x$CT 0x$CB"
  exit 0
fi
echo "poke $FT $FR"
echo "waitmem 0x$(mapva frozen) 1 $((20 + FR / 30)) 5 $RF"   # frozen at frame $FR or when the race ended; gives up after 5 s without progress of the race frame counter ($RF), 20 s at most
echo "shot end"
echo "dump cars 0x8B06E0 0xD040"
echo "dump vehicles 0x659f00 0xAB0"
echo "dump aibots 0x8c9ae0 0x1e0"
echo "dump statsrace 0x8c8ae0 0x4e0"
echo "dump powerups 0x6a8bc0 0x7d0"
[ -n "$DRIVE" ] && echo "dump matchpoints 0x8D8B40 0x10" && echo "dump matchmax 0x658DE4 0x4"
SL=$(mapva shadow_log); [ -n "$SL" ] && echo "dump shadowlog 0x$SL 0xC1C0"
if [ $(( ${DRIVE:-0} & 96 )) != 0 ]; then       # win/lose scenario: let the game run through the round end screens (points, standings, next round / match end) after the dumps
  for n in 1 2 3 4 5 6 7 8 9 10 11 12; do echo "wait 6"; echo "shot after$n"; [ $n -ge 2 ] && echo "keyabove Return 0x652f68 6"; done
fi
CT=$(mapva count_table); CB=$(mapva count_bytes)
[ -n "$CT" ] && echo "dump counters 0x$CT 0x$CB"
