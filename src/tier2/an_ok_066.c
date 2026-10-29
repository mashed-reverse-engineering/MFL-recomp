#include "mfl_tier2.h"
extern __declspec(dllimport) int __stdcall ShowCursor(int);
extern __declspec(dllimport) int __stdcall Sleep(int);
/*00403030*/ extern int app_boot_init();
/*00403320*/ extern int app_shutdown_resources();
/*004039b0*/ extern int attract_timer_update();
/*00405e40*/ extern int stub_return_zero();
/*0040c9c0*/ extern int player_add_wins();
/*0040cad0*/ extern int players_get_points();
/*0040cc60*/ extern int player_get_points();
/*0040cc70*/ extern int round_time_accumulate();
/*0040cca0*/ extern int placement_counter_increment();
/*0040fdb0*/ extern int get_player_count();
/*0040fdc0*/ extern int race_get_phase();
/*0040ff00*/ extern int race_get_race_652dec();
/*0040ff10*/ extern int race_set_view_player();
/*00410dc0*/ extern int course_load_time_report();
/*004136d0*/ extern undefined __cdecl race_start_setup(undefined4 p0);
/*0042cbf0*/ extern int screen_clear_black();
/*00430ad0*/ extern int ui_state_is_0();
/*00430ae0*/ extern int ui_state_is_2();
/*00430af0*/ extern int ui_state_is_5();
/*00430b10*/ extern int frontend_stub_return_0_a();
/*00430b20*/ extern int frontend_stub_return_0_b();
/*00430b30*/ extern int ui_get_menu_69d1ec();
/*00430b40*/ extern int ui_set_menu_69d1ec();
/*00430b50*/ extern int ui_input_mode_menu();
/*00431a20*/ extern int ui_get_message_state();
/*00431a30*/ extern int ui_clear_cbe8();
/*00431a80*/ extern int player_slots_count_human_with_controller();
/*00431ae0*/ extern int message_show_controller_missing();
/*00431b30*/ extern int ui_get_menu_69d1f0();
/*00431b40*/ extern int ui_get_ui_69d1f4();
/*00431b50*/ extern undefined __cdecl frontend_set_69d1f4(undefined4 p0, undefined4 p1);
/*00434a10*/ extern int is_team_mode();
/*00434bb0*/ extern int get_event_type();
/*00437cd0*/ extern int __cdecl frontend_pause_input_poll(undefined4 p0);
/*00438f50*/ extern int frontend_enter_menu();
/*00439040*/ extern int frontend_enter_race_menu();
/*00445180*/ extern int training_mode_update();
/*00445bc0*/ extern int message_box_update();
/*004463e0*/ extern int menu_update();
/*00451c50*/ extern int camera_records_warmup();
/*00475730*/ extern int get_viewport_camera();
/*004757a0*/ extern int get_viewport_camera_matrix();
/*00485240*/ extern RwBool RwTexDictionaryDestroy_004d5ac0(struct RwTexDictionary *p0);
/*0049a960*/ extern int RpPTankAtomicLock_00545d10();
/*0049a970*/ extern int RpPTankAtomicUnlock_00545f20();
/*004a03e0*/ extern undefined __cdecl line_rain_error(undefined4 p0, undefined4 p1);
/*004a04e0*/ extern undefined __cdecl ui_colour_a_set(undefined4 p0, undefined4 p1, undefined4 p2, undefined4 p3);
/*004a0510*/ extern undefined __cdecl ui_colour_b_set(undefined4 p0, undefined4 p1, undefined4 p2, undefined4 p3);
/*004a0540*/ extern int lua_RainSetPosition_impl();
/*004a0580*/ extern int lua_RainSetScale_impl();
/*004a05a0*/ extern int lua_RainSetCameraScale_impl();
/*004a05c0*/ extern int lua_RainSetDirection_impl();
/*004a0660*/ extern int rain_drops_update();
/*004a1170*/ extern undefined rain_emit_random(undefined4 p0);
/*004a14f0*/ extern int video_init_main_camera();
/*004a1890*/ extern int game_options_reset();
/*004a1a70*/ extern int debug_keys_and_profile();
/*004a1b70*/ extern int frontend_attract_menu_input();
/*004a2240*/ extern int app_state_menu_render();
/*004a2880*/ extern int app_frame_wait();
/*004a2940*/ extern int app_shutdown_sequence_a();
/*004a2950*/ extern int rw_engine_shutdown();
/*004a2b50*/ extern int __cdecl rw_engine_init(undefined4 p0);
/*004a2d40*/ extern int app_parse_command_line();
/*004a41f0*/ extern int timer_ticks();
/*004a4250*/ extern int timer_reset_last_ticks();
/*004a4710*/ extern int dinput_release_object_004a4710();
/*004a5160*/ extern int dinput_release_all();
/*004a5520*/ extern int dinput_release_keyboard_004a5520();
/*004a5530*/ extern undefined __cdecl log_print(char *p0);
/*004a5550*/ extern int log_printf();
/*004a5930*/ extern int joypads_poll_frame();
/*004a5b40*/ extern int __cdecl joypad_get_field_770fb4(undefined4 p0);
/*004a84d0*/ extern undefined __cdecl video_mode_name_format(undefined4 p0, undefined4 p1);
/*004a87d0*/ extern int video_dialog_free();
/*004a8860*/ extern int app_get_app_7712f4();
/*004a9300*/ extern int pump_windows_messages();
/*004a9360*/ extern int window_show();
/*004a9810*/ extern int window_destroy();
/*004a9c90*/ extern int window_create();
/*004c7150*/ extern int lua_gettop();
/*004c7220*/ extern double lua_tonumber();
/*004d1cd0*/ extern enum RwFrustumTestResult RwCameraFrustumTestSphere(struct RwCamera *p0, struct RwSphere *p1);
/*004d1e10*/ extern struct RwCamera * RwCameraSetViewWindow(struct RwCamera *p0, struct RwV2d *p1);
/*004d4e70*/ extern RwMatrix * RwMatrixCreate();
/*004d5290*/ extern RwReal RwV3dNormalize(struct RwV3d *p0, struct RwV3d *p1);
/*004d53a0*/ extern RwReal RwV3dLength(struct RwV3d *p0);
/*004d5e10*/ extern struct RwTexDictionary * RwTexDictionarySetCurrent(struct RwTexDictionary *p0);
/*0050b5e0*/ extern int nop_stub();
/*00524880*/ extern undefined __cdecl RtPNGImageRead(undefined4 p0, undefined4 p1, undefined4 p2, undefined4 p3);
/*0053d050*/ extern undefined __cdecl RtBMPImageRead(undefined4 p0, undefined4 p1, undefined4 p2, undefined4 p3);
/*005dd330*/ extern float float_1_0;
/*005dd374*/ extern float k_f_0_2;
/*005dda20*/ extern float k_f_4_2949673e9;
/*005ddedc*/ extern float k_f_3_3333333en7;
/*005de490*/ extern float k_f_0_02;
/*005e0b24*/ extern float k_f_75;
/*0062ac30*/ extern float g_app_62ac30;
/*0062ad2c*/ extern undefined g_game_62ad2c;
/*0076e3b8*/ extern undefined4 g_app_76e3b8;
/*0076e3bc*/ extern undefined4 g_app_76e3bc;
/*0076e3c0*/ extern undefined4 g_app_76e3c0;
/*0076e3e0*/ extern undefined4 g_app_76e3e0;
/*0076e3e4*/ extern undefined4 g_app_76e3e4;
/*0076e3e8*/ extern undefined4 g_app_76e3e8;
/*0076e3ec*/ extern char *g_course_arctic_76e3ec;
/*0076e3f0*/ extern char *g_course_arctic_76e3f0;
/*0076e3f4*/ extern char *g_course_arctic_76e3f4;
/*0076e3f8*/ extern char *g_course_arctic_76e3f8;
/*0076e3fc*/ extern char *g_course_arctic_76e3fc;
/*0076e400*/ extern char *g_course_arctic_76e400;
/*0076e404*/ extern char *g_course_arctic_76e404;
/*0076e408*/ extern char *g_app_76e408;
/*0076e40c*/ extern char *g_course_arctic_76e40c;
/*0076e410*/ extern char *g_course_arctic_76e410;
/*0076e414*/ extern char *g_course_arctic_76e414;
/*0076e5a0*/ extern undefined4 g_app_76e5a0;
/*0076e5a4*/ extern undefined4 g_rain_update_76e5a4;
/*0076e5a8*/ extern undefined4 g_app_76e5a8;
/*0076e5ac*/ extern undefined4 g_app_76e5ac;
/*0076e7e4*/ extern undefined4 g_app_76e7e4;
/*0076e7e8*/ extern undefined4 g_sleep_remainder;
/*0076e7f4*/ extern undefined4 g_app_state;
/*0076e7f8*/ extern undefined4 g_app_76e7f8;
/*0076e7fc*/ extern undefined4 g_app_76e7fc;
/*0076e800*/ extern undefined4 g_app_76e800;
/*0076e810*/ extern undefined4 g_app_frame_76e810;
/*0076e834*/ extern undefined4 g_app_state_76e834;
/*0076e840*/ extern undefined4 g_app_main_76e840;
/*0076e844*/ extern undefined4 g_app_main_76e844;
/*0076e848*/ extern undefined4 g_app_main_76e848;
/*0076e858*/ extern undefined4 g_app_state_76e858;
/*0076e890*/ extern undefined4 g_app_76e890;
/*0076e8a4*/ extern undefined4 g_app_76e8a4;
/*007d2128*/ extern struct RwGlobalsRelease *RwEngineInstance;
/*007da6bc*/ extern undefined4 g_ptank_plugin_offset;
/*007e7f40*/ extern undefined4 g_scratch_matrix;
/*007ef7b8*/ extern undefined4 game_mode;
/*007ef7d8*/ extern undefined4 g_camera_7ef7d8;
/*007ef7e4*/ extern undefined4 g_app_frame_7ef7e4;
/*007ef7e8*/ extern undefined4 g_frame_ticks;
/*007ef7ec*/ extern float g_frame_seconds;
/*007ef7f8*/ extern float g_menu_wave_time;
/*007ef804*/ extern undefined4 g_tick_number;
/*007ef808*/ extern undefined4 g_main_loop_7ef808;
/*007ef80c*/ extern undefined4 g_app_frame_7ef80c;
/*007ef810*/ extern undefined4 g_app_frame_7ef810;
/*007ef814*/ extern undefined4 g_app_frame_7ef814;
/*007ef818*/ extern undefined4 g_last_frame_ticks;
/*007f01fc*/ extern struct PlayerSlot g_player_slots[4];
/*00843e7c*/ extern undefined4 g_quit_requested;
/*00885820*/ extern undefined4 g_app_885820;
/* rain_drops_update_and_respawn */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* "moves the rain drops and respawns them relative to the camera (twin of rain_drops_update)" */

