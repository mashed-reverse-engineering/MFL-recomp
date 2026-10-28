// cluster-entries: car_apply_forces
/* Installed cluster car_apply_forces (tools/cluster_install.py from output/cluster_pilot_tr/car_apply_forces.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
#define SURFACE_ARGB_FFFF32FF (-0xcd01)
#define SURFACE_ARGB_FFA08080 (-0x5f7f80)
#define SURFACE_ARGB_FFAA8080 (-0x557f80)
#define SURFACE_ARGB_FF961E5A (-0x69e1a6)
#define SURFACE_ARGB_FF1E80B4 (-0xe17f4c)
#define SURFACE_ARGB_FFC81E5A (-0x37e1a6)
typedef int CourseId;
/*0042b110*/ extern int current_course_id();
/*00434bb0*/ extern int get_event_type();
/*0044c140*/ extern int __cdecl player_distance_warning(undefined4 p0);
/*0044c1a0*/ extern double camera_get_distance_factor(int index, float p1, float p2);
/*00481fd0*/ extern double random_range();
/*004d43f0*/ extern RwMatrix * RwMatrixRotate(RwMatrix *p0, struct RwV3d *p1, RwReal p2, enum RwOpCombineType p3);
/*004d5290*/ extern RwReal RwV3dNormalize(struct RwV3d *p0, struct RwV3d *p1);
/*004d53a0*/ extern RwReal RwV3dLength(struct RwV3d *p0);
/*004d56d0*/ extern struct RwV3d * RwV3dTransformVectors(struct RwV3d *p0, struct RwV3d *p1, RwInt32 p2, RwMatrix *p3);
/*005dd328*/ extern float k_f_0_6;
/*005dd32c*/ extern float k_f_3;
/*005dd330*/ extern float float_1_0;
/*005dd338*/ extern float k_f_0_01;
/*005dd340*/ extern float k_f_0_5;
/*005dd628*/ extern float k_f_0_25;
/*005dd634*/ extern float k_f_2;
/*005dd814*/ extern float k_f_6;
/*005dda1c*/ extern float k_f_3_3333333en4;
/*005ddaf0*/ extern float k_f_1_0en5;
/*005ddb10*/ extern float k_f_0_99;
/*005ddc1c*/ extern float k_f_0_33333334;
/*005ddc28*/ extern float k_f_0;
/*005dde3c*/ extern float k_f_3000;
/*005ddea0*/ extern float k_f_20;
/*005de19c*/ extern float k_f_1_0en4;
/*005de1ac*/ extern float k_f_0_125;
/*005de83c*/ extern float k_f_n20;
/*005df2c8*/ extern float k_f_0_002;
/*005dfd84*/ extern float k_f_65536;
/*005dfd88*/ extern float k_f_5_0en6;
/*005dfd8c*/ extern float k_f_0_0033333334;
/*005dfd90*/ extern float k_f_n0_15;
/*005dfd94*/ extern float k_f_n3_3333334en5;
/*005dfd98*/ extern float k_f_n30000;
/*005dfd9c*/ extern float k_f_60000;
/*006292e8*/ extern float g_car_apply_6292e8;
/*0062ac74*/ extern float g_car_62ac74;
/*0062ac80*/ extern undefined g_weapon_62ac80;
/*007ef7b8*/ extern undefined4 game_mode;
/*007ef7e0*/ extern undefined4 g_race_clock_b;
/*00801b1c*/ extern float g_gravity_vector;
/*00801b20*/ extern float g_cars_801b20;
/*00801b24*/ extern float g_cars_801b24;
/*00801b28*/ extern float g_cars_801b28;
/*008b06a0*/ extern undefined4 g_car_8b06a0;
/*008b06a4*/ extern float g_car_8b06a4;
/*008b06ac*/ extern undefined4 g_car_8b06ac;
/*008b06b0*/ extern float g_car_8b06b0;
/*008b06b8*/ extern undefined g_car_apply_8b06b8;
/*008b06bc*/ extern float g_car_apply_8b06bc;
/*008b06c8*/ extern float g_car_apply_8b06c8;
/*008b06e0*/ extern struct Car player_car[16];
/*008bd730*/ extern float g_car_8bd730;
/*008bd750*/ extern float g_cars_8bd750;

