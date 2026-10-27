// cluster-entries: explosion_spawn_fx
/* Installed cluster explosion_spawn_fx (tools/cluster_install.py from output/cluster_pilot_hb1/explosion_spawn_fx.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*00472340*/ extern int sfx_play_player_set_at();
/*00481fd0*/ extern double random_range();
/*004866b0*/ extern int fx_emitter_spawn_a_default();
/*0048e4f0*/ extern int rwp_object_set_enabled();
/*0048e6f0*/ extern int rwp_object_get_matrix();
/*00498f40*/ extern int fx_ring_add_point();
/*00499b40*/ extern int explosion_decal();
/*0049fd50*/ extern int fx_smoke_particle_spawn_tinted();
/*004d0ff0*/ extern RwMatrix * RwFrameGetLTM(struct RwFrame *p0);
/*0056b1d0*/ extern void rwp_world_set_bit_5c(int p0, int p1, int p2);
/*0058c8a0*/ extern int rwp_owner_object_get(int p0);
/*005dd32c*/ extern float k_f_3;
/*005dd330*/ extern float float_1_0;
/*005dd338*/ extern float k_f_0_01;
/*005dd340*/ extern float k_f_0_5;
/*005dd374*/ extern float k_f_0_2;
/*005dd620*/ extern float float_10_0;
/*005dd62c*/ extern float k_f_100;
/*005dd7e8*/ extern float k_f_60;
/*005ddb18*/ extern float float_0_8;
/*005ddb1c*/ extern float k_f_0_96;
/*005ddb20*/ extern float k_f_0_9;
/*0062ac74*/ extern float g_car_62ac74;
/*00651aa4*/ extern undefined4 g_explosion_651aa4;
/*007ef7f4*/ extern float g_tick_seconds;
/* explosion_spawn_fx */

/* "random colour / size, explosion_decal and sfx_play_player_set_at at the effect position" */

float10 explosion_spawn_fx_n(FxSceneObject *effect)