extern void rain_drops_update_and_respawn(void);


/* lua_RainSetHeadColour */

/* Lua function RainSetHeadColour (registered by the script engine) */

extern undefined4 lua_RainSetHeadColour(char *param_1);


/* lua_RainSetTailColour */

/* Lua function RainSetTailColour (registered by the script engine) */

extern undefined4 lua_RainSetTailColour(char *param_1);


/* lua_RainSetPosition */

/* "'Usage : RainSetPosition(xmin, xmax, ymin ...)' Lua function" */

extern undefined4 lua_RainSetPosition(char *param_1);


/* lua_RainSetScale */

/* Lua function RainSetScale (registered by the script engine) */

extern undefined4 lua_RainSetScale(char *param_1);


/* lua_RainSetCameraScale */

/* Lua function RainSetCameraScale (registered by the script engine) */

extern undefined4 lua_RainSetCameraScale(char *param_1);


/* lua_RainSetDirection */

/* "'Usage : RainSetDirection(xmin, xmax, ymin ...)' Lua function" */

extern undefined4 lua_RainSetDirection(char *param_1);


/* rain_splash_add */

/* "adds a splash record (max 8): position, velocity vectors" */

undefined4
rain_splash_add(undefined4 *pos,undefined4 splash_arg,undefined4 *vel,undefined4 *colour_or_size)
{
  int slot_offset;
  
  if (7 < (int)g_app_76e5a0) {      /* defectlint: undefined4 global, the original compares signed (jl) */
    return 0;
  }
  slot_offset = g_app_76e5a0 * 0x38;
  (&g_app_76e3e0)[g_app_76e5a0 * 0xe] = *pos;
  (&g_app_76e3e4)[g_app_76e5a0 * 0xe] = pos[1];
  (&g_app_76e3e8)[g_app_76e5a0 * 0xe] = pos[2];
  *(undefined4 *)((char *)&g_course_arctic_76e3f0 + slot_offset) = *vel;
  *(undefined4 *)((char *)&g_course_arctic_76e3f4 + slot_offset) = vel[1];
  *(undefined4 *)((char *)&g_course_arctic_76e3f8 + slot_offset) = vel[2];
  *(undefined4 *)((char *)&g_course_arctic_76e3fc + slot_offset) = *colour_or_size;
  *(undefined4 *)((char *)&g_course_arctic_76e400 + slot_offset) = colour_or_size[1];
  *(undefined4 *)((char *)&g_course_arctic_76e404 + slot_offset) = colour_or_size[2];
  *(undefined4 *)((char *)&g_course_arctic_76e3ec + slot_offset) = splash_arg;
  *(undefined4 *)((char *)&g_app_76e408 + slot_offset) = *pos;
  *(undefined4 *)((char *)&g_course_arctic_76e40c + slot_offset) = pos[1];
  *(undefined4 *)((char *)&g_course_arctic_76e410 + slot_offset) = pos[2];
  *(undefined4 *)((char *)&g_course_arctic_76e414 + slot_offset) = splash_arg;
  g_app_76e5a0 = g_app_76e5a0 + 1;
  return 1;
}


