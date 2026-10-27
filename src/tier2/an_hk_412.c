#include "mfl_tier2.h"
/*004b4ba7*/ extern int entry();
/*0076e80c*/ extern undefined4 g_frontend_init_76e80c;
/*007eebc0*/ extern struct GameState g_game_state;
/*007ef820*/ extern struct PlayerInput g_player_inputs[12];
/*007efce0*/ extern struct PlayerInput g_player_inputs_previous[12];
/* indices into PlayerInput.direction_flags */
#define DIR_LEFT (0)
#define DIR_RIGHT (1)
#define DIR_UP (2)
#define DIR_DOWN (3)
/* values of key_sequence[] and the switch cases (inferred: each case reads that direction's flags) */
#define KEY_UP (0)
#define KEY_DOWN (1)
#define KEY_RIGHT (2)
#define KEY_LEFT (3)
#define KEY_SEQUENCE_LEN (12)
#define PROGRESS_CELLS (270)
#define CELL_DONE (2)
#define ACTION_RESET (2)
/* frontend_attract_menu_input */
/* "12 step direction-key sequence on pads 0 and 2 (cheat code), unlocks all progress cells" */
/* Attract-screen key sequence (inferred): key_sequence[] is the direction expected at each of the
 * KEY_SEQUENCE_LEN steps, and g_frontend_init_76e80c is the current step. A newly pressed
 * expected direction on pad 0 advances the step; any other newly pressed direction on pad 0 or 2
 * resets to step 0. Pad 2 newly pressing the expected direction also advances. Reaching the
 * last step marks every progress cell done. Takes no parameters, returns nothing. */

void frontend_attract_menu_input(void)

{
  byte pad2_prev;
  byte pad2_now;
  int action;
  GameState *cell;
  undefined4 key_sequence [KEY_SEQUENCE_LEN];
  
  key_sequence[2] = KEY_DOWN;
  key_sequence[6] = KEY_DOWN;
  key_sequence[9] = KEY_DOWN;
  key_sequence[0xb] = KEY_DOWN;
  key_sequence[0] = KEY_UP;
  key_sequence[1] = KEY_RIGHT;
  key_sequence[3] = KEY_LEFT;
  key_sequence[4] = KEY_UP;
  key_sequence[5] = KEY_LEFT;
  key_sequence[7] = KEY_RIGHT;
  key_sequence[8] = KEY_UP;
  key_sequence[10] = KEY_UP;
  if (((int)g_frontend_init_76e80c < 0) || ((KEY_SEQUENCE_LEN - 1) < (int)g_frontend_init_76e80c)) {
    g_frontend_init_76e80c = 0;
  }
  action = 0;
  /* any newly pressed direction on pad 0 or 2 sets ACTION_RESET (overridden by the expected key below) */
  if ((g_player_inputs[0].direction_flags[DIR_UP] != 0) &&
     (g_player_inputs_previous[0].direction_flags[DIR_UP] == 0)) {
    action = ACTION_RESET;
  }
  if ((g_player_inputs[2].direction_flags[DIR_UP] != 0) &&
     (g_player_inputs_previous[2].direction_flags[DIR_UP] == 0)) {
    action = ACTION_RESET;
  }
  if ((g_player_inputs[0].direction_flags[DIR_DOWN] != 0) &&
     (g_player_inputs_previous[0].direction_flags[DIR_DOWN] == 0)) {
    action = ACTION_RESET;
  }
  if ((g_player_inputs[2].direction_flags[DIR_DOWN] != 0) &&
     (g_player_inputs_previous[2].direction_flags[DIR_DOWN] == 0)) {
    action = ACTION_RESET;
  }
  if ((g_player_inputs[0].direction_flags[DIR_LEFT] != 0) &&
     (g_player_inputs_previous[0].direction_flags[DIR_LEFT] == 0)) {
    action = ACTION_RESET;
  }
  if ((g_player_inputs[2].direction_flags[DIR_LEFT] != 0) &&
     (g_player_inputs_previous[2].direction_flags[DIR_LEFT] == 0)) {
    action = ACTION_RESET;
  }
  if ((g_player_inputs[0].direction_flags[DIR_RIGHT] != 0) &&
     (g_player_inputs_previous[0].direction_flags[DIR_RIGHT] == 0)) {
    action = ACTION_RESET;
  }
  if ((g_player_inputs[2].direction_flags[DIR_RIGHT] != 0) &&
     (g_player_inputs_previous[2].direction_flags[DIR_RIGHT] == 0)) {
    action = ACTION_RESET;
  }
  /* pad 0 newly pressing the expected direction sets action 1 (advance); pad 2 is tracked for the next check */
  switch(key_sequence[g_frontend_init_76e80c]) {
  case KEY_UP:
    pad2_prev = g_player_inputs_previous[2].direction_flags[DIR_UP];
    pad2_now = g_player_inputs[2].direction_flags[DIR_UP];
    if ((g_player_inputs[0].direction_flags[DIR_UP] != 0) &&
       (g_player_inputs_previous[0].direction_flags[DIR_UP] == 0)) {
      action = 1;
    }
    break;
  case KEY_DOWN:
    pad2_prev = g_player_inputs_previous[2].direction_flags[DIR_DOWN];
    pad2_now = g_player_inputs[2].direction_flags[DIR_DOWN];
    if ((g_player_inputs[0].direction_flags[DIR_DOWN] != 0) &&
       (g_player_inputs_previous[0].direction_flags[DIR_DOWN] == 0)) {
      action = 1;
    }
    break;
  case KEY_RIGHT:
    pad2_prev = g_player_inputs_previous[2].direction_flags[DIR_RIGHT];
    pad2_now = g_player_inputs[2].direction_flags[DIR_RIGHT];
    if ((g_player_inputs[0].direction_flags[DIR_RIGHT] != 0) &&
       (g_player_inputs_previous[0].direction_flags[DIR_RIGHT] == 0)) {
      action = 1;
    }
    break;
  case KEY_LEFT:
    pad2_prev = g_player_inputs_previous[2].direction_flags[DIR_LEFT];
    pad2_now = g_player_inputs[2].direction_flags[DIR_LEFT];
    if ((g_player_inputs[0].direction_flags[DIR_LEFT] != 0) &&
       (g_player_inputs_previous[0].direction_flags[DIR_LEFT] == 0)) {
      action = 1;
    }
    break;
  default:
    goto switchD_004a1c85_default;
  }
  /* pad 2 newly pressing the expected direction advances without the action check;
   * otherwise action 1 advances, ACTION_RESET restarts at step 0, and 0 leaves the step alone */
  if ((pad2_now == 0) || (pad2_prev != 0)) {
switchD_004a1c85_default:
    if (action != 1) {
      if (action == ACTION_RESET) {
        g_frontend_init_76e80c = 0;
        return;
      }
      goto LAB_004a1d27;
    }
  }
  g_frontend_init_76e80c = g_frontend_init_76e80c + 1;
LAB_004a1d27:
  /* sequence complete: mark all progress cells done and set option_flags[0] (inferred meaning) */
  if (g_frontend_init_76e80c == KEY_SEQUENCE_LEN) {
    cell = &g_game_state;
    for (action = PROGRESS_CELLS; action != 0; action = action + -1) {
      cell->progress[0] = CELL_DONE;
      cell = (GameState *)(cell->progress + 1);
    }
    g_game_state.option_flags[0] = 1;
  }
  return;
}


