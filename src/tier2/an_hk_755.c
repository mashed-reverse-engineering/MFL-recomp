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
/*007eff05*/ extern char g_player_inputs_previous_7__menu_next[];

#define INPUT_MODE_MENU (0x1000) /* g_input_mode while a menu is open (names/MFL_globals2.csv) */
#define PLAYER_INPUT_SIZE (0x4c) /* sizeof(struct PlayerInput): stride between player records */
#define PREVIOUS_INPUTS_DISTANCE (0x4c0) /* g_player_inputs_previous - g_player_inputs in bytes (16 records) */
#define SLOT_TYPE_IGNORED (2) /* race_setup_get_slot_type() value whose slot never counts as a pause press (inferred) */
/* menu_pause_pressed_any */

/* "edge detect of pause on any player input"
 * Returns 1 if any player pressed pause this frame (down now, up in the previous frame), else 0.
 * Takes no parameters. Menu mode scans the 8 player input records; otherwise the 4 player slots. */

undefined4 menu_pause_pressed_any(void)

{
  byte *prev_pause_ptr;
  int slot_type;
  PlayerSlot *slot;
  int slot_index;

  /* early out: nothing to check while the message / menu panel gate is closed (inferred) */
  if (((g_message_state != 2) && (g_menu_panel_state_69c998 == 0)) && (g_menu_8c8090 != 0)) {
    return 0;
  }
  /* menu mode: walk the input records, current pause byte is prev_pause_ptr[-PREVIOUS_INPUTS_DISTANCE] */
  if (g_input_mode == INPUT_MODE_MENU) {
    prev_pause_ptr = &g_player_inputs_previous[0].pause;
    while ((prev_pause_ptr[-PREVIOUS_INPUTS_DISTANCE] == '\0' || (*prev_pause_ptr != 0))) {
      prev_pause_ptr = prev_pause_ptr + PLAYER_INPUT_SIZE;
      if (((char *)&g_player_inputs_previous_7__menu_next + 0x43) < (char *)prev_pause_ptr) {
        return 0;
      }
    }
    return 1;
  }
  /* slot mode: scan the 4 player slots; skip empty slots, SLOT_TYPE_IGNORED slots and
     players without a fresh pause press (down now, up last frame). Bound: past slot 3 */
  slot = g_player_slots;
  slot_index = 0;
  while (((slot->controller == -1 || (slot_type = race_setup_get_slot_type(slot_index), slot_type == SLOT_TYPE_IGNORED)
          ) || ((g_player_inputs[slot->controller].pause == 0 ||
                (g_player_inputs_previous[slot->controller].pause != 0))))) {
    slot = slot + 1;
    slot_index = slot_index + 1;
    if (((char *)&g_player_slot3_extra + 0x3) < (char *)slot) {
      return 0;
    }
  }
  return 1;
}


