// cluster-entries: course_falling_objects_update
/* Installed cluster course_falling_objects_update (tools/cluster_install.py from output/cluster_pilot_j1/course_falling_objects_update.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
typedef signed char sbyte;
/*00472250*/ extern int sound_voices_set_position_wide();
/*00475760*/ extern int aux_camera_get_position();
/*00482010*/ extern int random_int_range();
/*004866d0*/ extern int fx_emitter_spawn_b_default();
/*00487670*/ extern int effect_sprite_spawn();
/*00496850*/ extern int trigger_sphere_register(float *param_1);
/*004980c0*/ extern int fx_trail_particle_spawn_interpolated();
/*0049a850*/ extern int fx_dust_add();
/*004c42f0*/ extern void FUN_004c42f0(undefined4 p0, undefined4 p1, undefined4 p2);
/*004c45c0*/ extern int rp_random_int_range(int p0, int p1);
/*004c45e0*/ extern float rp_random_float_range(undefined4 p0, undefined4 p1);
/*004d1460*/ extern struct RwFrame * RwFrameTranslate(struct RwFrame *p0, struct RwV3d *p1, enum RwOpCombineType p2);
/*004d1640*/ extern struct RwFrame * RwFrameRotate(struct RwFrame *p0, struct RwV3d *p1, RwReal p2, enum RwOpCombineType p3);
/*005dd330*/ extern float float_1_0;
/*005dd340*/ extern float k_f_0_5;
/*005dd41c*/ extern float float_5_0;
/*005ddb60*/ extern float k_f_n8;
/*005ddc28*/ extern float k_f_0;
/*005dece4*/ extern float k_f_20000;
/*005dfd04*/ extern float k_f_0_5_5dfd04;
/*0062ac68*/ extern float g_car_62ac68;
/*0062ac74*/ extern float g_car_62ac74;
/*006af5c0*/ extern undefined1 g_car_6af5c0;
/*006af734*/ extern undefined g_effects_update_6af734;
/*007ef7f4*/ extern float g_tick_seconds;
/*007ef804*/ extern undefined4 g_tick_number;
/* course_falling_objects_update */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* "per active falling object integrates velocity, rotates, spawns burst/dust on ground contact and
   removes it" */

#pragma auto_inline(off)
static float float_barrier(float v) { return v; }
#pragma auto_inline(on)

void course_falling_objects_update(void)

