#include "mfl_tier2.h"
/*0040e000*/ extern undefined __cdecl skid_mark_spawn_from_wheel(void *p0, void *p1, undefined4 p2, undefined4 p3);
/*0040fdc0*/ extern int race_get_phase();
/*00422070*/ extern int vehicle_is_active();
/*00422320*/ extern int vehicle_get_matrix_local();
/*00422350*/ extern int vehicle_part_ltm();
/*00469d20*/ extern int sound_get_8d9960();
/*004757a0*/ extern int get_viewport_camera_matrix();
/*0047a830*/ extern int car_is_active();
/*0047abb0*/ extern int car_get_velocity_any_mode();
/*0047ac90*/ extern int car_get_int_from_float_field();
/*0047b3a0*/ extern int car_wheel_get_field_88_b();
/*0047b5f0*/ extern int car_get_drift_angle(float *param_2, uint param_3);
/*0047b720*/ extern int car_get_field_b0c();
/*00480060*/ extern int hud_label_add_world_text();
/*00481fd0*/ extern double random_range();
/*004989a0*/ extern int fx_explosion_spawn();
/*004d53a0*/ extern RwReal RwV3dLength(struct RwV3d *p0);
/*005dd374*/ extern float k_f_0_2;
/*005dd378*/ extern float k_f_0_1;
/*005dde48*/ extern float k_f_0_33;
/*005dde4c*/ extern float k_f_n0_33;
/*005dde50*/ extern float k_f_5_0en5;
/*00652dc0*/ extern undefined4 g_effects_reset_652dc0;
/*007eebb4*/ extern undefined g_race_7eebb4;
/*007ef804*/ extern undefined4 g_tick_number;
/*007f023c*/ extern undefined4 g_ai_editor_mode[0x1000];
/*008b06e0*/ extern struct Car player_car[16];
/* cars_exhaust_and_damage_fx_update */

/* "per car (4) with the car mode >1: velocity, wheel/part matrices, spawns exhaust/damage puffs
   with random 20% chance and colour table" */

void cars_exhaust_and_damage_fx_update(void)

