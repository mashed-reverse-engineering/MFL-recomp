#include "mfl_tier2.h"
/*00403860*/ extern int attract_timer_reset();
/*00405e40*/ extern int stub_return_zero();
/*0040d0d0*/ extern int __cdecl find_texture_dict_b(RwChar *p0);
/*0040fbd0*/ extern int race_set_setup_index();
/*0040fbf0*/ extern int race_get_focus_player();
/*0040fdc0*/ extern int race_get_phase();
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
/*00449700*/ extern undefined __cdecl course_sky_atomic_create(struct RwTexture *p0, int p1, undefined4 p2);
/*0044aa50*/ extern undefined __cdecl camera_chase_update(undefined4 p0, undefined4 p1);
/*0044b110*/ extern undefined __cdecl camera_follow_update(undefined4 p0, undefined4 p1);
/*0044ef90*/ extern undefined __cdecl camera_record_update(undefined4 p0, undefined4 p1);
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
/*005dd32c*/ extern float k_f_3;
/*005dd330*/ extern float float_1_0;
/*005dd33c*/ extern float k_f_4;
/*005dd340*/ extern float k_f_0_5;
/*005dd41c*/ extern float float_5_0;
/*005dd7f4*/ extern float k_f_80;
/*005dda24*/ extern float k_f_0_75;
/*005ddb14*/ extern float float_7_0;
/*005ddc28*/ extern float k_f_0;
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
/*008c5940*/ extern undefined4 g_camera_records_8c5940;
/*008c598c*/ extern undefined4 g_car_8c598c;
/*008c5a00*/ extern float g_car_8c5a00;
/*008c5a08*/ extern undefined g_camera_record_8c5a08;
/*008c5a0c*/ extern undefined4 g_camera_record_8c5a0c;
/*008c5ae0*/ extern undefined g_camera_record_8c5ae0;
/*008c7440*/ extern undefined4 g_car_8c7440;
/*008c7460*/ extern undefined4 g_car_8c7460;
/*008c7470*/ extern undefined g_camera_get_8c7470;
/*008c747c*/ extern undefined4 g_car_8c747c;
/*008c7e00*/ extern float g_camera_get_8c7e00;
/*008c7e0c*/ extern float g_camera_8c7e0c;
/*008c7e18*/ extern int g_camera_records_8c7e18;
/*008c7e1c*/ extern undefined4 g_camera_8c7e1c;
/*008c7e20*/ extern undefined4 g_camera_8c7e20;
/*008c7e40*/ extern undefined4 distance_to_leader;
/*008c8070*/ extern undefined4 g_car_8c8070;
/*008c8078*/ extern undefined4 g_training_lesson;
/*008c86c8*/ extern undefined4 g_car_8c86c8;
/*008c8720*/ extern undefined4 g_hit_shake;
/*008c8788*/ extern undefined4 g_car_8c8788;
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

/* "calls ui_fn_4379c0 and sets the panel state flag for page types 4..8" */

extern void __cdecl menu_page_enter_set_panel_state(char *param_1);


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

/* message id of the "controller missing" message (0x218) */
#define MESSAGE_CONTROLLER_MISSING (0x218)
/* frontend page id of the race menu (0x21) */
#define MENU_RACE_PAGE (0x21)


/* controller_missing_message_update */

/* "shows/clears the 'controller missing' message (0x218) depending on input mode and connected
   controllers" */

/* TIER2: validated 2026-10-18 authority=diffemu */
void controller_missing_message_update(void)
{
  int controller_count;
  
  /* only act when no message is up, or the shown message is not one of the
     controller/tutorial texts (0x238, 0x236, 0x1c6, 0x237) (inferred) */
  if (((g_message_state == 0) || (g_message_code != -0xba0000)) ||
     ((g_message_text_id != 0x238 &&
      (((g_message_text_id != 0x236 && (g_message_text_id != 0x1c6)) && (g_message_text_id != 0x237)
       ))))) {
    /* input mode 0x1000: show the message only in the race menu (page 0x21) and
       only when a controller is connected; clear it if one is not */
    if (g_input_mode == 0x1000) {
      controller_count = ui_get_menu_69d1ec();
      if ((controller_count != MENU_RACE_PAGE) && (controller_count != 1)) {
        controller_count = controllers_count_present();
        if (controller_count != 0) {
          if (g_message_state == 0) {
            message_show_now(MESSAGE_CONTROLLER_MISSING,0xff220000,0,0,0,0);
            return;
          }
          if (g_message_code != -0xde0000) {
            message_replace_if_changed(MESSAGE_CONTROLLER_MISSING,0xff220000,0,0,0,0);
          }
        }
      }
    }
    /* other input modes, nothing shown: show the message if a human player
       has a controller (state 0 = no message up) */
    else if (g_message_state == 0) {
      ui_clear_cbe8();
      controller_count = player_slots_count_human_with_controller();
      if (controller_count != 0) {
        message_show_controller_missing();
        return;
      }
    }
    /* other input modes, a message is up: replace it with the variant
       (0xff210000) when a human player has a controller */
    else if ((g_message_code != -0xdf0000) && (g_message_code != -0xde0000)) {
      ui_clear_cbe8();
      controller_count = player_slots_count_human_with_controller();
      if (controller_count != 0) {
        message_replace_if_changed(MESSAGE_CONTROLLER_MISSING,0xff210000,0,0,0,0);
        return;
      }
    }
  }
  return;
}


