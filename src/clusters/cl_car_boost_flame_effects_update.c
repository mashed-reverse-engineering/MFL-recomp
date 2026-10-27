// cluster-entries: car_boost_flame_effects_update
/* Installed cluster car_boost_flame_effects_update (tools/cluster_install.py from output/cluster_pilot_kb/car_boost_flame_effects_update.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
typedef int CourseId;
/*00422320*/ extern int vehicle_get_matrix_local();
/*00422350*/ extern int vehicle_part_ltm();
/*0042b110*/ extern enum CourseId current_course_id();
/*0047ac90*/ extern int car_get_int_from_float_field();
/*0047b360*/ extern int car_wheel_get_field_88_a();
/*0047b720*/ extern int car_get_field_b0c();
/*00487be0*/ extern int effect_slot_alloc();
/*00487c60*/ extern int effect_trail_add_point();
/*00487e80*/ extern int effect_pool_set_field_14();
/*005ddafc*/ extern float k_f_0_05;
/*005ddea4*/ extern float k_f_450;
/*0064d8a0*/ extern undefined4 g_race_64d8a0;
/*00652e48*/ extern undefined g_race_update_652e48;
/*00652e4c*/ extern undefined g_race_update_652e4c;
/*00652e50*/ extern undefined g_race_update_652e50;
/*00652e54*/ extern undefined g_race_update_652e54;
/*007ef804*/ extern undefined4 g_tick_number;
/* car_boost_flame_effects_update */
#define COURSE_STORM (36)

/* "for car p alternates 4 flame effect slots when speed >450 in mode 4 or on certain surfaces
   (course 0x24)" */

void car_boost_flame_effects_update(int car_index)

{
  bool boosting;
  CourseId course_id;
  int part_matrix;
  int flame_index;
  int *trail_slot;
  struct {
    int wheel_surface;
    int car_mode;
    float speed;
    undefined4 flame_pos_x;
    float flame_pos_y;
    undefined4 flame_pos_z;
    undefined1 vehicle_matrix [64];
  } frame;
  
  boosting = false;
  if (g_race_64d8a0 == 0) {
    car_get_field_b0c(&frame.speed,car_index);
    car_get_int_from_float_field(&frame.car_mode,car_index);
    if ((k_f_450 < frame.speed) && (frame.car_mode == 4)) {
      boosting = true;
    }
    car_wheel_get_field_88_a(car_index,0,&frame.wheel_surface);
    if (frame.wheel_surface == -0x7fbfc0) {
      boosting = false;
    }
    course_id = current_course_id();
    if ((((course_id == COURSE_STORM) && (frame.wheel_surface == -0x237f80)) && (frame.car_mode == 4)) || (boosting)) {
      flame_index = 0;
      trail_slot = (int *)((char *)&g_race_update_652e48 + car_index * 0x10);
      do {
        if (*trail_slot == 0) {
          part_matrix = effect_slot_alloc();
          *trail_slot = part_matrix;
          if (part_matrix != 0) {
            effect_pool_set_field_14(part_matrix,1);
          }
          if (*trail_slot != 0) goto LAB_0040f4b0;
        }
        else {
LAB_0040f4b0:
          if (((byte)g_tick_number & 2) != 0) {
            vehicle_get_matrix_local(car_index,frame.vehicle_matrix);
            part_matrix = vehicle_part_ltm(car_index,flame_index + 0x21);
            frame.flame_pos_x = *(undefined4 *)(part_matrix + 0x30);
            frame.flame_pos_z = *(undefined4 *)(part_matrix + 0x38);
            frame.flame_pos_y = *(float *)(part_matrix + 0x34) - k_f_0_05;
            part_matrix = effect_trail_add_point(*trail_slot,&frame.flame_pos_x,frame.vehicle_matrix,0x3da3d70a,0x40000000);
            *trail_slot = part_matrix;
          }
        }
        flame_index = flame_index + 1;
        trail_slot = trail_slot + 1;
        if (3 < flame_index) {
          return;
        }
      } while( true );
    }
    car_index = car_index * 0x10;
    *(undefined4 *)((char *)&g_race_update_652e48 + car_index) = 0;
    *(undefined4 *)((char *)&g_race_update_652e4c + car_index) = 0;
    *(undefined4 *)((char *)&g_race_update_652e50 + car_index) = 0;
    *(undefined4 *)((char *)&g_race_update_652e54 + car_index) = 0;
  }
  return;
}

