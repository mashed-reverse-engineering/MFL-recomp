#include "mfl_tier2.h"
/*00403860*/ extern int attract_timer_reset();
/*00405e40*/ extern int stub_return_zero();
/*00409f30*/ extern double player_progress_b();
/*0040fbd0*/ extern int race_set_setup_index();
/*0040fbf0*/ extern int race_get_focus_player();
/*0040fdc0*/ extern int race_get_phase();
/*0040fe20*/ extern int __cdecl race_setup_slot_used(undefined4 p0);
/*0040ff40*/ extern int race_setup_set_slot_type();
/*0041a380*/ extern int ai_ftol_noop_wrapper();
/*00428420*/ extern int player_stats_copy_to_totals();
/*00428490*/ extern int player_stats_accumulate();
/*00430b30*/ extern int ui_get_menu_69d1ec();
/*00431130*/ extern undefined __cdecl message_show_now(undefined4 p0, undefined4 p1, undefined4 p2, undefined4 p3, undefined4 p4, undefined4 p5);
/*004311b0*/ extern int message_show();
/*004316f0*/ extern int message_close();
/*00431a30*/ extern int ui_clear_cbe8();
/*00431a50*/ extern int controllers_count_present();
/*00431a80*/ extern int player_slots_count_human_with_controller();
/*00431ae0*/ extern int message_show_controller_missing();
/*00432d20*/ extern int menu_table_clear_a();
/*004345d0*/ extern int hud_arrow_draw();
/*00435100*/ extern int menu_panel_draw_69c9b8();
/*00435420*/ extern int menu_panel_draw_69c9e0();
/*004356b0*/ extern int menu_panel_draw_69c9e8();
/*00435ec0*/ extern int event_type_uses_standard_menu();
/*00435f30*/ extern int career_time_under_target();
/*00436550*/ extern int options_menu_draw();
/*00436c00*/ extern int menu_panel_draw_69ca08();
/*00436e70*/ extern int menu_panel_draw_69ca10();
/*004370e0*/ extern int menu_panel_draw_69ca00();
/*00437340*/ extern int menu_panel_draw_69c9f0();
/*004377b0*/ extern int menu_team_choice_step(undefined4 p0, undefined4 p1, int p2);
/*004393f0*/ extern int menu_draw_controller_select();
/*0043abd0*/ extern int results_screen_draw();
/*004408c0*/ extern int menu_draw_event_descriptions();
/*00441c80*/ extern int menu_draw_skill_level_select();
/*004420a0*/ extern int menu_draw_team_select();
/*00442580*/ extern int menu_draw_event_icons();
/*00444c10*/ extern int menu_page_enter();
/*00475730*/ extern int get_viewport_camera();
/*0047a830*/ extern int car_is_active();
/*0047ac30*/ extern int car_get_mode();
/*0047b520*/ extern int car_current_matrix();
/*0047bc80*/ extern int car_set_field_8();
/*0048b640*/ extern int lua_Occluder_Filename();
/*0048b670*/ extern int lua_Sky_Filename();
/*0048b710*/ extern int lua_Mts_Filename();
/*0048b7b0*/ extern int lua_SetCopter();
/*0048b830*/ extern int lua_Clump_Filename();
/*0048b8d0*/ extern int lua_UVA_Filename();
/*0048b930*/ extern int lua_Spline_Filename();
/*0048b9d0*/ extern int lua_AI_Bsp_Filename();
/*0048ba10*/ extern int lua_AI_Data_Filename();
/*0048ba40*/ extern int lua_World_Bsp_Filename();
/*0048ba70*/ extern int lua_Hoppy_Bsp_Filename();
/*0048baa0*/ extern int lua_Object_Bsp_Filename();
/*0048bb40*/ extern int lua_Collision_Bsp_Filename();
/*0048bb80*/ extern int lua_Texture_Dictionary_Filename();
/*0048bbb0*/ extern int lua_Lap_Variations();
/*0048bbe0*/ extern int lua_Split_Sector();
/*0048bc20*/ extern int lua_Lap_Line();
/*0048bc60*/ extern int lua_Lap_Line_End();
/*0048bc80*/ extern int lua_Lap_Line_Change();
/*0048bca0*/ extern int lua_Safe_Start_Lines();
/*0048bd10*/ extern int lua_Camera_Anim_Filename();
/*0048bd40*/ extern int lua_General_Anim_Filename();
/*0048bde0*/ extern int lua_Clump_Mts_Pair();
/*0048be40*/ extern int lua_Physics_Object();
/*0048beb0*/ extern int lua_Clump_Exclude_From_World();
/*0048bee0*/ extern int lua_Powerup_Filename();
/*0048bf30*/ extern int lua_Lights_Filename();
/*0048bf60*/ extern int lua_Ambient_RGB();
/*0048bfc0*/ extern int lua_Setup_Fog();
/*0048c060*/ extern int lua_Modify_Fog();
/*0048c110*/ extern int lua_Vehicle_Shininess_Range();
/*0048c160*/ extern int lua_Shine_Scale();
/*0048c190*/ extern int lua_Shadow_Scale();
/*0048c1c0*/ extern int lua_Vehicle_Body_Dirt_Texture__Vehicle_Wheel_Dirt_Texture();
/*0048c1d0*/ extern int lua_SetClumpAnim();
/*0048c270*/ extern int lua_RWP_Object();
/*0048c310*/ extern int lua_RWP_Object_Box();
/*0048c380*/ extern int lua_RWP_Object_Cylinder();
/*0048c3e0*/ extern int lua_RWP_Object_Sphere();
/*0048c430*/ extern int lua_RWP_Object_Capsule();
/*0048c490*/ extern int lua_RWP_Object_Properties();
/*0048c530*/ extern int lua_RWP_Object_CentreOfMassOffset();
/*0048c5a0*/ extern int lua_RWP_Object_BaseLabel();
/*0048c5f0*/ extern int lua_RWP_Object_ShatterVel();
/*0048c640*/ extern int lua_Set_Current_Position();
/*0048c690*/ extern int lua_Set_Current_Type();
/*0048c6c0*/ extern int lua_Set_Current_Respawn_Time();
/*0048c6e0*/ extern int lua_Place_Powerup();
/*0048c710*/ extern int lua_Set_Track_Powerups();
/*0048c7e0*/ extern int lua_Physics_Scenario_Create();
/*0048c850*/ extern int lua_Physics_Scenario_Set_Sequence();
/*0048c8b0*/ extern int lua_Physics_Scenario_Add_Objects();
/*0048c910*/ extern int lua_Physics_Scenario_Add_Animated_Object();
/*0048c990*/ extern int lua_Physics_Scenario_Create_Trigger();
/*0048ca40*/ extern int lua_Physics_Scenario_Trigger_Add_Objects();
/*0048cac0*/ extern int lua_Physics_Scenario_Get_Function();
/*0048cb40*/ extern int lua_Physics_Scenario_Trigger_Set_AI_Poly();
/*0048cbb0*/ extern int lua_KTC_NewCopter();
/*0048cc60*/ extern int lua_KTC_AddPickUp();
/*0048ccf0*/ extern int lua_ExpObj_Clump_Mts_Pair();
/*0048cd80*/ extern int lua_ClumpFade();
/*0048cf50*/ extern int lua_register_function();
/*004a0ac0*/ extern int lua_RainEnable();
/*004a0ae0*/ extern int lua_RainSetHeadColour();
/*004a0b80*/ extern int lua_RainSetTailColour();
/*004a0c20*/ extern int lua_RainSetPosition();
/*004a0cd0*/ extern int lua_RainSetScale();
/*004a0d30*/ extern int lua_RainSetCameraScale();
/*004a0d90*/ extern int lua_RainSetDirection();
/*004b2b48*/ extern int _ftol();
/*004c7cc0*/ extern undefined __cdecl message(undefined4 p0, undefined4 p1);
/*004d1460*/ extern struct RwFrame * RwFrameTranslate(struct RwFrame *p0, struct RwV3d *p1, enum RwOpCombineType p2);
/*004d16e0*/ extern struct RwFrame * RwFrameSetIdentity(struct RwFrame *p0);
/*004d1e10*/ extern struct RwCamera * RwCameraSetViewWindow(struct RwCamera *p0, struct RwV2d *p1);
/*004d43f0*/ extern RwMatrix * RwMatrixRotate(RwMatrix *p0, struct RwV3d *p1, RwReal p2, enum RwOpCombineType p3);
/*004d4870*/ extern RwMatrix * RwMatrixTranslate(RwMatrix *p0, struct RwV3d *p1, enum RwOpCombineType p2);
/*004d53a0*/ extern RwReal RwV3dLength(struct RwV3d *p0);
/*005dd340*/ extern float k_f_0_5;
/*005dd62c*/ extern float k_f_100;
/*005dd7f4*/ extern float k_f_80;
/*005dda24*/ extern float k_f_0_75;
/*005ddb14*/ extern float float_7_0;
/*005ddb18*/ extern float float_0_8;
/*005ddea0*/ extern float k_f_20;
/*0060d698*/ extern undefined g_camera_record_60d698;
/*0062ac74*/ extern float g_car_62ac74;
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
/*0069c9f0*/ extern undefined4 g_menu_panel_state_69c9f0;
/*0069c9f8*/ extern undefined4 g_menu_panel_state_69c9f8;
/*0069ca00*/ extern undefined4 g_menu_panel_state_69ca00;
/*0069ca08*/ extern undefined4 g_menu_panel_state_69ca08;
/*0069ca10*/ extern undefined4 g_menu_panel_state_69ca10;
/*0069ca1c*/ extern undefined4 g_menu_69ca1c;
/*0069caec*/ extern undefined4 g_menu_start_race_requested;
/*0069cbd0*/ extern undefined4 g_menu_current;
/*0069cbd4*/ extern undefined4 g_event_type;
/*0069cbe0*/ extern undefined4 g_tutorial_page_index;
/*0069cc40*/ extern undefined4 g_team_mode;
/*0069cc48*/ extern undefined4 g_menu_69cc48;
/*0069cc4c*/ extern undefined4 g_menu_69cc4c;
/*0069cf90*/ extern undefined4 g_menu_powerup_set;
/*0069cf94*/ extern undefined4 g_menu_markers_enabled;
/*0069cf98*/ extern undefined4 g_menu_bot_skill_a;
/*0069cfa0*/ extern undefined4 g_menu_selected_vehicle;
/*0069cfac*/ extern undefined4 g_menu_bot_skill_b;
/*0069cfb0*/ extern undefined4 g_menu_event_course_index;
/*0069cfb4*/ extern undefined4 g_menu_player_team_choice;
/*0069cfb8*/ extern undefined4 g_menu_player_team_choice_1;
/*0069cfbc*/ extern undefined4 g_menu_player_team_choice_2;
/*0069cfc4*/ extern float g_menu_69cfc4;
/*0069cfc8*/ extern undefined4 g_menu_team_mode_choice;
/*0069cfd0*/ extern undefined4 g_message_state;
/*0069cfd4*/ extern undefined4 g_message_text_id;
/*0069cff0*/ extern undefined4 g_message_code;
/*0069cff4*/ extern undefined4 g_message_buttons;
/*0069d0e8*/ extern float g_tutorial_69d0e8;
/*0069d0ec*/ extern float g_tutorial_69d0ec;
/*0069d0f0*/ extern float g_tutorial_69d0f0;
/*0069d0f4*/ extern float g_tutorial_69d0f4;
/*0069d100*/ extern undefined4 g_tutorial_line_count;
/*0069d104*/ extern undefined4 g_tutorial_display_ticks;
/*0069d108*/ extern undefined4 g_tutorial_line_entries;
/*0069d158*/ extern float g_tutorial_line_width;
/*0069d15c*/ extern float g_tutorial_69d15c;
/*0069d1e0*/ extern undefined4 g_frontend_state;
/*0069d1e8*/ extern undefined4 g_menu_next_page;
/*0069d1ec*/ extern undefined4 g_menu_screen_id;
/*0069d21c*/ extern undefined4 g_menu_multiplayer_kind;
/*0069d228*/ extern undefined4 g_menu_69d228;
/*0069d22c*/ extern undefined4 g_menu_69d22c;
/*0069d248*/ extern undefined4 g_message_69d248;
/*0069d24c*/ extern undefined4 g_menu_69d24c;
/*0069d290*/ extern undefined4 g_menu_69d290;
/*0069d2d0*/ extern undefined4 g_menu_career_race_active;
/*0069d2d8*/ extern undefined4 g_car_69d2d8;
/*0069d2e0*/ extern struct MenuRecord g_menu_records[14];
/*0069d688*/ extern undefined4 g_career_row;
/*006a06e0*/ extern undefined4 g_sky_atomic;
/*006a06ec*/ extern undefined4 g_camera_record_6a06ec;
/*006a06f0*/ extern undefined4 g_camera_record_6a06f0;
/*006a06f8*/ extern undefined4 g_camera_record_6a06f8;
/*007eebc0*/ extern struct GameState g_game_state;
/*007ef7b8*/ extern undefined4 game_mode;
/*007f01f4*/ extern undefined4 g_input_mode;
/*007f01fc*/ extern struct PlayerSlot g_player_slots[4];
/*00801b2c*/ extern float g_camera_fov_factor;
/*008c5928*/ extern undefined4 g_car_8c5928;
/*008c592c*/ extern undefined4 g_car_8c592c;
/*008c5930*/ extern undefined4 g_camera_8c5930;
/*008c5934*/ extern undefined4 g_car_8c5934;
/*008c598c*/ extern undefined4 g_car_8c598c;
/*008c5a00*/ extern float g_car_8c5a00;
/*008c5a08*/ extern undefined g_camera_record_8c5a08;
/*008c5a0c*/ extern undefined4 g_camera_record_8c5a0c;
/*008c5ae0*/ extern undefined g_camera_record_8c5ae0;
/*008c7440*/ extern undefined4 g_car_8c7440;
/*008c7e1c*/ extern undefined4 g_camera_8c7e1c;
/*008c7e20*/ extern undefined4 g_camera_8c7e20;
/*008c7e30*/ extern undefined4 leader_index;
/*008c7e40*/ extern undefined4 distance_to_leader;
/*008c7e44*/ extern undefined4 g_distances_update_8c7e44;
/*008c7e48*/ extern undefined4 g_distances_update_8c7e48;
/*008c7e4c*/ extern undefined4 g_distances_update_8c7e4c;
/*008c7e60*/ extern undefined4 g_distances_8c7e60;
/*008c8070*/ extern undefined4 g_car_8c8070;
/*008c8078*/ extern undefined4 g_training_lesson;
/*008c86c8*/ extern undefined4 g_car_8c86c8;
/*008c8720*/ extern undefined4 g_hit_shake;
/*008c8788*/ extern undefined4 g_car_8c8788;
/* option_slider_1_reset */

