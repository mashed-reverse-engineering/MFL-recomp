#include "mfl_tier2.h"
typedef int CourseId;
/*0040fdb0*/ extern int get_player_count();
/*0040fe20*/ extern int __cdecl race_setup_slot_used(undefined4 p0);
/*0042b110*/ extern int current_course_id();
/*00430af0*/ extern int ui_state_is_5();
/*00430b00*/ extern int ui_get_frontend_state();
/*00434bb0*/ extern int get_event_type();
/*00437780*/ extern double option_get_volume_1();
/*00437790*/ extern double option_get_volume_2();
/*00437ee0*/ extern int message_is_controller_missing_shown();
/*004683f0*/ extern int voice_queue_tables_clear();
/*00469360*/ extern int voice_queue_frame_update();
/*00469770*/ extern int sound_listener_update();
/*00469b20*/ extern int sound_music_fade_step();
/*00469bb0*/ extern int sound_music_fade_step_b();
/*00469c50*/ extern int sound_resume_streams();
/*00469d30*/ extern int sound_music_volume_update();
/*00469f40*/ extern int sound_master_fade_towards(float param_2);
/*00469f90*/ extern undefined sound_master_fade_to_default(void);
/*0046a430*/ extern int sound_ambience_course_2_6_b();
/*0046a960*/ extern undefined __cdecl sound_ambience_course_8_19(undefined4 p0);
/*0046afc0*/ extern int sound_ambience_course_16_17();
/*0046b170*/ extern undefined __cdecl sound_ambience_course_1_1e(undefined4 p0);
/*0046b790*/ extern int sound_ambience_course_11_14();
/*0046bb10*/ extern int sound_ambience_course_b();
/*0046bfc0*/ extern int sound_ambience_course_5_27();
/*0046c470*/ extern int sound_ambience_course_9_22();
/*0046c830*/ extern int sound_ambience_course_3_4();
/*0046c910*/ extern int sound_ambience_course_25();
/*0046d290*/ extern int sound_ambience_course_1a();
/*0046daf0*/ extern int sound_ambience_course_12_18();
/*0046dd30*/ extern int sound_ambience_course_c_26();
/*0046e570*/ extern int __cdecl sound_music_for_menu_page(undefined4 p0, undefined4 p1, undefined4 p2, undefined4 p3);
/*0046eb60*/ extern undefined sound_music_fade_update(void);
/*0046eca0*/ extern int sound_music_for_career_row();
/*0046ef30*/ extern int sound_stub_return_0();
/*0046ef60*/ extern int sound_music_state_update();
/*0046f5c0*/ extern int soundengine_create_race_streams();
/*0046f6b0*/ extern int sound_streams_destroy();
/*0046f740*/ extern int sound_endpoint_panel_progress_cues();
/*0046f9e0*/ extern int sound_menu_voices_trigger();
/*0046fde0*/ extern int sound_replay_split_beep();
/*0046fe90*/ extern int sound_car_engine_update();
/*004704c0*/ extern int sound_car_voices_mute();
/*00470750*/ extern int sound_surface_roll_voices_update();
/*00470b00*/ extern int sound_car_engine_start();
/*00470bf0*/ extern undefined __cdecl sound_surface_skid_voice_update(undefined4 p0, undefined4 p1, undefined4 p2, undefined4 p3, undefined4 p4, undefined4 p5);
/*00470da0*/ extern int sound_car_landing_voice();
/*00470ec0*/ extern int sound_car_mode_2_voice();
/*004710f0*/ extern int sound_global_fade_update();
/*004711b0*/ extern int sound_event_log_process();
/*00471360*/ extern int sound_nearest_objects_sort();
/*004715b0*/ extern int sound_ambience_course_2_6_a();
/*00471720*/ extern int sound_ambience_course_0();
/*00471bc0*/ extern int sound_ambience_course_f_24();
/*00472c00*/ extern int sound_ambience_course_21();
/*004a20c0*/ extern int get_app_state();
/*005b8c40*/ extern int sound_stream_state(int p0);
/*005b8c50*/ extern int __cdecl FUN_005b8c50(int a1);
/*005b8c60*/ extern int FUN_005b8c60(undefined4 p0, undefined4 p1, undefined4 p2);
/*00617948*/ extern float g_sound_master_fade;
/*0061794c*/ extern float g_sound_frame_61794c;
/*006adcf4*/ extern float g_sound_frame_6adcf4;
/*006ae07c*/ extern undefined4 g_sound_stream_handles;
/*006ae0c4*/ extern undefined4 g_sound_frame_6ae0c4;
/*006ae0c8*/ extern undefined4 g_sound_frame_6ae0c8;
/*006ae0cc*/ extern undefined4 g_sound_frame_6ae0cc;
/*006ae0d0*/ extern undefined4 g_sound_frame_6ae0d0;
/*006ae100*/ extern undefined4 g_sound_frame_6ae100;
/*006ae104*/ extern undefined4 g_sound_endpoint_cue_state;
/*006ae108*/ extern undefined4 g_sound_streams_enabled;
/*006ae114*/ extern undefined4 g_sound_frame_6ae114;
/*006ae118*/ extern undefined4 g_sound_frame_6ae118;
/*006ae11c*/ extern undefined4 g_sound_frame_6ae11c;
/*006ae140*/ extern undefined4 g_sound_frame_6ae140;
/*006ae144*/ extern undefined4 g_sound_frame_6ae144;
/*006ae148*/ extern undefined4 g_sound_frame_6ae148;
/*006ae14c*/ extern undefined4 g_sound_frame_6ae14c;
/*008bd7b8*/ extern undefined4 g_sound_frame_8bd7b8;
/*008d98bc*/ extern int g_sound_player_count;
/*008d9970*/ extern undefined4 g_sound_frame_8d9970;
/* sound_frame_begin */
#define COURSE_ARCTIC (0)
#define COURSE_ANGELS (1)
#define COURSE_ROUABOUT (2)
#define COURSE_EGYPT (3)
#define COURSE_EGYPT_R (4)
#define COURSE_NEUST_R (5)
#define COURSE_TIERRA_N (6)
#define COURSE_SUPERG_N (8)
#define COURSE_FOREST_R (9)
#define COURSE_SANDS (11)
#define COURSE_HIGHWAYN (12)
#define COURSE_STORM_R (15)
#define COURSE_NT1 (17)
#define COURSE_DEMR (18)
#define COURSE_NT1_S (20)
#define COURSE_KEYS01 (22)
#define COURSE_KEYS (23)
#define COURSE_DEM (24)
#define COURSE_SUPERG (25)
#define COURSE_CITY (26)
#define COURSE_TRAINING (30)
#define COURSE_WARZONE (33)
#define COURSE_FOREST (34)
#define COURSE_STORM (36)
#define COURSE_DUMP (37)
#define COURSE_HIGHWAY (38)
#define COURSE_NEUSTEIN (39)

