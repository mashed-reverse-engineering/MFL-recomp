# W15 PREF: save / load message box wait.  options_save.sh up to Save Game, plus dumps of g_gamesave_wait_timer (0x8D8C08, float) after the load
# and after the save: the timer stops one frame after the box closed.  Original: > 1.0 (0x3F80xxxx or more) both times.  MFL_FIX_PREFS link with
# skip_save_wait=1 (SCEN_PREFS file): about one frame time, far below 1.0.
scenario() {
  to_single; to_main
  keys 0.6 Down Down; echo "key Return"; echo "wait 2"
  keys 0.6 Down Down; echo "key Return"; echo "wait 2"; echo "shot ls_page"
  # RES W15: fast_menu_scale > 1 makes freeze.c force g_gamesave_wait_timer = 2.0 in every front-end frame (both sides read 2.0 in run 474);
  # scale 1 from the confirm key until the dump, so the timer is the game's own.
  FM=$(mapva fast_menu_scale)
  echo "key Return"; echo "wait 1.5"; echo "shot ls_load_box"; echo "poke 0x$FM 1"; echo "key Return"; echo "wait 2.5"; echo "shot ls_load_ok"; echo "dump timer_load 0x8D8C08 4"; echo "poke 0x$FM 6"; echo "key Return"; echo "wait 2"
  keys 0.6 Down; echo "key Return"; echo "wait 1.5"; echo "shot ls_save_box"; echo "poke 0x$FM 1"; echo "key Return"; echo "wait 2.5"; echo "shot ls_save_ok"; echo "dump timer_save 0x8D8C08 4"; echo "poke 0x$FM 6"; echo "key Return"; echo "wait 2"
}
