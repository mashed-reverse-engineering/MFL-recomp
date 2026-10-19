#include "mfl_tier2.h"
/*0069cb10*/ extern undefined4 g_ui_69cb10;
/*0069cfa4*/ extern undefined4 g_menu_game_mode_choice;
/*007efce0*/ extern struct PlayerInput g_player_inputs_previous[12];
/*007f01fc*/ extern struct PlayerSlot g_player_slots[4];
/*007f007c*/ extern char g_menu_update_7f007c[];

#define PLAYER_INPUT_SIZE (0x4c)
/* byte offset from the previous-frame records back to the current-frame records (inferred: 16 records earlier) */
#define CURRENT_INPUTS_OFFSET (-0x4c0) /* previous to current frame inputs (inferred) */
#define MENU_POS_STRIDE (3)
#define MENU_POS_MAX (2)
#define GAME_MODE_CHOICE_COUNT (2)

/* menu_controller_navigation_update */

/* "per controller checks previous direction flags and adjusts counters" */

/*
 * Menu navigation edge detector, run once per frame.
 * For each of the 12 player input records, if that player is seated in any
 * of the 4 player slots (slot 0 / 1 / 2 / 3 controller index), compares the
 * current and previous left/right direction flags. A new left press (held now,
 * not held last frame) decrements that player's menu position, floored at 0.
 * A new right press increments it, capped at MENU_POS_MAX.
 * Takes no parameters, returns nothing. The per-player menu positions are
 * the MENU_POS_STRIDE-int records starting at g_ui_69cb10 (inferred).
 */

void menu_controller_navigation_update(void)

{
  int slot0_controller;
  int slot3_controller;
  byte *direction_flags_ptr;
  int player_index;
  int *menu_pos_ptr;

  slot3_controller = g_player_slots[3].controller;
  slot0_controller = g_player_slots[0].controller;
  /* reset the game mode choice when it runs past the last option (inferred) */
  if (g_menu_game_mode_choice == GAME_MODE_CHOICE_COUNT) {
    g_menu_game_mode_choice = 0;
  }
  player_index = 0;
  menu_pos_ptr = &g_ui_69cb10;
  direction_flags_ptr = g_player_inputs_previous[0].direction_flags;
  /* loop over the 12 player input records; each step moves to the next record and next menu position */
  do {
    /* skip players that are not seated in any slot */
    if ((slot3_controller == player_index) ||
       (g_player_slots[2].controller == player_index ||
        (g_player_slots[1].controller == player_index || slot0_controller == player_index))) {
      /* left: held now, not held in the previous frame, and position above the floor */
      if ((direction_flags_ptr[CURRENT_INPUTS_OFFSET] != '\0') && ((*direction_flags_ptr == 0 && (*menu_pos_ptr != 0)))) {
        *menu_pos_ptr = *menu_pos_ptr + -1;
      }
      /* right: held now, not held in the previous frame, and position below the cap */
      if (((direction_flags_ptr[CURRENT_INPUTS_OFFSET + 1] != '\0') && (direction_flags_ptr[1] == 0)) && (*menu_pos_ptr < MENU_POS_MAX)) {
        *menu_pos_ptr = *menu_pos_ptr + 1;
      }
    }
    direction_flags_ptr = direction_flags_ptr + PLAYER_INPUT_SIZE;
    player_index = player_index + 1;
    menu_pos_ptr = menu_pos_ptr + MENU_POS_STRIDE;
  } while (direction_flags_ptr < (byte *)&g_menu_update_7f007c);
  return;
}