/* "stores the player count in g_sound_8d98bc when sound is running" */

void __cdecl sound_frame_begin(undefined4 param_1,undefined4 param_2)

{
  bool streams_stalled; /* frontend: all 4 stream counters reached 3 (inferred) */
  undefined4 app_state;
  CourseId course_copy;
  CourseId course_id;
  int slot_used;
  int tmp_a;
  undefined4 stream_aux;
  int index_or_byte_offset; /* car slot 0..3 in the race branch, byte offset into g_sound_stream_handles (4 handles) in the frontend branch; was named event_type */
  double volume;
  int tmp_b;
  int stream_state_by_slot [4];
  undefined4 stream_aux_by_slot [4];
  undefined4 stream_aux2_by_slot [4];
  
  if (g_sound_streams_enabled == 0) {
    return;
  }
  g_sound_player_count = get_player_count();
  g_sound_frame_8d9970 = current_course_id();
  volume = (double)option_get_volume_1();
  g_sound_frame_6adcf4 = (float)volume;
  sound_listener_update();
  app_state = get_app_state();
  switch(app_state) {
  case APP_STATE_NONE:
    sound_music_fade_step_b(2);
    sound_music_fade_step(1);
    break;
  case APP_STATE_MAIN_LOOP_INIT:
    index_or_byte_offset = message_is_controller_missing_shown();
    if (index_or_byte_offset != 0) {
      g_sound_frame_6adcf4 = 0.0;
    }
    sound_resume_streams();
    if (g_sound_frame_6ae118 == 1) {
      sound_streams_destroy();
      g_sound_frame_6ae114 = 0;
    }
    voice_queue_tables_clear();
    g_sound_frame_6ae100 = 0;
    g_sound_endpoint_cue_state = 0;
    index_or_byte_offset = ui_get_frontend_state();
    if (0 < index_or_byte_offset) {
      if (index_or_byte_offset < 4) {
        sound_music_fade_step_b(1);
        index_or_byte_offset = sound_music_fade_step(0);
      }
      else if (index_or_byte_offset == 4) {
        course_id = current_course_id();
        course_copy = sound_music_for_career_row();
        sound_music_fade_step(course_id != course_copy);
        index_or_byte_offset = sound_music_fade_step_b(0);
      }
    }
    sound_master_fade_towards(0.0);
    g_sound_frame_6ae118 = 0;
    g_sound_frame_6ae11c = 1;
    break;
  case APP_STATE_COURSE_LOADED:
    index_or_byte_offset = get_event_type();
    if (index_or_byte_offset != EVENT_QUICK_RACE) {
      if (g_sound_frame_6ae11c == 1) {
        sound_streams_destroy();
      }
      g_sound_frame_6ae11c = 0;
    }
    g_sound_frame_6ae114 = 0;
    sound_music_fade_step_b(0);
    if (g_sound_frame_6ae11c == 0) {
      sound_music_fade_step(1);
    }
    goto LAB_00473980;
  case APP_STATE_RACE:
  case APP_STATE_RACE_MESSAGE:
    index_or_byte_offset = sound_music_state_update();
    if (index_or_byte_offset == 0) {
      sound_master_fade_to_default();
    }
    volume = (double)option_get_volume_2();
    g_sound_frame_61794c = (float)volume;
    index_or_byte_offset = message_is_controller_missing_shown();
    if (index_or_byte_offset != 0) {
      g_sound_frame_61794c = 0.0;
    }
    soundengine_create_race_streams();
    get_event_type();
    course_id = current_course_id();
    sound_event_log_process();
    index_or_byte_offset = 0;
    do {
      slot_used = race_setup_slot_used(index_or_byte_offset);
      if (slot_used != 0) {
        tmp_b = index_or_byte_offset;
        sound_car_engine_update(index_or_byte_offset);
        tmp_a = index_or_byte_offset;
        sound_car_mode_2_voice(tmp_a);
        slot_used = index_or_byte_offset;
        sound_car_landing_voice(tmp_a);
        slot_used = sound_music_for_menu_page(course_id,slot_used,tmp_a,tmp_b);
        if (slot_used != 0) {
          tmp_b = index_or_byte_offset;
          course_copy = course_id;
          sound_car_voices_mute(tmp_a, course_id);
          tmp_a = index_or_byte_offset;
          sound_surface_roll_voices_update(index_or_byte_offset);
          slot_used = index_or_byte_offset;
          sound_car_engine_start(index_or_byte_offset);
          sound_surface_skid_voice_update(index_or_byte_offset,course_id,slot_used,tmp_a,tmp_b,course_copy);
        }
      }
      index_or_byte_offset = index_or_byte_offset + 1;
    } while (index_or_byte_offset < 4);
    sound_nearest_objects_sort();
    switch(course_id) {
    case COURSE_ARCTIC:
      sound_ambience_course_0();
      break;
    case COURSE_ANGELS:
    case COURSE_TRAINING:
      sound_ambience_course_1_1e(course_id);
      break;
    case COURSE_ROUABOUT:
    case COURSE_TIERRA_N:
      sound_ambience_course_2_6_a();
      sound_ambience_course_2_6_b();
      break;
    case COURSE_EGYPT:
    case COURSE_EGYPT_R:
      sound_ambience_course_3_4();
      break;
    case COURSE_NEUST_R:
    case COURSE_NEUSTEIN:
      sound_ambience_course_5_27();
      break;
    case COURSE_SUPERG_N:
    case COURSE_SUPERG:
      sound_ambience_course_8_19(course_id);
      break;
    case COURSE_FOREST_R:
    case COURSE_FOREST:
      sound_ambience_course_9_22();
      break;
    case COURSE_SANDS:
      sound_ambience_course_b();
      break;
    case COURSE_HIGHWAYN:
    case COURSE_HIGHWAY:
      sound_ambience_course_c_26();
      break;
    case COURSE_STORM_R:
    case COURSE_STORM:
      sound_ambience_course_f_24();
      break;
    case COURSE_NT1:
    case COURSE_NT1_S:
      sound_ambience_course_11_14();
      break;
    case COURSE_DEMR:
    case COURSE_DEM:
      sound_ambience_course_12_18();
      break;
    case COURSE_KEYS01:
    case COURSE_KEYS:
      sound_ambience_course_16_17();
      break;
    case COURSE_CITY:
      sound_ambience_course_1a();
      break;
    case COURSE_WARZONE:
      sound_ambience_course_21();
      break;
    case COURSE_DUMP:
      sound_ambience_course_25();
    }
    sound_replay_split_beep();
    sound_endpoint_panel_progress_cues();
    index_or_byte_offset = sound_stub_return_0();
    if ((((index_or_byte_offset == 0) && (index_or_byte_offset = get_event_type(), index_or_byte_offset != 0x12)) &&
        (course_id != COURSE_KEYS)) && (course_id != COURSE_KEYS01)) {
      voice_queue_frame_update();
    }
    sound_music_fade_step(3);
    g_sound_frame_6ae118 = 1;
    g_sound_frame_6ae140 = 0;
    g_sound_frame_6ae144 = 0;
    g_sound_frame_6ae148 = 0;
    g_sound_frame_6ae14c = 0;
    break;
  case APP_STATE_MENU:
    index_or_byte_offset = message_is_controller_missing_shown();
    if (index_or_byte_offset != 0) {
      g_sound_frame_6adcf4 = 0.0;
    }
    index_or_byte_offset = ui_get_frontend_state();
    g_sound_frame_6ae100 = 0;
    if ((index_or_byte_offset == 2) || (index_or_byte_offset == 3)) {
      app_state = 1;
LAB_00473958:
      index_or_byte_offset = sound_music_fade_step_b(app_state);
    }
    else {
      index_or_byte_offset = index_or_byte_offset + -4;
      if (index_or_byte_offset == 0) {
        app_state = 0;
        goto LAB_00473958;
      }
    }
    sound_master_fade_towards(0.0);
    sound_music_fade_step(0);
    break;
  case APP_STATE_FRONTEND:
    g_sound_frame_6ae0c4 = 0;
    g_sound_frame_6ae0c8 = 0;
    g_sound_frame_6ae0cc = 0;
    g_sound_frame_6ae0d0 = 0;
    sound_resume_streams();
    index_or_byte_offset = 0;
    do {
      slot_used = *(int *)((char *)&g_sound_stream_handles + index_or_byte_offset);
      if ((slot_used != 0) && (g_sound_frame_6ae118 != 0)) {
        tmp_a = sound_stream_state(*(undefined4 *)((char *)&g_sound_stream_handles + index_or_byte_offset));
        app_state = *(undefined4 *)((char *)&g_sound_stream_handles + index_or_byte_offset);
        *(int *)((int)stream_state_by_slot + index_or_byte_offset) = tmp_a;
        stream_aux = FUN_005b8c50(*(undefined4 *)((char *)&g_sound_stream_handles + index_or_byte_offset));
        *(undefined4 *)((int)stream_aux_by_slot + index_or_byte_offset) = stream_aux;
        app_state = FUN_005b8c60(*(undefined4 *)((char *)&g_sound_stream_handles + index_or_byte_offset),
                                 app_state,slot_used);
        *(undefined4 *)((int)stream_aux2_by_slot + index_or_byte_offset) = app_state;
        if (tmp_a == 3) {
          *(int *)((char *)&g_sound_frame_6ae140 + index_or_byte_offset) =
               *(int *)((char *)&g_sound_frame_6ae140 + index_or_byte_offset) + 1;
        }
        else {
          *(undefined4 *)((char *)&g_sound_frame_6ae140 + index_or_byte_offset) = 0;
        }
      }
      index_or_byte_offset = index_or_byte_offset + 4;
    } while (index_or_byte_offset < 0x10);
    if (((((int)g_sound_frame_6ae140 < 3) || ((int)g_sound_frame_6ae144 < 3)) || ((int)g_sound_frame_6ae148 < 3))
       || ((int)g_sound_frame_6ae14c < 3)) {
      streams_stalled = false;
    }
    else {
      streams_stalled = true;
    }
    if ((g_sound_frame_6ae118 == 1) && (streams_stalled)) {
      sound_streams_destroy();
      g_sound_frame_6ae118 = 0;
      g_sound_frame_6ae11c = 0;
      g_sound_frame_6ae114 = 0;
      g_sound_frame_8bd7b8 = 0;
      voice_queue_tables_clear();
    }
    app_state = sound_music_fade_step_b(2);
    sound_master_fade_towards(0.0);
    sound_music_fade_step(0);
    break;
  case APP_STATE_RACE_START:
    sound_music_fade_step_b(2);
    ui_state_is_5();
LAB_00473980:
    g_sound_master_fade = 0;
  }
  sound_global_fade_update();
  app_state = sound_menu_voices_trigger();
  sound_music_fade_update();
  sound_music_volume_update();
  return;
}