{
  undefined4 *vec_ptr;
  int *body_ptr;
  float timer;
  int rwp_matrix;
  RwMatrix *ltm;
  int owner_or_count;
  RwV3d *pos_a;
  float *obj_pos;
  float10 rnd;
  RwV3d *pos_b;
  RwV3d *pos_c;
  struct {
    float damage_frac;
    undefined1 colour_r;
    undefined1 colour_g;
    undefined1 colour_b;
    undefined1 colour_a;
    float rand_a;
    float rand_b;
    float spark_x;
    float spark_y;
    float spark_z;
  } frame;
  
  rwp_matrix = rwp_object_get_matrix(effect->rwp_object);
  frame.damage_frac = (k_f_100 - effect->health) * k_f_0_01;
  obj_pos = (float *)(rwp_matrix + 0x30);
  rnd = (float10)random_range(0,0x3f800000);
  frame.rand_a = (float)rnd;
  rnd = (float10)random_range(0x3f4ccccd,0x3f99999a);
  frame.rand_b = (float)rnd;
  ltm = RwFrameGetLTM(*(RwFrame **)(effect->frame_c + 4));
  pos_a = &ltm->pos;
  ltm = RwFrameGetLTM(*(RwFrame **)(effect->frame_d + 4));
  timer = g_tick_seconds + effect->timer_6c;
  pos_b = &ltm->pos;
  effect->timer_6c = timer;
  frame.colour_a = 0x80;
  if (k_f_3 <= timer) {
    *(undefined4 *)&effect->timer_6c = 0;
    *(undefined4 *)&effect->f_68 = *(undefined4 *)&effect->spin_speed;
  }
  if (float_1_0 < frame.damage_frac) {
    frame.damage_frac = 1.0;
  }
  frame.colour_r = ((int)(frame.rand_b * 128.0f));
  frame.colour_g = frame.colour_r;
  frame.colour_b = ((int)(frame.rand_b * 150.0f));
  if (effect->use_b == 0) {
    if (float_1_0 <= frame.damage_frac) {
      g_explosion_651aa4 = g_explosion_651aa4 + 1;
      *(undefined4 *)&effect->use_b = 1;
      *(undefined4 *)&effect->vel.y = 0;
      fx_emitter_spawn_a_default(obj_pos,&g_car_62ac74,0x43b40000);
      sfx_play_player_set_at(obj_pos);
      if (effect->owner != 0) {
        rwp_object_set_enabled(effect->rwp_object,1);
        owner_or_count = rwp_owner_object_get(*(undefined4 *)&effect->owner);
        rwp_world_set_bit_5c(*(undefined4 *)(owner_or_count + 0x24),owner_or_count,1);
        body_ptr = *(int **)(*(int *)(owner_or_count + 0x10) + 4);
        if (body_ptr != (int *)0x0) {
          *(float *)&body_ptr[6] = (float_1_0 / (g_tick_seconds * float_10_0));
          vec_ptr = (undefined4 *)(*(int *)(*(int *)(*body_ptr + 0x10) + 8) + 0x10 + body_ptr[1] * 0x20);
          *vec_ptr = 0;
          vec_ptr[1] = 0x41a00000;
          vec_ptr[2] = 0;
          *(float *)&body_ptr[5] = (float_1_0 / (g_tick_seconds * k_f_60));
        }
      }
      owner_or_count = 6;
      do {
        frame.spark_x = *obj_pos;
        frame.spark_y = *(float *)(rwp_matrix + 0x34);
        frame.spark_z = *(float *)(rwp_matrix + 0x38);
        rnd = (float10)random_range(0xbf666666,0x3f666666);
        frame.spark_x = (float)(rnd + (float10)frame.spark_x);
        rnd = (float10)random_range(0xbf666666,0x3f666666);
        frame.spark_y = (float)(rnd + (float10)frame.spark_y);
        rnd = (float10)random_range(0xbf666666,0x3f666666);
        frame.spark_z = (float)(rnd + (float10)frame.spark_z);
        fx_emitter_spawn_a_default(&frame.spark_x,&g_car_62ac74,0x43b40000);
        explosion_decal(&frame.spark_x,0x3f99999a);
        owner_or_count = owner_or_count + -1;
      } while (owner_or_count != 0);
      return (float10)frame.damage_frac;
    }
    if (frame.rand_a + frame.rand_a < frame.damage_frac) {
      rnd = (float10)random_range(0,0x3f800000);
      pos_c = pos_b;
      if ((float10)k_f_0_5 < rnd) {
        pos_c = pos_a;
      }
      fx_smoke_particle_spawn_tinted(pos_c,0x3f333333,&frame.colour_r,0x40a00000,0x3f000000);
    }
    if ((k_f_0_5 < frame.damage_frac) && (effect->flag_64 == 0)) {
      fx_ring_add_point(pos_a,0x3f800000);
      fx_ring_add_point(pos_a,0x3f800000);
      fx_ring_add_point(pos_a,0x3f800000);
      fx_ring_add_point(pos_b,0x3f800000);
      fx_ring_add_point(pos_b,0x3f800000);
      fx_ring_add_point(pos_b,0x3f800000);
      *(undefined4 *)&effect->flag_64 = 1;
    }
  }
  else {
    if (k_f_0_9 < frame.rand_a) {
      rnd = (float10)random_range(0,0x3f800000);
      if ((float10)k_f_0_5 < rnd) {
        pos_b = pos_a;
      }
      fx_smoke_particle_spawn_tinted(pos_b,0x3f333333,&frame.colour_r,0x40a00000,0x3f000000);
    }
    if (k_f_0_96 < frame.rand_a) {
      frame.spark_x = *obj_pos;
      frame.spark_y = *(float *)(rwp_matrix + 0x34);
      frame.spark_z = *(float *)(rwp_matrix + 0x38);
      rnd = (float10)random_range(0xbf666666,0x3f666666);
      frame.spark_x = (float)(rnd + (float10)frame.spark_x);
      rnd = (float10)random_range(0xbf666666,0x3f666666);
      frame.spark_y = (float)(rnd * (float10)k_f_0_2 + (float10)frame.spark_y);
      rnd = (float10)random_range(0xbf666666,0x3f666666);
      frame.spark_z = (float)(rnd + (float10)frame.spark_z);
      fx_emitter_spawn_a_default(&frame.spark_x,&g_car_62ac74,0x43b40000);
      rnd = (float10)random_range(0,0x3f800000);
      if ((float10)float_0_8 < rnd) {
        explosion_decal(&frame.spark_x,0x3f99999a);
      }
      sfx_play_player_set_at(&frame.spark_x);
      return (float10)frame.damage_frac;
    }
  }
  return (float10)frame.damage_frac;
}

/* entry thunk at the original address of explosion_spawn_fx: register inputs (eax) -> natural C explosion_spawn_fx_n; ret 0 */
__declspec(naked) void explosion_spawn_fx(void)
{
    __asm {
        push eax
        call explosion_spawn_fx_n
        add esp, 4
        ret
    }
}
