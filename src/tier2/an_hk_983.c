#include "mfl_tier2.h"
/*0040c2d0*/ extern undefined __cdecl car_scale_velocity(undefined4 p0, undefined4 p1);
/*0040c320*/ extern undefined __cdecl car_push_from_matrix(undefined4 p0, undefined4 p1, undefined4 p2);
/*0040c3c0*/ extern undefined __cdecl car_apply_speed_boost_along_forward(undefined4 p0, undefined4 p1);
/*0040c480*/ extern undefined __cdecl car_wobble_tilt_update(int car);
/*0042d9a0*/ extern int store_u32();
/*0042d9b0*/ extern int rw_stream_chunk_copy();
/*0047b3a0*/ extern int car_wheel_get_field_88_b();
/*0047b3e0*/ extern int car_wheel_get_field_30();
/*005543d0*/ extern int rw_first_field(int * p0);
/*005dd340*/ extern float k_f_0_5;
/*00603578*/ extern undefined g_course_update_603578;
/*00652db8*/ extern float g_course_update_652db8;
/*0068b8f8*/ extern char g_race_phase_arg[];
/* course_editor_read_vec_chunk */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* "reads chunk 0x11/0x12 from the 0x68b8f8 stream into a table, then loops
   course_update_frame_sub_40c480" */

void course_editor_read_vec_chunk(void)

{
  float fbits1;
  int iVar1;
  int iVar2;
  int angle;
  int *piVar3;
  undefined *puVar4;
  struct {
    int local_14;
    int local_10 [4];
  } frame;
  
  iVar2 = rw_first_field(((char *)&g_race_phase_arg));
  if (iVar2 == 0x11) {
    puVar4 = &g_course_update_603578;
  }
  else {
    if (iVar2 != 0x12) goto LAB_0040c65d;
    puVar4 = (undefined *)&g_course_update_652db8;
  }
  rw_stream_chunk_copy(((char *)&g_race_phase_arg),puVar4);
  store_u32(((char *)&g_race_phase_arg),0);
LAB_0040c65d:
  angle = 0;
  do {
    car_wobble_tilt_update(angle);
    iVar2 = 0;
    piVar3 = frame.local_10;
    do {
      car_wheel_get_field_88_b(piVar3,angle,iVar2);
      car_wheel_get_field_30(&frame.local_14,angle,iVar2);
      if (frame.local_14 != 0) {
        if (*piVar3 == -0xff0001) break;
        if (*piVar3 == -0xfcfcfd) {
          car_scale_velocity(angle,0xbc23d70a);
        }
        iVar1 = *piVar3;
        if (iVar1 == -0xff01) {
          car_push_from_matrix(angle,0,(fbits1 = g_course_update_652db8 * k_f_0_5, *(int *)&fbits1));
          break;
        }
        if (iVar1 == -0xe5cc80) {
          car_apply_speed_boost_along_forward(angle,0xbc23d70a);
          break;
        }
        if (iVar1 == -0x42cf) break;
      }
      iVar2 = iVar2 + 1;
      piVar3 = piVar3 + 1;
    } while (iVar2 < 4);
    angle = (angle + 1);
    if (3 < angle) {
      return;
    }
  } while( true );
}


