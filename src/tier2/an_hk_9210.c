#include "mfl_tier2.h"
/*00413880*/ extern int current_course_id_with_mirror();
/*004138a0*/ extern int time_split();
/*00413cb0*/ extern int replay_records_reset();
/*0042eaf0*/ extern undefined __cdecl best_time_store(float p0, uint p1, uint p2);
/*00434bb0*/ extern int get_event_type();
/*00435fb0*/ extern int ui_get_menu_69d2d0();
/*00494460*/ extern int physics_buffer_size();
/*00494480*/ extern int physics_buffer_create();
/*00495860*/ extern int __cdecl replay_buffer_create_from_stream(undefined4 p0, undefined4 p1);
/*004a8420*/ extern int __cdecl debug_printf(const char *fmt, ...);
/*004b2bbd*/ extern int __cdecl FID_conflict__wprintf(const char *p0, ...);
typedef int (__cdecl *rwSprintfFn)(RwChar *, const RwChar *, ...);
#define rwsprintf RW_GLOBAL(0xc4, rwSprintfFn) /* stringFuncs.vecSprintf */
/*006037d0*/ extern undefined4 g_replay_6037d0[0x1000];
/*00652f7c*/ extern undefined4 g_replay_652f7c;
/*00652f80*/ extern undefined4 g_replay_652f80;
/*00652f88*/ extern undefined4 g_replay_652f88;
/*00652f8c*/ extern undefined4 g_replay_652f8c;
/*00652f90*/ extern undefined4 g_replay_652f90;
/*00652fa4*/ extern undefined4 g_replay_652fa4;
/*007d2128*/ extern char *RwEngineInstance;
/*007eebc0*/ extern struct GameState g_game_state;
/*008d8b08*/ extern undefined4 g_replay_buffer;
/* replay_create_buffer */
/* compile: /GS */

/* WARNING: Function: __security_check_cookie replaced with injection: security_check_cookie */
/* "'replay size will be %d bytes' and physics_buffer_create" */

void replay_create_buffer(int course_id)

{
  int status;
  undefined4 course_id_2;
  int *piVar2;
  int target_time;
  uint uStack_4c;
  uint uStack_48;
  RwChar replay_name [64];
  
  target_time = 0;
  if (g_game_state.mirror_mode != 0) {
    course_id = course_id + 1000;
  }
  status = get_event_type();
  if ((status >= 0) && (status <= 2)) {
    if (g_replay_6037d0[0] != -1) {
      piVar2 = g_replay_6037d0;
      status = g_replay_6037d0[0];
      do {
        if (status == course_id) {
          target_time = piVar2[1];
        }
        status = piVar2[2];
        piVar2 = piVar2 + 2;
      } while (status != -1);
    }
    course_id_2 = physics_buffer_size(target_time,0xf);
    FID_conflict__wprintf("replay size will be %d bytes\n",course_id_2);
    FID_conflict__wprintf("creating replay of length %f\n",(double)*(float *)&target_time);
    if (g_replay_652f7c == 0) {
      g_replay_652f7c = physics_buffer_create(target_time,0xf,course_id);
      g_replay_652f8c = g_replay_652f7c;
    }
    if (g_replay_652f80 == 0) {
      course_id_2 = current_course_id_with_mirror();
      rwsprintf(replay_name,"%sReplay%d.rep","U:\\toast\\",course_id_2);
      status = ui_get_menu_69d2d0();
      if (status == 0) {
        course_id_2 = current_course_id_with_mirror();
        g_replay_652f80 = replay_buffer_create_from_stream((undefined4)&g_replay_buffer,course_id_2);
      }
      if (g_replay_652f80 == 0) {
        g_replay_652fa4 = 0;
        g_replay_652f80 = physics_buffer_create(target_time,0xf,course_id);
      }
      else {
        g_replay_652f88 = g_replay_652f80;
        time_split(*(undefined4 *)(g_replay_652f80 + 0x174),(float *)&target_time,&uStack_4c,&uStack_48);
        debug_printf("setting time attack record too %d:%d:%f\n",uStack_48,uStack_4c,(double)*(float *)&target_time);
        g_replay_652fa4 = 1;
        best_time_store(*(float *)&target_time,uStack_4c,uStack_48);
      }
      g_replay_652f90 = 0;
    }
  }
  replay_records_reset();
  return;
}


