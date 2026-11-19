#include "mfl_tier2.h"
#include "mfl_fix.h"
/*007eebc0*/ extern struct GameState g_game_state;
/*007ef820*/ extern struct PlayerInput g_player_inputs[12];
/*00801b44*/ extern undefined4 g_savegame_replay[0x1000];
/*008432d0*/ extern undefined4 g_savegame_state[0x1000];
/*00843dc8*/ extern undefined4 g_gamesave_843dc8;
/*008d8b08*/ extern char *g_replay_buffer;
/*008d8c0c*/ extern undefined4 g_gamesave_flag_c0c;
extern void *memcpy(void *, const void *, unsigned int);
#pragma intrinsic(memcpy)
/* gamesave_apply */

/* g_savegame -> live state (replay buffer and game state block) */

void gamesave_apply(void)

{
  uint *save_src;
  int count;

  memcpy(&g_game_state, g_savegame_state, 0x2d9 * 4);
  count = 0;
  save_src = &g_player_inputs[0].rumble_enabled;
  do {
    *save_src = (uint)g_game_state.player_bytes[count];
    save_src = save_src + 0x13;
    count = count + 1;
  } while ((char *)save_src < (char *)&g_player_inputs + 0x3b4);
  g_gamesave_flag_c0c = g_gamesave_843dc8;
  if (g_replay_buffer != 0) {
    memcpy((void *)g_replay_buffer, g_savegame_replay, 0x105e3 * 4);
  }
#if MFL_FIX_UNLOCK_ALL || MFL_FIX_PREFS
  mfl_unlock_all_force();
#endif
  return;
}

#if MFL_FIX_UNLOCK_ALL || MFL_FIX_PREFS
#define UNLOCK_PROGRESS_CELLS (270)
#define UNLOCK_CELL_DONE (2)
static uint real_progress[UNLOCK_PROGRESS_CELLS];   /* the words the save file gets while the switch forces the live ones */
static uint real_bonus_entry;
static int have_real;

/* MFL_FIX_UNLOCK_ALL, called after the boot defaults (game_options_reset) and after a save load (gamesave_apply): keeps the real progress
   words and the bonus menu flag, then sets what the game's own attract-screen code sequence sets (frontend_attract_menu_input).
   Without the compile switch it acts only when the MFL_FIX_PREFS key unlock_all is 1. */
void mfl_unlock_all_force(void)
{
  int i;

#if !MFL_FIX_UNLOCK_ALL
  if (!mfl_pref(MFL_PREF_UNLOCK_ALL)) {
    return;
  }
#endif
  for (i = 0; i < UNLOCK_PROGRESS_CELLS; i++) {
    real_progress[i] = g_game_state.progress[i];
    g_game_state.progress[i] = UNLOCK_CELL_DONE;
  }
  real_bonus_entry = g_game_state.option_flags[0];
  g_game_state.option_flags[0] = 1;
  have_real = 1;
}

/* MFL_FIX_UNLOCK_ALL, called by gamesave_gather after its block copy: the save block gets the real words back, so the user's save never holds
   the forced state (career progress won while the switch is on is not saved; options, records and best times are). */
void mfl_unlock_all_unforce(void *save_block)
{
  struct GameState *save = (struct GameState *)save_block;
  int i;

  if (!have_real) {
    return;
  }
  for (i = 0; i < UNLOCK_PROGRESS_CELLS; i++) {
    save->progress[i] = real_progress[i];
  }
  save->option_flags[0] = real_bonus_entry;
}
#endif
