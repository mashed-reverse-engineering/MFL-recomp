#include "mfl_tier2.h"
/*0040ff20*/ extern int race_setup_get_slot_type();
/*007ef820*/ extern struct PlayerInput g_player_inputs[12];
/*007efce0*/ extern struct PlayerInput g_player_inputs_previous[12];
/*007f01f4*/ extern undefined4 g_input_mode;
/*007f01fc*/ extern struct PlayerSlot g_player_slots[4];
/*007f023c*/ extern char g_ai_editor_mode[];

#define INPUT_MODE_FIXED_PADS 0x1000 /* g_input_mode value: read pads 0..7 directly (mode name unknown) */
#define NO_CONTROLLER -1             /* PlayerSlot.controller when no pad is bound */
#define SLOT_TYPE_SKIPPED 2          /* race_setup_get_slot_type() result that is ignored (name is a guess) */
#define PLAYER_SLOTS_END ((int)&g_ai_editor_mode)    /* end address of g_player_slots[4] */


/* menu_input_next_pressed */

/* "edge detect of menu_next on controller 0 (any player slot)" */

/* Returns 1 if menu_next was newly pressed (down now, up last frame) on any relevant controller, else 0. */
undefined4 menu_input_next_pressed(void)
{
  int slot_type;
  undefined4 pressed;
  PlayerSlot *slot;
  int slot_index;
  
  pressed = 0;
  if (g_input_mode == INPUT_MODE_FIXED_PADS) {
    /* fixed-index path: pads 0..7 checked in turn; the last one returns directly */
    if ((g_player_inputs[0].menu_next != 0) && (g_player_inputs_previous[0].menu_next == 0)) {
      pressed = 1;
    }
    if ((g_player_inputs[1].menu_next != 0) && (g_player_inputs_previous[1].menu_next == 0)) {
      pressed = 1;
    }
    if ((g_player_inputs[2].menu_next != 0) && (g_player_inputs_previous[2].menu_next == 0)) {
      pressed = 1;
    }
    if ((g_player_inputs[3].menu_next != 0) && (g_player_inputs_previous[3].menu_next == 0)) {
      pressed = 1;
    }
    if ((g_player_inputs[4].menu_next != 0) && (g_player_inputs_previous[4].menu_next == 0)) {
      pressed = 1;
    }
    if ((g_player_inputs[5].menu_next != 0) && (g_player_inputs_previous[5].menu_next == 0)) {
      pressed = 1;
    }
    if ((g_player_inputs[6].menu_next != 0) && (g_player_inputs_previous[6].menu_next == 0)) {
      pressed = 1;
    }
    if ((g_player_inputs[7].menu_next != 0) && (g_player_inputs_previous[7].menu_next == 0)) {
      return 1;
    }
  }
  else {
    /* slot path: walk the player slots, skipping unbound and SLOT_TYPE_SKIPPED ones */
    slot_index = 0;
    slot = g_player_slots;
    do {
      if ((((slot->controller != NO_CONTROLLER) &&
           (slot_type = race_setup_get_slot_type(slot_index), slot_type != SLOT_TYPE_SKIPPED)) &&
          (g_player_inputs[slot->controller].menu_next != 0)) &&
         (g_player_inputs_previous[slot->controller].menu_next == 0)) {
        pressed = 1;
      }
      slot = slot + 1;
      slot_index = slot_index + 1;
    } while ((int)slot < PLAYER_SLOTS_END);
  }
  return pressed;
}
