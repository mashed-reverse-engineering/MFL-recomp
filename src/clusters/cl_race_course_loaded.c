// cluster-entries: race_course_loaded
/* Installed cluster race_course_loaded (tools/cluster_install.py from output/cluster_pilot_md/race_course_loaded.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
typedef int CourseId;
typedef struct { char c[8]; } Text8;   /* the 8 byte string "waiting\0" copied as two dwords */
/*00408900*/ extern int fx_objects_destroy();
/*00408c60*/ extern undefined __cdecl load_copter_models(undefined4 p0, undefined4 p1);
/*0040e6a0*/ extern int vehicles_any_active();
/*0040e6f0*/ extern int course_unload_current();
/*0040e730*/ extern int course_load_from_setup();
/*0040e760*/ extern int race_course_record_for_event(int p1, int p2);
/*0040e860*/ extern int race_course_model_path_for_event(int round_index, int param_3);
/*0040fdd0*/ extern int race_set_phase();
/*00413a30*/ extern int replay_free_buffers();
/*00414320*/ extern void __cdecl replay_create_buffer(int p0);
/*0042a3b0*/ extern int course_get_world_object();
/*0042a400*/ extern int course_get_collision_bsp();
/*0042a7f0*/ extern int course_run_ktc_script();
/*0042a980*/ extern int course_load_powerups_gold();
/*0042b0b0*/ extern int course_get_split_sector_8();
/*0042b110*/ extern int current_course_id();
/*0042d970*/ extern int build_info_frame();
/*00434bb0*/ extern int get_event_type();
/*00434d20*/ extern int ui_get_menu_69cc4c();
/*00434d30*/ extern int ui_menu_69cc4c_clear();
/*004353c0*/ extern int menu_powerup_set_for_event();
/*00464aa0*/ extern int powerups_clear();
/*0046ec80*/ extern int sound_flag_617af0_set();
/*0046ec90*/ extern int sound_flag_617af0_clear();
/*004a41f0*/ extern int timer_ticks();
/*004a8420*/ extern int debug_printf();
/*005dda20*/ extern float k_f_4_2949673e9;
/*005ddedc*/ extern float k_f_3_3333333en7;
/*00652e04*/ extern undefined4 g_race_course_loaded_event;
/*007ef7b4*/ extern undefined4 g_hud_7ef7b4;
/*007ef7b8*/ extern undefined4 game_mode;
/*007ef820*/ extern struct PlayerInput g_player_inputs[12];
/*007f01fc*/ extern struct PlayerSlot g_player_slots[4];
/*00801b0c*/ extern undefined4 g_car_801b0c;
/*00801b14*/ extern undefined4 g_rw_801b14;
/* race_course_loaded */
/* compile: /GS */

/* WARNING: Function: __security_check_cookie replaced with injection: security_check_cookie */
/* "logs 'course LoadTime is %u (%.3f)' and starts the race setup" */

void __cdecl race_course_loaded(int event,int round_index)

