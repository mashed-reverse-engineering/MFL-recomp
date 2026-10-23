#include "mfl_tier2.h"
/*0040ff20*/ extern int race_setup_get_slot_type();
/*0069cbd0*/ extern undefined4 g_menu_current;
/*0069d2e0*/ extern struct MenuRecord g_menu_records[14];
/*0069d6c8*/ extern undefined4 g_menu_right_69d6c8;
/*007ef820*/ extern struct PlayerInput g_player_inputs[12];
/*007efce0*/ extern struct PlayerInput g_player_inputs_previous[12];
/*007f01f4*/ extern undefined4 g_input_mode;
/*007f01fc*/ extern struct PlayerSlot g_player_slots[4];
/*007f023c*/ extern char g_ai_editor_mode[];
#define NO_CONTROLLER (-1)
/*
 * Returns 1 if the menu should treat "right" as pressed this frame, 0 otherwise.
 * Right is pressed when some player's right direction was newly pressed (held now,
 * not held last frame) (inferred), or unconditionally on the options page with
 * entry 0..2 selected and on page 0x1e (inferred).
 * When no player holds right, clears g_menu_right_69d6c8.
 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* "direction_flags[1] on any player input (with auto repeat)" */

undefined4 menu_right_pressed_any(void)

{
  bool right_held_any;
  int slot_type;
  undefined4 pressed;
  PlayerSlot *slot;
  int index;

  pressed = 0;
  right_held_any = false;
  /* input mode 0x1000 reads the input records directly (inferred) */
  if (g_input_mode == 0x1000) {
    right_held_any = g_player_inputs[6].direction_flags[1] != 0 ||
            (g_player_inputs[5].direction_flags[1] != 0 ||
            (g_player_inputs[4].direction_flags[1] != 0 ||
            (g_player_inputs[3].direction_flags[1] != 0 ||
            (g_player_inputs[2].direction_flags[1] != 0 ||
            (g_player_inputs[1].direction_flags[1] != 0 ||
            g_player_inputs[0].direction_flags[1] != 0)))));
    if (g_player_inputs[7].direction_flags[1] == 0) goto LAB_004307fb;
  }
  else {
    /* otherwise: each player slot with a controller and slot type != 2 (inferred) */
    index = 0;
    slot = g_player_slots;
    do {
      if (((slot->controller != NO_CONTROLLER) &&
          (slot_type = race_setup_get_slot_type(index), slot_type != 2)) &&
         (g_player_inputs[slot->controller].direction_flags[1] != 0)) {
        right_held_any = true;
      }
      slot = slot + 1;
      index = index + 1;
    } while ((LONG_PTR)slot < ((LONG_PTR)&g_ai_editor_mode));
LAB_004307fb:
    if (!right_held_any) {
      g_menu_right_69d6c8 = 0;
      goto LAB_00430849;
    }
  }
  /* options page: entries 0..2 accept right outright; other entries fall through to the edge check */
  if (g_menu_records[g_menu_current].page_id == MENU_PAGE_SOUND) {
    index = g_menu_records[g_menu_current].selected;
    if ((index != 0) && (index != 1)) {
      if (index == 2) {
        return 1;
      }
      goto LAB_00430849;
    }
  }
  else if (g_menu_records[g_menu_current].page_id != MENU_PAGE_GAMMA) {
LAB_00430849:
    /* edge check: right held now but not in the previous frame (inferred) */
    if (g_input_mode == 0x1000) {
      if ((g_player_inputs[0].direction_flags[1] != 0) &&
         (g_player_inputs_previous[0].direction_flags[1] == 0)) {
        pressed = 1;
      }
      if ((g_player_inputs[1].direction_flags[1] != 0) &&
         (g_player_inputs_previous[1].direction_flags[1] == 0)) {
        pressed = 1;
      }
      if ((g_player_inputs[2].direction_flags[1] != 0) &&
         (g_player_inputs_previous[2].direction_flags[1] == 0)) {
        pressed = 1;
      }
      if ((g_player_inputs[3].direction_flags[1] != 0) &&
         (g_player_inputs_previous[3].direction_flags[1] == 0)) {
        pressed = 1;
      }
      if ((g_player_inputs[4].direction_flags[1] != 0) &&
         (g_player_inputs_previous[4].direction_flags[1] == 0)) {
        pressed = 1;
      }
      if ((g_player_inputs[5].direction_flags[1] != 0) &&
         (g_player_inputs_previous[5].direction_flags[1] == 0)) {
        pressed = 1;
      }
      if ((g_player_inputs[6].direction_flags[1] != 0) &&
         (g_player_inputs_previous[6].direction_flags[1] == 0)) {
        pressed = 1;
      }
      if ((g_player_inputs[7].direction_flags[1] != 0) &&
         (g_player_inputs_previous[7].direction_flags[1] == 0)) {
        return 1;
      }
    }
    else {
      index = 0;
      slot = g_player_slots;
      do {
        if (((slot->controller != NO_CONTROLLER) &&
            (slot_type = race_setup_get_slot_type(index), slot_type != 2)) &&
           ((g_player_inputs[slot->controller].direction_flags[1] != 0 &&
            (g_player_inputs_previous[slot->controller].direction_flags[1] == 0)))) {
          pressed = 1;
        }
        slot = slot + 1;
        index = index + 1;
      } while ((LONG_PTR)slot < ((LONG_PTR)&g_ai_editor_mode));
    }
    return pressed;
  }
  return 1;
}