/* "g_game_state.option_sliders[1] = 0.7" */

extern void option_slider_1_reset(void);


/* option_slider_2_reset */

/* "g_game_state.option_sliders[2] = 0.7" */

extern void option_slider_2_reset(void);


/* option_slider_0_reset */

/* "g_game_state.option_sliders[0] = 0.7" */

extern void option_slider_0_reset(void);


/* option_slider_4_reset */

/* "g_game_state.option_sliders[4] = 2" */

extern void option_slider_4_reset(void);


/* menu_get_69cfc4 */

/* "float g_menu_69cfc4" */

extern double menu_get_69cfc4(void);


/* menu_set_69cfc4 */

/*  */

extern void menu_set_69cfc4(char *param_1);


/* option_get_slider_4 */

/* "g_game_state.option_sliders[4]" */

extern uint option_get_slider_4(void);


/* menu_options_defaults_reset */

/* "sets the menu option variables to their defaults and refreshes the team entries" */

extern void menu_options_defaults_reset(void);


/* menu_event_course_index_get */

/* "returns g_menu_event_course_index" */

extern undefined4 menu_event_course_index_get(void);


/* menu_bot_skill_a_get */

/* "returns g_menu_bot_skill_a" */

extern undefined4 menu_bot_skill_a_get(void);


