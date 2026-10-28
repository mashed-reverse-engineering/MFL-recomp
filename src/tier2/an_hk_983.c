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

/* "reads chunk 0x11/0x12 from the 0x68b8f8 stream into a table, then loops
   course_update_frame_sub_40c480" */

/* Reads a chunk id from the race stream (0x11 or 0x12) into a course table, then for each of 4 cars updates tilt and scans its 4 wheels' surface colours to apply a velocity scale / push / speed boost (inferred). */
void course_editor_read_vec_chunk(void)

{
  float half_speed_bits;
  int matched_color;
  int chunk_id;
  int car_index;
  int *wheel_color_ptr;
  undefined *chunk_dest;
  struct {
    int wheel_valid;
    int local_10 [4];
  } frame;
  
  chunk_id = rw_first_field(((char *)&g_race_phase_arg));
  if (chunk_id == 0x11) {
    chunk_dest = &g_course_update_603578;
  }
  else {
    if (chunk_id != 0x12) goto LAB_0040c65d;
    chunk_dest = (undefined *)&g_course_update_652db8;
  }
  rw_stream_chunk_copy(((char *)&g_race_phase_arg),chunk_dest);
  store_u32(((char *)&g_race_phase_arg),0);
LAB_0040c65d:
  car_index = 0;
  do {
    car_wobble_tilt_update(car_index);
    chunk_id = 0;
    wheel_color_ptr = frame.local_10;
    do {
      car_wheel_get_field_88_b(wheel_color_ptr,car_index,chunk_id);
      car_wheel_get_field_30(&frame.wheel_valid,car_index,chunk_id);
      if (frame.wheel_valid != 0) {
        if (*wheel_color_ptr == -0xff0001) break;
        if (*wheel_color_ptr == -0xfcfcfd) {
          car_scale_velocity(car_index,0xbc23d70a);
        }
        matched_color = *wheel_color_ptr;
        if (matched_color == -0xff01) {
          car_push_from_matrix(car_index,0,(half_speed_bits = g_course_update_652db8 * k_f_0_5, *(int *)&half_speed_bits));
          break;
        }
        if (matched_color == -0xe5cc80) {
          car_apply_speed_boost_along_forward(car_index,0xbc23d70a);
          break;
        }
        if (matched_color == -0x42cf) break;
      }
      chunk_id = chunk_id + 1;
      wheel_color_ptr = wheel_color_ptr + 1;
    } while (chunk_id < 4);
    car_index = (car_index + 1);
    if (3 < car_index) {
      return;
    }
  } while( true );
}


