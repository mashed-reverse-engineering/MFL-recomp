// cluster-entries: bomb_explode
/* Installed cluster bomb_explode (tools/cluster_install.py from output/cluster_pilot_hb1/bomb_explode.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*00419fa0*/ extern double ai_get_float_8c9980();
/*00424760*/ extern undefined __cdecl car_damage_at_world_point(undefined4 p0, undefined4 p1, struct RwV3d *p2);
/*00461cc0*/ extern int weapon_particle_spark_emit();
/*004726a0*/ extern int sfx_play_at();
/*0047a5b0*/ extern int car_add_velocity();
/*0047b7c0*/ extern int car_set_field_144();
/*00481fd0*/ extern double random_range();
/*004846e0*/ extern int point_lerp_to_aux_camera();
/*00486050*/ extern undefined __cdecl random_vector_in_cone(struct RwV3d *p0, float *p1, float p2);
/*0049d060*/ extern int fx_spawn_burst_default_scale();
/*004d0ff0*/ extern RwMatrix * RwFrameGetLTM(struct RwFrame *p0);
/*005dd338*/ extern float k_f_0_01;
/*005dd370*/ extern float k_f_0_3;
/*005dd378*/ extern float k_f_0_1;
/*005ddaf4*/ extern float k_f_0_071428575;
/*005ddaf8*/ extern float k_f_n0_3;
/*005ddafc*/ extern float k_f_0_05;
/*005ddb00*/ extern float k_f_0_025;
/*005ddb04*/ extern float k_f_0_023;
/*005ddc28*/ extern float k_f_0;
/* bomb_explode */

/* WARNING: Unknown calling convention */
/* "sfx 0x13 at the bomb position, then car_damage_at_world_point to nearby cars (inferred)" */

void bomb_explode_n(char *bomb, float *target_pos)