/* menu_panel_states_decay */

/* "every panel state: 1 stays 1, otherwise becomes 0 or 2 (the 'was visible' state)" */

extern void menu_panel_states_decay(void);


/* menu_page_enter_set_panel_state */

/* "calls menu_panel_states_decay and sets the panel state flag for page ids 4..8" */

/* Called when a menu page is entered. First decays all panel states, then sets the
   panel state flag belonging to page_id (the panel shown for that page, inferred). Page ids
   with no panel do nothing. */
/* TIER2: unvalidated 2026-10-17 authority=diffemu */
void __cdecl menu_page_enter_set_panel_state(undefined4 page_id)
{
  menu_panel_states_decay();
  switch(page_id) {
  case MENU_PAGE_TEAM_COLOUR_SELECT:
    g_menu_panel_state_69c978 = 1;
    break;
  case MENU_PAGE_RACE_RESULTS:
    g_menu_panel_state_69c998 = 1;
    return;
  case MENU_PAGE_CHALLENGE_SELECT_A:
  case MENU_PAGE_CHALLENGE_SELECT_B:
    g_menu_panel_state_69c988 = 1;
    return;
  case MENU_PAGE_OPTIONS:
    g_menu_panel_state_69c9d8 = 1;
    g_menu_panel_state_69c9f8 = 1;
    return;
  case MENU_PAGE_TEAMS:
  case MENU_PAGE_TEAM_SELECT:
    g_menu_panel_state_69c9b0 = 1;
    return;
  case MENU_PAGE_CUSTOMISE_PLAY:
    g_menu_panel_state_69c980 = 1;
    return;
  case MENU_PAGE_LOBBY_A:
    g_menu_panel_state_69c9a0 = 1;
    return;
  case MENU_PAGE_ABILITY_SELECT:
    g_menu_panel_state_69c9a8 = 1;
    return;
  case MENU_PAGE_CUSTOM_ABILITIES:
    g_menu_panel_state_69c9b8 = 1;
    return;
  case MENU_PAGE_GAME_SETUP_MULTI:
  case MENU_PAGE_GAME_SETUP_SINGLE:
    g_menu_panel_state_69c9c0 = 1;
    return;
  case MENU_PAGE_SOUND:
    g_menu_panel_state_69c9c8 = 1;
    return;
  case 0x14:
    g_menu_panel_state_69c9d0 = 1;
    return;
  case MENU_PAGE_VIBRATION:
    g_menu_panel_state_69c9e0 = 1;
    return;
  case MENU_PAGE_CONTROLLERS:
    g_menu_panel_state_69c9e8 = 1;
    return;
  case MENU_PAGE_GAMMA:
    g_menu_panel_state_69c9f0 = 1;
    return;
  case MENU_PAGE_BONUS_FEATURES:
  case MENU_PAGE_BONUS_CAMERAS:
    g_menu_panel_state_69ca00 = 1;
    return;
  case MENU_PAGE_AUTOSAVE:
    g_menu_panel_state_69ca08 = 1;
    return;
  case MENU_PAGE_WIDESCREEN:
    g_menu_panel_state_69ca10 = 1;
    return;
  }
  return;
}


