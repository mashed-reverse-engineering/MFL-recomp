#include "mfl_tier2.h"
/*0040ff20*/ extern int race_setup_get_slot_type(int slot);
/*00432d20*/ extern void menu_table_clear_a(void);
/*0069ca1c*/ extern int g_menu_69ca1c;
/*0069caec*/ extern int g_menu_start_race_requested;
/*0069cbd0*/ extern int g_menu_current;
/*0069d1e0*/ extern int g_frontend_state;
/*0069d1e8*/ extern int g_menu_next_page;
/*0069d6a8*/ extern int g_menu_select_pressed;
/*007ef820*/ extern struct PlayerInput g_player_inputs[12];
/*007efce0*/ extern struct PlayerInput g_player_inputs_previous[12];
/*007f01f4*/ extern int g_input_mode;
/*007f01fc*/ extern struct PlayerSlot g_player_slots[4];
/* frontend_pause_input_poll */
/* "sets g_frontend_state=1 when a pause button was pressed this frame depending on the input mode"
    */
int __cdecl frontend_pause_input_poll(int next_page)
{
  int i;
  int controller;
  struct PlayerSlot *slot;

  g_menu_select_pressed = 0;
  if (g_frontend_state == 0) {
    switch (g_input_mode) {
    case 0x1000:
      if (g_player_inputs[0].pause != 0 && g_player_inputs_previous[0].pause == 0) {
        g_frontend_state = 1;
      }
      if (g_player_inputs[1].pause != 0 && g_player_inputs_previous[1].pause == 0) {
        g_frontend_state = 1;
      }
      if (g_player_inputs[2].pause != 0 && g_player_inputs_previous[2].pause == 0) {
        g_frontend_state = 1;
      }
      if (g_player_inputs[3].pause != 0 && g_player_inputs_previous[3].pause == 0) {
        g_frontend_state = 1;
      }
      if (g_player_inputs[4].pause != 0 && g_player_inputs_previous[4].pause == 0) {
        g_frontend_state = 1;
      }
      if (g_player_inputs[5].pause != 0 && g_player_inputs_previous[5].pause == 0) {
        g_frontend_state = 1;
      }
      if (g_player_inputs[6].pause != 0 && g_player_inputs_previous[6].pause == 0) {
        g_frontend_state = 1;
      }
      if (g_player_inputs[7].pause != 0 && g_player_inputs_previous[7].pause == 0) {
        g_frontend_state = 1;
        goto pause_pressed;
      }
      break;
    case 2:
      i = 0;
      slot = g_player_slots;
      do {
        if (race_setup_get_slot_type(i) == 1) {
          controller = slot->controller;
          if (controller != -1 && g_player_inputs[controller].pause != 0 && g_player_inputs_previous[controller].pause == 0) {
            g_frontend_state = 1;
          }
        }
        slot++;
        i++;
      } while (slot < &g_player_slots[4]);
      break;
    case 1:
      controller = g_player_slots[0].controller;
      if (controller != -1 && g_player_inputs[controller].pause != 0 && g_player_inputs_previous[controller].pause == 0) {
        g_frontend_state = 1;
        goto pause_pressed;
      }
      return 0;
    default:
      return 0;
    }
    if (g_frontend_state == 1) {
pause_pressed:
      menu_table_clear_a();
      g_menu_next_page = next_page;
      g_menu_69ca1c = 0;
      g_menu_current = 0;
      g_menu_start_race_requested = 0;
      g_menu_select_pressed = 2;
      return 1;
    }
  }
  return 0;
}
