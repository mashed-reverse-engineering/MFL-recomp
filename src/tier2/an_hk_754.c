#include "mfl_tier2.h"
/*0040ff20*/ extern int race_setup_get_slot_type();
/*0069c998*/ extern undefined4 g_menu_panel_state_69c998;
/*0069cfd0*/ extern undefined4 g_message_state;
/*007ef820*/ extern struct PlayerInput g_player_inputs[12];
/*007efce0*/ extern struct PlayerInput g_player_inputs_previous[12];
/*007f01f4*/ extern undefined4 g_input_mode;
/*007f01fc*/ extern struct PlayerSlot g_player_slots[4];
/*008c8090*/ extern undefined4 g_menu_8c8090;
/*007f0238*/ extern char g_player_slot3_extra[];
/*007eff49*/ extern char g_player_inputs_previous_8__pause[];

#define CONTROLLER_NONE (-1) /* PlayerSlot.controller of a slot without a player */
#define SLOT_TYPE_SKIPPED (2) /* race_setup_get_slot_type result that is not polled (inferred) */
#define INPUT_MODE_MENU (0x1000) /* g_input_mode while in the menu (names/MFL_globals2.csv) */
#define PLAYER_INPUT_SIZE (0x4c) /* sizeof(struct PlayerInput) */
#define INPUTS_PREV_OFFSET (0x4c0) /* byte distance from g_player_inputs to g_player_inputs_previous */

/* menu_select_pressed_any
 * Returns 1 if any player has a fresh menu_select press this frame (down now, up on the previous frame), else 0.
 * Returns 0 early when the message gate blocks menu input: g_message_state != 2, the menu panel closed
 * (g_menu_panel_state_69c998 == 0) and g_menu_8c8090 != 0 (gate meaning inferred).
 * In menu mode (g_input_mode == INPUT_MODE_MENU) it scans the 8 PlayerInput records; otherwise it scans
 * the 4 player slots and skips slots with no controller or with slot type SLOT_TYPE_SKIPPED (inferred).
 * unused_arg is not read.
 * (was: "edge detect of menu_select on any of the 12 player inputs (blocked while a message is closing)";
 *  the 12 and the message wording do not match the code)
 */

undefined4 __cdecl menu_select_pressed_any(char *unused_arg)

{
  byte *prev_menu_select_ptr;
  int slot_type;
  PlayerSlot *player_slot;
  int slot_index;

  if (((g_message_state != 2) && (g_menu_panel_state_69c998 == 0)) && (g_menu_8c8090 != 0)) {
    return 0;
  }
  if (g_input_mode == INPUT_MODE_MENU) {
    /* Menu mode: walk the player records from player 0. Stop at the first player whose menu_select
       is down now and was up on the previous frame. The end check after each step stops the walk
       once the pointer passes g_player_inputs_previous_8__pause (8 players, inferred). */
    prev_menu_select_ptr = &g_player_inputs_previous[0].menu_select;
    while ((prev_menu_select_ptr[-INPUTS_PREV_OFFSET] == '\0' || (*prev_menu_select_ptr != 0))) {
      prev_menu_select_ptr = prev_menu_select_ptr + PLAYER_INPUT_SIZE;
      if (((char *)&g_player_inputs_previous_8__pause) < (char *)prev_menu_select_ptr) {
        return 0;
      }
    }
    return 1;
  }
  /* Slot mode: walk the 4 player slots in order. Skip a slot with no controller, a slot of type
     SLOT_TYPE_SKIPPED, or a slot whose controller has no fresh menu_select press. Stop after slot 3
     (the end check compares against g_player_slot3_extra + 3). */
  player_slot = g_player_slots;
  slot_index = 0;
  while (((player_slot->controller == CONTROLLER_NONE || (slot_type = race_setup_get_slot_type(slot_index), slot_type == SLOT_TYPE_SKIPPED)
          ) || ((g_player_inputs[player_slot->controller].menu_select == 0 ||
                (g_player_inputs_previous[player_slot->controller].menu_select != 0))))) {
    player_slot = player_slot + 1;
    slot_index = slot_index + 1;
    if (((char *)&g_player_slot3_extra + 0x3) < (char *)player_slot) {
      return 0;
    }
  }
  return 1;
}


