#include "mfl_tier2.h"
/*0040ff40*/ extern int race_setup_set_slot_type();
/*007efce0*/ extern struct PlayerInput g_player_inputs_previous[12];
/*007f01f4*/ extern undefined4 g_input_mode;
/*007f01fc*/ extern struct PlayerSlot g_player_slots[4];
/*007eff4a*/ extern char g_player_inputs_previous_8__menu_select[];


/* ui_input_reset */

/* "controller of slot 0 = 0 and clears the previous input table" */

void ui_input_reset(void)

{
  int n;
  byte *menu_select_cursor;
  
  n = 0;
  menu_select_cursor = &g_player_inputs_previous[0].menu_select;
  do {
    if ((menu_select_cursor[-0x4c0] != '\0') && (*menu_select_cursor == 0)) break;
    menu_select_cursor = menu_select_cursor + 0x4c;
    n = n + 1;
  } while ((int)menu_select_cursor < ((int)&g_player_inputs_previous_8__menu_select));
  g_player_slots[0].controller = n;
  g_player_slots[1].controller = -1;
  g_player_slots[2].controller = -1;
  g_player_slots[3].controller = -1;
  g_input_mode = 1;
  race_setup_set_slot_type(0,1);
  race_setup_set_slot_type(1,0);
  race_setup_set_slot_type(2,0);
  race_setup_set_slot_type(3,0);
  return;
}