{
  int *car_flag_ptr;
  float puff_alpha;
  int tmp;
  int vehicle_active;
  uint tick_phase;
  int *damage_ptr;
  uint wheel;
  uint car_index;
  undefined4 *car_field_ptr;
  int fx_flag;      /* 1 when the random draw is above 0.2, later whether the race phase is 6 (the original compares it with 2 in between) */
  double rand_val;
  uint wheel_arg;
  struct {
    int *damage_base;
    undefined1 damage_colours [24];
    undefined4 *car_flag_base;
    int race_phase;
    float puff_alpha_copy;
    undefined4 *car_field_ptr_saved;
    undefined4 wheel_field88;
    int car_mode;
    int wheel_ltm;
    undefined4 wheel_field88_copy;
    uint car_index_copy;
    uint wheel_copy;
    RwV3d *velocity_ptr;
    undefined4 field_b0c;
    undefined1 drift_angle [4];
    undefined4 car_field_val;
    RwReal speed;
    undefined4 car_flag_val;
    RwV3d velocity;
    undefined1 car_matrix [48];
    undefined1 car_matrix_pos [16];
  } frame;
  
  car_index = 4;
  frame.race_phase = race_get_phase();
  frame.car_flag_base = g_ai_editor_mode;
  frame.damage_base = (int *)&g_race_7eebb4;
  car_field_ptr = (undefined4 *)&player_car[4].field_0xcfc;
  do {
    car_flag_ptr = frame.car_flag_base + -4;
    frame.car_flag_base = frame.car_flag_base + -4;
    car_field_ptr = car_field_ptr + -0x341;
    damage_ptr = frame.damage_base + -3;
    car_index = car_index - 1;
    if (-1 < *car_flag_ptr) {
      frame.car_field_ptr_saved = car_field_ptr;
      rand_val = (double)random_range(0,0x3f800000);
      fx_flag = (double)k_f_0_2 < rand_val;
      wheel = 0;
      car_get_int_from_float_field(&frame.car_mode,car_index);
      if ((1 < frame.car_mode) && (tmp = car_is_active(car_index), tmp != 0)) {
        car_get_velocity_any_mode(&frame.velocity,car_index);
        vehicle_part_ltm(car_index,0x13);
        frame.car_index_copy = car_index;
        car_get_field_b0c(&frame.field_b0c,car_index);
        car_get_drift_angle((float *)frame.drift_angle,car_index);
        frame.car_field_val = *car_field_ptr;
        frame.velocity_ptr = &frame.velocity;
        frame.speed = RwV3dLength(&frame.velocity);
        tmp = sound_get_8d9960(car_index);
        vehicle_active = vehicle_is_active(car_index);
        if ((vehicle_active == 1) && (tmp != 0)) {
          fx_explosion_spawn(car_index,5,0x3e4ccccd,0x3dcccccd);
          fx_explosion_spawn(car_index,6,0x3e4ccccd,0x3dcccccd);
        }
        if (*damage_ptr == 0) {
LAB_0040e42d:
          if (fx_flag == 2) goto LAB_0040e434;
        }
        else {
          tmp = *damage_ptr + -0x18;
          *damage_ptr = tmp;
          if (tmp < 0) {
            *damage_ptr = 0;
          }
          if (frame.damage_base[-2] != 2) {
            puff_alpha = (float)*damage_ptr * k_f_5_0en5 + k_f_0_1;
            frame.puff_alpha_copy = puff_alpha;
            fx_explosion_spawn(car_index,5,0x3f4ccccd,*(int *)&puff_alpha);
            fx_explosion_spawn(car_index,6,0x3f4ccccd,*(int *)&puff_alpha);
            frame.damage_colours[0] = 0xff;
            frame.damage_colours[1] = 0;
            frame.damage_colours[2] = 0;
            frame.damage_colours[3] = 200;
            frame.damage_colours[4] = 0;
            frame.damage_colours[5] = 0;
            frame.damage_colours[6] = 0xff;
            frame.damage_colours[7] = 200;
            frame.damage_colours[8] = 0;
            frame.damage_colours[9] = 0xff;
            frame.damage_colours[10] = 0;
            frame.damage_colours[0xb] = 200;
            frame.damage_colours[0xc] = 0xff;
            frame.damage_colours[0xd] = 0xff;
            frame.damage_colours[0xe] = 0;
            frame.damage_colours[0xf] = 200;
            frame.damage_colours[0x10] = 0xff;
            frame.damage_colours[0x11] = 0;
            frame.damage_colours[0x12] = 0xff;
            frame.damage_colours[0x13] = 200;
            frame.damage_colours[0x14] = 0;
            frame.damage_colours[0x15] = 0;
            frame.damage_colours[0x16] = 0;
            frame.damage_colours[0x17] = 200;
            if ((*(int *)((char *)((undefined *)&g_effects_reset_652dc0) + car_index * 4) == 0) && (0x76b < *damage_ptr)) {
              vehicle_get_matrix_local(car_index,frame.car_matrix);
              hud_label_add_world_text
                        (0x32c,frame.car_matrix_pos,frame.damage_colours + frame.car_flag_base[2] * 4,frame.damage_colours + frame.car_flag_base[2] * 4,
                         0x41c00000,0x3f800000);
              *(undefined4 *)((char *)((undefined *)&g_effects_reset_652dc0) + car_index * 4) = 1;
            }
            goto LAB_0040e42d;
          }
          frame.field_b0c = 0x43480000;
LAB_0040e434:
          fx_flag = frame.race_phase == 6;
          frame.speed = 500.0;
        }
        tmp = get_viewport_camera_matrix(0);
        if ((k_f_n0_33 < *(float *)(tmp + 0x24)) && (*(float *)(tmp + 0x24) < k_f_0_33)) {
          tick_phase = g_tick_number & 0x80000003;
          if ((int)tick_phase < 0) {
            tick_phase = (tick_phase - 1 | 0xfffffffc) + 1;
          }
          fx_flag = tick_phase == 0;
        }
        frame.car_flag_val = *frame.car_flag_base;
        do {
          if (((((fx_flag != 0) && (wheel == (g_tick_number & 3))) || (frame.race_phase == 8)) || (frame.race_phase == 7))
             && (tmp = vehicle_part_ltm(car_index,wheel + 0x1d), tmp != 0)) {
            car_field_ptr = &frame.wheel_field88;
            tick_phase = car_index;
            wheel_arg = wheel;
            car_wheel_get_field_88_b(&frame.wheel_field88, tick_phase, wheel_arg);
            frame.wheel_field88_copy = frame.wheel_field88;
            frame.wheel_ltm = tmp;
            frame.wheel_copy = wheel;
            skid_mark_spawn_from_wheel(&frame.wheel_ltm,car_field_ptr,tick_phase,wheel_arg);
          }
          wheel = wheel + 1;
          car_field_ptr = frame.car_field_ptr_saved;
        } while ((int)wheel < 4);
      }
    }
    frame.damage_base = damage_ptr;
    if (car_field_ptr == (undefined4 *)&player_car[0].field_0xcfc) {
      return;
    }
  } while( true );
}


