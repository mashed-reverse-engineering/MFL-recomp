#include "mfl_tier2.h"
/*00403860*/ extern int attract_timer_reset();
/*00432d20*/ extern int menu_table_clear_a();
/*0069ca1c*/ extern undefined4 g_menu_69ca1c;
/*0069caec*/ extern undefined4 g_menu_start_race_requested;
/*0069cbd0*/ extern undefined4 g_menu_current;
/*0069cfa0*/ extern undefined4 g_menu_selected_vehicle;
/*0069d1e0*/ extern undefined4 g_frontend_state;
/*0069d1e8*/ extern undefined4 g_menu_next_page;
/*0069d1ec*/ extern undefined4 g_menu_screen_id;


#define FRONTEND_STATE_IDLE 0
#define FRONTEND_STATE_RACE_MENU 1
#define MENU_ID_RACE 0x21
#define MENU_SELECTED_VEHICLE_DEFAULT 1

/* frontend_enter_race_menu */

/* "state 0 -> 1, menu id 0x21, scene setup" */

/* Enters the race menu from the idle frontend state; no-op in any other state. */
void frontend_enter_race_menu(undefined4 next_page)
{
  if (g_frontend_state == FRONTEND_STATE_IDLE) {
    g_frontend_state = FRONTEND_STATE_RACE_MENU;
    g_menu_screen_id = MENU_ID_RACE;
    attract_timer_reset();
    menu_table_clear_a();
    g_menu_69ca1c = 0;
    g_menu_current = 0;
    g_menu_start_race_requested = 0;
    g_menu_next_page = next_page;
    g_menu_selected_vehicle = MENU_SELECTED_VEHICLE_DEFAULT;
  }
  return;
}
