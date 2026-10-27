#include "mfl_tier2.h"
/*0069d6a8*/ extern undefined4 g_menu_select_pressed;
/*0040d130*/ extern int __cdecl find_texture_a(RwChar *p0);
/*004169f0*/ extern int career_event_slot_table();
/*0042c440*/ extern int ui_rt2d_text_draw();
/*0042ebd0*/ extern double time_get_fraction();
/*0042ebe0*/ extern int time_get_minutes();
/*0042ebf0*/ extern int time_get_seconds();
/*00434bb0*/ extern int get_event_type();
/*004377b0*/ extern int menu_team_choice_step(undefined4 p0, undefined4 p1, int p2);
/*005dd7e8*/ extern float k_f_60;
/*0062ac90*/ extern undefined g_ui_62ac90;
/*0062ac94*/ extern undefined g_ui_62ac94;
/*0062ac98*/ extern undefined g_ui_62ac98;
/*0069c978*/ extern undefined4 g_menu_panel_state_69c978;
/*0069c980*/ extern undefined4 g_menu_panel_state_69c980;
/*0069c988*/ extern undefined4 g_menu_panel_state_69c988;
/*0069c990*/ extern undefined4 g_menu_panel_state_69c990;
/*0069c998*/ extern undefined4 g_menu_panel_state_69c998;
/*0069c9a0*/ extern undefined4 g_menu_panel_state_69c9a0;
/*0069c9a8*/ extern undefined4 g_menu_panel_state_69c9a8;
/*0069c9b0*/ extern undefined4 g_menu_panel_state_69c9b0;
/*0069c9b8*/ extern undefined4 g_menu_panel_state_69c9b8;
/*0069c9c0*/ extern undefined4 g_menu_panel_state_69c9c0;
/*0069c9c8*/ extern undefined4 g_menu_panel_state_69c9c8;
/*0069c9d0*/ extern undefined4 g_menu_panel_state_69c9d0;
/*0069c9d8*/ extern undefined4 g_menu_panel_state_69c9d8;
/*0069c9e0*/ extern undefined4 g_menu_panel_state_69c9e0;
/*0069c9e8*/ extern undefined4 g_menu_panel_state_69c9e8;
/*0069c9ec*/ extern undefined1 g_hud_69c9ec;
/*0069c9f0*/ extern undefined4 g_menu_panel_state_69c9f0;
/*0069c9f8*/ extern undefined4 g_menu_panel_state_69c9f8;
/*0069ca00*/ extern undefined4 g_menu_panel_state_69ca00;
/*0069ca08*/ extern undefined4 g_menu_panel_state_69ca08;
/*0069ca10*/ extern undefined4 g_menu_panel_state_69ca10;
/*0069ca34*/ extern undefined4 g_menu_update_69ca34;
/*0069cb10*/ extern undefined4 g_ui_69cb10;
/*0069cbd0*/ extern undefined4 g_menu_current;
/*0069cbd4*/ extern undefined4 g_event_type;
/*0069cc4c*/ extern undefined4 g_menu_69cc4c;
/*0069cf90*/ extern undefined4 g_menu_powerup_set;
/*0069cf94*/ extern undefined4 g_menu_markers_enabled;
/*0069cf98*/ extern undefined4 g_menu_bot_skill_a;
/*0069cf9c*/ extern undefined4 g_menu_race_length_choice;
/*0069cfa4*/ extern undefined4 g_menu_game_mode_choice;
/*0069cfac*/ extern undefined4 g_menu_bot_skill_b;
/*0069cfb0*/ extern undefined4 g_menu_event_course_index;
/*0069cfb4*/ extern undefined4 g_menu_player_team_choice;
/*0069cfb8*/ extern undefined4 g_menu_player_team_choice_1;
/*0069cfbc*/ extern undefined4 g_menu_player_team_choice_2;
/*0069cfc4*/ extern float g_menu_69cfc4;
/*0069cfc8*/ extern undefined4 g_menu_team_mode_choice;
/*0069cfd0*/ extern undefined4 g_message_state;
/*0069d2d0*/ extern undefined4 g_menu_career_race_active;
/*0069d2d8*/ extern undefined4 g_car_69d2d8;
/*0069d2e0*/ extern struct MenuRecord g_menu_records[14];
/*0069d688*/ extern undefined4 g_career_row;
/*0069d690*/ extern undefined4 g_menu_event_column;
/*0069d6a8*/ extern undefined4 g_menu_select_pressed;
/*0069d6ac*/ extern undefined4 g_message_69d6ac;
/*0069d6b0*/ extern undefined4 g_menu_69d6b0;
/*007d2128*/ extern struct RwGlobalsRelease *RwEngineInstance;
/*007eebc0*/ extern struct GameState g_game_state;
/*007ef7d0*/ extern undefined4 g_menu_7ef7d0;
/*007efce0*/ extern struct PlayerInput g_player_inputs_previous[12];
/*007f01fc*/ extern struct PlayerSlot g_player_slots[4];
/*008c8070*/ extern undefined4 g_car_8c8070;
/*008c8078*/ extern undefined4 g_training_lesson;
/*008c86c8*/ extern undefined4 g_car_8c86c8;
/*008c8720*/ extern undefined4 g_hit_shake;
/* ui_get_menu_69d6a8 */

/* "return g_menu_69d6a8;" */

undefined4 ui_get_menu_69d6a8(void)
{
  return g_menu_select_pressed;
}
