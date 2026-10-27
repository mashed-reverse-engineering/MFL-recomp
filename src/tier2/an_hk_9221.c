#include "mfl_tier2.h"
/*00413880*/ extern int current_course_id_with_mirror();
/*00434bb0*/ extern int get_event_type();
/*0047b520*/ extern int car_current_matrix();
/*004943c0*/ extern int rwp_world_clear_arena();
/*00494520*/ extern undefined __cdecl replay_buffer_record_frame(int p0, RwMatrix *p1, undefined4 p2, int p3);
/*00495580*/ extern undefined __cdecl physics_buffer_rewind(undefined4 p0);
/*004a8420*/ extern int __cdecl debug_printf(const char *fmt, ...);
/*005dda1c*/ extern float k_f_3_3333333en4;
/*005dda20*/ extern float k_f_4_2949673e9;
/*006037c8*/ extern undefined4 g_replay_6037c8;
/*006037d0*/ extern undefined4 g_replay_6037d0[0x1000];
/*00652f7c*/ extern undefined4 g_replay_652f7c;
/*00652f84*/ extern undefined4 g_ghost_652f84;
/*00652f88*/ extern undefined4 g_replay_652f88;
/*00652f8c*/ extern undefined4 g_replay_652f8c;
/*00652f90*/ extern undefined4 g_replay_652f90;
/*00652f94*/ extern undefined4 g_time_652f94;
/*00652f98*/ extern undefined4 g_replay_652f98;
/*00652fa4*/ extern undefined4 g_replay_652fa4;
/*007ef7dc*/ extern undefined4 g_race_clock_a;
/*007ef7f0*/ extern undefined4 g_tick_count;
/* replay_load_compare */

/* "'ReplayTime is %f' / 'Replay Time compa...'" */

void replay_load_compare(int update_flag)

{
  int *next_entry;
  float time_s;
  bool beats_compare_replay;
  int event_or_course;
  int *best_cursor;
  struct {
    float best_time;
    RwMatrix *car_matrix;
    int unused_c;
  } frame;
  
  frame.best_time = 0.0;
  event_or_course = get_event_type();
  if (event_or_course < 0) {
    return;
  }
  if (2 < event_or_course) {
    return;
  }
  if (g_replay_652f98 != 0) {
    return;
  }
  car_current_matrix(&frame.car_matrix,0);
  replay_buffer_record_frame(g_replay_652f8c,frame.car_matrix,g_race_clock_a - g_tick_count,-1);
  debug_printf("ReplayTime is %f\n",(double)((float)*(uint *)(g_replay_652f8c + 0x174) * k_f_3_3333333en4));
  if (g_replay_6037d0[0] != -1) {
    best_cursor = g_replay_6037d0;
    do {
      event_or_course = current_course_id_with_mirror();
      if (*best_cursor == event_or_course) {
        frame.best_time = *(float *)&best_cursor[1];
      }
      next_entry = best_cursor + 2;
      best_cursor = best_cursor + 2;
    } while (*next_entry != -1);
  }
  beats_compare_replay = false;
  if (g_replay_652f88 != 0) {
    debug_printf("\nReplay Time comparing %f to %f\n",
                 (double)((float)*(uint *)(g_replay_652f8c + 0x174) * k_f_3_3333333en4),
                 (double)((float)*(uint *)(g_replay_652f88 + 0x174) * k_f_3_3333333en4));
    debug_printf("\nSplitTime 0 is %f\n",(double)((float)*(uint *)(g_replay_652f8c + 0x17c) * k_f_3_3333333en4));
    debug_printf("\nSplitTime 1 is %f\n",(double)((float)*(uint *)(g_replay_652f8c + 0x180) * k_f_3_3333333en4));
    if (*(uint *)(g_replay_652f88 + 0x174) <= *(uint *)(g_replay_652f8c + 0x174)) goto LAB_00413f8a;
  }
  beats_compare_replay = true;
LAB_00413f8a:
  time_s = (float)*(int *)(g_replay_652f8c + 0x174);
  if (*(int *)(g_replay_652f8c + 0x174) < 0) {
    time_s = time_s + k_f_4_2949673e9;
  }
  if (!(frame.best_time > time_s * k_f_3_3333333en4)) {      /* NaN: too long (original: test ah,5; jp) */
    debug_printf("Time is too long\n");
  }
  else if (((beats_compare_replay) || (g_replay_652fa4 == 0)) && (update_flag != 0)) {
    g_replay_652f90 = g_replay_652f90 - 1 & 1;
    g_replay_652f88 = g_replay_652f8c;
    g_replay_6037c8 = 0;
    g_replay_652f8c = (&g_replay_652f7c)[g_replay_652f90];
  }
  if (g_replay_652f88 != 0) {
    physics_buffer_rewind(g_replay_652f88);
  }
  if (g_ghost_652f84 != 0) {
    physics_buffer_rewind(g_ghost_652f84);
  }
  if (g_replay_652f8c != 0) {
    rwp_world_clear_arena(g_replay_652f8c);
  }
  g_replay_652fa4 = 1;
  g_replay_652f98 = 1;
  g_time_652f94 = 0;
  return;
}