{
  bool waiting;
  float load_seconds_ticks;
  int loaded_or_tmp;
  CourseId course_id;
  int start_ticks;
  int menu_flag;
  undefined4 powerup_set_or_course;
  undefined4 world_object;
  undefined4 reg_edx_00;
  undefined4 reg_edx_01;
  undefined4 *clear_ptr;
  undefined8 powerup_set_wide;
  union {
    char gs_buf[136];
    struct {
      int iStack_8c;
      undefined1 pad_88[4];
      char text[8];              /* "waiting" (the title of the info frame) */
      undefined4 rest[30];
    } m;
  } frame;
  
  /* event+4+round_index*4 -> pointer to the round's course id; event+200 = cached powerup set. Loads (or reloads) the course, logs the load time, then waits on "waiting" until a player presses select/pause. */
  loaded_or_tmp = course_get_split_sector_8();
  start_ticks = timer_ticks();
  sound_flag_617af0_clear();
  if (((loaded_or_tmp == 0) ||
      (course_id = current_course_id(), **(CourseId **)(event + 4 + round_index * 4) != course_id)) ||
     (menu_flag = ui_get_menu_69cc4c(), menu_flag != 0)) {
    sound_flag_617af0_set();
    if (game_mode != 5) {
      powerup_set_or_course = menu_powerup_set_for_event();
      course_load_powerups_gold(**(undefined4 **)(event + 4 + round_index * 4));
      *(undefined4 *)(event + 200) = powerup_set_or_course;
    }
    vehicles_any_active();
    if (loaded_or_tmp != 0) {
      course_unload_current();
    }
    course_load_from_setup(event,round_index);
    if (game_mode == 5) {
      powerup_set_or_course = **(undefined4 **)(event + 4 + round_index * 4);
      course_run_ktc_script(powerup_set_or_course);
      world_object = course_get_world_object();
      load_copter_models(world_object,powerup_set_or_course);
      race_course_model_path_for_event(event,round_index);
    }
    else {
      fx_objects_destroy();
      race_course_model_path_for_event(event,round_index);
    }
  }
  else {
    powerup_set_wide = menu_powerup_set_for_event();
    loaded_or_tmp = race_course_record_for_event(event,round_index);
    if (loaded_or_tmp != 0) {
      sound_flag_617af0_set();
      vehicles_any_active();
      race_course_model_path_for_event(event,round_index);
    }
    if (game_mode == 5) {
      sound_flag_617af0_set();
      powerups_clear();
      powerup_set_or_course = **(undefined4 **)(event + 4 + round_index * 4);
      course_run_ktc_script(powerup_set_or_course);
      world_object = course_get_world_object();
      load_copter_models(world_object,powerup_set_or_course);
    }
    else {
      fx_objects_destroy();
      if ((*(int *)(event + 200) != (int)powerup_set_wide) ||
         (course_id = current_course_id(), **(CourseId **)(event + 4 + round_index * 4) != course_id))
      {
        course_load_powerups_gold(**(undefined4 **)(event + 4 + round_index * 4));
        *(int *)(event + 200) = (int)powerup_set_wide;
      }
    }
  }
  ui_menu_69cc4c_clear();
  g_rw_801b14 = course_get_collision_bsp();
  g_car_801b0c = 0x44c30000;
  replay_free_buffers();
  replay_create_buffer(**(int **)(event + 4 + round_index * 4));
  race_set_phase(RACE_PHASE_SETUP);
  loaded_or_tmp = timer_ticks();
  loaded_or_tmp = loaded_or_tmp - start_ticks;
  load_seconds_ticks = (float)loaded_or_tmp;
  if (loaded_or_tmp < 0) {
    load_seconds_ticks = load_seconds_ticks + k_f_4_2949673e9;
  }
  debug_printf("course LoadTime is %u (%.3f)\n",loaded_or_tmp,(double)(load_seconds_ticks * k_f_3_3333333en7));
  loaded_or_tmp = get_event_type();
  if (loaded_or_tmp == 0x12) {
    return;
  }
  *(Text8 *)frame.m.text = *(const Text8 *)"waiting";
  clear_ptr = frame.m.rest;
  for (loaded_or_tmp = 0x1e; loaded_or_tmp != 0; loaded_or_tmp = loaded_or_tmp + -1) {
    *clear_ptr = 0;
    clear_ptr = clear_ptr + 1;
  }
  waiting = true;
  g_race_course_loaded_event = 0;
  do {
    g_hud_7ef7b4 = 1;
    build_info_frame(frame.m.text,frame.m.text,0);
    if (g_player_slots[0].controller != -1) {
      if (g_player_inputs[g_player_slots[0].controller].menu_select != 0) {
        g_race_course_loaded_event = 1;
        waiting = false;
      }
      if (g_player_inputs[g_player_slots[0].controller].pause != 0) {
        g_race_course_loaded_event = 1;
        waiting = false;
      }
    }
    if (g_player_slots[1].controller != -1) {
      if (g_player_inputs[g_player_slots[1].controller].menu_select != 0) {
        g_race_course_loaded_event = 1;
        waiting = false;
      }
      if (g_player_inputs[g_player_slots[1].controller].pause != 0) {
        g_race_course_loaded_event = 1;
        waiting = false;
      }
    }
    if (g_player_slots[2].controller != -1) {
      if (g_player_inputs[g_player_slots[2].controller].menu_select != 0) {
        g_race_course_loaded_event = 1;
        waiting = false;
      }
      if (g_player_inputs[g_player_slots[2].controller].pause != 0) {
        g_race_course_loaded_event = 1;
        waiting = false;
      }
    }
    if (g_player_slots[3].controller != -1) {
      if (g_player_inputs[g_player_slots[3].controller].menu_select != 0) {
        g_race_course_loaded_event = 1;
        waiting = false;
      }
      if (g_player_inputs[g_player_slots[3].controller].pause != 0) {
        g_race_course_loaded_event = 1;
        return;
      }
    }
    if (!waiting) {
      return;
    }
  } while( true );
}

