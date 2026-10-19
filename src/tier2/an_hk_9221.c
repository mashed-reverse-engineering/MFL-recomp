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

void replay_load_compare(int param_1)

{
  int *piVar1;
  float fVar2;
  bool bVar4;
  int iVar5;
  int *piVar6;
  struct {
    float local_14;
    RwMatrix *local_10;
    int iStack_c;
  } frame;
  
  frame.local_14 = 0.0;
  iVar5 = get_event_type();
  if (iVar5 < 0) {
    return;
  }
  if (2 < iVar5) {
    return;
  }
  if (g_replay_652f98 != 0) {
    return;
  }
  car_current_matrix(&frame.local_10,0);
  replay_buffer_record_frame(g_replay_652f8c,frame.local_10,g_race_clock_a - g_tick_count,-1);
  debug_printf("ReplayTime is %f\n",(double)((float)*(uint *)(g_replay_652f8c + 0x174) * k_f_3_3333333en4));
  if (g_replay_6037d0[0] != -1) {
    piVar6 = g_replay_6037d0;
    do {
      iVar5 = current_course_id_with_mirror();
      if (*piVar6 == iVar5) {
        frame.local_14 = *(float *)&piVar6[1];
      }
      piVar1 = piVar6 + 2;
      piVar6 = piVar6 + 2;
    } while (*piVar1 != -1);
  }
  bVar4 = false;
  if (g_replay_652f88 != 0) {
    debug_printf("\nReplay Time comparing %f to %f\n",
                 (double)((float)*(uint *)(g_replay_652f8c + 0x174) * k_f_3_3333333en4),
                 (double)((float)*(uint *)(g_replay_652f88 + 0x174) * k_f_3_3333333en4));
    debug_printf("\nSplitTime 0 is %f\n",(double)((float)*(uint *)(g_replay_652f8c + 0x17c) * k_f_3_3333333en4));
    debug_printf("\nSplitTime 1 is %f\n",(double)((float)*(uint *)(g_replay_652f8c + 0x180) * k_f_3_3333333en4));
    if (*(uint *)(g_replay_652f88 + 0x174) <= *(uint *)(g_replay_652f8c + 0x174)) goto LAB_00413f8a;
  }
  bVar4 = true;
LAB_00413f8a:
  fVar2 = (float)*(int *)(g_replay_652f8c + 0x174);
  if (*(int *)(g_replay_652f8c + 0x174) < 0) {
    fVar2 = fVar2 + k_f_4_2949673e9;
  }
  if (frame.local_14 <= fVar2 * k_f_3_3333333en4) {
    debug_printf("Time is too long\n");
  }
  else if (((bVar4) || (g_replay_652fa4 == 0)) && (param_1 != 0)) {
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


