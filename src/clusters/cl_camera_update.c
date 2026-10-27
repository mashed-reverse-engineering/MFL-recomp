// cluster-entries: camera_update
/* Installed cluster camera_update (tools/cluster_install.py from output/cluster_pilot_md/camera_update.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
#define SECOND_CAR_MAX_GAP 0.03 /* a car counts as the runner-up only when it is less than this much lap counter behind the best car */
#define HARE_MIN_FOLLOW_DISTANCE 8.0 /* follow distance used when the focused car is the hare and the computed distance is below 1 */
typedef int CourseId;
/*00406cf0*/ extern int fx_objects_all_exploded();
/*00408a60*/ extern int fx_object_get_position();
/*00409eb0*/ extern double player_rank_key();
/*0040fbf0*/ extern int race_get_focus_player();
/*0040fdc0*/ extern int race_get_phase();
/*0040fe20*/ extern int __cdecl race_setup_slot_used(undefined4 p0);
/*00421fe0*/ extern int vehicle_set_field_198();
/*004221e0*/ extern int __cdecl hud_marker_get_distance(undefined4 p0, undefined4 p1, undefined4 p2);
/*0042b0c0*/ extern int course_get_ai_polygon_count();
/*0042b110*/ extern int current_course_id();
/*00434bb0*/ extern int get_event_type();
/*00437f10*/ extern int training_lesson_get();
/*0044a410*/ extern int camera_update_view_window();
/*0044a470*/ extern int camera_apply_rotation();
/*0044a530*/ extern undefined __cdecl camera_pick_ground_point(undefined4 p0, undefined4 p1, undefined4 p2);
/*0044ba80*/ extern int camera_records_init_for_course();
/*0044bc90*/ extern undefined __cdecl camera_record_add(undefined4 p0);
/*0044bee0*/ extern undefined __cdecl camera_apply_matrix(undefined4 p0);
/*0044bf20*/ extern int distances_update();
/*0044f9d0*/ extern undefined __cdecl camera_records_update_all(undefined4 p0);
/*00467ea0*/ extern int weapon_get_rw_801b14();
/*004682c0*/ extern int __cdecl rocket_surface_hit_test(void *p0, RwV3d *p1);
/*00475730*/ extern int get_viewport_camera();
/*004757a0*/ extern int get_viewport_camera_matrix();
/*0047a830*/ extern int car_is_active();
/*0047abb0*/ extern int car_get_velocity_any_mode();
/*0047ac30*/ extern int car_get_mode();
/*0047b520*/ extern int car_current_matrix();
/*00481fd0*/ extern double random_range(float lo, float hi);
/*004b2b48*/ extern int _ftol();
/*004b3284*/ extern double FUN_004b3284(double p0);
/*004c4460*/ extern void FUN_004c4460(undefined4 p0, undefined4 p1, undefined4 p2);
/*004c4de0*/ extern int world_intersect_line(RpWorld * p0, undefined4 p1, undefined4 p2);
/*004d43f0*/ extern RwMatrix * RwMatrixRotate(RwMatrix *p0, struct RwV3d *p1, RwReal p2, enum RwOpCombineType p3);
/*004d4870*/ extern RwMatrix * RwMatrixTranslate(RwMatrix *p0, struct RwV3d *p1, enum RwOpCombineType p2);
/*004d5290*/ extern RwReal RwV3dNormalize(struct RwV3d *p0, struct RwV3d *p1);
/*004d53a0*/ extern RwReal RwV3dLength(struct RwV3d *p0);
/*004d5410*/ extern RwReal _rwSqrt(RwReal p0);
/*004d56d0*/ extern struct RwV3d * RwV3dTransformVectors(struct RwV3d *p0, struct RwV3d *p1, RwInt32 p2, RwMatrix *p3);
/*005dd330*/ extern float float_1_0;
/*005dd340*/ extern float k_f_0_5;
/*005dd364*/ extern float k_f_n1;
/*005dd374*/ extern float k_f_0_2;
/*005dd378*/ extern float k_f_0_1;
/*005dd41c*/ extern float float_5_0;
/*005dd620*/ extern float float_10_0;
/*005dd628*/ extern float k_f_0_25;
/*005dd634*/ extern float k_f_2;
/*005ddacc*/ extern float k_f_360;
/*005ddad0*/ extern float k_f_180;
/*005ddad4*/ extern float k_f_90;
/*005ddafc*/ extern float k_f_0_05;
/*005ddb0c*/ extern float k_f_15;
/*005ddb18*/ extern float float_0_8;
/*005ddb58*/ extern float k_f_1000;
/*005ddc18*/ extern float k_f_1_1;
/*005ddc28*/ extern float k_f_0;
/*005de1d0*/ extern float k_f_1_25;
/*005de20c*/ extern float k_f_2_5;
/*005de290*/ extern float float_50_0;
/*005deb5c*/ extern float k_f_83;
/*005dec44*/ extern float k_f_155;
/*005df30c*/ extern float k_f_420;
/*005df310*/ extern float k_f_400;
/*005df454*/ extern float k_f_16_5;
/*005df458*/ extern float k_f_200;
/*005df45c*/ extern float k_f_33_9;
/*005df460*/ extern float k_f_139_5;
/*005df464*/ extern float k_f_175;
/*005df468*/ extern float k_f_635;
/*005df46c*/ extern float k_f_2_25;
/*005df470*/ extern float k_f_n2_5;
/*005de19c*/ extern float k_f_1_0en4;
/*007ef7d8*/ extern int g_camera_7ef7d8;
extern double sin(double);
extern double cos(double);
extern double atan(double);
/*005df474*/ extern float k_f_89_9;
/*005df478*/ extern float k_f_67_5;
/*005df47c*/ extern float k_f_n3_95;
/*005df480*/ extern float k_f_1_5en4;
/*005df484*/ extern float k_f_144;
/*005df488*/ extern float k_f_1_01;
/*0062ac68*/ extern float g_car_62ac68;
/*0062ac6c*/ extern float g_fx_62ac6c;
/*0062ac70*/ extern float g_fx_62ac70;
/*0062ac74*/ extern float g_car_62ac74;
/*0062ac78*/ extern float g_car_62ac78;
/*0062ac7c*/ extern float g_car_62ac7c;
/*007eebc0*/ extern struct GameState g_game_state;
/*007ef7ac*/ extern float g_camera_shake;
/*007ef7b8*/ extern undefined4 game_mode;
/*007ef7f4*/ extern float g_tick_seconds;
/*007ef818*/ extern undefined4 g_last_frame_ticks;
/*00801b2c*/ extern float g_camera_fov_factor;
/*008c7e0c*/ extern float g_camera_8c7e0c;
/*0041a370*/ extern float ai_get_counter();
#pragma optimize("g", off)   /* the original function is too large for cl's global optimizer: every temporary lives in a stack slot, divisions stay, sin / atan are CRT calls */
/* camera_update */
#define COURSE_CITY (26)

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* per-viewport camera: zoom (clamped to 10.0) -> tilt/height/distance; trainer patches its float
   operands */

