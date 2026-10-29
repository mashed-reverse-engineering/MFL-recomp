#include "mfl_tier2.h"
/*0040ff20*/ extern int race_setup_get_slot_type();
/*004a5b20*/ extern int __cdecl controller_present_thunk_4a5b20(int p0);
/*0069cbe8*/ extern undefined4 g_ui_69cbe8;
/*007f01fc*/ extern struct PlayerSlot g_player_slots[4];
/*007f023c*/ extern char g_ai_editor_mode[];

#define NO_CONTROLLER -1
#define SLOT_TYPE_HUMAN 1
#define MAX_CONTROLLERS 0xc
#define PLAYER_SLOTS_END ((int)&g_ai_editor_mode)

/* Check if any human player slot has an active controller, flagging them in the UI state. */
/* player_slots_count_human_with_controller */

/* "human slots (slot type 1) with a valid controller 0..11" */

undefined4 player_slots_count_human_with_controller(void)
{
  int slot_type;
  int has_human_with_controller;
  PlayerSlot *slot;
  int slot_idx;
  
  has_human_with_controller = 0;
  slot_idx = 0;
  slot = g_player_slots;
  /* Iterate through player slots */
  do {
    slot_type = race_setup_get_slot_type(slot_idx);
    /* Check if slot is human with a valid controller index (0..11) */
    if (((slot_type == SLOT_TYPE_HUMAN) && (slot_type = slot->controller, NO_CONTROLLER < slot_type)) && (slot_type < MAX_CONTROLLERS))
    {
      slot_type = controller_present_thunk_4a5b20(slot_type);
      /* Controller is present (unknown exact status code for 0): flag in UI */
      if (slot_type == 0) {
        has_human_with_controller = 1;
        (&g_ui_69cbe8)[slot->controller] = 1;
      }
    }
    slot = slot + 1;
    slot_idx = slot_idx + 1;
  } while ((int)slot < PLAYER_SLOTS_END);
  return has_human_with_controller;
}