/* training_lesson_get */

/* "returns g_training_lesson" */

extern undefined4 training_lesson_get(void);


/* ui_get_car_8c86c8 */

/* "return g_car_8c86c8;" */

extern undefined4 ui_get_car_8c86c8(void);


/* ui_get_car_69d2d8 */

/* "return g_car_69d2d8;" */

extern undefined4 ui_get_car_69d2d8(void);


/* ui_get_car_8c8070 */

/* "return g_car_8c8070;" */

extern undefined4 ui_get_car_8c8070(void);


/* screen_shake_set */

/* "sets g_hit_shake (inferred name)" */

extern void screen_shake_set(char *param_1);


/* ui_get_car_8c8788 */

/* "return g_car_8c8788;" */

extern undefined4 ui_get_car_8c8788(void);


/* message_replace_if_changed */

/* "if the message differs from the shown one copies the new state, closes and shows it" */

extern void __cdecl message_replace_if_changed (char *param_1,char *param_2,char *param_3,char *param_4, char *param_5,char *param_6);


/* controller_missing_message_update */

/* "shows/clears the 'controller missing' message (0x218) depending on input mode and connected
   controllers" */

extern void controller_missing_message_update(void);


/* frontend_enter_menu */

/* "state 0 -> 1 and resets g_menu_current to 0 (or event 0x11 special case)" */

