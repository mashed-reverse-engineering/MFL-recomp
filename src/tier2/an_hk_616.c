#include "mfl_tier2.h"
/*0040ff20*/ extern int race_setup_get_slot_type();
/*0069cbd0*/ extern undefined4 g_menu_current;
/*0069d2e0*/ extern struct MenuRecord g_menu_records[14];
/*0069d6c4*/ extern undefined4 g_menu_left_69d6c4;
/*007ef820*/ extern struct PlayerInput g_player_inputs[12];
/*007efce0*/ extern struct PlayerInput g_player_inputs_previous[12];
/*007f01f4*/ extern undefined4 g_input_mode;
/*007f01fc*/ extern struct PlayerSlot g_player_slots[4];
/*007f023c*/ extern char g_ai_editor_mode[];

#define INPUT_MODE_MENU (0x1000) /* g_input_mode value: input goes to the menu, which reads players 0-7 directly (inferred) */
#define NO_CONTROLLER (-1) /* PlayerSlot.controller value for an empty slot */

/*
 * Menu input test for "left". Returns 1 if the left direction was newly pressed
 * on any player this frame (held now, not held in g_player_inputs_previous), or
 * if the current menu page consumes the press itself; otherwise returns 0.
 * Clears g_menu_left_69d6c4 while no player is holding left.
 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* "direction_flags[0] on any player input (with auto repeat)" */

undefined4 menu_left_pressed_any(void)

{
  bool any_left_held;
  int slot_type;
  undefined4 left_pressed;
  PlayerSlot *slot;
  int iVar4;
  
  left_pressed = 0;
  any_left_held = false;
  /* Direct mode: is players 0-6 holding left (player 7 is checked right after)? */
  if (g_input_mode == INPUT_MODE_MENU) {
    any_left_held = g_player_inputs[6].direction_flags[0] != 0 ||
            (g_player_inputs[5].direction_flags[0] != 0 ||
            (g_player_inputs[4].direction_flags[0] != 0 ||
            (g_player_inputs[3].direction_flags[0] != 0 ||
            (g_player_inputs[2].direction_flags[0] != 0 ||
            (g_player_inputs[1].direction_flags[0] != 0 ||
            g_player_inputs[0].direction_flags[0] != 0)))));
    if (g_player_inputs[7].direction_flags[0] == 0) goto LAB_004305cb;
  }
  else {
    /* Slot mode: any active player slot holding left, skipping slot type 2 (inferred).
     * The slot array ends where g_ai_editor_mode starts. */
    iVar4 = 0;
    slot = g_player_slots;
    do {
      if (((slot->controller != NO_CONTROLLER) &&
          (slot_type = race_setup_get_slot_type(iVar4), slot_type != 2)) &&
         (g_player_inputs[slot->controller].direction_flags[0] != 0)) {
        any_left_held = true;
      }
      slot = slot + 1;
      iVar4 = iVar4 + 1;
    } while ((LONG_PTR)slot < ((LONG_PTR)&g_ai_editor_mode));
LAB_004305cb:
    if (!any_left_held) {
      g_menu_left_69d6c4 = 0;
      goto LAB_00430619;
    }
  }
  /* Options page: entries 0-2 consume the press, other entries go to the edge check.
   * Page 0x1e consumes it too. */
  if (g_menu_records[g_menu_current].page_id == MENU_PAGE_SOUND) {
    iVar4 = g_menu_records[g_menu_current].selected;
    if ((iVar4 != 0) && (iVar4 != 1)) {
      if (iVar4 == 2) {
        return 1;
      }
      goto LAB_00430619;
    }
  }
  else if (g_menu_records[g_menu_current].page_id != MENU_PAGE_GAMMA) {
LAB_00430619:
    /* Edge check, both modes: left newly pressed this frame. Player 7 returns at once. */
    if (g_input_mode == INPUT_MODE_MENU) {
      if ((g_player_inputs[0].direction_flags[0] != 0) &&
         (g_player_inputs_previous[0].direction_flags[0] == 0)) {
        left_pressed = 1;
      }
      if ((g_player_inputs[1].direction_flags[0] != 0) &&
         (g_player_inputs_previous[1].direction_flags[0] == 0)) {
        left_pressed = 1;
      }
      if ((g_player_inputs[2].direction_flags[0] != 0) &&
         (g_player_inputs_previous[2].direction_flags[0] == 0)) {
        left_pressed = 1;
      }
      if ((g_player_inputs[3].direction_flags[0] != 0) &&
         (g_player_inputs_previous[3].direction_flags[0] == 0)) {
        left_pressed = 1;
      }
      if ((g_player_inputs[4].direction_flags[0] != 0) &&
         (g_player_inputs_previous[4].direction_flags[0] == 0)) {
        left_pressed = 1;
      }
      if ((g_player_inputs[5].direction_flags[0] != 0) &&
         (g_player_inputs_previous[5].direction_flags[0] == 0)) {
        left_pressed = 1;
      }
      if ((g_player_inputs[6].direction_flags[0] != 0) &&
         (g_player_inputs_previous[6].direction_flags[0] == 0)) {
        left_pressed = 1;
      }
      if ((g_player_inputs[7].direction_flags[0] != 0) &&
         (g_player_inputs_previous[7].direction_flags[0] == 0)) {
        return 1;
      }
    }
    else {
      iVar4 = 0;
      slot = g_player_slots;
      do {
        if (((slot->controller != NO_CONTROLLER) &&
            (slot_type = race_setup_get_slot_type(iVar4), slot_type != 2)) &&
           ((g_player_inputs[slot->controller].direction_flags[0] != 0 &&
            (g_player_inputs_previous[slot->controller].direction_flags[0] == 0)))) {
          left_pressed = 1;
        }
        slot = slot + 1;
        iVar4 = iVar4 + 1;
      } while ((LONG_PTR)slot < ((LONG_PTR)&g_ai_editor_mode));
    }
    return left_pressed;
  }
  return 1;
}


