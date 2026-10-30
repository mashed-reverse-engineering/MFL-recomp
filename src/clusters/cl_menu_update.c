// cluster-entries: menu_update
/* Installed cluster menu_update (tools/cluster_install.py from output/cluster_pilot_uc3/menu_update.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
extern undefined4 g_menu_update_8c8770;
extern undefined4 g_ai_editor_view_origin_z;
extern undefined4 g_hud_69cab8;
extern int g_ai_editor_mode;
extern undefined4 g_menu_update_69d0b8;
/* DECLS-BEGIN */
/*00416c40*/ extern int ai_bots_clear_stats();
/*004a4030*/ extern int __cdecl app_get_app_76e890_4030(int p0);
/*0042ec00*/ extern int best_time_load_current();
/*0042faf0*/ extern int __cdecl camera_fov_factor_set_wide(int p0);
/*004362c0*/ extern int career_event_has_entry();
/*00435fc0*/ extern int __cdecl career_event_set_progress(int p0);
/*004169f0*/ extern int career_event_slot_table();
/*004380f0*/ extern int career_row_previous();
/*00435f30*/ extern int career_time_under_target();
/*00438220*/ extern int controller_missing_message_update();
/*00434be0*/ extern int event_type_from_menu_choice();
/*00435ec0*/ extern int event_type_uses_standard_menu();
/*005dd330*/ extern float float_1_0;
/*008c86b8*/ extern int g_ai_8c86b8;
/*008c86cc*/ extern int g_ai_8c86cc;
/*008c8740*/ extern int g_ai_8c8740;
/*0069d688*/ extern int g_career_row;
/*0069cbd4*/ extern int g_event_type;
/*007ef7ec*/ extern float g_frame_seconds;
/*0069d1e0*/ extern int g_frontend_state;
/*007eebc0*/ extern struct GameState g_game_state;
/*008c86bc*/ extern int g_hud_8c86bc;
/*00608428*/ extern int g_menu_608428;
/*00608848*/ extern undefined g_menu_608848;
/*006088fc*/ extern undefined g_menu_6088fc;
/*00608938*/ extern undefined g_menu_608938;
/*00608974*/ extern undefined g_menu_608974;
/*00609a58*/ extern int g_menu_609a58;
/*0060ad5c*/ extern int g_menu_60ad5c;
/*0060ad60*/ extern int g_menu_60ad60;
/*0069cc44*/ extern int g_menu_69cc44;
/*0069cc48*/ extern int g_menu_69cc48;
/*0069cc4c*/ extern int g_menu_69cc4c;
/*0069cfc4*/ extern float g_menu_69cfc4;
/*0069d1f0*/ extern int g_menu_69d1f0;
/*0069d290*/ extern int g_menu_69d290;
/*0069d68c*/ extern int g_menu_69d68c;
/*0069d6b0*/ extern int g_menu_69d6b0;
/*0069d6b4*/ extern int g_menu_69d6b4;
/*007ef7d0*/ extern int g_menu_7ef7d0;
/*008c8080*/ extern int g_menu_8c8080;
/*008c8090*/ extern int g_menu_8c8090;
/*0069cf98*/ extern int g_menu_bot_skill_a;
/*0069cfac*/ extern int g_menu_bot_skill_b;
/*0069d2d0*/ extern int g_menu_career_race_active;
/*0069ca28*/ extern int g_menu_controller_vehicle;
/*0069cbd0*/ extern int g_menu_current;
/*0069d690*/ extern int g_menu_event_column;
/*0069cfb0*/ extern int g_menu_event_course_index;
/*0069d1e4*/ extern int g_menu_fade_alpha;
/*0069cfa4*/ extern int g_menu_game_mode_choice;
/*0069cf94*/ extern int g_menu_markers_enabled;
/*0069d21c*/ extern int g_menu_multiplayer_kind;
/*0069d1e8*/ extern int g_menu_next_page;
/*0060a620*/ extern undefined g_menu_page_26_position_screen;
/*0069c9c0*/ extern int g_menu_panel_state_69c9c0;
/*0069d668*/ extern int g_menu_player0_team_choice;
/*0069cfa8*/ extern int g_menu_player_count_index;
/*0069cfb4*/ extern int g_menu_player_team_choice;
/*0069cfb8*/ extern int g_menu_player_team_choice_1;
/*0069cfbc*/ extern int g_menu_player_team_choice_2;
/*0069cf90*/ extern int g_menu_powerup_set;
/*0069cf9c*/ extern int g_menu_race_length_choice;
/*0069d2e0*/ extern struct MenuRecord g_menu_records[14];
/*0069d1ec*/ extern int g_menu_screen_id;
/*0069ca18*/ extern float g_menu_select_flash;
/*0069d6a8*/ extern int g_menu_select_pressed;
/*0069cfa0*/ extern int g_menu_selected_vehicle;
/*0069caec*/ extern int g_menu_start_race_requested;
/*0069cfc8*/ extern int g_menu_team_mode_choice;
/*00664360*/ extern undefined g_menu_update_664360;
/*0069d2bc*/ extern int g_menu_update_69d2bc;
/*0069d2c0*/ extern int g_menu_update_69d2c0;
/*0069d2d4*/ extern int g_menu_update_69d2d4;
/*0069d684*/ extern int g_menu_update_69d684;
/*0069d6b8*/ extern float g_menu_gamma_applied;
/*0069d6ac*/ extern int g_message_69d6ac;
/*0069cfd0*/ extern int g_message_state;
/*007ef820*/ extern struct PlayerInput g_player_inputs[12];
/*007f01fc*/ extern struct PlayerSlot g_player_slots[4];
/*0069cc40*/ extern int g_team_mode;
/*008c8078*/ extern int g_training_lesson;
/*0069d2a8*/ extern int g_tutorial_69d2a8;
/*0069d160*/ extern int g_tutorial_reset_requested;
/*0069cb10*/ extern int g_ui_69cb10;
/*0069d1f4*/ extern int g_ui_69d1f4;
/*007ef7b8*/ extern int game_mode;
/*0042fae0*/ extern int game_state_get_2812();
/*0040ae30*/ extern int gamesave_request_load_slot1();
/*0040aeb0*/ extern int gamesave_request_save_a();
/*0040c2c0*/ extern int __cdecl gamesave_set_autosave_enabled(int p0);
/*00481fc0*/ extern int gfx_set_89ddfc();
/*004a16e0*/ extern int __cdecl input_player_rumble_set(int p0);
/*00434a10*/ extern int is_team_mode();
/*005e64a4*/ extern float k_f_0;
/*005dd338*/ extern float k_f_0_01;
/*005de2fc*/ extern float k_f_0_04;
/*0040ae80*/ extern int loading_start_log();
/*004300b0*/ extern int menu_back_pressed_any();
/*00437f90*/ extern int menu_career_row_next();
/*0042fe90*/ extern int menu_current_item_offset();
/*00430380*/ extern int menu_down_pressed_any();
/*00436200*/ extern int menu_event_column_progress_unless_special();
/*00430970*/ extern int menu_input_next_pressed();
/*00438340*/ extern int menu_items_animate_update();
/*00430510*/ extern int menu_left_pressed_any();
/*00437930*/ extern int menu_options_defaults_reset();
/*00438580*/ extern int menu_page_update_entry_states();
/*00443900*/ extern void menu_panel_fades_update(void);
/*00430150*/ extern int menu_pause_pressed_any();
/*00430740*/ extern int menu_right_pressed_any();
/*00430010*/ extern int __cdecl menu_select_pressed_any(int p0);
/*0042fc00*/ extern int menu_selection_step();
/*00432cc0*/ extern int menu_state_save();
/*00434910*/ extern int menu_text_format_and_draw();
/*004301f0*/ extern int menu_up_pressed_any();
/*00430be0*/ extern int menu_validate_player_setup();
/*00431130*/ extern int __cdecl message_show_now(int p0, int p1, int p2, int p3, int p4, int p5);
/*004a4060*/ extern int __cdecl movie_stop_b(int p0);
/*0040c2b0*/ extern int nop_b();
/*0069d028*/ extern int player_color_id;
/*00436520*/ extern int player_slots_count_active();
/*00430d60*/ extern int player_slots_reset_state();
/*00426650*/ extern int player_stats_clear_b();
/*0042fad0*/ extern int race_load_course_0042fad0();
/*0040fdd0*/ extern int race_set_phase();
/*0040fbd0*/ extern int race_set_setup_index();
/*0040ff20*/ extern int race_setup_get_slot_type();
/*0040ff40*/ extern int race_setup_set_slot_type();
/*0040e610*/ extern int race_setup_use_0();
/*0040e620*/ extern int race_setup_use_1();
/*0040e630*/ extern int race_setup_use_2();
/*0040e640*/ extern int race_setup_use_3();
/*0040e650*/ extern int race_setup_use_4();
/*0040e660*/ extern int race_setup_use_5();
/*0040cdb0*/ extern int round_stats_reset();
/*00403820*/ extern int scene_get_log_64d898();
/*0042e9e0*/ extern int time_clear_current();
/*00430b50*/ extern int ui_input_mode_menu();
/*00430b60*/ extern int ui_input_reset();
/*00431710*/ extern int ui_menu_reset();
/*004a4040*/ extern int video_texture_setup_004a4040();
extern int gamesave_update(int seconds_bits);
extern int menu_page_enter(int page, int arg);
#define MENU_EVENT_COL_USE5_A 0 /* event column of the setup that fills race_setup_use_5 */
#define MENU_EVENT_COL_USE0 3 /* event column of the setup that fills race_setup_use_0 */
#define MENU_EVENT_COL_USE5_B 0xb /* event column of the setup that fills race_setup_use_5 */
#define MENU_EVENT_COL_USE2 0xe /* event column of the setup that fills race_setup_use_2 */
#define MENU_EVENT_COL_USE3 0xf /* event column of the setup that fills race_setup_use_3 */
#define MENU_EVENT_COL_USE4 0x10 /* event column of the setup that fills race_setup_use_4 */
#define MENU_EVENT_COL_USE1 0x11 /* event column of the setup that fills race_setup_use_1 */
/* (inferred) event columns come in groups of three: a group's variant n adds 0xf*n to the setup index (race_set_setup_index(row + 0xf * n)); groups start at 0, 3, 6, 0xb */
#define MENU_SETUP_VARIANT_STRIDE 0xf /* setup index step between the three variants of an event column group */
#define MENU_EVENT_COL_USE5_A_V1 0x1
#define MENU_EVENT_COL_USE5_A_V2 0x2
#define MENU_EVENT_COL_USE0_V1 0x4
#define MENU_EVENT_COL_USE0_V2 0x5
#define MENU_EVENT_COL_G6 0x6
#define MENU_EVENT_COL_G6_V1 0x7
#define MENU_EVENT_COL_G6_V2 0x8
#define MENU_EVENT_COL_USE5_B_V1 0xc
#define MENU_EVENT_COL_USE5_B_V2 0xd
extern int video_get_default_mode(void);
/* DECLS-END */
extern float k_f_n500;
extern float k_f_1000;
extern float k_f_n800;
extern unsigned int PTR_g_menu_page_01_game_type_select_0060acac;
#define GS(off) (*(int *)((char *)&g_game_state + (off)))
#define SLF(n) (*(float *)&g_game_state.option_sliders[n])
extern float menu_get_69cfc4(void);
extern void gamma_apply_via_device_scaled(float a, float b);
extern void nop_stub(void);
extern void __cdecl movie_poll_and_set_surface_b(int a);
/* menu item tokens (high word 0xffNN): the item id a menu record hands back; named by what the handler does (inferred) */
#define MENU_ITEM_START_RACE 0xff150000 /* menu_page_enter(PAUSE,2) + start_race_requested */
#define MENU_ITEM_PLAY_GAME_CHECKED 0xff1d0000 /* validates the player setup before starting */
#define MENU_ITEM_MSG_33 0xff1e0000 /* "locked" help message 0x33 */
#define MENU_ITEM_MSG_34 0xff1f0000 /* "locked" help message 0x34 */
#define MENU_ITEM_MSG_35_A 0xff200000 /* help message 0x35 */
#define MENU_ITEM_CHALLENGE_PLAY 0xff240000 /* career challenge: starts, or message 0x39/0xac */
#define MENU_ITEM_MSG_13B 0xff270000 /* career challenge select: message 0x13b when career_time_under_target() is 0, then career_race_active = 2 */
#define MENU_ITEM_MSG_13A 0xff280000 /* message 0x13a when career_time_under_target() is nonzero, then career_race_active = 1 */
#define MENU_ITEM_MSG_AC 0xff320000 /* message 0xac */
#define MENU_ITEM_STANDARD_PLAY 0xff2c0000 /* multiplayer standard play (g_team_mode 0) */
#define MENU_ITEM_TEAM_PLAY 0xff2e0000 /* multiplayer team play (g_team_mode 1) */
#define MENU_ITEM_TIME_TRIAL_STANDARD 0xff2d0000 /* time trial, standard */
#define MENU_ITEM_TIME_TRIAL_TEAM 0xff2f0000 /* time trial, team */
#define MENU_ITEM_MULTIPLAYER_KIND_1 0xff300000 /* g_menu_multiplayer_kind = 1 */
#define MENU_ITEM_MULTIPLAYER_KIND_2 0xff310000 /* g_menu_multiplayer_kind = 2 */
#define MENU_ITEM_OPEN_XBOX_LIVE 0xff360000 /* page XBOX_LIVE */
#define MENU_ITEM_OPEN_JOIN 0xff380000 /* page JOIN */
#define MENU_ITEM_OPEN_CHALLENGE_SELECT_A 0xff3b0000 /* page CHALLENGE_SELECT_A, resets slots */
#define MENU_ITEM_SETUP_USE_0 0xff3d0000 /* race_setup_use_0, event column 3 */
#define MENU_ITEM_SETUP_USE_5_COL0 0xff400000 /* race_setup_use_5, event column 0 */
#define MENU_ITEM_SETUP_USE_5_COL_B 0xff4d0000 /* race_setup_use_5, event column 0xb */
#define MENU_ITEM_SETUP_USE_2 0xff850000 /* race_setup_use_2, event column 0xe */
#define MENU_ITEM_SETUP_USE_3 0xff860000 /* race_setup_use_3, event column 0xf */
#define MENU_ITEM_SETUP_USE_4 0xff870000 /* race_setup_use_4, event column 0x10 */
#define MENU_ITEM_SETUP_USE_1 0xff880000 /* race_setup_use_1, event column 0x11 */
#define MENU_ITEM_SETUP_INDEX_0 0xff3a0000 /* race_set_setup_index(0) */
#define MENU_ITEM_RESET_PLAYER_SLOTS 0xff3c0000 /* player_slots_reset_state: 0x1000 resets the vehicles and opens ABILITY_SELECT, 0..3 show message 0xd9..0xdc */
#define MENU_ITEM_QUICK_RACE_START 0xff420000 /* quick race 11..13 start */
#define MENU_ITEM_START_LOADING 0xff440000 /* loading_start_log */
#define MENU_ITEM_OPEN_SOUND 0xff430000 /* page SOUND */
#define MENU_ITEM_RESUME 0xff450000 /* menu_page_enter(PAUSE,1) */
#define MENU_ITEM_SAVE_GAME 0xff470000 /* gamesave_request_save_a */
#define MENU_ITEM_OPEN_CHALLENGE_SELECT_B 0xff490000 /* page CHALLENGE_SELECT_B */
#define MENU_ITEM_START_RACE_SETUP 0xff4a0000 /* starts a race with race phase SETUP */
#define MENU_ITEM_EXIT_TO_MAIN 0xff4b0000 /* load slot 1, back to the main page */
#define MENU_ITEM_BACK_FROM_SAVE_LOAD 0xff4c0000 /* back item: on TRANSMISSION_INTERRUPTED_B returns to the main page (reloads slot 1), else returns to the pause page and saves (gamesave_request_save_a) or logs a load (loading_start_log) depending on g_menu_update_69d2bc/c0 */
#define MENU_ITEM_MSG_35_B 0xff4e0000 /* help message 0x35 */
#define MENU_ITEM_OPEN_OPTIONS 0xff500000 /* page OPTIONS */
#define MENU_ITEM_TEAM_COLOUR_A 0xff710000 /* page TEAM_COLOUR_SELECT, g_menu_7ef7d0 = 0 */
#define MENU_ITEM_TEAM_COLOUR_B 0xff720000 /* page TEAM_COLOUR_SELECT, g_menu_7ef7d0 = 1 */
#define MENU_ITEM_OPEN_GAMMA 0xff730000 /* page GAMMA */
#define MENU_ITEM_MSG_267 0xff800000 /* help message 0x267 */
#define MENU_ITEM_DESELECT 0xff810000 /* clears the selection */
#define MENU_ITEM_OPEN_BONUS 0xff820000 /* page BONUS_FEATURES or BONUS_CAMERAS */
#define MENU_ITEM_OPEN_AUTOSAVE 0xff830000 /* page AUTOSAVE */
#define MENU_ITEM_LOAD_COURSE 0xff840000 /* race_load_course_0042fad0 */
#define MENU_ITEM_OPEN_TRANSMISSION_INTERRUPTED_A 0xff480000 /* page TRANSMISSION_INTERRUPTED_A (0x14, title text 72) */

