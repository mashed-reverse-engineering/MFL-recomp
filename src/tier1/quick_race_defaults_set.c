#include "mfl_tier2.h"
/*0040fbd0*/ extern int race_set_setup_index();
/*0040ff40*/ extern int race_setup_set_slot_type();
/*00444c10*/ extern int menu_page_enter();
/*0047bc80*/ extern int car_set_field_8();
/*0069caec*/ extern undefined4 g_menu_start_race_requested;
/*0069cbd4*/ extern undefined4 g_event_type;
/*0069cc40*/ extern undefined4 g_team_mode;
/*0069cc4c*/ extern undefined4 g_menu_69cc4c;
/*0069cf90*/ extern undefined4 g_menu_powerup_set;
/*0069cf94*/ extern undefined4 g_menu_markers_enabled;
/*0069cfa0*/ extern undefined4 g_menu_selected_vehicle;
/*0069d1ec*/ extern undefined4 g_menu_screen_id;
/*0069d21c*/ extern undefined4 g_menu_multiplayer_kind;
/*0069d290*/ extern undefined4 g_menu_69d290;
/*0069d2d0*/ extern undefined4 g_menu_career_race_active;
/*007ef7b8*/ extern undefined4 game_mode;
/*007f01fc*/ extern struct PlayerSlot g_player_slots[4];


/* quick_race_defaults_set */

/* "game_mode = GAME_MODE_QUICK_RACE, event type 0x12, team mode off ..." */

void quick_race_defaults_set(void)
{
  struct PlayerSlot *p_3;
  struct PlayerSlot *p_2;
  struct PlayerSlot *p_1;
  struct PlayerSlot *p_0;
  g_menu_career_race_active = 0;
  game_mode = GAME_MODE_QUICK_RACE;
  g_event_type = EVENT_QUICK_RACE;
  g_team_mode = 0;
  g_menu_multiplayer_kind = 0;
  g_menu_powerup_set = 0;
  g_menu_markers_enabled = 0;
  g_menu_69cc4c = 0;
  p_0 = &g_player_slots[0];
  p_0->controller = 0;
  p_1 = &g_player_slots[1];
  p_1->controller = 1;
  p_2 = &g_player_slots[2];
  p_2->controller = 2;
  p_3 = &g_player_slots[3];
  p_3->controller = -1;
  p_0->state = -1;
  p_1->state = -1;
  p_2->state = -1;
  p_3->state = -1;
  race_setup_set_slot_type(0,2);
  race_setup_set_slot_type(1,2);
  race_setup_set_slot_type(2,2);
  race_setup_set_slot_type(3,0);
  car_set_field_8(0,g_menu_selected_vehicle);
  race_set_setup_index(0);
  menu_page_enter(0,2);
  g_menu_69d290 = 0;
  g_menu_start_race_requested = 1;
  g_menu_screen_id = 1;
  return;
}