{
  float fbits1;
  float *start_ptr;
  float *end_ptr;
  RwMatrix *bomb_ltm;
  RwV3d *bomb_pos;
  int burst_count;
  float10 rnd;
  RwV3d *spark_ptr;
  struct {
    float impulse_x;
    float impulse_y;
    float impulse_z;
    RwV3d spark_pos;
    RwV3d burst_dir;
    undefined4 burst_vec_x;
    undefined4 burst_vec_y;
    undefined4 burst_vec_z;
    float cone_axis [3];
  } frame;
  
  frame.cone_axis[0] = 1.0;
  frame.cone_axis[1] = 1.5;
  frame.cone_axis[2] = 1.0;
  frame.burst_vec_x = 0;
  frame.burst_vec_y = 0xba83126f;
  frame.burst_vec_z = 0;
  bomb_ltm = RwFrameGetLTM(*(RwFrame **)(*(char **)(bomb + 0x28) + 4));
  bomb_pos = &bomb_ltm->pos;
  sfx_play_at(0x13,bomb_pos);
  sfx_play_at(0x25,target_pos);
  burst_count = 5;
  do {
    random_vector_in_cone(&frame.burst_dir,frame.cone_axis,10.0);
    frame.burst_dir.x = frame.burst_dir.x * k_f_0_023;
    frame.burst_dir.y = frame.burst_dir.y * k_f_0_023;
    frame.burst_dir.z = frame.burst_dir.z * k_f_0_023;
    fx_spawn_burst_default_scale(bomb_pos,1,&frame.burst_dir,0,&frame.burst_vec_x,0x40000000);
    fx_spawn_burst_default_scale(target_pos,1,&frame.burst_dir,0,&frame.burst_vec_x,0x40000000);
    burst_count = burst_count + -1;
  } while (burst_count != 0);
  point_lerp_to_aux_camera(target_pos,&frame.spark_pos,0x3dcccccd);
  rnd = (float10)random_range(0xbe4ccccd,0x3e4ccccd);
  frame.spark_pos.x = (RwReal)(rnd + (float10)frame.spark_pos.x);
  rnd = (float10)random_range(0xbe4ccccd,0x3e4ccccd);
  frame.spark_pos.y = (RwReal)(rnd + (float10)frame.spark_pos.y);
  rnd = (float10)random_range(0xbe4ccccd,0x3e4ccccd);
  frame.spark_pos.z = (RwReal)(rnd + (float10)frame.spark_pos.z);
  weapon_particle_spark_emit(&frame.spark_pos,0x3f19999a);
  point_lerp_to_aux_camera(bomb_pos,&frame.spark_pos,0x3dcccccd);
  rnd = (float10)random_range(0xbdcccccd,0x3dcccccd);
  frame.spark_pos.x = (RwReal)(rnd + (float10)frame.spark_pos.x);
  rnd = (float10)random_range(0xbdcccccd,0x3dcccccd);
  frame.spark_pos.y = (RwReal)(rnd + (float10)frame.spark_pos.y);
  rnd = (float10)random_range(0xbdcccccd,0x3dcccccd);
  frame.spark_pos.z = (RwReal)(rnd + (float10)frame.spark_pos.z);
  weapon_particle_spark_emit(&frame.spark_pos,0x3f19999a);
  spark_ptr = &frame.spark_pos;
  rnd = (float10)ai_get_float_8c9980();
  car_damage_at_world_point(0,(fbits1 = (float)(rnd * (float10)k_f_0_025 + (float10)k_f_0_01), *(int *)&fbits1),spark_ptr);
  rnd = (float10)random_range(0xc3480000,0x43480000);
  frame.impulse_x = (float)rnd;
  frame.impulse_y = 0.0;
  rnd = (float10)random_range(0xc3480000,0x43480000);
  frame.impulse_z = (float)rnd;
  rnd = (float10)ai_get_float_8c9980();
  frame.impulse_x = (float)((rnd * (float10)k_f_0_1 + (float10)k_f_0_05) * (float10)frame.impulse_x);
  rnd = (float10)ai_get_float_8c9980();
  frame.impulse_y = (float)((rnd * (float10)k_f_0_1 + (float10)k_f_0_05) * (float10)frame.impulse_y);
  rnd = (float10)ai_get_float_8c9980();
  frame.impulse_z = (float)((rnd * (float10)k_f_0_1 + (float10)k_f_0_05) * (float10)frame.impulse_z);
  car_add_velocity(&frame.impulse_x,0);
  frame.impulse_x = 0.0;
  rnd = (float10)random_range(0xbf4ccccd,0x3f4ccccd);
  frame.impulse_y = (float)rnd;
  if (((float10)k_f_0_3 <= rnd) || (frame.impulse_y <= k_f_0)) {
    if ((k_f_n0_3 < frame.impulse_y) && (frame.impulse_y < k_f_0)) {
      frame.impulse_y = -0.3;
    }
  }
  else {
    frame.impulse_y = 0.3;
  }
  frame.impulse_z = 0.0;
  rnd = (float10)ai_get_float_8c9980();
  frame.impulse_x = (float)((rnd * (float10)k_f_0_071428575 + (float10)k_f_0_05) * (float10)frame.impulse_x);
  rnd = (float10)ai_get_float_8c9980();
  frame.impulse_y = (float)((rnd * (float10)k_f_0_071428575 + (float10)k_f_0_05) * (float10)frame.impulse_y);
  rnd = (float10)ai_get_float_8c9980();
  frame.impulse_z = (float)((rnd * (float10)k_f_0_071428575 + (float10)k_f_0_05) * (float10)frame.impulse_z);
  car_set_field_144(&frame.impulse_x,0);
  start_ptr = (float *)(bomb + 0x7c);
  *start_ptr = bomb_pos->x;
  *(RwReal *)(bomb + 0x80) = (bomb_ltm->pos).y;
  *(RwReal *)(bomb + 0x84) = (bomb_ltm->pos).z;
  end_ptr = (float *)(bomb + 0x88);
  *end_ptr = *target_pos;
  *(float *)(bomb + 0x8c) = target_pos[1];
  *(float *)(bomb + 0x90) = target_pos[2];
  frame.burst_dir.x = *end_ptr - *start_ptr;
  frame.burst_dir.y = *(float *)(bomb + 0x8c) - *(float *)(bomb + 0x80);
  frame.burst_dir.z = *(float *)(bomb + 0x90) - *(float *)(bomb + 0x84);
  rnd = (float10)random_range(0x3dcccccd,0x3ecccccd);
  *start_ptr = (float)(rnd * (float10)frame.burst_dir.x + (float10)*start_ptr);
  rnd = (float10)random_range(0x3dcccccd,0x3ecccccd);
  *(float *)(bomb + 0x80) =
       (float)(rnd * (float10)frame.burst_dir.y + (float10)*(float *)(bomb + 0x80));
  rnd = (float10)random_range(0x3dcccccd,0x3ecccccd);
  *(float *)(bomb + 0x84) =
       (float)(rnd * (float10)frame.burst_dir.z + (float10)*(float *)(bomb + 0x84));
  rnd = (float10)random_range(0xbdcccccd,0xbecccccd);
  *end_ptr = (float)(rnd * (float10)frame.burst_dir.x + (float10)*end_ptr);
  rnd = (float10)random_range(0xbdcccccd,0xbecccccd);
  *(float *)(bomb + 0x8c) =
       (float)(rnd * (float10)frame.burst_dir.y + (float10)*(float *)(bomb + 0x8c));
  rnd = (float10)random_range(0xbdcccccd,0xbecccccd);
  *(float *)(bomb + 0x90) =
       (float)(rnd * (float10)frame.burst_dir.z + (float10)*(float *)(bomb + 0x90));
  rnd = (float10)random_range(0xbdcccccd,0x3dcccccd);
  frame.burst_dir.x = (RwReal)rnd;
  rnd = (float10)random_range(0xbdcccccd,0x3dcccccd);
  frame.burst_dir.y = (RwReal)rnd;
  rnd = (float10)random_range(0xbdcccccd,0x3dcccccd);
  *start_ptr = frame.burst_dir.x + *start_ptr;
  *(float *)(bomb + 0x80) = frame.burst_dir.y + *(float *)(bomb + 0x80);
  *(float *)(bomb + 0x84) = (float)(rnd + (float10)*(float *)(bomb + 0x84));
  *end_ptr = frame.burst_dir.x + *end_ptr;
  *(float *)(bomb + 0x8c) = frame.burst_dir.y + *(float *)(bomb + 0x8c);
  *(float *)(bomb + 0x90) = (float)(rnd + (float10)*(float *)(bomb + 0x90));
  return;
}

/* entry thunk at the original address of bomb_explode: register inputs (esi,s0) -> natural C bomb_explode_n; ret 0 */
__declspec(naked) void bomb_explode(void)
{
    __asm {
        push dword ptr [esp + 4]
        push esi
        call bomb_explode_n
        add esp, 8
        ret
    }
}