/* NAMING NOTE: this function is compiled without global optimization, so the stack frame layout (hence the object code) depends on the local variable NAMES. The odd suffixes / prefixes (target_x3, eye_z6, a_look_z ...) were searched so that the object stays identical; only change a local name together with a readable_vs_head.py run. Legend: target_* = look-at point of the focus pair, eye_* = camera position, look_* = smoothed look-at, zoom = 0..10 spread factor, focus_a/focus_b = the two followed cars, vec_* = scratch vectors, tmp_f/tmp_g = scratch floats. */
/* Legend for the locals that keep Ghidra names (renaming them changes the stack layout and was tried: no name variant of local_f8 / local_f4 / local_d8 / local_114 / iVar3 keeps the object identical): local_f8 / local_f4 = eye x / y (eye_z6 is z); local_d8 = horizontal follow distance * 0.8 (copied to follow_dist_raw_e); local_114 = viewport camera of viewport 0; the ground ray of the trigger cast: cam.local_12c / local_128 / local_124 = ray start (camera position), cam.local_120 = ray direction (unit, y from the camera tilt), local_19c = ray end (start + direction * cast_len_f), local_18c / local_188 = ray start y / z copies (ray_start_x_x is x), local_184 / local_180 / local_17c = copy of the ray end; local_178 / local_174 / local_170 = axis vector (g_car_62ac68..70 or a computed one) passed to FUN_004c4460; iVar3 = scratch int for call results. */
void __cdecl camera_update(int *camera,int force_snap)