/* rain_update_render */

/* "updates the rain particles visible in the camera frustum" */

extern void rain_update_render(RwCamera *camera);


/* app_initialise_rw_and_camera */

/* "rw_engine_init, video_init_main_camera, screen_clear_black" */

extern undefined4 app_initialise_rw_and_camera(void);


/* app_main_loop */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* called after the assets are loaded */

extern undefined4 app_main_loop(void);


/* app_initialise_on_bootup */

/* "'Calling AppInitialiseOnBootup' ... boot sequence" */

extern undefined4 app_initialise_on_bootup (char *param_1,char *param_2,char *param_3,char *param_4);

#define MAX_PLAYER_SLOTS (4)
#define FRAMERATE_SAMPLE_COUNT (0x3c)
#define FRAMERATE_OFS_FRAME_A (0x20)
#define FRAMERATE_OFS_FRAME_B (0x24)
#define FRAMERATE_OFS_SAMPLE_COUNT (0x28)
#define FRAMERATE_OFS_SUM_A (0x2c)
#define FRAMERATE_OFS_SUM_B (0x30)
#define FRAMERATE_OFS_AVG_A (0x34)
#define FRAMERATE_OFS_AVG_B (0x38)
#define FRAMERATE_OFS_MAX_A (0x3c)
#define FRAMERATE_OFS_MAX_B (0x40)


