#include "mfl_tier2.h"
/*00416e20*/ extern int __cdecl point_project_onto_line_2d(int car_index, float *a, float *b, float *c, float scale, float *out);
/*00416fb0*/ extern int __cdecl ai_bot_weapon_logic(undefined4 p0, void *p1, void *p2, void *p3, undefined4 p4, void *p5);
/*004220f0*/ extern int vehicle_get_position_158();
/*0044c180*/ extern double player_distance_get();
/*0047b520*/ extern int car_current_matrix();
/*004967d0*/ extern int triggers_get_array();
/*0049cb60*/ extern int sphere_vs_box_test(float *p1, float *p2);
/*004d53a0*/ extern RwReal RwV3dLength(struct RwV3d *p0);
/*005dd340*/ extern float k_f_0_5;
/*005ddc1c*/ extern float k_f_0_33333334;
/*005dd36c*/ extern float k_f_0_4;
/*005dda24*/ extern float k_f_0_75;
/*005ddb50*/ extern float k_f_8;
/*005de230*/ extern float k_f_n0_75;
/*005de234*/ extern float k_f_0_875;
/*005e64a4*/ extern float k_f_0;
/* ai_trigger_zone_target_for_car */

/* "scans the trigger array for type 7 boxes the car is in and projects a target onto the line" */

undefined4 __cdecl ai_trigger_zone_target_for_car(undefined4 unused_1,char *out_target,char *out_aux,undefined4 car_index)

{
  float fbits1;
  float tmp_f;
  int type_id;
  char *owner;
  int tmp_i;
  float *trigger_ptr;
  float *copy_src;
  float *copy_dst;
  double player_dist;
  RwReal dist;
  struct {
    float zone_height;
    char *car_mtx;
    int trigger_i;
    int trigger_count;
    float zone_x;
    float zone_z;
    undefined4 car_x;
    undefined4 car_z;
    float car_pos_x;
    float car_pos_y;
    float car_pos_z;
    RwV3d to_zone;
    undefined1 car_pos158 [16];
    float zone_box [5];
    float zone_box_end;
  } frame;
  
  car_current_matrix(&frame.car_mtx,car_index);
  frame.car_pos_x = *(float *)(frame.car_mtx + 0x30);
  frame.car_pos_y = *(float *)(frame.car_mtx + 0x34);
  frame.car_pos_z = *(float *)(frame.car_mtx + 0x38);
  tmp_i = triggers_get_array(&frame.trigger_count);
  frame.trigger_i = 0;
  if (0 < frame.trigger_count) {
    trigger_ptr = (float *)(tmp_i + 0xc);
    do {
      type_id = *(int *)&trigger_ptr[1];
      if (type_id < 0xff00) {
        if (type_id == 21) {
          owner = *(char **)&trigger_ptr[2];
          vehicle_get_position_158(car_index,frame.car_pos158);
          copy_src = (float *)(owner + 0x68);
          copy_dst = frame.zone_box;
          for (tmp_i = 6; tmp_i != 0; tmp_i = tmp_i + -1) {
            *copy_dst = *copy_src;
            copy_src = copy_src + 1;
            copy_dst = copy_dst + 1;
          }
          tmp_f = (frame.zone_box[2] - frame.zone_box_end) * k_f_0_875;
          frame.zone_box[2] = frame.zone_box[2] + tmp_f;
          frame.zone_box_end = frame.zone_box_end - tmp_f;
          tmp_i = sphere_vs_box_test(frame.zone_box,(float *)frame.car_pos158);
          if (tmp_i == 1) {
            frame.car_x = *(undefined4 *)(frame.car_mtx + 0x30);
            frame.car_z = *(undefined4 *)(frame.car_mtx + 0x38);
            frame.zone_x = trigger_ptr[-3];
            frame.zone_z = trigger_ptr[-1];
            tmp_i = point_project_onto_line_2d(car_index,(float *)&frame.car_x,(float *)out_target,&frame.zone_x,*trigger_ptr * k_f_0_33333334 + k_f_0_5,(float *)out_aux);
joined_r0x00417a28:
            if (tmp_i != 0) {
              return 1;
            }
          }
        }
        else if ((((type_id != 10) || (*(int *)&trigger_ptr[2] == 0)) ||
                 (tmp_i = *(int *)(*(int *)&trigger_ptr[2] + 4), tmp_i == 0)) ||
                !(*(float *)(tmp_i + 0x10) < k_f_0_4))) {      /* NaN enters (original: test ah,5; jnp) */
          frame.zone_height = *trigger_ptr;
          if (type_id == 13) {
            frame.zone_height = frame.zone_height + frame.zone_height;
          }
          frame.to_zone.x = frame.car_pos_x - trigger_ptr[-3];
          frame.to_zone.y = frame.car_pos_y - trigger_ptr[-2];
          frame.to_zone.z = frame.car_pos_z - trigger_ptr[-1];
          dist = RwV3dLength(&frame.to_zone);
          if ((frame.to_zone.y < frame.zone_height) && (dist < k_f_8)) {
            frame.car_x = *(undefined4 *)(frame.car_mtx + 0x30);
            frame.car_z = *(undefined4 *)(frame.car_mtx + 0x38);
            frame.zone_x = trigger_ptr[-3];
            frame.zone_z = trigger_ptr[-1];
            if (*(int *)&trigger_ptr[1] != 7) {
              tmp_i = point_project_onto_line_2d(car_index,(float *)&frame.car_x,(float *)out_target,&frame.zone_x,frame.zone_height + k_f_0_5,(float *)out_aux);
              goto joined_r0x00417a28;
            }
            player_dist = (double)player_distance_get(car_index);
            if (((player_dist == k_f_0) &&
                (tmp_i = ai_bot_weapon_logic(car_index,&frame.car_x,out_target,&frame.zone_x,(fbits1 = frame.zone_height + k_f_0_5, *(int *)&fbits1),
                                             out_aux), tmp_i != 0)) &&
               ((tmp_f = *(float *)(frame.car_mtx + 0x34) - trigger_ptr[-2], k_f_n0_75 < tmp_f &&
                (tmp_f < k_f_0_75)))) {
              return 2;
            }
          }
        }
      }
      frame.trigger_i = frame.trigger_i + 1;
      trigger_ptr = trigger_ptr + 0x23;
    } while (frame.trigger_i < frame.trigger_count);
  }
  return 0;
}