{
  float c_tmp_f1;
  float tmp_f2_;
  int iVar3;
  float *pos_ptr_c;
  undefined4 tmp_world;
  CourseId course6;
  float10 tmp_x87;
  RwReal new_length;
  undefined1 *hit_ptr2;
  int second_car_d;
  float best_delta3;
  int best_car5;
  float best_counter_;
  float marker_dist;
  float hit_dist4;
  int hit_found7;
  float cast_len_f;
  RwMatrix *view_matrix_d;
  RwV3d local_19c;
  float ray_start_x_x;
  float local_18c;
  float local_188;
  float local_184;
  float local_180;
  float local_17c;
  float local_178;
  undefined4 local_174;
  float local_170;
  struct {
    float hit[16];       /* hit record of world_intersect_line (0x40 bytes), local_134 = hit[14] */
    float local_12c;
    float local_128;
    float local_124;
    RwV3d local_120;
  } cam;
  undefined4 local_114;
  int next_index5;
  RwV3d n_vec_d;
  float ground_c7;
  float smooth_y7;
  float local_f8;
  float local_f4;
  float eye_z6;
  int runner_cars_e;
  float follow_dist_b9;
  float smooth_z;
  int is_runner_b;
  float blend_inv8;
  float local_d8;
  int same_pair6;
  int phase_e;
  RwV3d vec_c;
  float target_z_v;
  float zoom_t;
  float tmp_f7;
  float tmp_g_;
  float blend_z_d;
  RwMatrix rot_matrix_;
  int focus_b_;
  int the_car_matrix;
  float ground_b5;
  int runner_time_c;
  uint i2;
  RwV3d vec_b8;
  float smooth_x4;
  int active_cars5;
  float target_x3;
  float target_y3;
  float my_blend_x;
  float new_blend_y;
  float follow_dist_raw_e;
  int focus_a_n;
  float pitch_deg5;
  RwV3d vec_a_a;
  float ground_a8;
  float look_x4;
  float look_y_;
  float a_look_z;
  int slot_t;
  
  for (i2 = 0; (int)i2 < 4; i2 = i2 + 1) {
    vehicle_set_field_198(i2, 0x0);
  }
  *(float *)&camera[9] = g_camera_fov_factor;
  camera_records_init_for_course();
  phase_e = race_get_phase();
  if ((game_mode == 5) && (iVar3 = fx_objects_all_exploded(), iVar3 != 0)) {
    pos_ptr_c = (float *)fx_object_get_position(0x0);
    cam.local_12c = *pos_ptr_c;
    cam.local_128 = pos_ptr_c[1];
    cam.local_124 = pos_ptr_c[2];
    local_114 = get_viewport_camera(0x0);
    view_matrix_d = (RwMatrix *)get_viewport_camera_matrix(0x0);
    hit_found7 = 0;
    cast_len_f = 5.0;
    local_178 = g_car_62ac68;
    local_174 = g_fx_62ac6c;
    local_170 = (float)g_fx_62ac70;
    cam.local_120.x = 0.0;
    tmp_x87 = (float10)cos(g_camera_7ef7d8 * k_f_1_0en4);
    cam.local_120.y = (RwReal)(tmp_x87 * (float10)(0.30000001192092896) + (float10)(0.5));
    cam.local_120.z = 0.0;
    RwV3dNormalize(&cam.local_120,&cam.local_120);
    ray_start_x_x = cam.local_12c;
    local_18c = cam.local_128;
    local_188 = cam.local_124;
    local_19c.x = cam.local_120.x * cast_len_f + cam.local_12c;
    local_19c.y = cam.local_120.y * cast_len_f + cam.local_128;
    local_19c.z = cam.local_120.z * cast_len_f + cam.local_124;
    hit_ptr2 = cam.hit;
    pos_ptr_c = &ray_start_x_x;
    local_184 = local_19c.x;
    local_180 = local_19c.y;
    local_17c = local_19c.z;
    tmp_world = weapon_get_rw_801b14();
    hit_found7 = world_intersect_line((RpWorld *)tmp_world,(undefined4)pos_ptr_c,(undefined4)hit_ptr2);
    if (hit_found7 == 0) {
      if (*(float *)&camera[0x269] < k_f_2_5 != (*(float *)&camera[0x269] == k_f_2_5)) {
        camera[0x269] = 0x40200000;
      }
      if (cast_len_f <= *(float *)&camera[0x269]) {
        *(float *)&camera[0x269] = cast_len_f;
      }
      else {
        *(float *)&camera[0x269] = (*(float *)&camera[0x269] * k_f_1_01);
      }
    }
    else {
      hit_dist4 = 0.0;
      iVar3 = rocket_surface_hit_test(cam.hit,(RwV3d *)&cam.local_12c);
      if (iVar3 == 1) {
        hit_found7 = 0;
      }
      hit_dist4 = cam.hit[14] * cast_len_f;
      *(float *)&camera[0x269] = (hit_dist4 - k_f_0_1);
    }
    local_19c.x = *(float *)&camera[0x269] * cam.local_120.x + cam.local_12c;
    local_19c.y = *(float *)&camera[0x269] * cam.local_120.y + cam.local_128;
    local_19c.z = *(float *)&camera[0x269] * cam.local_120.z + cam.local_124;
    RwMatrixTranslate(view_matrix_d,&local_19c,rwCOMBINEREPLACE);
    tmp_x87 = (float10)sin(g_camera_7ef7d8 * k_f_1_0en4);
    local_178 = (float)tmp_x87;
    tmp_x87 = (float10)cos(g_camera_7ef7d8 * k_f_1_0en4);
    local_170 = (float)tmp_x87;
    FUN_004c4460((undefined4)view_matrix_d,(undefined4)&cam.local_12c,(undefined4)&local_178);
    camera[0x16] = 0x3f4ccccd;
    camera_update_view_window(camera);
  }
  else {
    camera_records_update_all((undefined4)camera);
    if (((phase_e == 6) && (iVar3 = get_event_type(), iVar3 != 0x12)) || (camera[7] == 0)) {
      for (i2 = 0; (int)i2 < 4; i2 = i2 + 1) {
        marker_dist = 0.0;
        hud_marker_get_distance((undefined4)i2,0,(undefined4)&marker_dist);
        if (marker_dist < k_f_144) {
          vehicle_set_field_198(i2, 0x1);
        }
        else {
          vehicle_set_field_198(i2, 0x2);
        }
      }
      iVar3 = get_event_type();
      if (((((iVar3 == 0) || (iVar3 = get_event_type(), iVar3 == 1)) ||
           (iVar3 = get_event_type(), iVar3 == 2)) || ((game_mode == 5 || (game_mode == 10)))) ||
         (game_mode == 9)) {
        vehicle_set_field_198(0, 0x0);
      }
      distances_update();
      race_get_focus_player(&focus_a_n, &focus_b_);
      if (game_mode == 7) {
        best_car5 = 0;
        best_counter_ = -1.0;
        best_delta3 = -1.0;
        for (i2 = 0; (int)i2 < 4; i2 = i2 + 1) {
          tmp_x87 = (float10)ai_get_counter(i2);
          if ((float10)best_counter_ < tmp_x87) {
            best_car5 = i2;
            best_counter_ = (float)tmp_x87;
          }
        }
        second_car_d = best_car5;
        for (i2 = 0; (int)i2 < 4; i2 = i2 + 1) {
          if ((i2 != best_car5) && (iVar3 = car_is_active(0x0), iVar3 == 1)) {
            tmp_x87 = (float10)ai_get_counter(i2);
            c_tmp_f1 = best_counter_ - (float)tmp_x87;
            if ((c_tmp_f1 < SECOND_CAR_MAX_GAP) && (best_delta3 < c_tmp_f1)) {
              second_car_d = i2;
              best_delta3 = c_tmp_f1;
            }
          }
        }
        focus_a_n = best_car5;
        focus_b_ = second_car_d;
      }
      if (game_mode == 4) {
        focus_b_ = 0;
        focus_a_n = 0;
      }
      if (game_mode == 9) {
        focus_b_ = 0;
        focus_a_n = 0;
      }
      if (game_mode == 8) {
        focus_b_ = 0;
        focus_a_n = 0;
      }
      car_current_matrix(&the_car_matrix, focus_a_n);
      target_x3 = *(float *)(the_car_matrix + 0x30);
      target_y3 = *(float *)(the_car_matrix + 0x34);
      target_z_v = *(float *)(the_car_matrix + 0x38);
      car_get_velocity_any_mode(&vec_a_a, focus_a_n);
      vec_a_a.x = vec_a_a.x * k_f_1_5en4;
      vec_a_a.y = vec_a_a.y * k_f_1_5en4;
      vec_a_a.z = vec_a_a.z * k_f_1_5en4;
      target_x3 = target_x3 + vec_a_a.x;
      target_z_v = target_z_v + vec_a_a.z;
      car_current_matrix(&the_car_matrix, focus_b_);
      vec_a_a.x = target_x3 - *(float *)(the_car_matrix + 0x30);
      vec_a_a.y = target_y3 - *(float *)(the_car_matrix + 0x34);
      vec_a_a.z = target_z_v - *(float *)(the_car_matrix + 0x38);
      car_get_velocity_any_mode(&vec_b8, focus_a_n);
      vec_b8.x = vec_b8.x * k_f_1_5en4;
      vec_b8.y = vec_b8.y * k_f_1_5en4;
      vec_b8.z = vec_b8.z * k_f_1_5en4;
      vec_c.x = vec_a_a.x - vec_b8.x;
      vec_c.z = vec_a_a.z - vec_b8.z;
      vec_c.y = 0.0;
      vec_a_a.x = vec_c.x;
      vec_a_a.z = vec_c.z;
      new_length = RwV3dLength(&vec_c);
      local_d8 = new_length * float_0_8;
      follow_dist_raw_e = local_d8;
      if (((focus_a_n == focus_b_) && (local_d8 < float_1_0)) &&
         (car_get_mode(focus_a_n,&is_runner_b, &runner_time_c), is_runner_b != 0)) {
        local_d8 = HARE_MIN_FOLLOW_DISTANCE;
        follow_dist_raw_e = HARE_MIN_FOLLOW_DISTANCE;
      }
      smooth_x4 = target_x3 - vec_a_a.x / k_f_2;
      smooth_y7 = target_y3 - vec_a_a.y / k_f_2;
      smooth_z = target_z_v - vec_a_a.z / k_f_2;
      same_pair6 = 0;
      if ((focus_a_n == camera[0x265]) && (focus_b_ == camera[0x266])) {
        same_pair6 = 1;
      }
      if ((focus_a_n == camera[0x266]) && (focus_b_ == camera[0x265])) {
        same_pair6 = 1;
      }
      if (force_snap != 0) {
        same_pair6 = 0;
      }
      target_z_v = smooth_z;
      target_x3 = smooth_x4;
      target_y3 = smooth_y7;
      if (same_pair6 == 0) {
        *(float *)&camera[0x267] = (g_tick_seconds * float_1_0 + *(float *)&camera[0x267]);
        if (*(float *)&camera[0x267] < float_1_0) {
          car_current_matrix(&the_car_matrix, *(int *)((char *)camera + 2452));
          my_blend_x = *(float *)(the_car_matrix + 0x30);
          new_blend_y = *(float *)(the_car_matrix + 0x34);
          blend_z_d = *(float *)(the_car_matrix + 0x38);
          car_current_matrix(&the_car_matrix, *(int *)((char *)camera + 2456));
          vec_c.x = my_blend_x - *(float *)(the_car_matrix + 0x30);
          vec_a_a.y = new_blend_y - *(float *)(the_car_matrix + 0x34);
          vec_c.z = blend_z_d - *(float *)(the_car_matrix + 0x38);
          vec_c.y = 0.0;
          vec_a_a.x = vec_c.x;
          vec_a_a.z = vec_c.z;
          new_length = RwV3dLength(&vec_c);
          follow_dist_b9 = new_length * float_0_8;
          my_blend_x = my_blend_x - vec_a_a.x / k_f_2;
          new_blend_y = new_blend_y - vec_a_a.y / k_f_2;
          blend_z_d = blend_z_d - vec_a_a.z / k_f_2;
          tmp_f7 = (*(float *)&camera[0x267] + *(float *)&camera[0x267]) - float_1_0;
          if (tmp_f7 < k_f_0) {
            tmp_f7 = 0.0;
          }
          blend_inv8 = float_1_0 - tmp_f7;
          target_x3 = my_blend_x * blend_inv8 + target_x3 * tmp_f7;
          target_y3 = new_blend_y * blend_inv8 + target_y3 * tmp_f7;
          target_z_v = blend_z_d * blend_inv8 + target_z_v * tmp_f7;
          if (follow_dist_b9 < local_d8) {
            *(float *)&camera[0x267] = (g_tick_seconds + g_tick_seconds + *(float *)&camera[0x267]);
          }
          local_d8 = follow_dist_b9 * blend_inv8 + local_d8 * tmp_f7;
        }
        else {
          camera[0x265] = focus_a_n;
          camera[0x266] = focus_b_;
        }
      }
      else {
        camera[0x267] = 0;
      }
      tmp_x87 = (float10)player_rank_key(focus_a_n);
      g_camera_8c7e0c = (float)tmp_x87;
      tmp_g_ = g_camera_8c7e0c;
      i2 = ((int)(tmp_g_));
      camera_pick_ground_point((undefined4)i2,(undefined4)&vec_a_a,(undefined4)&ground_a8);
      next_index5 = i2 + 1;
      iVar3 = course_get_ai_polygon_count(0x0);
      if (iVar3 <= next_index5) {
        next_index5 = 0;
      }
      camera_pick_ground_point((undefined4)next_index5,(undefined4)&vec_b8,(undefined4)&ground_b5);
      tmp_g_ = (tmp_g_ - (float)(int)i2);
      tmp_f7 = (float_1_0 - tmp_g_);
      n_vec_d.x = vec_b8.x * tmp_g_ + vec_a_a.x * tmp_f7;
      n_vec_d.y = vec_b8.y * tmp_g_ + vec_a_a.y * tmp_f7;
      n_vec_d.z = vec_b8.z * tmp_g_ + vec_a_a.z * tmp_f7;
      zoom_t = ground_b5 * tmp_g_ + ground_a8 * tmp_f7;
      tmp_x87 = (float10)player_rank_key(focus_b_);
      tmp_g_ = (float)tmp_x87;
      g_camera_8c7e0c = (g_camera_8c7e0c + tmp_g_) * k_f_0_5;
      i2 = ((int)(tmp_g_));
      camera_pick_ground_point((undefined4)i2,(undefined4)&vec_a_a,(undefined4)&ground_a8);
      next_index5 = i2 + 1;
      iVar3 = course_get_ai_polygon_count(0x0);
      if (iVar3 <= next_index5) {
        next_index5 = 0;
      }
      camera_pick_ground_point((undefined4)next_index5,(undefined4)&vec_b8,(undefined4)&ground_b5);
      tmp_g_ = (tmp_g_ - (float)(int)i2);
      tmp_f7 = (float_1_0 - tmp_g_);
      vec_c.x = vec_b8.x * tmp_g_ + vec_a_a.x * tmp_f7;
      vec_c.y = vec_b8.y * tmp_g_ + vec_a_a.y * tmp_f7;
      vec_c.z = vec_b8.z * tmp_g_ + vec_a_a.z * tmp_f7;
      ground_c7 = ground_b5 * tmp_g_ + ground_a8 * tmp_f7;
      zoom_t = (zoom_t + ground_c7) * k_f_0_5;
      vec_a_a.x = n_vec_d.x + vec_c.x;
      vec_a_a.y = n_vec_d.y + vec_c.y;
      vec_a_a.z = n_vec_d.z + vec_c.z;
      RwV3dNormalize(&vec_a_a,&vec_a_a);
      active_cars5 = 0;
      runner_cars_e = 0;
      for (slot_t = 0; slot_t < 4; slot_t = slot_t + 1) {
        iVar3 = race_setup_slot_used(slot_t);
        if ((iVar3 != 0) && (iVar3 = car_is_active(slot_t), iVar3 == 1)) {
          active_cars5 = active_cars5 + 1;
          car_get_mode(slot_t,&is_runner_b, &runner_time_c);
          if (is_runner_b != 0) {
            runner_cars_e = runner_cars_e + 1;
          }
        }
      }
      if ((active_cars5 == 1) && (runner_cars_e == 1)) {
        tmp_f7 = (float)runner_time_c / k_f_1000 + zoom_t;
        if (float_10_0 < tmp_f7) {
          tmp_f7 = 10.0;
        }
        zoom_t = tmp_f7;
      }
      zoom_t = zoom_t * k_f_1_1;
      if (game_mode == 8) {
        zoom_t = 4.0;
      }
      if (game_mode == 4) {
        zoom_t = 4.0;
      }
      if (game_mode == 5) {
        zoom_t = 2.0;
      }
      iVar3 = get_event_type();
      if (iVar3 == 0) {
        zoom_t = 4.0;
      }
      iVar3 = get_event_type();
      if (iVar3 == 1) {
        zoom_t = 4.0;
      }
      iVar3 = get_event_type();
      if (iVar3 == 2) {
        zoom_t = 4.0;
      }
      if (game_mode == 9) {
        zoom_t = 4.0;
      }
      if (game_mode == 10) {
        zoom_t = 4.0;
      }
      iVar3 = get_event_type();
      if ((iVar3 == 0x11) && (iVar3 = training_lesson_get(), iVar3 == 1)) {
        zoom_t = 4.0;
      }
      iVar3 = get_event_type();
      if ((iVar3 == 0x11) && (iVar3 = training_lesson_get(), iVar3 == 4)) {
        zoom_t = 4.0;
      }
      c_tmp_f1 = zoom_t;
      tmp_f7 = zoom_t;
      if (zoom_t < local_d8) {
        zoom_t = local_d8;
      }
      if (float_10_0 < zoom_t) {
        zoom_t = 10.0;
      }
      if (g_game_state.option_flags[3] != 0) {
        zoom_t = 10.0;
      }
      if (c_tmp_f1 < follow_dist_raw_e) {
        tmp_f7 = follow_dist_raw_e;
      }
      if (float_10_0 < tmp_f7) {
        tmp_f7 = 10.0;
      }
      *(float *)&camera[0x268] = tmp_f7;
      pitch_deg5 = (g_car_62ac74 * vec_a_a.x + g_car_62ac78 * vec_a_a.y + g_car_62ac7c * vec_a_a.z);
      if (pitch_deg5 < k_f_n1) {
        pitch_deg5 = -1.0;
      }
      if (float_1_0 < pitch_deg5) {
        pitch_deg5 = 1.0;
      }
      tmp_x87 = (float10)FUN_004b3284((double)pitch_deg5);
      pitch_deg5 = (float)((float10)k_f_90 - ((float10)k_f_180 - tmp_x87 * (float10)(57.295799255371094)));
      tmp_f7 = (k_f_n3_95 - (k_f_0_25 * zoom_t) / float_5_0);
      vec_a_a.x = vec_a_a.x * tmp_f7;
      vec_a_a.y = vec_a_a.y * tmp_f7;
      vec_a_a.z = vec_a_a.z * tmp_f7;
      RwV3dNormalize(&vec_b8,&vec_a_a);
      n_vec_d.x = vec_b8.y * g_car_62ac7c - vec_b8.z * g_car_62ac78;
      n_vec_d.y = vec_b8.z * g_car_62ac74 - vec_b8.x * g_car_62ac7c;
      n_vec_d.z = vec_b8.x * g_car_62ac78 - vec_b8.y * g_car_62ac74;
      tmp_f7 = (float_50_0 * zoom_t) / float_10_0;
      course6 = current_course_id();
      if (course6 == COURSE_CITY) {
        tmp_f7 = (k_f_67_5 * zoom_t) / float_10_0;
        zoom_t = zoom_t * k_f_1_25;
      }
      tmp_f7 = tmp_f7 - float_5_0;
      if (k_f_89_9 <= tmp_f7 + pitch_deg5) {
        tmp_f7 = k_f_89_9 - pitch_deg5;
      }
      if (g_game_state.option_flags[3] != 0) {
        tmp_f7 = 90.0;
      }
      RwMatrixRotate(&rot_matrix_,&n_vec_d,tmp_f7,rwCOMBINEREPLACE);
      RwV3dTransformVectors(&vec_a_a,&vec_a_a,1,&rot_matrix_);
      tmp_f7 = ((zoom_t / float_0_8) / float_5_0 + float_1_0);
      vec_a_a.x = vec_a_a.x * tmp_f7;
      vec_a_a.y = vec_a_a.y * tmp_f7;
      vec_a_a.z = vec_a_a.z * tmp_f7;
      local_f8 = vec_a_a.x + target_x3;
      local_f4 = vec_a_a.y + target_y3;
      eye_z6 = vec_a_a.z + target_z_v;
      if (g_game_state.option_flags[3] != 0) {
        local_f8 = target_x3;
        local_f4 = target_y3 + k_f_15;
        eye_z6 = target_z_v;
      }
      vec_a_a.y = 0.0;
      RwV3dNormalize(&vec_a_a,&vec_a_a);
      tmp_f7 = ((float_10_0 - zoom_t) * k_f_n2_5) / float_10_0;
      tmp_f7 = (k_f_2_5 * zoom_t) / float_10_0 + tmp_f7;
      course6 = current_course_id();
      if (course6 == COURSE_CITY) {
        tmp_f7 = tmp_f7 - (k_f_2_25 * zoom_t) / float_10_0;
      }
      if (g_game_state.option_flags[3] == 0) {
        vec_a_a.x = vec_a_a.x * tmp_f7;
        vec_a_a.y = vec_a_a.y * tmp_f7;
        vec_a_a.z = vec_a_a.z * tmp_f7;
        target_x3 = target_x3 + vec_a_a.x;
        target_y3 = target_y3 + vec_a_a.y;
        target_z_v = target_z_v + vec_a_a.z;
        local_f8 = local_f8 + vec_a_a.x;
        local_f4 = local_f4 + vec_a_a.y;
        eye_z6 = eye_z6 + vec_a_a.z;
      }
      same_pair6 = 0;
      vec_a_a.x = local_f8 - *(float *)&camera[0x259];
      vec_a_a.y = local_f4 - *(float *)&camera[0x25a];
      vec_a_a.z = eye_z6 - *(float *)&camera[0x25b];
      tmp_f7 = RwV3dLength(&vec_a_a);
      if (float_5_0 < tmp_f7) {
        same_pair6 = 1;
      }
      if ((force_snap != 0) || (same_pair6 != 0)) {
        *(float *)&camera[0x259] = local_f8;
        *(float *)&camera[0x25a] = local_f4;
        *(float *)&camera[0x25b] = eye_z6;
        camera[0x25e] = 0;
        camera[0x25d] = 0;
        camera[0x25c] = 0;
        smooth_x4 = target_x3;
        *(float *)&camera[0x25f] = target_x3;
        smooth_y7 = target_y3;
        *(float *)&camera[0x260] = target_y3;
        smooth_z = target_z_v;
        *(float *)&camera[0x261] = target_z_v;
        camera[0x264] = 0;
        camera[0x263] = 0;
        camera[0x262] = 0;
        camera[0x265] = focus_a_n;
        camera[0x266] = focus_b_;
        camera[0x267] = 0;
      }
      /* spring-damper smoothing of the camera position (smoothed position camera[0x259..0x25b], velocity [0x25c..0x25e])
         and of the look-at point (position [0x25f..0x261], velocity [0x262..0x264]); every vector step is stored (VC7 without global optimization) */
      vec_a_a.x = local_f8 - *(float *)&camera[0x259];
      vec_a_a.y = local_f4 - *(float *)&camera[0x25a];
      vec_a_a.z = eye_z6 - *(float *)&camera[0x25b];
      vec_a_a.x = vec_a_a.x * k_f_0_05;
      vec_a_a.y = vec_a_a.y * k_f_0_05;
      vec_a_a.z = vec_a_a.z * k_f_0_05;
      *(float *)&camera[0x25c] = *(float *)&camera[0x25c] + vec_a_a.x;
      *(float *)&camera[0x25d] = *(float *)&camera[0x25d] + vec_a_a.y;
      *(float *)&camera[0x25e] = *(float *)&camera[0x25e] + vec_a_a.z;
      *(float *)&camera[0x25c] = *(float *)&camera[0x25c] * float_0_8;
      *(float *)&camera[0x25d] = *(float *)&camera[0x25d] * float_0_8;
      *(float *)&camera[0x25e] = *(float *)&camera[0x25e] * float_0_8;
      vec_a_a.x = *(float *)&camera[0x259] + *(float *)&camera[0x25c];
      vec_a_a.y = *(float *)&camera[0x25a] + *(float *)&camera[0x25d];
      vec_a_a.z = *(float *)&camera[0x25b] + *(float *)&camera[0x25e];
      vec_b8.x = local_f8 - vec_a_a.x;
      vec_b8.y = local_f4 - vec_a_a.y;
      vec_b8.z = eye_z6 - vec_a_a.z;
      vec_b8.x = ((float_10_0 - zoom_t) / float_10_0) * vec_b8.x;
      vec_b8.y = ((float_10_0 - zoom_t) / float_10_0) * vec_b8.y;
      vec_b8.z = ((float_10_0 - zoom_t) / float_10_0) * vec_b8.z;
      local_f8 = local_f8 - vec_b8.x;
      local_f4 = local_f4 - vec_b8.y;
      eye_z6 = eye_z6 - vec_b8.z;
      look_x4 = target_x3;
      look_y_ = target_y3;
      a_look_z = target_z_v;
      vec_a_a.x = look_x4 - *(float *)&camera[0x25f];
      vec_a_a.y = look_y_ - *(float *)&camera[0x260];
      vec_a_a.z = a_look_z - *(float *)&camera[0x261];
      vec_a_a.x = vec_a_a.x * k_f_0_05;
      vec_a_a.y = vec_a_a.y * k_f_0_05;
      vec_a_a.z = vec_a_a.z * k_f_0_05;
      *(float *)&camera[0x262] = *(float *)&camera[0x262] + vec_a_a.x;
      *(float *)&camera[0x263] = *(float *)&camera[0x263] + vec_a_a.y;
      *(float *)&camera[0x264] = *(float *)&camera[0x264] + vec_a_a.z;
      *(float *)&camera[0x262] = *(float *)&camera[0x262] * float_0_8;
      *(float *)&camera[0x263] = *(float *)&camera[0x263] * float_0_8;
      *(float *)&camera[0x264] = *(float *)&camera[0x264] * float_0_8;
      vec_a_a.x = *(float *)&camera[0x25f] + *(float *)&camera[0x262];
      vec_a_a.y = *(float *)&camera[0x260] + *(float *)&camera[0x263];
      vec_a_a.z = *(float *)&camera[0x261] + *(float *)&camera[0x264];
      vec_b8.x = ((float_10_0 - zoom_t) / float_10_0) * (target_x3 - vec_a_a.x);
      vec_b8.y = ((float_10_0 - zoom_t) / float_10_0) * (target_y3 - vec_a_a.y);
      vec_b8.z = ((float_10_0 - zoom_t) / float_10_0) * (target_z_v - vec_a_a.z);
      look_x4 = target_x3 - vec_b8.x;
      look_y_ = target_y3 - vec_b8.y;
      a_look_z = target_z_v - vec_b8.z;
      if (g_game_state.option_flags[3] == 0) {
        i2 = g_last_frame_ticks / 60000;
        blend_inv8 = (zoom_t + k_f_0_2) / float_5_0;
        tmp_f7 = (float)(int)i2;
        tmp_f7 = tmp_f7 / k_f_420;
        tmp_f7 = (float)sin(tmp_f7);
        tmp_f7 = tmp_f7 / float_10_0;
        tmp_f7 = tmp_f7 * blend_inv8;
        look_x4 = look_x4 + tmp_f7;
        tmp_f7 = (float)(int)i2;
        tmp_f7 = tmp_f7 / k_f_635;
        tmp_f7 = (float)sin(tmp_f7);
        tmp_f7 = tmp_f7 / float_10_0;
        tmp_f7 = tmp_f7 * blend_inv8;
        a_look_z = a_look_z + tmp_f7;
        tmp_f7 = (float)(int)i2;
        tmp_f7 = tmp_f7 / k_f_400;
        tmp_f7 = (float)sin(tmp_f7);
        tmp_f7 = tmp_f7 / float_5_0;
        tmp_f7 = tmp_f7 * blend_inv8;
        local_f8 = local_f8 + tmp_f7;
        tmp_f7 = (float)(int)i2;
        tmp_f7 = tmp_f7 / k_f_155;
        tmp_f7 = (float)sin(tmp_f7);
        tmp_f7 = tmp_f7 / float_5_0;
        tmp_f7 = tmp_f7 * blend_inv8;
        local_f4 = local_f4 + tmp_f7;
        tmp_f7 = (float)(int)i2;
        tmp_f7 = tmp_f7 / k_f_175;
        tmp_f7 = (float)sin(tmp_f7);
        tmp_f7 = tmp_f7 / float_5_0;
        tmp_f7 = tmp_f7 * blend_inv8;
        eye_z6 = eye_z6 + tmp_f7;
        tmp_f7 = (float)(int)i2;
        tmp_f7 = tmp_f7 / k_f_83;
        tmp_f7 = (float)sin(tmp_f7);
        tmp_f7 = tmp_f7 / float_10_0;
        tmp_f7 = tmp_f7 * blend_inv8;
        local_f8 = local_f8 + tmp_f7;
        tmp_f7 = (float)(int)i2;
        tmp_f7 = tmp_f7 / k_f_139_5;
        tmp_f7 = (float)sin(tmp_f7);
        tmp_f7 = tmp_f7 / float_10_0;
        tmp_f7 = tmp_f7 * blend_inv8;
        eye_z6 = eye_z6 + tmp_f7;
        tmp_f7 = (float)(int)i2;
        tmp_f7 = tmp_f7 / k_f_33_9;
        tmp_f7 = (float)sin(tmp_f7);
        tmp_f7 = tmp_f7 / k_f_200;
        tmp_f7 = tmp_f7 * blend_inv8;
        local_f8 = local_f8 + tmp_f7;
        tmp_f7 = (float)(int)i2;
        tmp_f7 = tmp_f7 / k_f_16_5;
        tmp_f7 = (float)sin(tmp_f7);
        tmp_f7 = tmp_f7 / k_f_200;
        tmp_f7 = tmp_f7 * blend_inv8;
        eye_z6 = eye_z6 + tmp_f7;
      }
      tmp_x87 = (float10)random_range(-g_camera_shake, g_camera_shake);
      look_x4 = (float)(tmp_x87 + (float10)look_x4);
      tmp_x87 = (float10)random_range(-g_camera_shake, g_camera_shake);
      look_y_ = (float)(tmp_x87 + (float10)look_y_);
      tmp_x87 = (float10)random_range(-g_camera_shake, g_camera_shake);
      a_look_z = (float)(tmp_x87 + (float10)a_look_z);
      *(float *)&camera[0x10] = local_f8;
      *(float *)&camera[0x11] = local_f4;
      *(float *)&camera[0x12] = eye_z6;
      *(float *)&camera[0x259] = local_f8;
      *(float *)&camera[0x25a] = local_f4;
      *(float *)&camera[0x25b] = eye_z6;
      *(float *)&camera[0x25f] = look_x4;
      *(float *)&camera[0x260] = look_y_;
      *(float *)&camera[0x261] = a_look_z;
      target_x3 = look_x4 - *(float *)&camera[0x10];
      target_y3 = look_y_ - *(float *)&camera[0x11];
      target_z_v = a_look_z - *(float *)&camera[0x12];
      *(float *)&camera[0x13] = target_x3;
      *(float *)&camera[0x14] = target_y3;
      *(float *)&camera[0x15] = target_z_v;
      camera[0x16] = 0x3f19999a;
      if (target_z_v == k_f_0) {
        if (!(target_x3 < k_f_0)) {      /* NaN takes the 90 branch (as atan2_degrees) */
          tmp_g_ = 90.0;
        }
        else {
          tmp_g_ = 270.0;
        }
      }
      else {
        tmp_g_ = target_x3 / target_z_v;
        tmp_x87 = (float10)atan(tmp_g_);
        tmp_g_ = k_f_180 - (float)-(tmp_x87 * (float10)(57.295799255371094));
        if (k_f_0 < target_z_v) {
          tmp_g_ = k_f_180 + tmp_g_;
        }
      }
      *(float *)&camera[0xe] = tmp_g_;
      target_x3 = target_x3 * target_x3;
      target_z_v = target_z_v * target_z_v + target_x3;
      if (target_z_v != k_f_0) {
        target_z_v = _rwSqrt(target_z_v);
      }
      if (target_z_v == k_f_0) {
        if (!(target_y3 < k_f_0)) {      /* NaN takes the 90 branch (as atan2_degrees) */
          tmp_g_ = 90.0;
        }
        else {
          tmp_g_ = 270.0;
        }
      }
      else {
        tmp_g_ = target_y3 / target_z_v;
        tmp_x87 = (float10)atan(tmp_g_);
        tmp_g_ = k_f_180 - (float)-(tmp_x87 * (float10)(57.295799255371094));
        if (k_f_0 < target_z_v) {
          tmp_g_ = k_f_180 + tmp_g_;
        }
      }
      *(float *)&camera[0xd] = (k_f_360 - tmp_g_);
      camera[0xf] = 0;
      if (g_game_state.option_flags[3] != 0) {
        camera[0xe] = 0;
      }
      camera_apply_rotation(camera);
      camera[1] = camera[0x10];
      camera[2] = camera[0x11];
      camera[3] = camera[0x12];
      *(float *)&camera[4] = look_x4;
      *(float *)&camera[5] = look_y_;
      *(float *)&camera[6] = a_look_z;
      if (*camera != 0) {
        camera_apply_matrix((undefined4)camera);
      }
    }
    else {
      camera_record_add((undefined4)camera);
    }
  }
  return;
}