extern void frontend_enter_menu(char *param_1);


/* frontend_enter_race_menu */

/* "state 0 -> 1, menu id 0x21, scene setup" */

extern void frontend_enter_race_menu(char *param_1);
extern char g_game_state_records_6_[];
extern char g_game_state_records_3_[];


/* menu_event_is_selectable */

/* "false for event 0xE, else depends on ui_fn_435ec0 (event type is a career event)" */

/* TIER2: validated 2026-10-19 authority=diffemu */
bool __cdecl menu_event_is_selectable(int event_row)
{
  int uses_menu_or_count;
  uint completed_count;
  uint *progress_ptr;
  
  if (event_row == 0xe) {
    return false;
  }
  uses_menu_or_count = event_type_uses_standard_menu();
  if (uses_menu_or_count != 0) {
    if (g_event_type == EVENT_TRAINING) {
      completed_count = (uint)(g_game_state.progress[0x11] == 2);
      if (g_game_state.progress[0x23] == 2) {
        completed_count = completed_count + 1;
      }
      if (g_game_state.progress[0x35] == 2) {
        completed_count = completed_count + 1;
      }
      if (g_game_state.progress[0x47] == 2) {
        completed_count = completed_count + 1;
      }
      if (g_game_state.progress[0x59] == 2) {
        completed_count = completed_count + 1;
      }
      if (g_game_state.progress[0x6b] == 2) {
        completed_count = completed_count + 1;
      }
      if (g_game_state.progress[event_row * 0x12 + 0x11] == 0) {
        return false;
      }
    }
    else if (g_event_type == EVENT_TURBO_TOURNAMENT) {
      completed_count = (uint)(g_game_state.progress[0xe] == 2);
      if (g_game_state.progress[0x20] == 2) {
        completed_count = completed_count + 1;
      }
      if (g_game_state.progress[0x32] == 2) {
        completed_count = completed_count + 1;
      }
      if (g_game_state.progress[0x44] == 2) {
        completed_count = completed_count + 1;
      }
      if (g_game_state.progress[0x56] == 2) {
        completed_count = completed_count + 1;
      }
      if (g_game_state.progress[event_row * 0x12 + 0xe] == 0) {
        return false;
      }
    }
    else if (g_event_type == EVENT_ICE_TOURNAMENT) {
      completed_count = (uint)(g_game_state.progress[0xf] == 2);
      if (g_game_state.progress[0x21] == 2) {
        completed_count = completed_count + 1;
      }
      if (g_game_state.progress[0x33] == 2) {
        completed_count = completed_count + 1;
      }
      if (g_game_state.progress[0x45] == 2) {
        completed_count = completed_count + 1;
      }
      if (g_game_state.progress[0x57] == 2) {
        completed_count = completed_count + 1;
      }
      if (g_game_state.progress[event_row * 0x12 + 0xf] == 0) {
        return false;
      }
    }
    else {
      if (g_event_type != EVENT_NIGHT_TOURNAMENT) {
        uses_menu_or_count = 0;
        progress_ptr = g_game_state.progress + 0x15;
        do {
          if (progress_ptr[-0x12] == 2) {
            uses_menu_or_count = uses_menu_or_count + 1;
          }
          if (*progress_ptr == 2) {
            uses_menu_or_count = uses_menu_or_count + 1;
          }
          if (progress_ptr[0x12] == 2) {
            uses_menu_or_count = uses_menu_or_count + 1;
          }
          if (progress_ptr[0x24] == 2) {
            uses_menu_or_count = uses_menu_or_count + 1;
          }
          if (progress_ptr[0x36] == 2) {
            uses_menu_or_count = uses_menu_or_count + 1;
          }
          if (progress_ptr[0x48] == 2) {
            uses_menu_or_count = uses_menu_or_count + 1;
          }
          if (progress_ptr[0x5a] == 2) {
            uses_menu_or_count = uses_menu_or_count + 1;
          }
          progress_ptr = progress_ptr + 0x7e;
        } while (progress_ptr < (uint *)&g_game_state_records_3_);
        if (g_game_state.progress[event_row * 0x12 + 3] == 0) {
          return false;
        }
        return event_row <= uses_menu_or_count;
      }
      completed_count = (uint)(g_game_state.progress[0x10] == 2);
      if (g_game_state.progress[0x22] == 2) {
        completed_count = completed_count + 1;
      }
      if (g_game_state.progress[0x34] == 2) {
        completed_count = completed_count + 1;
      }
      if (g_game_state.progress[0x46] == 2) {
        completed_count = completed_count + 1;
      }
      if (g_game_state.progress[0x58] == 2) {
        completed_count = completed_count + 1;
      }
      if (g_game_state.progress[event_row * 0x12 + 0x10] == 0) {
        return false;
      }
    }
    return event_row <= (int)completed_count;
  }
  uses_menu_or_count = 0;
  progress_ptr = g_game_state.progress + 0x18;
  do {
    if (progress_ptr[-0x12] != 0) {
      uses_menu_or_count = uses_menu_or_count + 1;
    }
    if (*progress_ptr != 0) {
      uses_menu_or_count = uses_menu_or_count + 1;
    }
    if (progress_ptr[0x12] != 0) {
      uses_menu_or_count = uses_menu_or_count + 1;
    }
    if (progress_ptr[0x24] != 0) {
      uses_menu_or_count = uses_menu_or_count + 1;
    }
    if (progress_ptr[0x36] != 0) {
      uses_menu_or_count = uses_menu_or_count + 1;
    }
    if (progress_ptr[0x48] != 0) {
      uses_menu_or_count = uses_menu_or_count + 1;
    }
    if (progress_ptr[0x5a] != 0) {
      uses_menu_or_count = uses_menu_or_count + 1;
    }
    progress_ptr = progress_ptr + 0x7e;
  } while (progress_ptr < (uint *)&g_game_state_records_6_);
  return event_row <= uses_menu_or_count + -1;
}