/* Frontend menu state machine (0x4463e0): applies the gamma slider value when it changed, decays the select flash, then per frontend state 1..5 the fade in/out of the page, and in state 3 the input handling: the
   position screen, select (the item offset token switch that starts races, opens pages, sets up the player slots), back, up, down, pause, left/right value
   changes per page id, and the next key.  The item list of the current page (g_menu_records) is a token list; select/back/up/down/left/right/next are enabled
   by tokens between the 0xff090000 and 0xff0a0000 markers (select by 0xff100000 or 0xff230000). */
static int menu_team_choice_step_r(int p0, int p1, int p2);
extern void menu_team_choice_step(void);
static int career_flag_clear_r(int p0, int p1);
extern void career_flag_clear(void);
void menu_update(void)
{
  int team_mode, aux, active_slots, slot_type, team_pick, team_cand, controller_skip, teams_left, next_controller;
  unsigned int token;
  unsigned int text_id;
  unsigned int *color_ptr, *src_word, *dst_word;
  PlayerSlot *slot_scan, *slot_ptr;
  int *slot_team_ptr, *tbl, *tbl_next;
  int select_enabled;
  char team_count;
  int slot_clash, back_enabled, up_enabled, down_enabled, left_enabled, right_enabled, next_enabled;
  float v;

  v = menu_get_69cfc4();
  if (v != g_menu_gamma_applied) {
    g_menu_gamma_applied = v;
    gamma_apply_via_device_scaled(v, 1.0f);
  }
  g_menu_select_flash = g_menu_select_flash - k_f_0_04;
  if (g_menu_select_flash < k_f_0) {
    g_menu_select_flash = 0.0;
  }
  team_mode = app_get_app_76e890_4030(0);
  if (team_mode == 0) {
    video_texture_setup_004a4040(0,0,1);
  }
  else {
    movie_poll_and_set_surface_b(0);
    video_get_default_mode();
  }
  /* per frame: panel fades, savegame update, then the frontend_state machine (1 fade alpha up to 0xff, then enter the next page; 2 alpha back down to 0; 3 interactive; 4 set when g_menu_start_race_requested: alpha up to 0xff; 5 fade out, then back to 0) */
  menu_panel_fades_update();
  gamesave_update(*(int *)&g_frame_seconds);
  if (g_menu_select_pressed == 2) {
    g_menu_select_pressed = 1;
    return;
  }
  g_menu_select_pressed = 0;
  g_message_69d6ac = 0;
  g_menu_69d6b0 = 0;
  if (g_frontend_state == 1) {
    team_mode = (int)(g_frame_seconds * k_f_1000);
    if (g_menu_fade_alpha + team_mode < 0xff) {
      g_menu_fade_alpha = g_menu_fade_alpha + team_mode;
      return;
    }
    g_menu_fade_alpha = 0xff;
    g_frontend_state = 2;
    menu_page_enter(g_menu_next_page,0);
    return;
  }
  if (g_frontend_state == 2) {
    menu_items_animate_update();
    team_mode = (int)(g_frame_seconds * k_f_n500);
    g_menu_fade_alpha = g_menu_fade_alpha + team_mode;
    if (-1 < g_menu_fade_alpha) {
      return;
    }
    g_menu_fade_alpha = 0;
    g_frontend_state = 3;
    return;
  }
  /* state 3 (interactive): position screen nudges first, then input handling */
  if (g_frontend_state != 3) goto LAB_004490d6;
  if ((undefined *)g_menu_records[g_menu_current].item_list == &g_menu_page_26_position_screen) {
    if (g_player_inputs[0].direction_flags[2] != 0) {
      GS(2796) = GS(2796) + -2;
    }
    if (g_player_inputs[0].direction_flags[3] != 0) {
      GS(2796) = GS(2796) + 2;
    }
    if (g_player_inputs[0].direction_flags[0] != 0) {
      GS(2792) = GS(2792) + -4;
    }
    if (g_player_inputs[0].direction_flags[1] != 0) {
      GS(2792) = GS(2792) + 4;
    }
    if (g_player_inputs[1].direction_flags[2] != 0) {
      GS(2796) = GS(2796) + -2;
    }
    if (g_player_inputs[1].direction_flags[3] != 0) {
      GS(2796) = GS(2796) + 2;
    }
    if (g_player_inputs[1].direction_flags[0] != 0) {
      GS(2792) = GS(2792) + -4;
    }
    if (g_player_inputs[1].direction_flags[1] != 0) {
      GS(2792) = GS(2792) + 4;
    }
    if (g_player_inputs[2].direction_flags[2] != 0) {
      GS(2796) = GS(2796) + -2;
    }
    if (g_player_inputs[2].direction_flags[3] != 0) {
      GS(2796) = GS(2796) + 2;
    }
    if (g_player_inputs[2].direction_flags[0] != 0) {
      GS(2792) = GS(2792) + -4;
    }
    if (g_player_inputs[2].direction_flags[1] != 0) {
      GS(2792) = GS(2792) + 4;
    }
    if (g_player_inputs[3].direction_flags[2] != 0) {
      GS(2796) = GS(2796) + -2;
    }
    if (g_player_inputs[3].direction_flags[3] != 0) {
      GS(2796) = GS(2796) + 2;
    }
    if (g_player_inputs[3].direction_flags[0] != 0) {
      GS(2792) = GS(2792) + -4;
    }
    if (g_player_inputs[3].direction_flags[1] != 0) {
      GS(2792) = GS(2792) + 4;
    }
    if (g_player_inputs[4].direction_flags[2] != 0) {
      GS(2796) = GS(2796) + -2;
    }
    if (g_player_inputs[4].direction_flags[3] != 0) {
      GS(2796) = GS(2796) + 2;
    }
    if (g_player_inputs[4].direction_flags[0] != 0) {
      GS(2792) = GS(2792) + -4;
    }
    if (g_player_inputs[4].direction_flags[1] != 0) {
      GS(2792) = GS(2792) + 4;
    }
    if (g_player_inputs[5].direction_flags[2] != 0) {
      GS(2796) = GS(2796) + -2;
    }
    if (g_player_inputs[5].direction_flags[3] != 0) {
      GS(2796) = GS(2796) + 2;
    }
    if (g_player_inputs[5].direction_flags[0] != 0) {
      GS(2792) = GS(2792) + -4;
    }
    if (g_player_inputs[5].direction_flags[1] != 0) {
      GS(2792) = GS(2792) + 4;
    }
    if (g_player_inputs[6].direction_flags[2] != 0) {
      GS(2796) = GS(2796) + -2;
    }
    if (g_player_inputs[6].direction_flags[3] != 0) {
      GS(2796) = GS(2796) + 2;
    }
    if (g_player_inputs[6].direction_flags[0] != 0) {
      GS(2792) = GS(2792) + -4;
    }
    if (g_player_inputs[6].direction_flags[1] != 0) {
      GS(2792) = GS(2792) + 4;
    }
    if (g_player_inputs[7].direction_flags[2] != 0) {
      GS(2796) = GS(2796) + -2;
    }
    if (g_player_inputs[7].direction_flags[3] != 0) {
      GS(2796) = GS(2796) + 2;
    }
    if (g_player_inputs[7].direction_flags[0] != 0) {
      GS(2792) = GS(2792) + -4;
    }
    if (g_player_inputs[7].direction_flags[1] != 0) {
      GS(2792) = GS(2792) + 4;
    }
    if ((int)GS(2796) < 0) {
      GS(2796) = 0;
    }
    else if (0x76 < (int)GS(2796)) {
      GS(2796) = 0x76;
    }
    if ((int)GS(2792) < 0x15e) {
      GS(2792) = 0x15e;
    }
    else if (0x41a < (int)GS(2792)) {
      GS(2792) = 0x41a;
    }
  }
  team_mode = menu_items_animate_update();
  controller_missing_message_update();
  if (g_menu_start_race_requested != 0) {
    g_frontend_state = 4;
  }
  if ((team_mode != 0) && (g_message_state == 0)) {
    /* scan the page item token list up to its end marker (-0xf90000) or the input marker (-0xf70000); the tokens after the input marker enable select/back/up/down/left/right/pause/next (aux = pause token 0xff120000 present; it is also passed to menu_select_pressed_any (inferred: allows pause as select)) */
    tbl = (int *)g_menu_records[g_menu_current].item_list;
    aux = 0;
    team_mode = tbl[0];
    while (team_mode != MENU_ITEM_LIST_END && team_mode != MENU_ITEM_LIST_INPUT_BEGIN) {
      aux = aux + 1;
      team_mode = tbl[aux];
    }
    if (tbl[aux] == MENU_ITEM_LIST_INPUT_BEGIN) {
      tbl = tbl + aux + 1;
      aux = 0;
      next_enabled = false;
      right_enabled = false;
      left_enabled = false;
      down_enabled = false;
      up_enabled = false;
      back_enabled = false;
      select_enabled = 0;
      team_mode = *tbl;
      while (team_mode != MENU_ITEM_LIST_INPUT_END) {
        if ((team_mode == MENU_INPUT_SELECT) || (team_mode == MENU_INPUT_SELECT_ALT)) {
          select_enabled = 1;
        }
        else {
          if (team_mode == MENU_INPUT_BACK) {
            back_enabled = true;
          }
          else if (team_mode == MENU_INPUT_UP) {
            up_enabled = true;
          }
          else if (team_mode == MENU_INPUT_DOWN) {
            down_enabled = true;
          }
          else if (team_mode == MENU_INPUT_LEFT) {
            left_enabled = true;
          }
          else if (team_mode == MENU_INPUT_RIGHT) {
            right_enabled = true;
          }
          else if (team_mode == MENU_INPUT_PAUSE) {
            aux = 1;
          }
          else if (team_mode == MENU_INPUT_NEXT) {
            next_enabled = true;
          }
        }
        tbl_next = tbl + 1;
        tbl = tbl + 1;
        team_mode = *tbl_next;
      }
      nop_stub();
      if ((select_enabled != 0) && (team_mode = menu_select_pressed_any(aux), team_mode != 0)) {
        g_menu_select_pressed = 1;
        /* select pressed: the highlighted item's token (0xff....0000) picks the action (start race, open page, set up player slots ...) */
        token = menu_current_item_offset();
        if (((g_menu_panel_state_69c9c0 != 1) && (g_menu_panel_state_69c9c0 != 2)) ||
           (g_menu_records[g_menu_current + 1].selected == 0)) {
          g_menu_select_flash = 1.0;
        }
        if (token != 0xffffffff) {
          if (token < 0xff440001) {
            if (token == MENU_ITEM_START_LOADING) {
              loading_start_log();
            }
            else if (token < 0xff310001) {
              if (token == MENU_ITEM_MULTIPLAYER_KIND_2) {
                g_menu_multiplayer_kind = 2;
LAB_00447824:
                color_ptr = &player_color_id;
                do {
                  *color_ptr = 0xffffffff;
                  color_ptr = color_ptr + 3;
                } while ((int)color_ptr < (int)&g_menu_update_69d0b8);
                goto LAB_004477e2;
              }
              if (token < 0xff260001) {
                if (token == 0xff260000) {
                  active_slots = career_event_has_entry();
                  slot_type = g_career_row;
                  team_mode = g_event_type;
                  if (g_game_state.progress[active_slots + g_career_row * 0x12] == 0) {
                    g_menu_select_flash = 0.0;
                  }
                  else {
                    g_menu_career_race_active = 0;
                    game_mode = GAME_MODE_STANDARD;
                    career_flag_clear_r(g_event_type, g_career_row);
                    active_slots = event_type_uses_standard_menu();
                    if (active_slots != 0) {
                      race_setup_set_slot_type(0,1);
                      race_setup_set_slot_type(1,0);
                      race_setup_set_slot_type(2,0);
                      race_setup_set_slot_type(3,0);
                      team_mode = g_event_type;
                      slot_type = g_career_row;
                    }
                    switch(team_mode) {
                    case 0:
                    case 1:
                    case 2:
                      g_team_mode = 0;
                      g_menu_multiplayer_kind = 0;
                      g_menu_powerup_set = 0;
                      g_menu_69cc4c = 0;
                      g_player_slots[1].controller = -1;
                      g_player_slots[2].controller = -1;
                      g_player_slots[3].controller = -1;
                      g_player_slots[0].state = -1;
                      g_player_slots[1].state = -1;
                      g_player_slots[2].state = -1;
                      g_player_slots[3].state = -1;
                      best_time_load_current();
                      time_clear_current();
                      car_set_field_8(0,g_menu_selected_vehicle);
                      team_mode = g_event_type;
                      slot_type = g_career_row;
                      break;
                    case 3:
                    case 4:
                    case 5:
                    case 0xe:
                    case 0xf:
                    case 0x10:
                      active_slots = 0;
                      if (team_mode == 4) {
                        active_slots = 1;
                      }
                      else if (team_mode == 5) {
                        active_slots = 2;
                      }
                      active_slots = (active_slots + slot_type * 3) * 4;
                      /* career events 3,4,5,0xe,0xf,0x10: token = mode class of the event (high word): 0/1/0xc team race, 2/3/0xb race, 6 survival, 5 chase fugitive, 7 kill copter, 8 airstrike (GAME_MODE_* below); each branch resets the player slots and fills the opponent slots (controller_skip steps over the human's controller id, team_pick avoids the teams already used) */
                      g_menu_markers_enabled = *(uint *)(&g_menu_update_664360 + active_slots);
                      token = *(uint *)(&g_menu_608848 + active_slots);
                      if (team_mode == 0xe) {
                        token = *(uint *)(&g_menu_6088fc + active_slots);
                      }
                      else if (team_mode == 0xf) {
                        token = *(uint *)(&g_menu_608938 + active_slots);
                      }
                      else if (team_mode == 0x10) {
                        token = *(uint *)(&g_menu_608974 + active_slots);
                      }
                      token = token & 0xffff0000;
                      if (token == 0x70000) {
                        game_mode = GAME_MODE_KILL_COPTER;
                        g_menu_powerup_set = 1;
                        g_player_slots[0].state = -1;
                      }
                      else {
                        if (token != 0x40000) {
                          if (token == 0x80000) {
                            game_mode = GAME_MODE_AIRSTRIKE;
                            g_team_mode = 0;
                            g_menu_multiplayer_kind = 0;
                            g_menu_powerup_set = 0;
                            g_menu_69cc4c = 0;
                            g_player_slots[1].controller = -1;
                            g_player_slots[2].controller = -1;
                            g_player_slots[3].controller = -1;
                            g_player_slots[0].state = 0;
                            g_player_slots[1].state = -1;
                            g_player_slots[2].state = -1;
                            g_player_slots[3].state = -1;
                            team_mode = career_event_slot_table();
                            tbl = (int *)(team_mode + g_career_row * 0x30);
                            if (g_event_type == EVENT_CUP_4) {
                              tbl = tbl + 4;
                            }
                            else if (g_event_type == EVENT_CUP_5) {
                              tbl = tbl + 8;
                            }
                            team_mode = *tbl;
                            controller_skip = 0;
                            if (0 < team_mode) {
                              slot_team_ptr = &g_player_slots[1].team;
                              slot_type = 0;
                              tbl_next = tbl + 1;
                              do {
                                active_slots = slot_type + 1;
                                if (g_player_slots[0].controller == active_slots) {
                                  controller_skip = controller_skip + 1;
                                }
                                ((PlayerSlot *)(slot_team_ptr + -2))->controller =
                                     controller_skip + 1 + slot_type;
                                race_setup_set_slot_type(active_slots,2);
                                slot_type = *tbl_next;
                                tbl_next = tbl_next + 1;
                                team_pick = slot_type;
                                if (slot_type == g_player_slots[0].team) {
                                  team_cand = 0;
                                  do {
                                    if (((team_cand != g_player_slots[0].team) &&
                                        (team_cand != tbl[1])) &&
                                       ((team_cand != tbl[2] &&
                                        (team_pick = team_cand, team_cand != tbl[3])))) break;
                                    team_cand = team_cand + 1;
                                    team_pick = slot_type;
                                  } while (team_cand < 6);
                                }
                                *slot_team_ptr = team_pick;
                                slot_team_ptr[-1] = 1;
                                slot_team_ptr = slot_team_ptr + 4;
                                slot_type = active_slots;
                              } while (active_slots < team_mode);
                            }
                            car_set_field_8(0,g_menu_selected_vehicle);
                            g_menu_markers_enabled = 1;
                            team_mode = g_event_type;
                            slot_type = g_career_row;
                          }
                          else {
                            if (token == 0x50000) {
                              game_mode = GAME_MODE_CHASE_FUGITIVE;
                              g_menu_powerup_set = 0;
                              g_menu_69cc4c = 0;
                              g_team_mode = 0;
                              g_player_slots[1].controller = -1;
                              g_player_slots[2].controller = -1;
                              g_player_slots[3].controller = -1;
                              g_player_slots[0].state = 0;
                              g_player_slots[1].state = -1;
                              g_player_slots[2].state = -1;
                              g_player_slots[3].state = -1;
                              team_mode = career_event_slot_table();
                              tbl = (int *)(team_mode + g_career_row * 0x30);
                              if (g_event_type == EVENT_CUP_4) {
                                tbl = tbl + 4;
                              }
                              else if (g_event_type == EVENT_CUP_5) {
                                tbl = tbl + 8;
                              }
                              team_mode = *tbl;
                              controller_skip = 0;
                              if (0 < team_mode) {
                                slot_team_ptr = &g_player_slots[1].team;
                                slot_type = 0;
                                tbl_next = tbl + 1;
                                do {
                                  active_slots = slot_type + 1;
                                  if (g_player_slots[0].controller == active_slots) {
                                    controller_skip = controller_skip + 1;
                                  }
                                  ((PlayerSlot *)(slot_team_ptr + -2))->controller =
                                       controller_skip + 1 + slot_type;
                                  race_setup_set_slot_type(active_slots,2);
                                  slot_type = *tbl_next;
                                  tbl_next = tbl_next + 1;
                                  team_pick = slot_type;
                                  if (slot_type == g_player_slots[0].team) {
                                    team_cand = 0;
                                    do {
                                      if (((team_cand != g_player_slots[0].team) &&
                                          (team_cand != tbl[1])) &&
                                         ((team_cand != tbl[2] &&
                                          (team_pick = team_cand, team_cand != tbl[3])))) break;
                                      team_cand = team_cand + 1;
                                      team_pick = slot_type;
                                    } while (team_cand < 6);
                                  }
                                  *slot_team_ptr = team_pick;
                                  slot_team_ptr[-1] = 1;
                                  slot_team_ptr = slot_team_ptr + 4;
                                  slot_type = active_slots;
                                } while (active_slots < team_mode);
                              }
                            }
                            else {
                              if (((token == 0x20000) || (token == 0x30000)) ||
                                 ((token == 0xb0000 || (token == 0x60000)))) {
                                game_mode = GAME_MODE_RACE;
                                if (token == 0x60000) {
                                  game_mode = GAME_MODE_SURVIVAL;
                                }
                                g_menu_powerup_set = 0;
                                if (token == 0x30000) {
                                  g_menu_powerup_set = 1;
                                }
                                else if (token == 0x60000) {
                                  g_menu_powerup_set = 1;
                                }
                                else if (token == 0xb0000) {
                                  g_menu_powerup_set = 2;
                                }
                                g_menu_69cc4c = 0;
                                g_team_mode = 0;
                                g_player_slots[1].controller = -1;
                                g_player_slots[2].controller = -1;
                                g_player_slots[3].controller = -1;
                                g_player_slots[0].state = 0;
                                g_player_slots[1].state = -1;
                                g_player_slots[2].state = -1;
                                g_player_slots[3].state = -1;
                                team_mode = career_event_slot_table();
                                tbl = (int *)(team_mode + g_career_row * 0x30);
                                if (g_event_type == EVENT_CUP_4) {
                                  tbl = tbl + 4;
                                }
                                else if (g_event_type == EVENT_CUP_5) {
                                  tbl = tbl + 8;
                                }
                                team_mode = *tbl;
                                controller_skip = 0;
                                if (0 < team_mode) {
                                  slot_team_ptr = &g_player_slots[1].team;
                                  slot_type = 0;
                                  tbl_next = tbl + 1;
                                  do {
                                    active_slots = slot_type + 1;
                                    if (g_player_slots[0].controller == active_slots) {
                                      controller_skip = controller_skip + 1;
                                    }
                                    ((PlayerSlot *)(slot_team_ptr + -2))->controller =
                                         controller_skip + 1 + slot_type;
                                    race_setup_set_slot_type(active_slots,2);
                                    slot_type = *tbl_next;
                                    tbl_next = tbl_next + 1;
                                    team_pick = slot_type;
                                    if (slot_type == g_player_slots[0].team) {
                                      team_cand = 0;
                                      do {
                                        if (((team_cand != g_player_slots[0].team) &&
                                            (team_cand != tbl[1])) &&
                                           ((team_cand != tbl[2] &&
                                            (team_pick = team_cand, team_cand != tbl[3])))) break;
                                        team_cand = team_cand + 1;
                                        team_pick = slot_type;
                                      } while (team_cand < 6);
                                    }
                                    *slot_team_ptr = team_pick;
                                    slot_team_ptr[-1] = 1;
                                    slot_team_ptr = slot_team_ptr + 4;
                                    slot_type = active_slots;
                                  } while (active_slots < team_mode);
                                }
                                car_set_field_8(0,g_menu_selected_vehicle);
                                team_mode = g_event_type;
                                slot_type = g_career_row;
                              }
                              if (((token == 0) || (token == 0x10000)) || (token == 0xc0000)) {
                                g_team_mode = 1;
                                g_menu_multiplayer_kind = 0;
                                g_menu_powerup_set = 0;
                                if (token == 0x10000) {
                                  g_menu_powerup_set = 1;
                                }
                                else if (token == 0xc0000) {
                                  g_menu_powerup_set = 2;
                                }
                                g_menu_69cc4c = 0;
                                g_player_slots[1].controller = -1;
                                g_player_slots[2].controller = -1;
                                g_player_slots[3].controller = -1;
                                g_player_slots[0].state = 0;
                                g_player_slots[1].state = -1;
                                g_player_slots[2].state = -1;
                                g_player_slots[3].state = -1;
                                team_mode = career_event_slot_table();
                                tbl = (int *)(team_mode + g_career_row * 0x30);
                                if (g_event_type == EVENT_CUP_4) {
                                  tbl = tbl + 4;
                                }
                                else if (g_event_type == EVENT_CUP_5) {
                                  tbl = tbl + 8;
                                }
                                team_mode = *tbl;
                                controller_skip = 0;
                                if (0 < team_mode) {
                                  slot_team_ptr = &g_player_slots[1].team;
                                  slot_type = 0;
                                  tbl_next = tbl + 1;
                                  do {
                                    active_slots = slot_type + 1;
                                    if (g_player_slots[0].controller == active_slots) {
                                      controller_skip = controller_skip + 1;
                                    }
                                    ((PlayerSlot *)(slot_team_ptr + -2))->controller =
                                         controller_skip + 1 + slot_type;
                                    race_setup_set_slot_type(active_slots,2);
                                    slot_type = *tbl_next;
                                    tbl_next = tbl_next + 1;
                                    team_pick = slot_type;
                                    if (slot_type == g_player_slots[0].team) {
                                      team_cand = 0;
                                      do {
                                        if (((team_cand != g_player_slots[0].team) &&
                                            (team_cand != tbl[1])) &&
                                           ((team_cand != tbl[2] &&
                                            (team_pick = team_cand, team_cand != tbl[3])))) break;
                                        team_cand = team_cand + 1;
                                        team_pick = slot_type;
                                      } while (team_cand < 6);
                                    }
                                    *slot_team_ptr = team_pick;
                                    slot_team_ptr[-1] = 1;
                                    slot_team_ptr = slot_team_ptr + 4;
                                    slot_type = active_slots;
                                  } while (active_slots < team_mode);
                                }
                                car_set_field_8(0,g_menu_selected_vehicle);
                                team_mode = g_event_type;
                                slot_type = g_career_row;
                              }
                              if (((token != 0x90000) && (token != 0xa0000)) &&
                                 (token != 0xd0000)) break;
                              g_team_mode = 0;
                              g_menu_multiplayer_kind = 0;
                              if (token == 0x90000) {
                                g_menu_powerup_set = 0;
                              }
                              else {
                                g_menu_powerup_set = 1;
                                if (token == 0xd0000) {
                                  g_menu_powerup_set = 2;
                                }
                              }
                              g_menu_69cc4c = 0;
                              g_player_slots[1].controller = -1;
                              g_player_slots[2].controller = -1;
                              g_player_slots[3].controller = -1;
                              g_player_slots[0].state = 0;
                              g_player_slots[1].state = -1;
                              g_player_slots[2].state = -1;
                              g_player_slots[3].state = -1;
                              team_mode = career_event_slot_table();
                              tbl = (int *)(team_mode + g_career_row * 0x30);
                              if (g_event_type == EVENT_CUP_4) {
                                tbl = tbl + 4;
                              }
                              else if (g_event_type == EVENT_CUP_5) {
                                tbl = tbl + 8;
                              }
                              team_mode = *tbl;
                              controller_skip = 0;
                              if (0 < team_mode) {
                                slot_team_ptr = &g_player_slots[1].team;
                                slot_type = 0;
                                tbl_next = tbl + 1;
                                do {
                                  active_slots = slot_type + 1;
                                  if (g_player_slots[0].controller == active_slots) {
                                    controller_skip = controller_skip + 1;
                                  }
                                  ((PlayerSlot *)(slot_team_ptr + -2))->controller =
                                       controller_skip + 1 + slot_type;
                                  race_setup_set_slot_type(active_slots,2);
                                  slot_type = *tbl_next;
                                  tbl_next = tbl_next + 1;
                                  team_pick = slot_type;
                                  if (slot_type == g_player_slots[0].team) {
                                    team_cand = 0;
                                    do {
                                      if ((((team_cand != g_player_slots[0].team) &&
                                           (team_cand != tbl[1])) && (team_cand != tbl[2])) &&
                                         (team_pick = team_cand, team_cand != tbl[3])) break;
                                      team_cand = team_cand + 1;
                                      team_pick = slot_type;
                                    } while (team_cand < 6);
                                  }
                                  *slot_team_ptr = team_pick;
                                  slot_team_ptr[-1] = 0;
                                  slot_team_ptr = slot_team_ptr + 4;
                                  slot_type = active_slots;
                                } while (active_slots < team_mode);
                              }
                            }
                            car_set_field_8(0,g_menu_selected_vehicle);
                            team_mode = g_event_type;
                            slot_type = g_career_row;
                          }
                          break;
                        }
                        game_mode = GAME_MODE_BEAT_THE_BOMB;
                        g_menu_powerup_set = 0;
                        g_player_slots[0].state = 0;
                      }
                      g_team_mode = 0;
                      g_menu_multiplayer_kind = 0;
                      g_menu_69cc4c = 0;
                      g_player_slots[1].controller = -1;
                      g_player_slots[2].controller = -1;
                      g_player_slots[3].controller = -1;
                      g_player_slots[1].state = -1;
                      g_player_slots[2].state = -1;
                      g_player_slots[3].state = -1;
                      race_setup_set_slot_type(0,1);
                      race_setup_set_slot_type(1,0);
                      race_setup_set_slot_type(2,0);
                      race_setup_set_slot_type(3,0);
                      team_mode = g_event_type;
                      slot_type = g_career_row;
                      break;
                    case 6:
                    case 7:
                    case 8:
                      active_slots = player_slots_count_active();
                      if (active_slots == 2) goto LAB_00447628;
                      break;
                    case 0xb:
                    case 0xc:
                    case 0xd:
                      if (team_mode == 0xd) {
                        slot_type = slot_type + 2 * MENU_SETUP_VARIANT_STRIDE;
                      }
                      else if (team_mode == 0xc) {
                        slot_type = slot_type + MENU_SETUP_VARIANT_STRIDE;
                      }
                      race_set_setup_index(slot_type);
                      menu_page_enter(MENU_PAGE_GAME_SETUP_SINGLE,0);
                      g_menu_69d6b4 = 1;
                      team_mode = g_event_type;
                      slot_type = g_career_row;
LAB_00447628:
                      g_menu_markers_enabled = 0;
                      break;
                    case 0x11:
                      menu_state_save();
                      team_mode = 1;
                      g_menu_update_69d684 = 1;
                      ui_menu_reset();
                      g_team_mode = 0;
                      g_menu_multiplayer_kind = 0;
                      g_menu_powerup_set = 0;
                      g_player_slots[1].controller = -1;
                      g_player_slots[2].controller = -1;
                      g_player_slots[3].controller = -1;
                      g_player_slots[0].state = 0;
                      g_player_slots[1].state = -1;
                      g_player_slots[2].state = -1;
                      g_player_slots[3].state = -1;
                      if (slot_type == 1) {
                        team_mode = 0;
                      }
                      else if (slot_type == 4) {
                        team_mode = 0;
                      }
                      else if (slot_type == 5) {
                        g_menu_markers_enabled = 1;
                        team_mode = 3;
                      }
                      controller_skip = 0;
                      g_training_lesson = slot_type;
                      if (team_mode != 0) {
                        tbl = &g_player_slots[1].team;
                        slot_type = 0;
                        do {
                          active_slots = slot_type + 1;
                          if (g_player_slots[0].controller == active_slots) {
                            controller_skip = controller_skip + 1;
                          }
                          ((PlayerSlot *)(tbl + -2))->controller = controller_skip + 1 + slot_type;
                          race_setup_set_slot_type(active_slots,2);
                          team_pick = slot_type;
                          if (slot_type == g_player_slots[0].team) {
                            team_cand = 0;
                            do {
                              if ((((team_cand != g_player_slots[0].team) && (team_cand != 0)) &&
                                  (team_cand != 1)) && (team_pick = team_cand, team_cand != 2)) break;
                              team_cand = team_cand + 1;
                              team_pick = slot_type;
                            } while (team_cand < 6);
                          }
                          *tbl = team_pick;
                          tbl[-1] = 1;
                          tbl = tbl + 4;
                          slot_type = active_slots;
                        } while (active_slots < team_mode);
                      }
                      car_set_field_8(0,g_menu_selected_vehicle);
                      race_set_phase(RACE_PHASE_SETUP);
                      team_mode = g_event_type;
                      slot_type = g_career_row;
                    }
                    active_slots = event_type_uses_standard_menu();
                    if (active_slots == 0) {
                      if (g_menu_event_column == MENU_EVENT_COL_G6_V2) {
                        slot_type = slot_type + 2 * MENU_SETUP_VARIANT_STRIDE;
                      }
                      else if (g_menu_event_column == MENU_EVENT_COL_G6_V1) {
                        slot_type = slot_type + MENU_SETUP_VARIANT_STRIDE;
                      }
                      race_set_setup_index(slot_type);
                      menu_page_enter(MENU_PAGE_GAME_SETUP_MULTI,0);
                      team_mode = 0;
                      slot_ptr = g_player_slots;
                      do {
                        slot_type = race_setup_get_slot_type(team_mode);
                        if (slot_type == 2) {
                          slot_ptr->controller = -1;
                        }
                        slot_ptr = slot_ptr + 1;
                        team_mode = team_mode + 1;
                      } while ((int)slot_ptr < (int)&g_ai_editor_mode);
                      g_menu_69d6b4 = 1;
                    }
                    else if (((team_mode != 0xb) && (team_mode != 0xc)) && (team_mode != 0xd)) {
                      if (g_menu_event_column == MENU_EVENT_COL_USE0_V2) {
                        race_set_setup_index(slot_type + 2 * MENU_SETUP_VARIANT_STRIDE);
                      }
                      else {
                        if (g_menu_event_column == MENU_EVENT_COL_USE0_V1) {
LAB_004476ee:
                          slot_type = slot_type + MENU_SETUP_VARIANT_STRIDE;
                        }
                        else if (g_menu_event_column != MENU_EVENT_COL_USE0) {
                          if (g_menu_event_column == MENU_EVENT_COL_USE5_A_V2) {
                            race_set_setup_index(slot_type + 2 * MENU_SETUP_VARIANT_STRIDE);
                            goto LAB_004478ac;
                          }
                          if (g_menu_event_column == MENU_EVENT_COL_USE5_A_V1) goto LAB_004476ee;
                        }
                        race_set_setup_index(slot_type);
                      }
LAB_004478ac:
                      menu_page_enter(MENU_PAGE_PAUSE,2);
                      g_menu_start_race_requested = 1;
                      g_menu_screen_id = 1;
                      g_menu_69d290 = 0;
                    }
                  }
                }
                else if (token < 0xff1f0001) {
                  if (token == MENU_ITEM_MSG_34) {
                    message_show_now(0x34,MENU_ITEM_MSG_34,2,0x2f,0x30,0);
                  }
                  else if (token == MENU_ITEM_START_RACE) {
                    menu_page_enter(MENU_PAGE_PAUSE,2);
                    g_menu_start_race_requested = 1;
                  }
                  else if (token == MENU_ITEM_PLAY_GAME_CHECKED) {
                    team_mode = event_type_uses_standard_menu();
                    if (team_mode == 0) {
                      team_mode = menu_validate_player_setup();
                      if (team_mode == 0x1000) {
                        team_mode = 0;
                        do {
                          *(undefined4 *)((int)&g_menu_controller_vehicle + team_mode) = 1;
                          *(undefined4 *)((int)&g_ui_69cb10 + team_mode) = 0;
                          team_mode = team_mode + 0xc;
                        } while (team_mode < 0x90);
                        team_mode = 0;
                        do {
                          car_set_field_8(team_mode,1);
                          team_mode = team_mode + 1;
                        } while (team_mode < 4);
                        if (g_team_mode == 0) {
                          menu_page_enter(MENU_PAGE_ABILITY_SELECT,0);
                        }
                        else {
                          menu_page_enter(MENU_PAGE_TEAM_SELECT,0);
                        }
                      }
                      else if (team_mode == 0) {
                        message_show_now(0x24f,0,1,0x2e,0,0);
                      }
                      else if (team_mode == 1) {
                        message_show_now(0x32,0,1,0x2e,0,0);
                      }
                      else if (team_mode == 2) {
                        message_show_now(0xa0,0,1,0x2e,0,0);
                      }
                      else if (team_mode == 3) {
                        message_show_now(0xa1,0,1,0x2e,0,0);
                      }
                    }
                    else {
                      if (g_menu_player0_team_choice == 0) {
                        g_player_slots[0].team = 0;
                      }
                      else {
                        g_player_slots[0].team = g_menu_player0_team_choice + -1;
                      }
                      menu_page_enter(MENU_PAGE_CHALLENGE_SELECT_A,0);
                      menu_options_defaults_reset();
                      g_menu_markers_enabled = 0;
                    }
                  }
                  else {
                    if (token != MENU_ITEM_MSG_33) goto LAB_00448159;
                    message_show_now(0x33,MENU_ITEM_MSG_33,2,0x2f,0x30,0);
                  }
                }
                else if (token == MENU_ITEM_MSG_35_A) {
                  message_show_now(0x35,MENU_ITEM_MSG_35_A,2,0x2f,0x30,0);
                }
                else {
                  if (token != MENU_ITEM_CHALLENGE_PLAY) goto LAB_00448159;
                  if ((((g_event_type == EVENT_MULTIPLAYER_6) || (g_event_type == EVENT_MULTIPLAYER_7)) ||
                      ((g_event_type == EVENT_MULTIPLAYER_8 || ((g_event_type == EVENT_TIME_TRIAL_0 || (g_event_type == EVENT_TIME_TRIAL_1)))))) ||
                     (g_event_type == EVENT_TIME_TRIAL_2)) {
                    if (g_menu_69cc48 == 1) {
                      g_menu_69cc48 = 2;
                    }
                    else if (g_menu_69cc48 == 4) goto LAB_00446b4b;
                  }
                  else {
                    g_menu_69cc48 = 4;
LAB_00446b4b:
                    if (g_menu_multiplayer_kind == 0) {
                      if (g_menu_career_race_active == 0) {
                        g_menu_69d6b4 = 0;
                        menu_page_enter(MENU_PAGE_CHALLENGE_SELECT_B,0);
                        if (g_menu_career_race_active == 2) {
                          g_event_type = EVENT_CUP_3;
                        }
                        g_menu_69cc48 = 5;
                      }
                      else {
                        team_mode = career_time_under_target();
                        if (team_mode == 0) {
                          message_show_now(0x13b,MENU_ITEM_MSG_13B,2,0x2f,0x30,0);
                          g_menu_career_race_active = 2;
                          g_menu_69cc48 = 5;
                        }
                        else {
                          message_show_now(0x13a,MENU_ITEM_MSG_13A,1,0x2e,0,0);
                          g_menu_career_race_active = 1;
                          g_menu_69cc48 = 5;
                        }
                      }
                    }
                    else if (g_tutorial_69d2a8 == 0) {
                      message_show_now(0x39,MENU_ITEM_CHALLENGE_PLAY,2,0x2f,0x30,0);
                      g_menu_69cc48 = 5;
                    }
                    else {
                      message_show_now(0xac,MENU_ITEM_MSG_AC,2,0x2f,0x30,0);
                      g_menu_69cc48 = 5;
                    }
                  }
                }
              }
              else {
                if (token < 0xff2e0001) {
                  if ((token == MENU_ITEM_TEAM_PLAY) || (token == MENU_ITEM_STANDARD_PLAY)) {
                    if (g_menu_player_count_index == 2) {
                      g_menu_multiplayer_kind = 1;
                    }
                    else {
                      g_menu_multiplayer_kind = (g_menu_player_count_index != 3) - 1 & 2;
                    }
                    g_career_row = 0;
                    g_menu_69d68c = 0;
                    g_menu_event_column = MENU_EVENT_COL_G6;
                    event_type_from_menu_choice();
                    g_team_mode = (uint)(token == MENU_ITEM_TEAM_PLAY);
                    color_ptr = &player_color_id;
                    do {
                      *color_ptr = 0xffffffff;
                      color_ptr = color_ptr + 3;
                    } while ((int)color_ptr < (int)&g_menu_update_69d0b8);
                    goto LAB_004477dc;
                  }
                  if (token == MENU_ITEM_TIME_TRIAL_STANDARD) goto LAB_00447720;
                }
                else {
                  if (token == MENU_ITEM_TIME_TRIAL_TEAM) {
LAB_00447720:
                    g_event_type = EVENT_TIME_TRIAL_0;
                    g_menu_60ad5c = 10;
                    g_menu_60ad60 = 3;
                    menu_text_format_and_draw(100,10);
                    menu_text_format_and_draw(0x65,g_menu_60ad60);
                    g_team_mode = (uint)(token == MENU_ITEM_TIME_TRIAL_TEAM);
                    g_menu_player_count_index = 0;
                    menu_page_enter(MENU_PAGE_CUSTOMISE_PLAY,0);
                    goto LAB_00448214;
                  }
                  if (token == MENU_ITEM_MULTIPLAYER_KIND_1) {
                    g_menu_multiplayer_kind = 1;
                    goto LAB_00447824;
                  }
                }
LAB_00448159:
                if ((g_menu_records[g_menu_current].page_id == MENU_PAGE_MAIN) &&
                   (g_menu_records[g_menu_current].selected == 0)) {
                  ui_input_reset();
                }
                menu_page_enter(token,0);
              }
            }
            else if (token < 0xff3b0001) {
              if (token == MENU_ITEM_OPEN_CHALLENGE_SELECT_A) {
                menu_page_enter(MENU_PAGE_CHALLENGE_SELECT_A,0);
                team_mode = 0;
                slot_ptr = g_player_slots;
                do {
                  if (-1 < slot_ptr->controller) {
                    car_set_field_8(team_mode,(&g_menu_controller_vehicle)[slot_ptr->controller * 3])
                    ;
                  }
                  slot_ptr = slot_ptr + 1;
                  team_mode = team_mode + 1;
                } while ((int)slot_ptr < (int)&g_ai_editor_mode);
              }
              else if (token < 0xff380001) {
                if (token == MENU_ITEM_OPEN_JOIN) {
                  menu_page_enter(MENU_PAGE_JOIN,0);
                }
                else if (token == MENU_ITEM_OPEN_XBOX_LIVE) {
                  menu_page_enter(MENU_PAGE_XBOX_LIVE,0);
                }
                else if (token != 0xff370000) goto LAB_00448159;
              }
              else if (token != 0xff390000) {
                if (token != MENU_ITEM_SETUP_INDEX_0) goto LAB_00448159;
                race_set_setup_index(0);
                goto LAB_004478ac;
              }
            }
            else {
              if (token < 0xff400001) {
                if (token == MENU_ITEM_SETUP_USE_5_COL0) {
                  race_setup_use_5();
                  g_menu_7ef7d0 = 0;
                  g_menu_event_column = MENU_EVENT_COL_USE5_A;
                  event_type_from_menu_choice();
                  team_mode = 0;
                  do {
                    car_set_field_8(team_mode,1);
                    team_mode = team_mode + 1;
                  } while (team_mode < 4);
                }
                else {
                  if (token == MENU_ITEM_RESET_PLAYER_SLOTS) {
                    team_mode = player_slots_reset_state();
                    if (team_mode == 0x1000) {
                      color_ptr = &g_menu_controller_vehicle;
                      do {
                        *color_ptr = 1;
                        color_ptr = color_ptr + 3;
                      } while ((int)color_ptr < (int)&g_hud_69cab8);
                      team_mode = 0;
                      do {
                        car_set_field_8(team_mode,0);
                        team_mode = team_mode + 1;
                      } while (team_mode < 4);
                      menu_page_enter(MENU_PAGE_ABILITY_SELECT,0);
                    }
                    else if (team_mode == 0) {
                      message_show_now(0xd9,0,1,0x2e,0,0);
                    }
                    else if (team_mode == 1) {
                      message_show_now(0xda,0,1,0x2e,0,0);
                    }
                    else if (team_mode == 2) {
                      message_show_now(0xdb,0,1,0x2e,0,0);
                    }
                    else if (team_mode == 3) {
                      message_show_now(0xdc,0,1,0x2e,0,0);
                    }
                    goto LAB_00448214;
                  }
                  if (token != MENU_ITEM_SETUP_USE_0) goto LAB_00448159;
                  race_setup_use_0();
                  g_menu_7ef7d0 = 0;
                  g_menu_event_column = MENU_EVENT_COL_USE0;
                  event_type_from_menu_choice();
                  team_mode = 0;
                  do {
                    car_set_field_8(team_mode,1);
                    team_mode = team_mode + 1;
                  } while (team_mode < 4);
                }
                goto LAB_004481e1;
              }
              if (token == MENU_ITEM_QUICK_RACE_START) {
                if (((g_event_type == EVENT_QUICK_RACE_11) || (g_event_type == EVENT_QUICK_RACE_12)) || (g_event_type == EVENT_QUICK_RACE_13)) {
                  /* quick race 11..13: count the teams that have players (up to 3); starting needs at least one else message 0x254 */
                  team_count = g_menu_player_team_choice != 0;
                  if (g_menu_player_team_choice_1 != 0) {
                    team_count = team_count + '\x01';
                  }
                  if (g_menu_player_team_choice_2 != 0) {
                    team_count = team_count + '\x01';
                  }
                  if (team_count != '\0') {
                    menu_page_enter(MENU_PAGE_PAUSE,2);
                    g_team_mode = g_menu_team_mode_choice ^ 1;
                    g_menu_start_race_requested = 1;
                    g_menu_screen_id = 1;
                    g_menu_69d290 = 0;
                    g_menu_multiplayer_kind = 0;
                    if (2 < g_menu_powerup_set) {
                      g_menu_powerup_set = 2;
                    }
                    g_menu_69cc4c = 0;
                    g_player_slots[0].state = 0;
                    g_player_slots[1].state = -1;
                    g_player_slots[2].state = -1;
                    g_player_slots[3].state = -1;
                    g_player_slots[1].controller = -1;
                    g_player_slots[2].controller = -1;
                    g_player_slots[3].controller = -1;
                    /* teams_left: number of selected teams still to place into slots 1..3 */
                    teams_left = (uint)(g_menu_player_team_choice != 0);
                    if (g_menu_player_team_choice_1 != 0) {
                      teams_left = teams_left + 1;
                    }
                    if (g_menu_player_team_choice_2 != 0) {
                      teams_left = teams_left + 1;
                    }
                    team_mode = 0;
                    slot_type = 0;
                    next_controller = 6;
                    if (teams_left != 0) {
                      active_slots = 1;
                      tbl = &g_player_slots[1].team;
                      do {
                        if ((&g_menu_player_team_choice)[slot_type] == 0) {
                          do {
                            tbl_next = &g_menu_player_team_choice_1 + slot_type;
                            slot_type = slot_type + 1;
                          } while (*tbl_next == 0);
                          if (*tbl_next != 0) goto LAB_00447d20;
                        }
                        else {
LAB_00447d20:
                          if (g_player_slots[0].controller == active_slots) {
                            slot_type = slot_type + 1;
                          }
                          ((PlayerSlot *)(tbl + -2))->controller = next_controller;
                          next_controller = next_controller + 1;
                          race_setup_set_slot_type(active_slots,2);
                          team_pick = (&g_menu_player_team_choice)[team_mode];
                          while (team_pick == 0) {
                            tbl_next = &g_menu_player_team_choice_1 + team_mode;
                            team_mode = team_mode + 1;
                            team_pick = *tbl_next;
                          }
                          *tbl = (&g_menu_player_team_choice)[team_mode] + -1;
                          tbl[-1] = 1;
                          team_mode = team_mode + 1;
                        }
                        active_slots = active_slots + 1;
                        tbl = tbl + 4;
                        teams_left = teams_left - 1;
                      } while (teams_left != 0);
                    }
                    goto LAB_00447bf5;
                  }
                  message_show_now(0x254,0,1,0x2e,0,0);
                }
                else {
                  g_team_mode = g_hud_8c86bc;
                  if (g_menu_game_mode_choice == 0) {
                    game_mode = GAME_MODE_STANDARD;
                  }
                  else if (g_menu_game_mode_choice == 1) {
                    game_mode = GAME_MODE_HOLD_THE_FLAG;
                  }
                  else if (g_menu_game_mode_choice == 2) {
                    game_mode = GAME_MODE_HARE_AND_HOUNDS;
                    g_team_mode = 1;
                  }
                  menu_page_enter(MENU_PAGE_PAUSE,2);
                  g_menu_start_race_requested = 1;
                  g_menu_screen_id = 1;
                  g_menu_69d290 = 0;
                  team_mode = 0;
                  slot_ptr = g_player_slots;
                  do {
                    if (slot_ptr->controller != -1) {
                      race_setup_set_slot_type(team_mode,1);
                    }
                    slot_type = race_setup_get_slot_type(team_mode);
                    if (slot_type == 2) {
                      slot_ptr->controller = -1;
                      race_setup_set_slot_type(team_mode,0);
                    }
                    slot_ptr = slot_ptr + 1;
                    team_mode = team_mode + 1;
                  } while ((int)slot_ptr < (int)&g_ai_editor_mode);
                  if ((g_menu_bot_skill_a != 0) && (g_menu_game_mode_choice != 1)) {
                    team_mode = 0;
                    slot_ptr = g_player_slots;
                    do {
                      if (slot_ptr->controller == -1) {
                        slot_type = 0;
                        do {
                          slot_clash = false;
                          slot_scan = g_player_slots;
                          do {
                            if ((int)&g_ai_editor_mode - 1 < (int)slot_scan) {
                              if (!slot_clash) {
                                slot_ptr->controller = slot_type;
                              }
                              break;
                            }
                            if (slot_scan->controller == slot_type) {
                              slot_type = slot_type + 1;
                              slot_clash = true;
                            }
                            slot_scan = slot_scan + 1;
                          } while (!slot_clash);
                          race_setup_set_slot_type(team_mode,2);
                        } while (slot_clash);
                        slot_type = 0;
                        do {
                          slot_clash = false;
                          tbl = &g_player_slots[0].team;
                          do {
                            if ((int)&g_ai_editor_view_origin_z - 1 < (int)tbl) {
                              if (!slot_clash) {
                                slot_ptr->team = slot_type;
                                goto LAB_00447be5;
                              }
                              break;
                            }
                            if (*tbl == slot_type) {
                              slot_type = slot_type + 1;
                              slot_clash = true;
                            }
                            tbl = tbl + 4;
                          } while (!slot_clash);
                        } while( true );
                      }
                      race_setup_set_slot_type(team_mode,1);
LAB_00447be5:
                      slot_ptr = slot_ptr + 1;
                      team_mode = team_mode + 1;
                    } while ((int)slot_ptr < (int)&g_ai_editor_mode);
LAB_00447bf5:
                    car_set_field_8(0,g_menu_selected_vehicle);
                  }
                }
              }
              else {
                if (token != MENU_ITEM_OPEN_SOUND) goto LAB_00448159;
                menu_page_enter(MENU_PAGE_SOUND,0);
              }
            }
          }
          else if (token < 0xff710001) {
            if (token == MENU_ITEM_TEAM_COLOUR_A) {
LAB_004477dc:
              g_menu_7ef7d0 = 0;
LAB_004477e2:
              menu_page_enter(MENU_PAGE_TEAM_COLOUR_SELECT,0);
              g_menu_69cc44 = 1;
            }
            else if (token < 0xff4b0001) {
              if (token == MENU_ITEM_EXIT_TO_MAIN) {
                gfx_set_89ddfc(0xff);
                g_ui_69d1f4 = 1;
                g_menu_69d1f0 = 0;
                gamesave_request_load_slot1();
                menu_page_enter(MENU_PAGE_MAIN,0);
                g_menu_7ef7d0 = 0;
              }
              else if (token < 0xff480001) {
                if (token == MENU_ITEM_OPEN_TRANSMISSION_INTERRUPTED_A) {
                  menu_page_enter(MENU_PAGE_TRANSMISSION_INTERRUPTED_A,0);
                }
                else if (token == MENU_ITEM_RESUME) {
                  menu_page_enter(MENU_PAGE_PAUSE,1);
                }
                else {
                  if (token != MENU_ITEM_SAVE_GAME) goto LAB_00448159;
                  gamesave_request_save_a();
                }
              }
              else if (token == MENU_ITEM_OPEN_CHALLENGE_SELECT_B) {
                g_menu_69d6b4 = 0;
                menu_page_enter(MENU_PAGE_CHALLENGE_SELECT_B,0);
                if (g_menu_career_race_active == 2) {
                  g_event_type = EVENT_CUP_3;
                }
              }
              else {
                if (token != MENU_ITEM_START_RACE_SETUP) goto LAB_00448159;
                menu_page_enter(MENU_PAGE_PAUSE,2);
                g_menu_start_race_requested = 1;
                race_set_phase(RACE_PHASE_SETUP);
                g_menu_69d1f0 = 4;
                g_ui_69d1f4 = 1;
                best_time_load_current();
                time_clear_current();
              }
            }
            else if (token < 0xff4e0001) {
              if (token == MENU_ITEM_MSG_35_B) {
                message_show_now(0x35,MENU_ITEM_MSG_35_B,2,0x2f,0x30,0);
              }
              else {
                if (token != MENU_ITEM_BACK_FROM_SAVE_LOAD) {
                  if (token != MENU_ITEM_SETUP_USE_5_COL_B) goto LAB_00448159;
                  race_setup_use_5();
                  g_menu_7ef7d0 = 0;
                  g_menu_event_column = MENU_EVENT_COL_USE5_B;
                  event_type_from_menu_choice();
                  team_mode = 0;
                  do {
                    car_set_field_8(team_mode,1);
                    team_mode = team_mode + 1;
                  } while (team_mode < 4);
                  g_team_mode = 1;
                  g_menu_multiplayer_kind = 0;
                  g_career_row = 0;
                  g_menu_69d68c = 0;
                  menu_page_enter(MENU_PAGE_TEAM_COLOUR_SELECT,0);
                  goto LAB_0044820b;
                }
                g_menu_69d1f0 = 0;
                if (g_menu_records[g_menu_current].page_id == MENU_PAGE_TRANSMISSION_INTERRUPTED_B) {
                  menu_page_enter(MENU_PAGE_MAIN,0);
                  g_menu_7ef7d0 = 0;
                  gamesave_request_load_slot1();
                  g_menu_update_69d2c0 = 0;
                  g_menu_update_69d2bc = 0;
                }
                else {
                  menu_page_enter(MENU_PAGE_PAUSE,1);
                  if (g_menu_update_69d2bc == 0) {
                    if (g_menu_update_69d2c0 != 0) {
                      gamesave_request_save_a();
                    }
                    g_menu_update_69d2c0 = 0;
                    g_menu_update_69d2bc = 0;
                  }
                  else {
                    loading_start_log();
                    g_menu_update_69d2c0 = 0;
                    g_menu_update_69d2bc = 0;
                  }
                }
              }
            }
            else if (token != 0xff4f0000) {
              if (token != MENU_ITEM_OPEN_OPTIONS) goto LAB_00448159;
              menu_page_enter(MENU_PAGE_OPTIONS,0);
            }
          }
          else if (token < 0xff830001) {
            if (token == MENU_ITEM_OPEN_AUTOSAVE) {
              menu_page_enter(MENU_PAGE_AUTOSAVE,0);
            }
            else if (token < 0xff800001) {
              if (token == MENU_ITEM_MSG_267) {
                message_show_now(0x267,MENU_ITEM_MSG_267,2,0x2f,0x30,0);
              }
              else if (token == MENU_ITEM_TEAM_COLOUR_B) {
                g_menu_7ef7d0 = 1;
                menu_page_enter(MENU_PAGE_TEAM_COLOUR_SELECT,0);
                g_menu_69cc44 = 1;
              }
              else {
                if (token != MENU_ITEM_OPEN_GAMMA) goto LAB_00448159;
                menu_page_enter(MENU_PAGE_GAMMA,0);
              }
            }
            else if (token == MENU_ITEM_DESELECT) {
              g_menu_records[g_menu_current].selected = 0;
              g_menu_select_flash = 0.0;
            }
            else {
              if (token != MENU_ITEM_OPEN_BONUS) goto LAB_00448159;
              team_mode = scene_get_log_64d898();
              if (team_mode == 0) {
                menu_page_enter(MENU_PAGE_BONUS_FEATURES,0);
              }
              else {
                menu_page_enter(MENU_PAGE_BONUS_CAMERAS,0);
              }
            }
          }
          else {
            if (token < 0xff860001) {
              if (token == MENU_ITEM_SETUP_USE_3) {
                race_setup_use_3();
                g_menu_7ef7d0 = 0;
                g_menu_event_column = MENU_EVENT_COL_USE3;
                event_type_from_menu_choice();
                team_mode = 0;
                do {
                  car_set_field_8(team_mode,1);
                  team_mode = team_mode + 1;
                } while (team_mode < 4);
              }
              else {
                if (token == MENU_ITEM_LOAD_COURSE) {
                  race_load_course_0042fad0();
                  goto LAB_00448214;
                }
                if (token != MENU_ITEM_SETUP_USE_2) goto LAB_00448159;
                race_setup_use_2();
                g_menu_7ef7d0 = 0;
                g_menu_event_column = MENU_EVENT_COL_USE2;
                event_type_from_menu_choice();
                team_mode = 0;
                do {
                  car_set_field_8(team_mode,1);
                  team_mode = team_mode + 1;
                } while (team_mode < 4);
              }
            }
            else if (token == MENU_ITEM_SETUP_USE_4) {
              race_setup_use_4();
              g_menu_7ef7d0 = 0;
              g_menu_event_column = MENU_EVENT_COL_USE4;
              event_type_from_menu_choice();
              team_mode = 0;
              do {
                car_set_field_8(team_mode,1);
                team_mode = team_mode + 1;
              } while (team_mode < 4);
            }
            else {
              if (token != MENU_ITEM_SETUP_USE_1) goto LAB_00448159;
              race_setup_use_1();
              g_menu_7ef7d0 = 0;
              g_menu_event_column = MENU_EVENT_COL_USE1;
              event_type_from_menu_choice();
              team_mode = 0;
              do {
                car_set_field_8(team_mode,1);
                team_mode = team_mode + 1;
              } while (team_mode < 4);
            }
LAB_004481e1:
            g_menu_69d68c = 0;
            g_career_row = 0;
            g_menu_multiplayer_kind = 0;
            g_team_mode = 0;
            menu_page_enter(MENU_PAGE_TEAM_COLOUR_SELECT,0);
LAB_0044820b:
            g_menu_player0_team_choice = 1;
            g_menu_69cc44 = 0;
          }
        }
      }
LAB_00448214:
      /* back: restore what the leaving page edited (option sliders / flags / rumble / widescreen saved copies), per-page cleanup, then return to the pause page */
      if (((back_enabled) && (team_mode = menu_back_pressed_any(), team_mode != 0)) &&
         (g_menu_select_pressed == 0)) {
        team_mode = g_menu_records[g_menu_current].page_id;
        g_menu_select_flash = 1.0;
        g_message_69d6ac = 1;
        if (team_mode == MENU_PAGE_BONUS_FEATURES) {
          g_game_state.option_flags[1] = GS(2884);
          g_game_state.option_flags[2] = GS(2888);
          g_game_state.option_flags[3] = GS(2892);
          g_game_state.option_flags[4] = GS(2896);
        }
        if (team_mode == MENU_PAGE_BONUS_CAMERAS) {
          g_game_state.option_flags[1] = GS(2884);
          g_game_state.option_flags[2] = GS(2888);
          g_game_state.option_flags[3] = GS(2892);
          g_game_state.option_flags[4] = GS(2896);
        }
        switch(g_menu_records[g_menu_current + -1].page_id) {
        case MENU_PAGE_PAUSE:
        case MENU_PAGE_RACE_RESULTS:
          if (team_mode != MENU_PAGE_OPTIONS) {
            g_menu_records[1].item_list = (char *)PTR_g_menu_page_01_game_type_select_0060acac;
            g_menu_records[1].page_id = 1;
            g_menu_records[1].entry_count = 5;
            g_menu_records[1].selected = 0;
LAB_0044847c:
            g_menu_7ef7d0 = 0;
            ai_bots_clear_stats();
            player_stats_clear_b();
            round_stats_reset();
            g_menu_69cc4c = 1;
            ui_input_mode_menu();
LAB_0044849c:
            g_menu_69d1f0 = 0;
          }
          break;
        case MENU_PAGE_MAIN:
          ui_input_mode_menu();
          g_menu_69cc4c = 1;
          break;
        case MENU_PAGE_SINGLE_PLAYER:
          g_menu_powerup_set = 1;
          if (g_menu_screen_id == MENU_PAGE_SINGLE_PLAYER) goto LAB_0044849c;
          break;
        case MENU_PAGE_MULTIPLAYER:
        case MENU_PAGE_TEAMS:
          race_setup_use_5();
          if (((g_menu_screen_id == MENU_PAGE_TEAMS) || (g_menu_screen_id == MENU_PAGE_SINGLE_PLAYER)) || (g_menu_screen_id == MENU_PAGE_VIBRATION))
          goto LAB_0044849c;
          break;
        case MENU_PAGE_TEAM_COLOUR_SELECT:
          team_mode = event_type_uses_standard_menu();
          if (team_mode == 0) {
            ui_input_mode_menu();
          }
          break;
        case MENU_PAGE_OPTIONS:
          if (team_mode == MENU_PAGE_SOUND) {
            g_game_state.option_sliders[0] = GS(2836);
            g_game_state.option_sliders[4] = GS(2852);
            g_game_state.option_sliders[3] = GS(2848);
            g_game_state.option_sliders[2] = GS(2844);
            g_game_state.option_sliders[1] = GS(2840);
          }
          if (team_mode == MENU_PAGE_VIBRATION) {
            dst_word = &g_player_inputs[0].rumble_enabled;
            src_word = &g_ai_8c8740;
            do {
              *dst_word = *src_word;
              src_word = src_word + 1;
              dst_word = dst_word + 0x13;
            } while ((int)src_word < (int)&g_menu_update_8c8770);
          }
          if ((g_menu_screen_id == MENU_PAGE_AUTOSAVE) || (g_menu_screen_id == MENU_PAGE_ARCADE_MODES)) goto LAB_0044849c;
          break;
        case MENU_PAGE_ABILITY_SELECT:
          g_menu_69d6b4 = 0;
          break;
        case MENU_PAGE_OPTIONS_INGAME:
          if (team_mode != MENU_PAGE_OPTIONS) {
            g_menu_records[2].item_list = (char *)PTR_g_menu_page_01_game_type_select_0060acac;
            g_menu_records[2].page_id = 1;
            g_menu_records[2].entry_count = 5;
            g_menu_records[2].selected = 0;
            goto LAB_0044847c;
          }
          break;
        case MENU_PAGE_SCREEN:
          if (team_mode == MENU_PAGE_GAME_OPTIONS) {
            GS(2792) = GS(2800);
            GS(2796) = GS(2804);
          }
          if (team_mode == MENU_PAGE_WIDESCREEN) {
            camera_fov_factor_set_wide(g_ai_8c86cc);
          }
          break;
        case MENU_PAGE_LOAD_SAVE:
          if (team_mode == MENU_PAGE_AUTOSAVE) {
            gamesave_set_autosave_enabled(g_ai_8c86b8);
          }
        }
        menu_page_enter(MENU_PAGE_PAUSE,1);
      }
      /* up/down: move the selection; up steps the career row on the challenge pages (6, 7) when g_menu_8c8090 == 0, down calls menu_career_row_next without a page test (it may test the page itself, inferred) */
      if ((up_enabled) && (team_mode = menu_up_pressed_any(), team_mode != 0)) {
        g_menu_69d6b0 = 1;
        menu_selection_step(0xffffffff);
        if ((5 < g_menu_records[g_menu_current].page_id) &&
           ((g_menu_records[g_menu_current].page_id < 8 && (g_menu_8c8090 == 0)))) {
          career_row_previous();
        }
      }
      if (((down_enabled) && (team_mode = menu_down_pressed_any(), team_mode != 0)) &&
         (g_menu_69d6b0 = 1, g_menu_8c8090 == 0)) {
        menu_career_row_next();
      }
      /* pause button: back to the race with the start-race request */
      if ((aux != 0) && (team_mode = menu_pause_pressed_any(), team_mode != 0)) {
        g_menu_select_pressed = 1;
        menu_page_enter(MENU_PAGE_PAUSE,2);
        g_menu_start_race_requested = 1;
      }
      /* left: decrement the value of the selected entry, per page (wraps; sliders step by 0.01 and clamp to 0) */
      if ((left_enabled) &&
         (slot_type = menu_left_pressed_any(), aux = g_career_row, team_mode = g_menu_69cc44,
         slot_type != 0)) {
        g_menu_69d6b0 = 1;
        switch(g_menu_records[g_menu_current].page_id) {
        case MENU_PAGE_TEAM_COLOUR_SELECT:
          if (g_menu_update_69d2d4 != 0) {
            g_menu_69d6b0 = 0;
          }
          break;
        case MENU_PAGE_CHALLENGE_SELECT_A:
        case MENU_PAGE_CHALLENGE_SELECT_B:
          if (g_menu_8c8090 == 0) {
            if ((((((g_menu_event_column == MENU_EVENT_COL_USE0) || (g_menu_event_column == MENU_EVENT_COL_USE2)) ||
                  (g_menu_event_column == MENU_EVENT_COL_USE3)) ||
                 ((g_menu_event_column == MENU_EVENT_COL_USE4 || (g_menu_event_column == MENU_EVENT_COL_USE1)))) ||
                (g_menu_event_column == MENU_EVENT_COL_G6)) ||
               ((g_menu_event_column == MENU_EVENT_COL_USE5_A || (g_menu_event_column == MENU_EVENT_COL_USE5_B)))) {
              g_menu_69d6b0 = 0;
            }
            else if ((((g_menu_event_column == MENU_EVENT_COL_USE0_V1) || (g_menu_event_column == MENU_EVENT_COL_G6_V1)) ||
                     (g_menu_event_column == MENU_EVENT_COL_USE5_A_V1)) ||
                    (((g_menu_event_column == MENU_EVENT_COL_USE5_B_V1 || (g_menu_event_column == MENU_EVENT_COL_USE0_V2)) ||
                     ((g_menu_event_column == MENU_EVENT_COL_G6_V2 ||
                      ((g_menu_event_column == MENU_EVENT_COL_USE5_A_V2 ||
                       (aux = g_menu_event_column, g_menu_event_column == MENU_EVENT_COL_USE5_B_V2)))))))) {
              g_menu_event_column = g_menu_event_column + -1;
            }
            else {
              do {
                active_slots = (-(uint)(team_mode != 0) & 3) + 3;
                slot_type = aux + -1;
                if (aux + -1 < active_slots) {
                  g_menu_69d6b0 = 0;
                  slot_type = active_slots;
                }
                g_menu_event_column = slot_type;
                active_slots = menu_event_column_progress_unless_special();
                aux = slot_type;
              } while (active_slots == 0);
            }
            event_type_from_menu_choice();
          }
          break;
        /* entry 0 player count index (0..3), 1/2 round count values; text ids 0x6c/0x65 are the singular forms */
        case MENU_PAGE_CUSTOMISE_PLAY:
          team_mode = g_menu_records[g_menu_current].selected;
          if ((team_mode == 0) &&
             (g_menu_player_count_index = g_menu_player_count_index + -1,
             g_menu_player_count_index < 0)) {
            g_menu_player_count_index = 3;
          }
          if ((team_mode == 1) && (1 < g_menu_60ad5c)) {
            g_menu_60ad5c = g_menu_60ad5c + -1;
          }
          if ((team_mode == 2) && (1 < g_menu_60ad60)) {
            g_menu_60ad60 = g_menu_60ad60 + -1;
          }
          if (g_menu_60ad5c == 1) {
            text_id = 0x6c;
            team_mode = 1;
          }
          else {
            text_id = 100;
            team_mode = g_menu_60ad5c;
          }
          menu_text_format_and_draw(text_id,team_mode);
          if (g_menu_60ad60 == 1) {
            menu_text_format_and_draw(0x6d,1);
          }
          else {
            menu_text_format_and_draw(0x65,g_menu_60ad60);
          }
          break;
        /* entries: 0 g_menu_selected_vehicle (0..3; the page header calls the entry ability, so the field may be an ability index, inferred), 1 markers on/off, 2 race length (0..2) */
        case MENU_PAGE_CUSTOM_ABILITIES:
          team_mode = g_menu_records[g_menu_current].selected;
          if ((team_mode == 0) &&
             (g_menu_selected_vehicle = g_menu_selected_vehicle + -1, g_menu_selected_vehicle < 0))
          {
            g_menu_selected_vehicle = 3;
          }
          if (team_mode == 1) {
            g_menu_markers_enabled = g_menu_markers_enabled ^ 1;
          }
          if ((team_mode == 2) &&
             (g_menu_race_length_choice = g_menu_race_length_choice + -1,
             g_menu_race_length_choice < 0)) {
            g_menu_race_length_choice = 2;
          }
          break;
        /* entries: 1 power-up set (0..2), 2 race length, 3 bot skill (0..4), 4 course index (0..0xe, skips locked courses), 5 markers, 6 game mode */
        case MENU_PAGE_GAME_SETUP_MULTI:
          team_mode = g_menu_records[g_menu_current].selected;
          if ((team_mode == 1) &&
             (g_menu_powerup_set = g_menu_powerup_set + -1, g_menu_powerup_set < 0)) {
            g_menu_powerup_set = 2;
          }
          if ((team_mode == 2) &&
             (g_menu_race_length_choice = g_menu_race_length_choice + -1,
             g_menu_race_length_choice < 0)) {
            g_menu_race_length_choice = 2;
          }
          if ((team_mode == 3) &&
             (g_menu_bot_skill_a = g_menu_bot_skill_a + -1, g_menu_bot_skill_a < 0)) {
            g_menu_bot_skill_a = 4;
          }
          slot_type = g_menu_event_course_index;
          if (team_mode == 4) {
            do {
              slot_type = slot_type + -1;
              if (slot_type < 0) {
                slot_type = 0xe;
              }
              active_slots = career_event_set_progress(slot_type);
            } while (active_slots == 0);
            g_menu_event_course_index = slot_type;
            (&g_menu_609a58)[aux] = slot_type;
          }
          if (team_mode == 5) {
            g_menu_markers_enabled = g_menu_markers_enabled ^ 1;
          }
          if (team_mode == 6) {
            g_menu_game_mode_choice = g_menu_game_mode_choice + -1;
            if (g_menu_game_mode_choice < 0) {
              if (g_game_state.progress[aux * 0x12 + 10] != 0) {
                g_menu_game_mode_choice = 2;
LAB_004488f8:
                team_mode = is_team_mode();
                if (team_mode == 0) break;
              }
              g_menu_game_mode_choice = 1;
            }
            else if (g_menu_game_mode_choice == 2) goto LAB_004488f8;
          }
          break;
        case MENU_PAGE_SOUND:
          team_mode = g_menu_records[g_menu_current].selected;
          if ((team_mode == 0) &&
             (SLF(1) = SLF(1) - k_f_0_01,
             SLF(1) < k_f_0)) {
            g_game_state.option_sliders[1] = 0;
          }
          if ((team_mode == 1) &&
             (SLF(0) = SLF(0) - k_f_0_01,
             SLF(0) < k_f_0)) {
            g_game_state.option_sliders[0] = 0;
          }
          if ((team_mode == 2) &&
             (SLF(2) = SLF(2) - k_f_0_01,
             SLF(2) < k_f_0)) {
            g_game_state.option_sliders[2] = 0;
          }
          if ((team_mode == 3) &&
             (g_game_state.option_sliders[4] = g_game_state.option_sliders[4] - 1,
             (int)g_game_state.option_sliders[4] < 0)) {
            g_game_state.option_sliders[4] = 2;
          }
          break;
        /* entries: 1 power-up set, 2 bot skill (1..4), 3 course index, 4..6 team choice of players 0..2 (0..6), 7 team mode toggle */
        case MENU_PAGE_GAME_SETUP_SINGLE:
          team_mode = g_menu_records[g_menu_current].selected;
          if ((team_mode == 1) &&
             (g_menu_powerup_set = g_menu_powerup_set + -1, g_menu_powerup_set < 0)) {
            g_menu_powerup_set = 2;
          }
          if ((team_mode == 2) &&
             (g_menu_bot_skill_b = g_menu_bot_skill_b + -1, g_menu_bot_skill_b < 1)) {
            g_menu_bot_skill_b = 4;
          }
          aux = g_menu_event_course_index;
          if (team_mode == 3) {
            do {
              aux = aux + -1;
              if (aux < 0) {
                aux = 0xe;
              }
              slot_type = career_event_set_progress(aux);
            } while (slot_type == 0);
            g_menu_event_course_index = aux;
            (&g_menu_609a58)[g_career_row] = aux;
          }
          if (team_mode == 4) {
            g_menu_player_team_choice = g_menu_player_team_choice + -1;
            if (g_menu_player_team_choice < 0) {
              g_menu_player_team_choice = 6;
            }
            menu_team_choice_step_r(0,0xffffffff,1);
          }
          if (team_mode == 5) {
            g_menu_player_team_choice_1 = g_menu_player_team_choice_1 + -1;
            if (g_menu_player_team_choice_1 < 0) {
              g_menu_player_team_choice_1 = 6;
            }
            menu_team_choice_step_r(1,0xffffffff,1);
          }
          if (team_mode == 6) {
            g_menu_player_team_choice_2 = g_menu_player_team_choice_2 + -1;
            if (g_menu_player_team_choice_2 < 0) {
              g_menu_player_team_choice_2 = 6;
            }
            menu_team_choice_step_r(2,0xffffffff,1);
          }
          if (team_mode == 7) {
            g_menu_team_mode_choice = g_menu_team_mode_choice ^ 1;
          }
          break;
        /* toggle rumble of the controller behind the selected row */
        case MENU_PAGE_VIBRATION:
          team_mode = 0;
          do {
            if (g_menu_records[g_menu_current].selected == team_mode) {
              aux = (&g_menu_8c8080)[team_mode];
              g_player_inputs[aux].rumble_enabled = g_player_inputs[aux].rumble_enabled ^ 1;
              input_player_rumble_set(aux);
            }
            team_mode = team_mode + 1;
          } while (team_mode < 4);
          break;
        case MENU_PAGE_GAMMA:
          if ((g_menu_records[g_menu_current].selected == 0) &&
             (g_menu_69cfc4 = g_menu_69cfc4 - k_f_0_01, g_menu_69cfc4 < k_f_0)) {
            g_menu_69cfc4 = 0.0;
          }
          break;
        /* bonus features: flags 1..4 behave as radio buttons */
        case MENU_PAGE_BONUS_FEATURES:
          team_mode = g_menu_records[g_menu_current].selected;
          g_game_state.option_flags[team_mode + 1] = g_game_state.option_flags[team_mode + 1] ^ 1;
          aux = 0;
          do {
            if ((aux != team_mode) && (g_game_state.option_flags[team_mode + 1] == 1)) {
              g_game_state.option_flags[aux + 1] = 0;
            }
            aux = aux + 1;
          } while (aux < 4);
          break;
        case MENU_PAGE_AUTOSAVE:
          g_menu_69d6b0 = 0;
          token = nop_b();
          gamesave_set_autosave_enabled(token ^ 1);
          g_menu_69d6b0 = 1;
          break;
        /* bonus cameras: flags 2..3 are mutually exclusive */
        case MENU_PAGE_BONUS_CAMERAS:
          team_mode = g_menu_records[g_menu_current].selected;
          g_game_state.option_flags[team_mode + 2] = g_game_state.option_flags[team_mode + 2] ^ 1;
          aux = 0;
          do {
            if ((aux != team_mode) && (g_game_state.option_flags[team_mode + 2] == 1)) {
              g_game_state.option_flags[aux + 2] = 0;
            }
            aux = aux + 1;
          } while (aux < 2);
          break;
        case MENU_PAGE_WIDESCREEN:
          token = game_state_get_2812();
          camera_fov_factor_set_wide(token ^ 1);
          g_menu_69d6b0 = 1;
        }
      }
      /* right: mirror of left, increments (sliders clamp to 1.0) */
      if ((right_enabled) &&
         (slot_type = menu_right_pressed_any(), aux = g_menu_event_column,
         team_mode = g_career_row, slot_type != 0)) {
        g_menu_69d6b0 = 1;
        switch(g_menu_records[g_menu_current].page_id) {
        case MENU_PAGE_TEAM_COLOUR_SELECT:
          if (g_menu_update_69d2d4 != 0) {
            g_menu_69d6b0 = 0;
          }
          break;
        case MENU_PAGE_CHALLENGE_SELECT_A:
        case MENU_PAGE_CHALLENGE_SELECT_B:
          if (g_menu_8c8090 == 0) {
            if ((((g_menu_event_column == MENU_EVENT_COL_USE0_V2) || (g_menu_event_column == MENU_EVENT_COL_G6_V2)) ||
                (g_menu_event_column == MENU_EVENT_COL_USE5_A_V2)) ||
               (((g_menu_event_column == MENU_EVENT_COL_USE5_B_V2 || (g_menu_event_column == MENU_EVENT_COL_USE1)) ||
                ((g_menu_event_column == MENU_EVENT_COL_USE2 ||
                 ((g_menu_event_column == MENU_EVENT_COL_USE3 || (g_menu_event_column == MENU_EVENT_COL_USE4)))))))) {
              g_menu_69d6b0 = 0;
            }
            else if ((((g_menu_event_column == MENU_EVENT_COL_USE0_V1) || (g_menu_event_column == MENU_EVENT_COL_G6_V1)) ||
                     (g_menu_event_column == MENU_EVENT_COL_USE5_A_V1)) || (g_menu_event_column == MENU_EVENT_COL_USE5_B_V1)) {
              if (((g_event_type == EVENT_CUP_3) || (g_event_type == EVENT_CUP_4)) ||
                 ((g_event_type == EVENT_CUP_5 || ((&g_menu_608428)[g_career_row * 0xc] != -1))))
              goto LAB_0044907d;
            }
            else if ((((g_menu_event_column == MENU_EVENT_COL_USE0) || (g_menu_event_column == MENU_EVENT_COL_G6)) ||
                     (g_menu_event_column == MENU_EVENT_COL_USE5_A)) ||
                    (team_mode = g_menu_event_column, g_menu_event_column == MENU_EVENT_COL_USE5_B)) {
LAB_0044907d:
              g_menu_event_column = g_menu_event_column + 1;
            }
            else {
              do {
                slot_type = team_mode + 1;
                if (team_mode + 1 == 0x12) {
                  g_menu_69d6b0 = 0;
                  slot_type = aux;
                }
                g_menu_event_column = slot_type;
                active_slots = menu_event_column_progress_unless_special();
                team_mode = slot_type;
              } while (active_slots == 0);
            }
            event_type_from_menu_choice();
          }
          break;
        /* entry 0 player count index (0..3), 1/2 round count values; text ids 0x6c/0x65 are the singular forms */
        case MENU_PAGE_CUSTOMISE_PLAY:
          team_mode = g_menu_records[g_menu_current].selected;
          if ((team_mode == 0) &&
             (g_menu_player_count_index = g_menu_player_count_index + 1,
             g_menu_player_count_index == 4)) {
            g_menu_player_count_index = 0;
          }
          if (team_mode == 1) {
            g_menu_60ad5c = g_menu_60ad5c + 1;
          }
          if (team_mode == 2) {
            g_menu_60ad60 = g_menu_60ad60 + 1;
          }
          if (g_menu_60ad5c == 1) {
            text_id = 0x6c;
            team_mode = 1;
          }
          else {
            text_id = 100;
            team_mode = g_menu_60ad5c;
          }
          menu_text_format_and_draw(text_id,team_mode);
          if (g_menu_60ad60 == 1) {
            menu_text_format_and_draw(0x6d,1);
          }
          else {
            menu_text_format_and_draw(0x65,g_menu_60ad60);
          }
          break;
        /* entries: 0 g_menu_selected_vehicle (0..3; the page header calls the entry ability, so the field may be an ability index, inferred), 1 markers on/off, 2 race length (0..2) */
        case MENU_PAGE_CUSTOM_ABILITIES:
          team_mode = g_menu_records[g_menu_current].selected;
          if ((team_mode == 0) &&
             (g_menu_selected_vehicle = g_menu_selected_vehicle + 1, g_menu_selected_vehicle == 4))
          {
            g_menu_selected_vehicle = 0;
          }
          if (team_mode == 1) {
            g_menu_markers_enabled = g_menu_markers_enabled ^ 1;
          }
          if ((team_mode == 2) &&
             (g_menu_race_length_choice = g_menu_race_length_choice + 1,
             g_menu_race_length_choice == 3)) {
            g_menu_race_length_choice = 0;
          }
          break;
        /* entries: 1 power-up set (0..2), 2 race length, 3 bot skill (0..4), 4 course index (0..0xe, skips locked courses), 5 markers, 6 game mode */
        case MENU_PAGE_GAME_SETUP_MULTI:
          aux = g_menu_records[g_menu_current].selected;
          if ((aux == 1) && (g_menu_powerup_set = g_menu_powerup_set + 1, 2 < g_menu_powerup_set))
          {
            g_menu_powerup_set = 0;
          }
          if ((aux == 2) &&
             (g_menu_race_length_choice = g_menu_race_length_choice + 1,
             g_menu_race_length_choice == 3)) {
            g_menu_race_length_choice = 0;
          }
          if ((aux == 3) && (g_menu_bot_skill_a = g_menu_bot_skill_a + 1, 4 < g_menu_bot_skill_a))
          {
            g_menu_bot_skill_a = 0;
          }
          slot_type = g_menu_event_course_index;
          if (aux == 4) {
            do {
              slot_type = slot_type + 1;
              if (0xe < slot_type) {
                slot_type = 0;
              }
              active_slots = career_event_set_progress(slot_type);
            } while (active_slots == 0);
            g_menu_event_course_index = slot_type;
            (&g_menu_609a58)[team_mode] = slot_type;
          }
          if (aux == 5) {
            g_menu_markers_enabled = g_menu_markers_enabled ^ 1;
          }
          if (aux == 6) {
            g_menu_game_mode_choice = g_menu_game_mode_choice + 1;
            if (g_menu_game_mode_choice == (g_game_state.progress[team_mode * 0x12 + 10] != 0) + 2)
            {
              g_menu_game_mode_choice = 0;
            }
            else if ((g_menu_game_mode_choice == 2) && (team_mode = is_team_mode(), team_mode != 0))
            {
              g_menu_game_mode_choice = 0;
            }
          }
          break;
        case MENU_PAGE_SOUND:
          team_mode = g_menu_records[g_menu_current].selected;
          if ((team_mode == 0) &&
             (SLF(1) = SLF(1) + k_f_0_01,
             float_1_0 < SLF(1))) {
            g_game_state.option_sliders[1] = 0x3f800000;
          }
          if ((team_mode == 1) &&
             (SLF(0) = SLF(0) + k_f_0_01,
             float_1_0 < SLF(0))) {
            g_game_state.option_sliders[0] = 0x3f800000;
          }
          if ((team_mode == 2) &&
             (SLF(2) = SLF(2) + k_f_0_01,
             float_1_0 < SLF(2))) {
            g_game_state.option_sliders[2] = 0x3f800000;
          }
          if ((team_mode == 3) &&
             (g_game_state.option_sliders[4] = g_game_state.option_sliders[4] + 1,
             g_game_state.option_sliders[4] == 3)) {
            g_game_state.option_sliders[4] = 0;
          }
          break;
        /* entries: 1 power-up set, 2 bot skill (1..4), 3 course index, 4..6 team choice of players 0..2 (0..6), 7 team mode toggle */
        case MENU_PAGE_GAME_SETUP_SINGLE:
          team_mode = g_menu_records[g_menu_current].selected;
          if ((team_mode == 1) &&
             (g_menu_powerup_set = g_menu_powerup_set + 1, 2 < g_menu_powerup_set)) {
            g_menu_powerup_set = 0;
          }
          if ((team_mode == 2) &&
             (g_menu_bot_skill_b = g_menu_bot_skill_b + 1, 4 < g_menu_bot_skill_b)) {
            g_menu_bot_skill_b = 1;
          }
          aux = g_menu_event_course_index;
          if (team_mode == 3) {
            do {
              aux = aux + 1;
              if (0xe < aux) {
                aux = 0;
              }
              slot_type = career_event_set_progress(aux);
            } while (slot_type == 0);
            g_menu_event_course_index = aux;
            (&g_menu_609a58)[g_career_row] = aux;
          }
          if (team_mode == 4) {
            g_menu_player_team_choice = g_menu_player_team_choice + 1;
            if (6 < g_menu_player_team_choice) {
              g_menu_player_team_choice = 0;
            }
            menu_team_choice_step_r(0,1,1);
          }
          if (team_mode == 5) {
            g_menu_player_team_choice_1 = g_menu_player_team_choice_1 + 1;
            if (6 < g_menu_player_team_choice_1) {
              g_menu_player_team_choice_1 = 0;
            }
            menu_team_choice_step_r(1,1,1);
          }
          if (team_mode == 6) {
            g_menu_player_team_choice_2 = g_menu_player_team_choice_2 + 1;
            if (6 < g_menu_player_team_choice_2) {
              g_menu_player_team_choice_2 = 0;
            }
            menu_team_choice_step_r(2,1,1);
          }
          if (team_mode == 7) {
            g_menu_team_mode_choice = g_menu_team_mode_choice ^ 1;
          }
          break;
        /* toggle rumble of the controller behind the selected row */
        case MENU_PAGE_VIBRATION:
          team_mode = 0;
          do {
            if (g_menu_records[g_menu_current].selected == team_mode) {
              aux = (&g_menu_8c8080)[team_mode];
              g_player_inputs[aux].rumble_enabled = g_player_inputs[aux].rumble_enabled ^ 1;
              input_player_rumble_set(aux);
            }
            team_mode = team_mode + 1;
          } while (team_mode < 4);
          break;
        case MENU_PAGE_GAMMA:
          if ((g_menu_records[g_menu_current].selected == 0) &&
             (g_menu_69cfc4 = g_menu_69cfc4 + k_f_0_01, float_1_0 < g_menu_69cfc4)) {
            g_menu_69cfc4 = 1.0;
          }
          break;
        /* bonus features: flags 1..4 behave as radio buttons */
        case MENU_PAGE_BONUS_FEATURES:
          team_mode = g_menu_records[g_menu_current].selected;
          g_game_state.option_flags[team_mode + 1] = g_game_state.option_flags[team_mode + 1] ^ 1;
          aux = 0;
          do {
            if ((aux != team_mode) && (g_game_state.option_flags[team_mode + 1] == 1)) {
              g_game_state.option_flags[aux + 1] = 0;
            }
            aux = aux + 1;
          } while (aux < 4);
          break;
        case MENU_PAGE_AUTOSAVE:
          g_menu_69d6b0 = 0;
          token = nop_b();
          gamesave_set_autosave_enabled(token ^ 1);
          g_menu_69d6b0 = 1;
          break;
        /* bonus cameras: flags 2..3 are mutually exclusive */
        case MENU_PAGE_BONUS_CAMERAS:
          team_mode = g_menu_records[g_menu_current].selected;
          g_game_state.option_flags[team_mode + 2] = g_game_state.option_flags[team_mode + 2] ^ 1;
          aux = 0;
          do {
            if ((aux != team_mode) && (g_game_state.option_flags[team_mode + 2] == 1)) {
              g_game_state.option_flags[aux + 2] = 0;
            }
            aux = aux + 1;
          } while (aux < 2);
          break;
        case MENU_PAGE_WIDESCREEN:
          token = game_state_get_2812();
          camera_fov_factor_set_wide(token ^ 1);
          g_menu_69d6b0 = 1;
        }
      }
      if (((next_enabled) && (team_mode = menu_input_next_pressed(), team_mode != 0)) &&
         (g_menu_records[g_menu_current].page_id == MENU_PAGE_RACE_RESULTS)) {
        g_tutorial_reset_requested = 1;
      }
    }
  }
  menu_page_update_entry_states(g_menu_current + -1);
LAB_004490d6:
  /* frontend fade: state 4 raises the fade alpha (alpha += 1000/s up to 0xff, then state 5 once the item animation is done); state 5 fades out at 800/s, stops the movie and returns to state 0 */
  if (g_frontend_state == 4) {
    team_mode = menu_items_animate_update();
    aux = (int)(g_frame_seconds * k_f_1000);
    if (g_menu_fade_alpha + aux < 0xff) {
      g_menu_fade_alpha = g_menu_fade_alpha + aux;
      return;
    }
    g_menu_fade_alpha = 0xff;
    if (team_mode == 0) {
      g_menu_fade_alpha = 0xff;
      return;
    }
    g_frontend_state = 5;
  }
  else if (g_frontend_state != 5) {
    return;
  }
  team_mode = (int)(g_frame_seconds * k_f_n800);
  g_menu_fade_alpha = g_menu_fade_alpha + team_mode;
  if (g_menu_fade_alpha < 1) {
    team_mode = app_get_app_76e890_4030(0);
    if (team_mode != 0) {
      movie_stop_b(0);
    }
    g_frontend_state = 0;
    g_menu_fade_alpha = 0;
  }
  return;
}

/* menu_team_choice_step_r: C -> original menu_team_choice_step (registers eax,esi,s0) */
__declspec(naked) int menu_team_choice_step_r(int p0, int p1, int p2)
{
    __asm {
        push esi
        push dword ptr [esp + 16]
        mov eax, dword ptr [esp + 12]
        mov esi, dword ptr [esp + 16]
        call menu_team_choice_step
        add esp, 4
        pop esi
        ret
    }
}

/* career_flag_clear_r: C -> original career_flag_clear (registers ecx,eax) */
__declspec(naked) int career_flag_clear_r(int p0, int p1)
{
    __asm {
        mov ecx, dword ptr [esp + 4]
        mov eax, dword ptr [esp + 8]
        call career_flag_clear
        ret
    }
}