/* framerate_sample_accumulate */

/* "accumulates two counters over 60 samples, then stores the averages" */

/* Called once per frame with a pointer (as int) to a sampler record. Adds this frame's two
 * counters (FRAMERATE_OFS_FRAME_A / _B, inferred: per-frame values) into running sums, tracks
 * the maximum of each, and every FRAMERATE_SAMPLE_COUNT frames stores the averages of the sums
 * (FRAMERATE_OFS_AVG_A / _B) and clears the sums and the sample count. Returns nothing.
 * Note: the flush test is "count > 59", so it fires on the 61st call and that call's sample is
 * already in the sums, which are then divided by 60 (inferred from the code, not checked). */

/* TIER2: validated 2026-10-18 authority=diffemu */
void framerate_sample_accumulate(char *sampler)
{
  int frame_a;
  int frame_b;
  int sum_b;
  int sum_a;

  frame_a = *(int *)(sampler + FRAMERATE_OFS_FRAME_A);
  frame_b = *(int *)(sampler + FRAMERATE_OFS_FRAME_B);
  sum_b = *(int *)(sampler + FRAMERATE_OFS_SUM_B) + frame_b;
  sum_a = *(int *)(sampler + FRAMERATE_OFS_SUM_A) + frame_a;
  *(int *)(sampler + FRAMERATE_OFS_SUM_A) = sum_a;
  *(int *)(sampler + FRAMERATE_OFS_SUM_B) = sum_b;
  /* window full: store the averages of the sums, then restart the window */
  if ((FRAMERATE_SAMPLE_COUNT - 1) < *(int *)(sampler + FRAMERATE_OFS_SAMPLE_COUNT)) {
    *(int *)(sampler + FRAMERATE_OFS_AVG_A) = sum_a / FRAMERATE_SAMPLE_COUNT;
    *(undefined4 *)(sampler + FRAMERATE_OFS_SUM_A) = 0;
    *(int *)(sampler + FRAMERATE_OFS_AVG_B) = sum_b / FRAMERATE_SAMPLE_COUNT;
    *(undefined4 *)(sampler + FRAMERATE_OFS_SUM_B) = 0;
    *(undefined4 *)(sampler + FRAMERATE_OFS_SAMPLE_COUNT) = 0;
  }
  *(int *)(sampler + FRAMERATE_OFS_SAMPLE_COUNT) = *(int *)(sampler + FRAMERATE_OFS_SAMPLE_COUNT) + 1;
  /* per-frame maxima of the two counters */
  if (*(int *)(sampler + FRAMERATE_OFS_MAX_A) < frame_a) {
    *(int *)(sampler + FRAMERATE_OFS_MAX_A) = frame_a;
  }
  if (*(int *)(sampler + FRAMERATE_OFS_MAX_B) < frame_b) {
    *(int *)(sampler + FRAMERATE_OFS_MAX_B) = frame_b;
  }
  return;
}


