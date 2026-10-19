#include "mfl_tier2.h"
/*0040ff20*/ extern int race_setup_get_slot_type();
/*005dd36c*/ extern float k_f_0_4;
/*0069cbd0*/ extern undefined4 g_menu_current;
/*0069d2e0*/ extern struct MenuRecord g_menu_records[14];
/*0069d6c0*/ extern float g_menu_down_69d6c0;
/*007ef7ec*/ extern float g_frame_seconds;
/*007ef820*/ extern struct PlayerInput g_player_inputs[12];
/*007efce0*/ extern struct PlayerInput g_player_inputs_previous[12];
/*007f01f4*/ extern undefined4 g_input_mode;
/*007f01fc*/ extern struct PlayerSlot g_player_slots[4];
/*007f0238*/ extern char g_player_slot3_extra[];
/*007eff4e*/ extern char g_player_inputs_previous_8__direction_flags_2_[];
/*007f023c*/ extern char g_ai_editor_mode[];

#define NO_CONTROLLER (-1) /* PlayerSlot.controller when no player sits in the slot */
#define DIR_DOWN (3) /* index of the down flag in PlayerInput.direction_flags (inferred: left, right, up, down order) */
#define MENU_DOWN_REPEAT_RESET (0.2) /* value the repeat timer is set to after a repeat fires (inferred) */

/* menu_down_pressed_any */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/*
 * Returns 1 when the menu should move down this frame, 0 otherwise. Two ways to get 1:
 *   - auto-repeat: down is held while the current menu page is 6 or 7, and the hold timer
 *     g_menu_down_69d6c0 passes k_f_0_4 (inferred 0.4); the timer is then reset to MENU_DOWN_REPEAT_RESET
 *   - a fresh press: some player has down held this frame that was not held last frame
 * Takes no parameters; reads g_input_mode, g_player_inputs, g_player_slots and g_menu_records.
 */

undefined4 menu_down_pressed_any(void)

{
  bool any_down_held;
  byte *prev_down_flag;
  int slot_type;
  PlayerSlot *player_slot;
  int slot_index;

  any_down_held = false;
  /* Is down held? Mode 0x1000 (inferred: single shared input) looks at players 0-6, and
     player 7 gates the result; otherwise any slot with a controller, skipping slot type 2 */
  if (g_input_mode == 0x1000) {
    any_down_held = g_player_inputs[6].direction_flags[DIR_DOWN] != 0 ||
            (g_player_inputs[5].direction_flags[DIR_DOWN] != 0 ||
            (g_player_inputs[4].direction_flags[DIR_DOWN] != 0 ||
            (g_player_inputs[3].direction_flags[DIR_DOWN] != 0 ||
            (g_player_inputs[2].direction_flags[DIR_DOWN] != 0 ||
            (g_player_inputs[1].direction_flags[DIR_DOWN] != 0 ||
            g_player_inputs[0].direction_flags[DIR_DOWN] != 0)))));
    if (g_player_inputs[7].direction_flags[DIR_DOWN] == 0) goto LAB_00430439;
  }
  else {
    slot_index = 0;
    player_slot = g_player_slots;
    do {
      if (((player_slot->controller != NO_CONTROLLER) &&
          (slot_type = race_setup_get_slot_type(slot_index), slot_type != 2)) &&
         (g_player_inputs[player_slot->controller].direction_flags[DIR_DOWN] != 0)) {
        any_down_held = true;
      }
      player_slot = player_slot + 1;
      slot_index = slot_index + 1;
    } while ((LONG_PTR)player_slot < ((LONG_PTR)&g_ai_editor_mode));
LAB_00430439:
    if (!any_down_held) {
      g_menu_down_69d6c0 = 0.0;
      goto LAB_00430491;
    }
  }
  /* Auto-repeat: while down is held on menu pages 6-7 (page_id strictly between 5 and 8),
     add the frame time to the hold timer; past k_f_0_4 fire a repeat and restart the timer */
  if (((5 < g_menu_records[g_menu_current].page_id) && (g_menu_records[g_menu_current].page_id < 8))
     && (g_menu_down_69d6c0 = g_frame_seconds + g_menu_down_69d6c0, k_f_0_4 < g_menu_down_69d6c0)
     ) {
    g_menu_down_69d6c0 = MENU_DOWN_REPEAT_RESET;
    return 1;
  }
LAB_00430491:
  /* Fresh press: find the first player whose down is held now but was not held last frame.
     Return 0 if none is found */
  if (g_input_mode == 0x1000) {
    prev_down_flag = g_player_inputs_previous[0].direction_flags + DIR_DOWN;
    while ((prev_down_flag[-0x4c0] == '\0' || (*prev_down_flag != 0))) {
      prev_down_flag = prev_down_flag + 0x4c;
      if (((LONG_PTR)&g_player_inputs_previous_8__direction_flags_2_) < (LONG_PTR)prev_down_flag) {
        return 0;
      }
    }
  }
  else {
    slot_index = 0;
    player_slot = g_player_slots;
    while (((player_slot->controller == NO_CONTROLLER ||
            (slot_type = race_setup_get_slot_type(slot_index), slot_type == 2)) ||
           ((g_player_inputs[player_slot->controller].direction_flags[DIR_DOWN] == 0 ||
            (g_player_inputs_previous[player_slot->controller].direction_flags[DIR_DOWN] != 0))))) {
      player_slot = player_slot + 1;
      slot_index = slot_index + 1;
      if (((LONG_PTR)&g_player_slot3_extra + 0x3) < (LONG_PTR)player_slot) {
        return 0;
      }
    }
  }
  return 1;
}


