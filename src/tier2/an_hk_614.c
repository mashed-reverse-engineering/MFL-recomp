#include "mfl_tier2.h"
/*0040ff20*/ extern int race_setup_get_slot_type();
/*005dd36c*/ extern float k_f_0_4;
/*0069cbd0*/ extern undefined4 g_menu_current;
/*0069d2e0*/ extern struct MenuRecord g_menu_records[14];
/*0069d6bc*/ extern float g_menu_up_69d6bc;
/*007ef7ec*/ extern float g_frame_seconds;
/*007ef820*/ extern struct PlayerInput g_player_inputs[12];
/*007efce0*/ extern struct PlayerInput g_player_inputs_previous[12];
/*007f01f4*/ extern undefined4 g_input_mode;
/*007f01fc*/ extern struct PlayerSlot g_player_slots[4];
/*007f0238*/ extern char g_player_slot3_extra[];
/*007eff4b*/ extern char g_player_inputs_previous_8__menu_back[];
/*007f023c*/ extern char g_ai_editor_mode[];

#define INPUT_MODE_MENU (0x1000)
#define DIR_FLAG_UP (2)
#define PLAYER_INPUT_SIZE (0x4c)
#define SLOT_EMPTY (-1)
#define MENU_REPEAT_INTERVAL (0.2)

/* menu_up_pressed_any */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* "direction_flags[2] set on any of the 12 player inputs while in menu input mode" */

/* Returns 1 if the menu "up" direction fired this frame, 0 otherwise.
 * Two ways to fire: an auto-repeat tick on menu pages 6 and 7 while up is held
 * (g_menu_up_69d6bc is the repeat timer, inferred), or a fresh press (up held now, not held last frame).
 * The repeat timer is reset whenever up is not held. */
undefined4 menu_up_pressed_any(void)

{
  bool any_up_held;
  byte *prev_up_flag;
  int slot_type;
  PlayerSlot *slot;
  int slot_index;

  /* Is up held? Menu input mode: players 0..6 plus player 7 (checked on its own below).
   * Otherwise: any active slot's controller, skipping slots of type 2 (meaning inferred). */
  any_up_held = false;
  if (g_input_mode == INPUT_MODE_MENU) {
    any_up_held = g_player_inputs[6].direction_flags[DIR_FLAG_UP] != 0 ||
            (g_player_inputs[5].direction_flags[DIR_FLAG_UP] != 0 ||
            (g_player_inputs[4].direction_flags[DIR_FLAG_UP] != 0 ||
            (g_player_inputs[3].direction_flags[DIR_FLAG_UP] != 0 ||
            (g_player_inputs[2].direction_flags[DIR_FLAG_UP] != 0 ||
            (g_player_inputs[1].direction_flags[DIR_FLAG_UP] != 0 ||
            g_player_inputs[0].direction_flags[DIR_FLAG_UP] != 0)))));
    if (g_player_inputs[7].direction_flags[DIR_FLAG_UP] == 0) goto LAB_004302a9;
  }
  else {
    slot_index = 0;
    slot = g_player_slots;
    do {
      if (((slot->controller != SLOT_EMPTY) &&
          (slot_type = race_setup_get_slot_type(slot_index), slot_type != 2)) &&
         (g_player_inputs[slot->controller].direction_flags[DIR_FLAG_UP] != 0)) {
        any_up_held = true;
      }
      slot = slot + 1;
      slot_index = slot_index + 1;
    } while ((LONG_PTR)slot < ((LONG_PTR)&g_ai_editor_mode));
LAB_004302a9:
    if (!any_up_held) {
      g_menu_up_69d6bc = 0.0;
      goto LAB_00430301;
    }
  }
  /* Auto-repeat: on menu pages 6 and 7 (inferred), accumulate hold time; once it passes the threshold, fire and restart the interval. */
  if (((5 < g_menu_records[g_menu_current].page_id) && (g_menu_records[g_menu_current].page_id < 8))
     && (g_menu_up_69d6bc = g_frame_seconds + g_menu_up_69d6bc, k_f_0_4 < g_menu_up_69d6bc)) {
    g_menu_up_69d6bc = MENU_REPEAT_INTERVAL;
    return 1;
  }
LAB_00430301:
  /* Fresh press: return 1 if some player holds up now but did not hold it in g_player_inputs_previous; 0 if none did. */
  if (g_input_mode == INPUT_MODE_MENU) {
    prev_up_flag = g_player_inputs_previous[0].direction_flags + DIR_FLAG_UP;
    while ((prev_up_flag[-0x4c0] == '\0' || (*prev_up_flag != 0))) {
      prev_up_flag = prev_up_flag + PLAYER_INPUT_SIZE;
      if (((LONG_PTR)&g_player_inputs_previous_8__menu_back + 0x2) < (LONG_PTR)prev_up_flag) {
        return 0;
      }
    }
  }
  else {
    slot_index = 0;
    slot = g_player_slots;
    while (((slot->controller == SLOT_EMPTY ||
            (slot_type = race_setup_get_slot_type(slot_index), slot_type == 2)) ||
           ((g_player_inputs[slot->controller].direction_flags[DIR_FLAG_UP] == 0 ||
            (g_player_inputs_previous[slot->controller].direction_flags[DIR_FLAG_UP] != 0))))) {
      slot = slot + 1;
      slot_index = slot_index + 1;
      if (((LONG_PTR)&g_player_slot3_extra + 0x3) < (LONG_PTR)slot) {
        return 0;
      }
    }
  }
  return 1;
}