/* game_get_62ad2c */

/* "returns &g_game_62ad2c (three byte option flags)" */

extern undefined * game_get_62ad2c(void);


/* main_loop_init */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* resets the quit flag and frame counters; state = 1; creates the global RwMatrix */

extern undefined4 main_loop_init(void);


/* frontend_init_menus */

/* "menu / frontend initialisation" */

/* Frontend state machine, one step per frame: moves g_app_state between the menu, race and
 * message states, and when a race has finished awards wins and points to the players.
 * Parameters: none. Returns 1 on every path.
 * players_by_points holds the player ids in points order (filled by players_get_points).
 * The state meanings are inferred from the callees. */

/* TIER2: unvalidated 2026-10-29 authority=static */
undefined4 frontend_init_menus(void)
{
  int player_index;
  int player_points;
  int next_points;
  int result;
  int tied_count;
  undefined4 menu_id;
  int players_by_points [MAX_PLAYER_SLOTS];

  frontend_attract_menu_input();
  /* dispatch on g_app_state (values inferred): 1 menu, 2 course load report, 3 race running / race end,
     4 menu wait, 5 paused, 6 menu transition, 7 message box */
  switch(g_app_state) {
  case APP_STATE_MAIN_LOOP_INIT:
    result = ui_get_ui_69d1f4();
    if (result != 0) {
      menu_id = ui_get_menu_69d1f0();
      ui_set_menu_69d1ec(menu_id);
      frontend_set_69d1f4(0,menu_id);
    }
    result = ui_get_menu_69d1ec();
    if (result == 1) {
      result = ui_state_is_0();
      if (result != 0) {
        g_app_state = APP_STATE_COURSE_LOADED;
        ui_set_menu_69d1ec(0);
        return 1;
      }
    }
    else if (result == 4) {
      g_app_state = APP_STATE_MENU;
      ui_set_menu_69d1ec(0);
      return 1;
    }
    break;
  case APP_STATE_COURSE_LOADED:
    course_load_time_report();
    g_app_state = APP_STATE_RACE;
    return 1;
  case APP_STATE_RACE:
    ui_clear_cbe8();
    result = player_slots_count_human_with_controller();
    if (result != 0) {
      message_show_controller_missing();
    }
    result = ui_get_message_state();
    if (result != 0) {
      g_app_state = APP_STATE_RACE_MESSAGE;
    }
    result = get_event_type();
    if (((result != EVENT_QUICK_RACE) && (result = race_get_phase(), result != 0xb)) && (result != 9)) {
      menu_id = stub_return_zero();
      result = frontend_pause_input_poll(menu_id);
      if (result != 0) {
        g_app_state = APP_STATE_FRONTEND;
      }
    }
    result = race_get_race_652dec();
    /* race finished: award wins and points for the placings (inferred) */
    if (result != 0) {
      menu_id = frontend_stub_return_0_a();
      frontend_enter_menu(menu_id);
      g_app_state = APP_STATE_FRONTEND;
      g_app_76e800 = 1;
      round_time_accumulate(1);
      players_get_points(players_by_points);
      result = get_player_count();
      result = result + -1;
      player_index = is_team_mode();
      /* individual scoring (or game_mode 2): walk players from the top of the points order; a player
         with fewer points than the next one starts a new placing (tied_count = players on the same points) */
      if ((player_index == 0) || (game_mode == GAME_MODE_HARE_AND_HOUNDS)) {
        tied_count = 0;
        player_index = get_player_count();
        if (0 < player_index) {
          player_index = 0;
          do {
            player_add_wins(players_by_points[player_index],result);
            placement_counter_increment(players_by_points[player_index],result);
            player_points = player_get_points(players_by_points[player_index]);
            if (player_index < 3) {
              next_points = player_get_points(players_by_points[player_index + 1]);
              if (next_points < player_points) {
                result = result + (-1 - tied_count);
                tied_count = 0;
              }
              else {
                tied_count = tied_count + 1;
              }
            }
            player_index = player_index + 1;
            player_points = get_player_count();
          } while (player_index < player_points);
        }
      }
      /* team scoring: players whose state matches the top player's get a win (inferred) */
      else {
        result = g_player_slots[players_by_points[0]].state;
        player_index = 0;
        do {
          tied_count = players_by_points[player_index];
          if (result == g_player_slots[tied_count].state) {
            player_add_wins(tied_count,1);
            placement_counter_increment(players_by_points[player_index],1);
          }
          else {
            placement_counter_increment(tied_count,0);
          }
          player_index = player_index + 1;
        } while (player_index < MAX_PLAYER_SLOTS);
      }
      camera_records_warmup(1,players_by_points[0]);
      race_set_view_player(0);
      return 1;
    }
    break;
  case APP_STATE_MENU:
    if (((g_app_76e7f8 != 0) || (g_app_76e7fc != 0)) || (g_app_76e800 != 0)) {
      g_app_state = APP_STATE_MAIN_LOOP_INIT;
      if (g_app_76e800 == 0) {
        menu_id = 0;
      }
      else {
        menu_id = 3;
      }
      ui_set_menu_69d1ec(menu_id);
      g_app_76e7f8 = 0;
      g_app_76e7fc = 0;
      g_app_76e800 = 0;
    }
    result = ui_get_ui_69d1f4();
    if (result != 0) {
      menu_id = ui_get_menu_69d1f0();
      ui_set_menu_69d1ec(menu_id);
      frontend_set_69d1f4(0,menu_id);
    }
    result = ui_state_is_5();
    if (result != 0) {
      g_app_state = APP_STATE_RACE_START;
      return 1;
    }
    break;
  case APP_STATE_FRONTEND:
    result = ui_state_is_2();
    if (result != 0) {
      g_app_state = APP_STATE_MENU;
      return 1;
    }
    break;
  case APP_STATE_RACE_START:
    result = ui_state_is_0();
    if (result != 0) {
      g_app_state = APP_STATE_RACE;
    }
    break;
  case APP_STATE_RACE_MESSAGE:
    result = ui_get_message_state();
    if (result == 0) {
      g_app_state = APP_STATE_RACE;
      return 1;
    }
  }
  return 1;
}