/* career_mark_row_unlocked */

/* "sets g_game_state.progress[row][0] = 1 when the row is available" */

extern void career_mark_row_unlocked(void);


/* hud_menu_draw_all_panels */

/* "draws the menu panels that are visible" */

extern void hud_menu_draw_all_panels(void);


/* quick_race_defaults_set */

/* "game_mode = GAME_MODE_QUICK_RACE, event type 0x12, team mode off ..." */

extern void quick_race_defaults_set(void);


/* lua_register_course_functions */

/* "registers the course script functions (Course_Id, AI_Bsp_Filename, World_Bsp_Filename ...)" */

extern void lua_register_course_functions(void);


/* course_sky_atomic_render */

/* "calls the atomic render callback of g_course_6a06e0 if set" */

extern void course_sky_atomic_render(void);


/* camera_update_view_window */

/* "view window from the camera's fov / aspect, 0.75 factor except for the AI editor slot" */

extern void camera_update_view_window(char *param_1);


/* camera_apply_rotation */

/* "sets fov, view window and rotates the camera frame by pitch / yaw" */

extern void camera_apply_rotation(char *param_1);


/* camera_get_position */

/* "(out xyz): position of the viewport camera 0" */

extern void camera_get_position(undefined4 *param_1);


/* camera_set_from_record */

/* "copies position and quaternion from a camera record" */

extern void camera_set_from_record(undefined4 *param_1,RwReal *param_2);


/* camera_get_target_position */

/* "position of the followed car (or the override at +0x1C)" */

extern void camera_get_target_position(float *param_1,char *param_2);


/* camera_record_add */

/* "adds one camera record to the ring at 0x8C7440 (mask 0xFFF)" */

extern void __cdecl camera_record_add(char *param_1);


/* camera_apply_matrix */

/* "copies the 4x4 camera matrix into the camera frame" */

extern void __cdecl camera_apply_matrix(char *param_1);


/* distances_update */

/* per-player horizontal distance to the leader * 0.8 -> distance_to_leader[4] */

extern void distances_update(void);


/* player_distance_warning */

/* returns 1 in race mode when distance_to_leader[p] > 7.0 (trainer warning threshold patch at
   0044c16b) */

extern undefined4 __cdecl player_distance_warning(char *param_1);