/* number of word entries in the tutorial line table at g_tutorial_line_entries (inferred) */
#define TUTORIAL_LINE_ENTRY_COUNT (20)


/* frontend_enter_menu */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* "state 0 -> 1 and resets g_menu_current to 0 (or event 0x11 special case)" */
/* Enters the frontend menu, run once: does nothing unless g_frontend_state is 0. Sets the state to 1, sets the
   menu cursor (g_menu_current) to 0 or steps it back one page (event 0x11 special case, inferred), stores
   next_page in g_menu_next_page, resets the tutorial overlay values and clears the tutorial line entries,
   then copies and accumulates the player stats. */

/* TIER2: unvalidated 2026-10-16 authority=diffemu */
void frontend_enter_menu(undefined4 next_page)
{
  int remaining;
  undefined4 *entry_ptr;
  
  if (g_frontend_state == 0) {
    g_frontend_state = 1;
    /* normal case: cursor back to 0. Special case (else): event 0x11 with page 6 in record 4 or 5 steps back one (inferred) */
    if ((((int)g_menu_current < 1) || (g_event_type != EVENT_TRAINING)) ||
       ((g_menu_records[4].page_id != MENU_PAGE_CHALLENGE_SELECT_A && (g_menu_records[5].page_id != MENU_PAGE_CHALLENGE_SELECT_A)))) {
      g_menu_69ca1c = 0;
      g_menu_current = 0;
      g_menu_start_race_requested = 0;
      menu_table_clear_a();
    }
    else {
      g_menu_current = g_menu_current + -1;
      g_menu_69ca1c = 0;
      g_menu_start_race_requested = 0;
    }
    g_tutorial_69d0e8 = 210.0f;      /* diffemu: was the int literal 0x43520000, converted to a float */
    g_tutorial_69d0ec = 210.0f;      /* diffemu: was the int literal 0x43520000, converted to a float */
    g_tutorial_69d0f0 = 210.0f;      /* diffemu: was the int literal 0x43520000, converted to a float */
    g_tutorial_69d0f4 = 210.0f;      /* diffemu: was the int literal 0x43520000, converted to a float */
    g_menu_next_page = next_page;
    g_tutorial_line_count = 0;
    g_tutorial_line_width = 0;
    g_tutorial_display_ticks = 0;
    g_tutorial_69d15c = 0;
    g_menu_69cc48 = 0;
    /* clear every tutorial line entry */
    entry_ptr = &g_tutorial_line_entries;
    for (remaining = TUTORIAL_LINE_ENTRY_COUNT; remaining != 0; remaining = remaining + -1) {
      *entry_ptr = 0;
      entry_ptr = entry_ptr + 1;
    }
    g_tutorial_page_index = 0;
    player_stats_copy_to_totals();
    player_stats_accumulate();
    return;
  }
  return;
}


/* frontend_enter_race_menu */

/* "state 0 -> 1, menu id 0x21, scene setup" */

extern void frontend_enter_race_menu(char *param_1);


/* menu_event_is_selectable */

/* "false for event 0xE, else depends on ui_fn_435ec0 (event type is a career event)" */

extern bool __cdecl menu_event_is_selectable(char *param_1);


/* career_mark_row_unlocked */

/* "sets g_game_state.progress[row][0] = 1 when the row is available" */

extern void career_mark_row_unlocked(void);


/* hud_menu_draw_all_panels */

/* "draws the menu panels that are visible" */

extern void hud_menu_draw_all_panels(void);


/* quick_race_defaults_set */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* "game_mode = 6, event type 0x12, team mode off ..." */

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


/* player_distance_warning */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* returns 1 in race mode when distance_to_leader[p] > 7.0 (trainer warning threshold patch at
   0044c16b) */

extern undefined4 __cdecl player_distance_warning(char *param_1);


/* camera_get_8c7e00 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* "float g_camera_8c7e00" */

extern double camera_get_8c7e00(void);


/* camera_get_car_8c747c */

/* "return g_car_8c747c;" */

extern undefined4 camera_get_car_8c747c(void);


/* camera_get_8c7470 */

/* "returns &g_camera_8c7470" */

extern undefined * camera_get_8c7470(void);


/* camera_get_car_8c7460 */

/* "return g_car_8c7460;" */

extern undefined4 camera_get_car_8c7460(void);


/* camera_path_sample */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* "(path, index, t, out): interpolates between the ring buffer points of a camera path" */

extern void camera_path_sample(char *param_1,char *param_2,float param_3,float *param_4);


/* camera_get_8c7e0c */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* "float g_camera_8c7e0c" */

extern double camera_get_8c7e0c(void);


/* camera_records_update_all */

/* "calls camera_record_update for every active record" */

extern void __cdecl camera_records_update_all(char *param_1);


/* course_angels_cb0 */

extern void course_angels_cb0(void);