/* get_app_state */

/* "returns g_app_state" */

extern undefined4 get_app_state(void);


/* input_update */

/* calls input_poll */

extern undefined4 input_update(void);


/* app_state_frontend_frame */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* "one frontend frame: game_tick, menu_update" */

extern undefined4 app_state_frontend_frame(void);


/* camera_set_default_view_window */

/* "view window 0.8 x 0.8 on the viewport camera -1" */

extern void camera_set_default_view_window(void);

/* app_frame_timing: tick constants (inferred from use) */
#define FRAME_DELTA_CLAMP (200000)    /* frame delta is clamped to this when it is >= FRAME_DELTA_CLAMP + 1 */
#define NOMINAL_FRAME_TICKS (50000)   /* expected ticks per frame, subtracted to get the timing error */
#define TIMING_ERROR_DEADZONE (63)    /* timing error inside +/- this is cleared to 0 */
#define TIMING_ERROR_STEP (2)         /* timing error is pulled towards 0 by this much per frame */
#define STAT_WINDOW_TICKS (3000000)   /* length of the window after which the g_app_frame_7ef80c counter rolls over */


/* app_frame_timing */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* "timer_ticks based frame delta (g_app_7ef7e4)" */

/* Per-frame timing update (inferred). Reads timer_ticks(), takes the frame delta since the last
   call (g_last_frame_ticks), with the carried timing error g_app_frame_76e810 added in, stores
   the new timing error for the next frame (pulled back towards 0), and updates the frame seconds,
   menu wave time and camera counter. Every STAT_WINDOW_TICKS the g_app_frame_7ef80c counter is
   rolled over into g_app_frame_7ef810. Takes no parameters, returns nothing. */