/* car_apply_forces */
#define COURSE_NT1_S (20)

/* per car: wheel anchor positions; speed factor from the proximity of other cars (catch-up style)
   and distance warning; adds gravity; wheel grip/drag forces */

static int triangle_normal_r(float * p0, float * p1, float * p2, float * p3);
extern void triangle_normal(void);
void car_apply_forces_n(Car *self_car, float dt, RwMatrix *car_matrix)

{
  uint matrix_slot;
  float tmp_f;
  float slip;
  uint index_tmp;
  int other_car_off;
  int other_car_matrix_off;
  CourseId course_id;
  RwV3d *wheel_ground_pos;
  float *wheel_pos_ptr;
  int other_car_byte_off;
  float *wheel_force_ptr;
  byte active_cars;
  float10 fl_tmp;
  RwReal other_speed;
  float scale_tmp;
  int surface_color;
  float slip3;
  float speed_keep;
  float raw_keep;
  volatile float base_load;      /* the original keeps this value in a stack slot (rounded to float) between its uses */
  int wheels_left;
  struct {
    float proximity_factor;
    float catchup_t;
    undefined1 pad_68[4];
    RwV3d rel_pos;
    RwV3d other_vel;
    RwV3d dir_to_other;
    RwMatrix wheel_rot_matrix;
  } frame;
  
  /* 1: rotate the forward vector and every wheel anchor into world space; count wheels in contact; wheel steering rotation */
  wheel_force_ptr = self_car->forward;
  RwV3dTransformVectors((RwV3d *)wheel_force_ptr,(RwV3d *)&g_weapon_62ac80,1,car_matrix);
  self_car->wheels_in_contact = 0.0;
  wheel_ground_pos = (RwV3d *)&g_car_8b06a0;
  wheel_pos_ptr = &self_car->wheels[0].pos_x;
  do {
    if (*(int *)&wheel_pos_ptr[0xb] != 0) {
      self_car->wheels_in_contact = self_car->wheels_in_contact + float_1_0;
    }
    wheel_ground_pos->x = *wheel_pos_ptr;
    wheel_ground_pos->y = 0.0;
    wheel_ground_pos->z = wheel_pos_ptr[2];
    RwV3dTransformVectors((RwV3d *)(wheel_pos_ptr + 5),wheel_ground_pos,1,car_matrix);
    if (wheel_pos_ptr[0xf] == k_f_0) {
      wheel_pos_ptr[0x2d] = *wheel_force_ptr;
      wheel_pos_ptr[0x2e] = self_car->forward[1];
      wheel_pos_ptr[0x2f] = self_car->forward[2];
    }
    else {
      RwMatrixRotate(&frame.wheel_rot_matrix,(RwV3d *)&g_car_62ac74,wheel_pos_ptr[0xf],rwCOMBINEREPLACE);
      RwV3dTransformVectors((RwV3d *)(wheel_pos_ptr + 0x2d),(RwV3d *)wheel_force_ptr,1,&frame.wheel_rot_matrix);
    }
    wheel_ground_pos = wheel_ground_pos + 1;
    wheel_pos_ptr = wheel_pos_ptr + 0x31;
  } while ((int)wheel_ground_pos < (int)&g_car_apply_8b06c8 + 0x8);
  index_tmp = self_car->velocity_history_index - 1U & 1;
  self_car->velocity_history_index = index_tmp;
  self_car->velocity_history[index_tmp * 3] = self_car->velocity[0];
  self_car->velocity_history[index_tmp * 3 + 1] = self_car->velocity[1];
  self_car->velocity_history[index_tmp * 3 + 2] = self_car->velocity[2];
  /* 2: base drag factor: speed * scale, halved when not all 4 wheels touch the ground */
  scale_tmp = self_car->speed;
  if (self_car->speed < k_f_1_0en4) {
    scale_tmp = k_f_0;
  }
  scale_tmp = *(float *)((char *)self_car + 0x150) * self_car->speed_scale * *(float *)((char *)self_car + 0x158)
           * scale_tmp;
  if (*(int *)&self_car->wheels_in_contact != 0x40800000 /* 4.0f as an int word */) {
    scale_tmp = scale_tmp * k_f_0_5;
  }
  /* 3: slow down when another active car is close ahead on the same heading (rubber band), factor 1 - (6 - dist) / 8 */
  index_tmp = 0;
  frame.proximity_factor = 1.0;
  other_car_byte_off = 0;
  scale_tmp = scale_tmp * self_car->time_step_const * dt * k_f_3_3333333en4;
  do {
    if ((index_tmp != self_car->index) &&
       (*(int *)((int)player_car[0].effect_level + other_car_byte_off + -0x1c) == 1)) {
      matrix_slot = self_car->matrix_current;
      other_car_off = *(int *)((int)player_car[0].velocity + other_car_byte_off + -8) * 0x40;
      other_car_matrix_off = other_car_off + other_car_byte_off;
      frame.rel_pos.x = *(float *)(other_car_off + 0x8b1038 + other_car_byte_off) - self_car->matrices[matrix_slot * 0x10 + 0xc];
      frame.rel_pos.y = *(float *)((int)player_car[0].matrices + other_car_matrix_off + 0x34) -
                   self_car->matrices[matrix_slot * 0x10 + 0xd];
      frame.other_vel.x = *(float *)((int)player_car[0].velocity + other_car_byte_off);
      frame.rel_pos.z = *(float *)((int)player_car[0].matrices + other_car_matrix_off + 0x38) -
                   self_car->matrices[matrix_slot * 0x10 + 0xe];
      frame.other_vel.y = *(float *)((int)player_car[0].velocity + other_car_byte_off + 4);
      frame.other_vel.z = *(float *)((int)player_car[0].velocity + other_car_byte_off + 8);
      other_speed = RwV3dLength(&frame.other_vel);
      if (!(k_f_1_0en4 > other_speed)) {
        RwV3dNormalize(&frame.other_vel,&frame.other_vel);
        RwV3dNormalize(&frame.dir_to_other,&frame.rel_pos);
        if (((k_f_0_99 < frame.dir_to_other.z * frame.other_vel.z + frame.dir_to_other.x * frame.other_vel.x + frame.dir_to_other.y * frame.other_vel.y
             ) && (other_speed = RwV3dLength(&frame.rel_pos), other_speed < k_f_6)) &&
           (tmp_f = float_1_0 - (k_f_6 - other_speed) * k_f_0_125, tmp_f < frame.proximity_factor)) {
          frame.proximity_factor = tmp_f;
        }
      }
    }
    other_car_byte_off = other_car_byte_off + 0xd04;
    index_tmp = index_tmp + 1;
  } while (other_car_byte_off < 0x3410);
  /* 4: no proximity slowdown in races / fugitive chase; survival: 0.75 for non-human cars */
  if (game_mode == GAME_MODE_RACE) {
    frame.proximity_factor = 1.0;
  }
  else if (game_mode == GAME_MODE_CHASE_FUGITIVE) {
    frame.proximity_factor = 1.0;
  }
  else if ((game_mode == GAME_MODE_SURVIVAL) && (self_car->index != 0)) {
    frame.proximity_factor = 0.75;
  }
  other_car_byte_off = get_event_type();
  if ((other_car_byte_off == EVENT_TRAINING) && (self_car->index == 1)) {
    frame.proximity_factor = float_1_0;
  }
  /* 5: catch-up factor from the race clock and the best / worst vehicle id of the players */
  tmp_f = k_f_60000 - (float)(int)g_race_clock_b;
  if (!(k_f_0 > tmp_f)) {
    frame.catchup_t = 1.0;
  }
  else {
    if (tmp_f < k_f_n30000) {
      tmp_f = k_f_n30000;
    }
    frame.catchup_t = float_1_0 - tmp_f * k_f_n3_3333334en5;
  }
  other_car_byte_off = 0;
  if ((player_car[0].active != 0) && (0 < player_car[0].vehicle_id)) {
    other_car_byte_off = player_car[0].vehicle_id;
  }
  if ((player_car[1].active != 0) && (other_car_byte_off < player_car[1].vehicle_id)) {
    other_car_byte_off = player_car[1].vehicle_id;
  }
  if ((player_car[2].active != 0) && (other_car_byte_off < player_car[2].vehicle_id)) {
    other_car_byte_off = player_car[2].vehicle_id;
  }
  if ((player_car[3].active != 0) && (other_car_byte_off < player_car[3].vehicle_id)) {
    other_car_byte_off = player_car[3].vehicle_id;
  }
  tmp_f = k_f_n0_15;
  if ((((other_car_byte_off != 0) && (tmp_f = k_f_0, other_car_byte_off != 1)) && (tmp_f = k_f_0_5, other_car_byte_off != 2)) &&
     (tmp_f = k_f_0, other_car_byte_off == 3)) {
    tmp_f = float_1_0;
  }
  if (frame.catchup_t == k_f_0) {
    other_car_byte_off = 3;
    if ((player_car[0].active != 0) && (player_car[0].vehicle_id < 3)) {
      other_car_byte_off = player_car[0].vehicle_id;
    }
    if ((player_car[1].active != 0) && (player_car[1].vehicle_id < other_car_byte_off)) {
      other_car_byte_off = player_car[1].vehicle_id;
    }
    if ((player_car[2].active != 0) && (player_car[2].vehicle_id < other_car_byte_off)) {
      other_car_byte_off = player_car[2].vehicle_id;
    }
    if ((player_car[3].active != 0) && (player_car[3].vehicle_id < other_car_byte_off)) {
      other_car_byte_off = player_car[3].vehicle_id;
    }
    if (other_car_byte_off == 0) {
      frame.catchup_t = 1.0;
      tmp_f = k_f_n0_15;
    }
  }
  frame.proximity_factor = (tmp_f * frame.catchup_t + float_1_0) * frame.proximity_factor;
  fl_tmp = (float10)camera_get_distance_factor(self_car->index,frame.proximity_factor,frame.catchup_t);
  frame.proximity_factor = (float)(fl_tmp * (float10)frame.proximity_factor);
  if ((k_f_0 < frame.catchup_t) && (other_car_byte_off = player_distance_warning(self_car->index), other_car_byte_off != 0)) {
    active_cars = player_car[0].active != 0;
    if (player_car[1].active != 0) {
      active_cars = active_cars + 1;
    }
    if (player_car[2].active != 0) {
      active_cars = active_cars + 1;
    }
    if (player_car[3].active != 0) {
      active_cars = active_cars + 1;
    }
    if (2 < active_cars) {
      frame.proximity_factor = frame.proximity_factor * k_f_0_5;
    }
  }
  /* 6: drag: keep factor clamped to 0..1 applied to velocity, then gravity along g_gravity_vector scaled by speed */
  raw_keep = float_1_0 - frame.proximity_factor * scale_tmp;
  speed_keep = ((raw_keep < k_f_0) || (float_1_0 < raw_keep)) ? k_f_0 : raw_keep;
  self_car->velocity[0] = speed_keep * self_car->velocity[0];
  self_car->velocity[1] = speed_keep * self_car->velocity[1];
  self_car->velocity[2] = speed_keep * self_car->velocity[2];
  frame.rel_pos.z = dt * self_car->speed * k_f_1_0en5;
  if (k_f_2 < frame.rel_pos.z) {
    frame.rel_pos.z = k_f_2;
  }
  frame.rel_pos.z = frame.rel_pos.z * g_cars_801b28;
  frame.rel_pos.x = g_gravity_vector * frame.rel_pos.z;
  frame.rel_pos.y = g_cars_801b20 * frame.rel_pos.z;
  frame.rel_pos.z = g_cars_801b24 * frame.rel_pos.z;
  self_car->velocity[0] = frame.rel_pos.x + self_car->velocity[0];
  self_car->velocity[1] = frame.rel_pos.y + self_car->velocity[1];
  self_car->velocity[2] = frame.rel_pos.z + self_car->velocity[2];
  if (self_car->wheels_in_contact != k_f_0) {
    other_car_byte_off = self_car->velocity_history_index;
    index_tmp = other_car_byte_off - 1U & 1;
    frame.rel_pos.x = self_car->velocity_history[other_car_byte_off * 3] - self_car->velocity_history[index_tmp * 3];      /* the original rounds the x and y differences to float, z stays in the register */
    frame.rel_pos.y = self_car->velocity_history[other_car_byte_off * 3 + 1] - self_car->velocity_history[index_tmp * 3 + 1];
    frame.rel_pos.z = self_car->velocity_history[other_car_byte_off * 3 + 2] - self_car->velocity_history[index_tmp * 3 + 2];
    frame.rel_pos.x = frame.rel_pos.x * dt;
    frame.rel_pos.y = frame.rel_pos.y * dt;
    frame.rel_pos.z = frame.rel_pos.z * dt;
    /* 7: per wheel load from the acceleration change since last frame (clamped to +-1/3 of base load), store wheel slip values */
    base_load = (self_car->mass / self_car->wheels_in_contact) * g_cars_8bd750;
    tmp_f = base_load * k_f_0_33333334;
    if (self_car->wheels[0].in_contact == 0) {
      self_car->wheels[0].load = 0.0;
    }
    else {
      slip = (*(float *)((char *)&self_car->wheels[0] + 0x20) * frame.rel_pos.z +
              (*(float *)((char *)&self_car->wheels[0] + 0x18) * frame.rel_pos.x +
               *(float *)((char *)&self_car->wheels[0] + 0x1c) * frame.rel_pos.y)) * k_f_0_002;
      if (tmp_f < slip) {
        slip = tmp_f;
      }
      if (slip < -tmp_f) {
        slip = -tmp_f;
      }
      self_car->wheels[0].load = base_load + slip;
      if (slip > k_f_20) {
        g_car_8b06a4 = k_f_20 * k_f_0_0033333334;
      }
      else {
        if (slip < k_f_n20) {
          slip = k_f_n20;
        }
        g_car_8b06a4 = slip * k_f_0_0033333334;
      }
    }
    if (self_car->wheels[1].in_contact == 0) {
      self_car->wheels[1].load = 0.0;
    }
    else {
      slip = (*(float *)((char *)&self_car->wheels[1] + 0x20) * frame.rel_pos.z +
              (*(float *)((char *)&self_car->wheels[1] + 0x18) * frame.rel_pos.x +
               *(float *)((char *)&self_car->wheels[1] + 0x1c) * frame.rel_pos.y)) * k_f_0_002;
      if (tmp_f < slip) {
        slip = tmp_f;
      }
      if (slip < -tmp_f) {
        slip = -tmp_f;
      }
      self_car->wheels[1].load = base_load + slip;
      if (slip > k_f_20) {
        g_car_8b06b0 = k_f_20 * k_f_0_0033333334;
      }
      else {
        if (slip < k_f_n20) {
          slip = k_f_n20;
        }
        g_car_8b06b0 = slip * k_f_0_0033333334;
      }
    }
    if (self_car->wheels[2].in_contact == 0) {
      self_car->wheels[2].load = 0.0;
    }
    else {
      slip =
           (*(float *)((char *)&self_car->wheels[2] + 0x20) * frame.rel_pos.z +
              (*(float *)((char *)&self_car->wheels[2] + 0x18) * frame.rel_pos.x +
               *(float *)((char *)&self_car->wheels[2] + 0x1c) * frame.rel_pos.y)) * k_f_0_002;
      if (tmp_f < slip) {
        slip = tmp_f;
      }
      if (slip < -tmp_f) {
        slip = -tmp_f;
      }
      self_car->wheels[2].load = base_load + slip;
      if (slip > k_f_20) {
        g_car_apply_8b06bc = k_f_20 * k_f_0_0033333334;
      }
      else {
        if (slip < k_f_n20) {
          slip = k_f_n20;
        }
        g_car_apply_8b06bc = slip * k_f_0_0033333334;
      }
    }
    if (self_car->wheels[3].in_contact == 0) {
      self_car->wheels[3].load = 0.0;
    }
    else {
      slip3 =
           (*(float *)((char *)&self_car->wheels[3] + 0x20) * frame.rel_pos.z +
              (*(float *)((char *)&self_car->wheels[3] + 0x18) * frame.rel_pos.x +
               *(float *)((char *)&self_car->wheels[3] + 0x1c) * frame.rel_pos.y)) * k_f_0_002;
      if (tmp_f < slip3) {
        slip3 = tmp_f;
      }
      if (slip3 < -tmp_f) {
        slip3 = -tmp_f;
      }
      self_car->wheels[3].load = base_load + slip3;
      if (slip3 > k_f_20) {
        g_car_apply_8b06c8 = k_f_20 * k_f_0_0033333334;
      }
      else {
        if (slip3 < k_f_n20) {
          slip3 = k_f_n20;
        }
        g_car_apply_8b06c8 = slip3 * k_f_0_0033333334;
      }
    }
    self_car->wheels[3].load = self_car->wheels[3].load * k_f_0_5;
    index_tmp = *(int *)((char *)self_car + 0xbec) + 1U & 0xf;
    *(uint *)((char *)self_car + 0xbec) = index_tmp;
    triangle_normal_r(&g_car_8b06a0,&g_car_8b06ac,&g_car_apply_8b06b8,
                    self_car->field_b00 + index_tmp * 3 + 0xb);
    if (self_car->field_b00[index_tmp * 3 + 0xd] < k_f_0) {
      self_car->field_b00[index_tmp * 3 + 0xd] = self_car->field_b00[index_tmp * 3 + 0xd] * k_f_0_25;
    }
  }
  /* 8: per wheel grip / drag forces from the surface type (pfVar[-1]) and the wheel load */
  wheel_force_ptr = (float *)((char *)&self_car->wheels[0] + 0x8c);
  wheels_left = 4;
  do {
    if (*(int *)&wheel_force_ptr[-0x17] == 0) {
      wheel_force_ptr[10] = 0.0;
      wheel_force_ptr[9] = 0.0;
      wheel_force_ptr[8] = 0.0;
    }
    else {
      surface_color = *(int *)&wheel_force_ptr[-1]; /* int ARGB word of the surface, not a float */
      *wheel_force_ptr = 0.25;
      self_car->field_b00[0] = 0.0;
      self_car->field_b00[1] = 0.0;
      self_car->field_b00[2] = 0.0;
      if (surface_color == SURFACE_ARGB_FFFF32FF) {
        scale_tmp = self_car->speed * k_f_5_0en6;
        fl_tmp = (float10)random_range(-scale_tmp,*(int *)&scale_tmp);
        self_car->field_b00[0] = (float)fl_tmp;
        self_car->field_b00[1] = 0.0;
        fl_tmp = (float10)random_range(-scale_tmp,*(int *)&scale_tmp);
        self_car->field_b00[2] = (float)fl_tmp;
      }
      else if (surface_color == SURFACE_ARGB_FFA08080) {
        *wheel_force_ptr = 0.1;
      }
      else if (surface_color == SURFACE_ARGB_FFAA8080) {
        *wheel_force_ptr = 0.01;
      }
      else {
        if ((surface_color == SURFACE_ARGB_FF961E5A) || (surface_color == SURFACE_ARGB_FF1E80B4)) {
          if (self_car->is_bot == 0) {
            *wheel_force_ptr = 0.01;
            course_id = current_course_id();
            if (course_id == COURSE_NT1_S) {
              *wheel_force_ptr = 0.1;
            }
          }
          else {
            *wheel_force_ptr = 0.2;
          }
        }
        if (surface_color == SURFACE_ARGB_FFC81E5A) {
          *wheel_force_ptr = 0.2;
        }
      }
      if (self_car->effect_level[3] != 0) {
        *wheel_force_ptr = (float)(int)self_car->effect_level[3] * *wheel_force_ptr * k_f_0_01;
      }
      scale_tmp = wheel_force_ptr[-0x1a] * *wheel_force_ptr;
      *wheel_force_ptr = scale_tmp;
      wheel_force_ptr[1] = k_f_0_6 * scale_tmp;
      wheel_force_ptr[2] = g_car_apply_6292e8 * scale_tmp;
      if (self_car->controls_disabled == 1) {
        wheel_force_ptr[1] = scale_tmp;
        wheel_force_ptr[2] = scale_tmp;
      }
      wheel_force_ptr[7] = wheel_force_ptr[6] * wheel_force_ptr[4];
      if (wheel_force_ptr[6] * wheel_force_ptr[4] < k_f_0) {
        wheel_force_ptr[7] = 0.0;
      }
      if (*(int *)&self_car->wheels_in_contact == 0x40800000 /* 4.0f as an int word */) {
        wheel_ground_pos = (RwV3d *)(wheel_force_ptr + 8);
        scale_tmp = g_car_8bd730 * wheel_force_ptr[6] * k_f_0_5;
        wheel_ground_pos->x = scale_tmp * wheel_force_ptr[3];
        wheel_force_ptr[9] = (wheel_force_ptr[4] - float_1_0) * scale_tmp;
        wheel_force_ptr[10] = scale_tmp * wheel_force_ptr[5];
        scale_tmp = RwV3dLength(wheel_ground_pos);
        if (scale_tmp != k_f_0) {
          frame.rel_pos.z = float_1_0 / scale_tmp;
          frame.rel_pos.x = frame.rel_pos.z * wheel_ground_pos->x;
          frame.rel_pos.y = frame.rel_pos.z * wheel_force_ptr[9];
          frame.rel_pos.z = frame.rel_pos.z * wheel_force_ptr[10];
          tmp_f = (frame.rel_pos.z * wheel_force_ptr[0xd] + frame.rel_pos.x * wheel_force_ptr[0xb] + frame.rel_pos.y * wheel_force_ptr[0xc]
                  ) * scale_tmp;
          wheel_ground_pos->x = tmp_f * wheel_force_ptr[0xb];
          wheel_force_ptr[9] = tmp_f * wheel_force_ptr[0xc];
          tmp_f = tmp_f * wheel_force_ptr[0xd];
          goto LAB_0047cad1;
        }
      }
      else {
        scale_tmp = (wheel_force_ptr[6] / dt) * g_car_8bd730;
        wheel_force_ptr[8] = scale_tmp * wheel_force_ptr[3];
        wheel_force_ptr[9] = scale_tmp * wheel_force_ptr[4];
        tmp_f = scale_tmp * wheel_force_ptr[5];
LAB_0047cad1:
        wheel_force_ptr[10] = tmp_f;
      }
      if ((*(int *)((char *)self_car + 0xb20) == 0) && (k_f_65536 < scale_tmp)) {
        *(undefined4 *)((char *)self_car + 0xb20) = 1;
      }
    }
    scale_tmp = g_cars_8bd750;
    wheel_force_ptr = wheel_force_ptr + 0x31;
    wheels_left = wheels_left + -1;
    if (wheels_left == 0) {
      if (self_car->wheels_in_contact < k_f_3) {
        *(undefined4 *)((char *)self_car + 0xb20) = 1;
        *(float *)((char *)self_car + 0xb18) =
             *(float *)((char *)self_car + 0xb18) - scale_tmp * self_car->mass * k_f_3000;
      }
      return;
    }
  } while( true );
}

/* triangle_normal_r: C -> original triangle_normal (registers eax,edx,ecx,esi) */
__declspec(naked) int triangle_normal_r(float * p0, float * p1, float * p2, float * p3)
{
    __asm {
        push esi
        mov eax, dword ptr [esp + 8]
        mov edx, dword ptr [esp + 12]
        mov ecx, dword ptr [esp + 16]
        mov esi, dword ptr [esp + 20]
        call triangle_normal
        pop esi
        ret
    }
}

/* entry thunk at the original address of car_apply_forces: register inputs (edi,s0,s1) -> natural C car_apply_forces_n; ret 0 */
__declspec(naked) void car_apply_forces(void)
{
    __asm {
        push dword ptr [esp + 8]
        push dword ptr [esp + 8]
        push edi
        call car_apply_forces_n
        add esp, 12
        ret
    }
}