{
  float fbits1;
  float old_x, old_y, old_z, yn, zn;
  double xn;
  float fbits2;
  RwV3d *obj_pos;
  RwFrame *frame;
  float age_or_dx;
  float dz_cam;
  float dy_cam;
  char random_int;
  float *cam_pos;
  uint bit_index;
  float *obj;
  float10 rand_val;
  struct {
    char burst_r;
    char burst_g;
    char burst_b;
    undefined1 burst_a;
    float half_dt;
    uint group_base_index;
    RwV3d frame_delta;
    int slots_left;
    float *group_objs_ptr;
    int *group_live_word_ptr;
    uint slot_index;
    byte *live_byte_ptr;
    uint live_bit;
    float unused_58;
    float unused_54;
    float unused_50;
    float unused_4c;
    RwReal trail_pos_x;
    RwReal trail_pos_y;
    RwReal trail_pos_z;
    char trail_r;
    char trail_g;
    char trail_b;
    undefined1 trail_a;
    float trail_size_w;
    float trail_size_h;
    float trail_vel_x;
    undefined4 trail_vel_y;
    float trail_vel_z;
    undefined4 trail_accel_x;
    undefined4 trail_accel_y;
    undefined4 trail_accel_z;
    undefined4 trail_life_scale;
    undefined1 trail_field_34[4];
    undefined4 trail_field_38;
    undefined1 trail_field_3c[4];
    undefined4 trail_flags;
    undefined1 trail_flag_44[1];
    undefined1 trail_kind;
  } frame_obj;   /* trail_pos_x .. trail_kind: the 0x48 byte particle record of fx_trail_particle_spawn_interpolated (layout as FxParticle in cl_fx_dust_particle_spawn.c) */
  
  /* 8 groups of 32 falling objects (live bitmap g_car_6af5c0); each object record is 0x4c floats */
  frame_obj.group_base_index = 0;
  frame_obj.group_objs_ptr = (float *)&g_effects_update_6af734;
  frame_obj.group_live_word_ptr = (int *)&g_car_6af5c0;
  do {
    if (*frame_obj.group_live_word_ptr != 0) {
      frame_obj.slots_left = 0x20;
      frame_obj.slot_index = frame_obj.group_base_index;
      obj = frame_obj.group_objs_ptr;
      do {
        bit_index = frame_obj.slot_index & 7;
        frame_obj.live_byte_ptr = &g_car_6af5c0 + ((int)frame_obj.slot_index >> 3);
        if ((*frame_obj.live_byte_ptr & (byte)(1 << (sbyte)bit_index)) != 0) {
          frame_obj.live_bit = bit_index;
          /* object not launched yet (timer 0): register its trigger sphere and raise dust */
          if (obj[-0x12] == k_f_0) {
            trigger_sphere_register(obj + -0x37);
            fx_dust_add(obj + 5,0x80);
          }
          else {
            frame = *(RwFrame **)(*(int *)&obj[-0x14] + 4);
            old_x = *obj;
            old_y = obj[1];
            old_z = obj[2];
            obj_pos = &RwFrame_modelling(frame).pos;
            /* integrate: gravity (0, -8, 0) * dt, move the frame and spin it */
            xn = (double)g_tick_seconds * (double)k_f_0 + (double)old_x;
            *obj = (float)xn;
            yn = float_barrier((double)g_tick_seconds * (double)k_f_n8 + (double)old_y);
            obj[1] = yn;
            zn = float_barrier((double)g_tick_seconds * (double)k_f_0 + (double)old_z);
            obj[2] = zn;
            frame_obj.half_dt = g_tick_seconds * k_f_0_5;
            frame_obj.frame_delta.x = ((double)g_tick_seconds * (double)k_f_0_5) * (xn + (double)old_x);
            frame_obj.frame_delta.y = frame_obj.half_dt * ((double)yn + (double)old_y);
            frame_obj.frame_delta.z = frame_obj.half_dt * float_barrier((double)zn + (double)old_z);
            RwFrameTranslate(frame,&frame_obj.frame_delta,rwCOMBINEPOSTCONCAT);
            RwFrameRotate(frame,(RwV3d *)&g_car_62ac68,obj[3],rwCOMBINEPRECONCAT);
            age_or_dx = g_tick_seconds + obj[-0x12];
            obj[-0x12] = age_or_dx;
            obj[-0x13] = age_or_dx * k_f_0_5_5dfd04;
            /* below ground: impact burst, sound, debris sprite, hide the object and free the slot */
            if (obj[1] < k_f_0) {
              frame_obj.burst_r = -1;
              frame_obj.burst_g = -0x80;
              frame_obj.burst_b = '\0';
              frame_obj.burst_a = 0x78;
              cam_pos = (float *)aux_camera_get_position(0);
              rand_val = (float10)rp_random_float_range(0x3f4ccccd,0x3f99999a);
              fx_emitter_spawn_b_default(obj_pos,&g_car_62ac74,0x41f00000,(fbits1 = (float)rand_val, *(int *)&fbits1));
              rand_val = (float10)rp_random_float_range(0x3f800000,0x459c4000);
              sound_voices_set_position_wide(obj_pos,(fbits2 = (float)(rand_val + (float10)k_f_20000), *(int *)&fbits2));
              age_or_dx = *cam_pos - obj_pos->x;
              dy_cam = cam_pos[1] - RwFrame_modelling(frame).pos.y;
              dz_cam = cam_pos[2] - RwFrame_modelling(frame).pos.z;
              if (float_5_0 < dz_cam * dz_cam + age_or_dx * age_or_dx + dy_cam * dy_cam) {
                FUN_004c42f0((undefined4)frame,(undefined4)cam_pos,(undefined4)&g_car_62ac74);
                random_int = random_int_range(0,0x19);
                frame_obj.burst_r = frame_obj.burst_r - random_int;
                random_int = random_int_range(0,0x19);
                frame_obj.burst_g = frame_obj.burst_g + random_int;
                random_int = random_int_range(0,0x19);
                frame_obj.burst_b = frame_obj.burst_b + random_int;
                effect_sprite_spawn(1,&RwFrame_modelling(frame),&frame_obj.burst_r,0x3fc00000,0x40266666);
              }
              *frame_obj.live_byte_ptr = *frame_obj.live_byte_ptr & ~('\x01' << ((byte)frame_obj.live_bit & 0x1f));
              *(char *)(*(int *)&obj[-0x14] + 2) = *(char *)(*(int *)&obj[-0x14] + 2) + -4;
              frame_obj.frame_delta.x = 0.0;
              frame_obj.frame_delta.y = -10000.0;
              frame_obj.frame_delta.z = 0.0;
              RwFrameTranslate(frame,&frame_obj.frame_delta,rwCOMBINEPOSTCONCAT);
              bit_index = frame_obj.live_bit;
            }
            /* every third tick: spawn a smoke trail particle */
            if ((int)g_tick_number % 3 == 0) {
              frame_obj.trail_r = rp_random_int_range(0xffffffd8,0x28);
              frame_obj.trail_pos_x = obj_pos->x;
              frame_obj.trail_pos_y = RwFrame_modelling(frame).pos.y;
              frame_obj.trail_r = frame_obj.trail_r + -0x80;
              frame_obj.trail_pos_z = RwFrame_modelling(frame).pos.z;
              frame_obj.trail_life_scale = 0x3fb6db6e;
              frame_obj.trail_a = 0x96;
              frame_obj.trail_flags = 0x1120;
              frame_obj.trail_field_38 = 0x3f800000;
              frame_obj.trail_g = frame_obj.trail_r;
              frame_obj.trail_b = frame_obj.trail_r;
              rand_val = (float10)rp_random_float_range(0x3ecccccd,0x3f666666);
              frame_obj.trail_size_w = (float)rand_val;
              frame_obj.trail_size_h = (float)rand_val;
              frame_obj.trail_kind = 6;
              rand_val = (float10)rp_random_float_range(0xbf8ccccd,0x3f8ccccd);
              frame_obj.trail_vel_x = (float)rand_val;
              frame_obj.trail_vel_y = 0xbd4ccccd;
              rand_val = (float10)rp_random_float_range(0xbf8ccccd,0x3f8ccccd);
              frame_obj.trail_vel_z = (float)rand_val;
              frame_obj.trail_accel_x = 0;
              frame_obj.trail_accel_y = 0;
              frame_obj.trail_accel_z = 0;
              fx_trail_particle_spawn_interpolated(&frame_obj.trail_pos_x,0,0,0);
            }
            if (float_1_0 <= obj[-0x13]) {
              *frame_obj.live_byte_ptr = *frame_obj.live_byte_ptr & ~('\x01' << ((byte)bit_index & 0x1f));
            }
          }
        }
        obj = obj + 0x4c;
        frame_obj.slot_index = frame_obj.slot_index + 1;
        frame_obj.slots_left = frame_obj.slots_left + -1;
      } while (frame_obj.slots_left != 0);
    }
    frame_obj.group_live_word_ptr = frame_obj.group_live_word_ptr + 1;
    frame_obj.group_base_index = frame_obj.group_base_index + 0x20;
    frame_obj.group_objs_ptr = frame_obj.group_objs_ptr + 0x980;
  } while ((int)frame_obj.group_live_word_ptr < (int)&g_car_6af5c0 + 0x20);
  return;
}