/* TIER2: unvalidated 2026-10-17 authority=diffemu */
void app_frame_timing(void)
{
  float ticks_float;
  int ticks;
  uint corrected_ticks;
  int frame_delta;      /* diffemu: the globals are undefined4 (unsigned compares); the original compares signed */
  int timing_error;

  /* frame delta since the last call: current ticks plus carried error, minus last frame's ticks */
  ticks = timer_ticks();
  corrected_ticks = ticks + g_app_frame_76e810;
  frame_delta = corrected_ticks - g_last_frame_ticks;
  g_app_frame_7ef7e4 = frame_delta;
  /* clamp huge or negative deltas (stalled or wrapped timer): absolute value, capped */
  if (frame_delta < (FRAME_DELTA_CLAMP + 1)) {
    if (frame_delta < 0) {
      frame_delta = -frame_delta;
      g_app_frame_7ef7e4 = frame_delta;
    }
  }
  else {
    frame_delta = FRAME_DELTA_CLAMP;
    g_app_frame_7ef7e4 = frame_delta;
  }
  /* timing error = how far this frame is from nominal; stored for the next frame */
  timing_error = frame_delta + -NOMINAL_FRAME_TICKS;
  g_app_frame_76e810 = timing_error;
  g_frame_ticks = 0x32;
  /* outside the deadzone: pull the error towards 0 by TIMING_ERROR_STEP per frame; inside: clear it */
  if ((timing_error < -TIMING_ERROR_DEADZONE) || (TIMING_ERROR_DEADZONE < timing_error)) {
    if (timing_error < -TIMING_ERROR_STEP) {
      timing_error = timing_error + TIMING_ERROR_STEP;
      g_app_frame_76e810 = timing_error;
    }
    if (TIMING_ERROR_STEP < timing_error) {
      timing_error = timing_error + -TIMING_ERROR_STEP;
      g_app_frame_76e810 = timing_error;
    }
  }
  else {
    g_app_frame_76e810 = 0;
  }
  /* menu wave time from ticks; a negative (wrapped unsigned) tick count gets 2^32 added back */
  ticks_float = (float)ticks;
  g_frame_seconds = 0.016666668f;      /* diffemu: was the int literal 0x3c888889 (1/60), converted to a float */
  if (ticks < 0) {
    ticks_float = ticks_float + k_f_4_2949673e9;
  }
  g_menu_wave_time = ticks_float * k_f_3_3333333en7;
  g_camera_7ef7d8 = g_camera_7ef7d8 + 0x32;
  g_last_frame_ticks = ticks;
  /* once per STAT_WINDOW_TICKS: roll the g_app_frame_7ef80c counter over into g_app_frame_7ef810 */
  if (g_app_frame_7ef814 + STAT_WINDOW_TICKS < corrected_ticks) {
    g_app_frame_7ef810 = g_app_frame_7ef80c;
    g_app_frame_7ef80c = 0;
    g_app_frame_7ef814 = corrected_ticks;
  }
  return;
}


extern undefined4 cursor_show_if_windowed(void);


extern void app_flag_76e7f8_set(void);


extern void app_flag_76e7fc_set(void);


extern void app_state_set_1(void);


extern void texture_read_log(char *param_1);


extern void texture_read_png_logged(char *param_1);


/* app_get_app_76e890_3490 */

/* "return g_app_76e890;" */

extern undefined4 __cdecl app_get_app_76e890_3490(char *param_1);


/* stub_return_one */

/* "returns 1 (memory card present check on the PC build)" */

extern undefined4 stub_return_one(void);


/* app_get_app_76e8a4 */

/* "return g_app_76e8a4;" */

extern undefined4 app_get_app_76e8a4(void);


/* video_draw_textured_quad */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* "screen quad with uv rect and colour using the movie/video texture in the viewport" */

extern void video_draw_textured_quad (char *param_1,float *param_2,float *param_3,undefined1 *param_4,void *param_5, char *param_6,char *param_7);


