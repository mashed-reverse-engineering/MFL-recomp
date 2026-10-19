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

void bomb_explode_n(char *in_esi, float *param_2)

{
  float fbits1;
  float *pfVar1;
  float *pfVar2;
  RwMatrix *pRVar3;
  RwV3d *pRVar4;
  int iVar5;
  float10 fVar6;
  RwV3d *pointsIn;
  struct {
    float local_3c;
    float local_38;
    float local_34;
    RwV3d local_30;
    RwV3d local_24;
    undefined4 local_18;
    undefined4 local_14;
    undefined4 local_10;
    float local_c [3];
  } frame;
  
  frame.local_c[0] = 1.0;
  frame.local_c[1] = 1.5;
  frame.local_c[2] = 1.0;
  frame.local_18 = 0;
  frame.local_14 = 0xba83126f;
  frame.local_10 = 0;
  pRVar3 = RwFrameGetLTM(*(RwFrame **)(*(char **)(in_esi + 0x28) + 4));
  pRVar4 = &pRVar3->pos;
  sfx_play_at(0x13,pRVar4);
  sfx_play_at(0x25,param_2);
  iVar5 = 5;
  do {
    random_vector_in_cone(&frame.local_24,frame.local_c,10.0);
    frame.local_24.x = frame.local_24.x * k_f_0_023;
    frame.local_24.y = frame.local_24.y * k_f_0_023;
    frame.local_24.z = frame.local_24.z * k_f_0_023;
    fx_spawn_burst_default_scale(pRVar4,1,&frame.local_24,0,&frame.local_18,0x40000000);
    fx_spawn_burst_default_scale(param_2,1,&frame.local_24,0,&frame.local_18,0x40000000);
    iVar5 = iVar5 + -1;
  } while (iVar5 != 0);
  point_lerp_to_aux_camera(param_2,&frame.local_30,0x3dcccccd);
  fVar6 = (float10)random_range(0xbe4ccccd,0x3e4ccccd);
  frame.local_30.x = (RwReal)(fVar6 + (float10)frame.local_30.x);
  fVar6 = (float10)random_range(0xbe4ccccd,0x3e4ccccd);
  frame.local_30.y = (RwReal)(fVar6 + (float10)frame.local_30.y);
  fVar6 = (float10)random_range(0xbe4ccccd,0x3e4ccccd);
  frame.local_30.z = (RwReal)(fVar6 + (float10)frame.local_30.z);
  weapon_particle_spark_emit(&frame.local_30,0x3f19999a);
  point_lerp_to_aux_camera(pRVar4,&frame.local_30,0x3dcccccd);
  fVar6 = (float10)random_range(0xbdcccccd,0x3dcccccd);
  frame.local_30.x = (RwReal)(fVar6 + (float10)frame.local_30.x);
  fVar6 = (float10)random_range(0xbdcccccd,0x3dcccccd);
  frame.local_30.y = (RwReal)(fVar6 + (float10)frame.local_30.y);
  fVar6 = (float10)random_range(0xbdcccccd,0x3dcccccd);
  frame.local_30.z = (RwReal)(fVar6 + (float10)frame.local_30.z);
  weapon_particle_spark_emit(&frame.local_30,0x3f19999a);
  pointsIn = &frame.local_30;
  fVar6 = (float10)ai_get_float_8c9980();
  car_damage_at_world_point(0,(fbits1 = (float)(fVar6 * (float10)k_f_0_025 + (float10)k_f_0_01), *(int *)&fbits1),pointsIn);
  fVar6 = (float10)random_range(0xc3480000,0x43480000);
  frame.local_3c = (float)fVar6;
  frame.local_38 = 0.0;
  fVar6 = (float10)random_range(0xc3480000,0x43480000);
  frame.local_34 = (float)fVar6;
  fVar6 = (float10)ai_get_float_8c9980();
  frame.local_3c = (float)((fVar6 * (float10)k_f_0_1 + (float10)k_f_0_05) * (float10)frame.local_3c);
  fVar6 = (float10)ai_get_float_8c9980();
  frame.local_38 = (float)((fVar6 * (float10)k_f_0_1 + (float10)k_f_0_05) * (float10)frame.local_38);
  fVar6 = (float10)ai_get_float_8c9980();
  frame.local_34 = (float)((fVar6 * (float10)k_f_0_1 + (float10)k_f_0_05) * (float10)frame.local_34);
  car_add_velocity(&frame.local_3c,0);
  frame.local_3c = 0.0;
  fVar6 = (float10)random_range(0xbf4ccccd,0x3f4ccccd);
  frame.local_38 = (float)fVar6;
  if (((float10)k_f_0_3 <= fVar6) || (frame.local_38 <= k_f_0)) {
    if ((k_f_n0_3 < frame.local_38) && (frame.local_38 < k_f_0)) {
      frame.local_38 = -0.3;
    }
  }
  else {
    frame.local_38 = 0.3;
  }
  frame.local_34 = 0.0;
  fVar6 = (float10)ai_get_float_8c9980();
  frame.local_3c = (float)((fVar6 * (float10)k_f_0_071428575 + (float10)k_f_0_05) * (float10)frame.local_3c);
  fVar6 = (float10)ai_get_float_8c9980();
  frame.local_38 = (float)((fVar6 * (float10)k_f_0_071428575 + (float10)k_f_0_05) * (float10)frame.local_38);
  fVar6 = (float10)ai_get_float_8c9980();
  frame.local_34 = (float)((fVar6 * (float10)k_f_0_071428575 + (float10)k_f_0_05) * (float10)frame.local_34);
  car_set_field_144(&frame.local_3c,0);
  pfVar1 = (float *)(in_esi + 0x7c);
  *pfVar1 = pRVar4->x;
  *(RwReal *)(in_esi + 0x80) = (pRVar3->pos).y;
  *(RwReal *)(in_esi + 0x84) = (pRVar3->pos).z;
  pfVar2 = (float *)(in_esi + 0x88);
  *pfVar2 = *param_2;
  *(float *)(in_esi + 0x8c) = param_2[1];
  *(float *)(in_esi + 0x90) = param_2[2];
  frame.local_24.x = *pfVar2 - *pfVar1;
  frame.local_24.y = *(float *)(in_esi + 0x8c) - *(float *)(in_esi + 0x80);
  frame.local_24.z = *(float *)(in_esi + 0x90) - *(float *)(in_esi + 0x84);
  fVar6 = (float10)random_range(0x3dcccccd,0x3ecccccd);
  *pfVar1 = (float)(fVar6 * (float10)frame.local_24.x + (float10)*pfVar1);
  fVar6 = (float10)random_range(0x3dcccccd,0x3ecccccd);
  *(float *)(in_esi + 0x80) =
       (float)(fVar6 * (float10)frame.local_24.y + (float10)*(float *)(in_esi + 0x80));
  fVar6 = (float10)random_range(0x3dcccccd,0x3ecccccd);
  *(float *)(in_esi + 0x84) =
       (float)(fVar6 * (float10)frame.local_24.z + (float10)*(float *)(in_esi + 0x84));
  fVar6 = (float10)random_range(0xbdcccccd,0xbecccccd);
  *pfVar2 = (float)(fVar6 * (float10)frame.local_24.x + (float10)*pfVar2);
  fVar6 = (float10)random_range(0xbdcccccd,0xbecccccd);
  *(float *)(in_esi + 0x8c) =
       (float)(fVar6 * (float10)frame.local_24.y + (float10)*(float *)(in_esi + 0x8c));
  fVar6 = (float10)random_range(0xbdcccccd,0xbecccccd);
  *(float *)(in_esi + 0x90) =
       (float)(fVar6 * (float10)frame.local_24.z + (float10)*(float *)(in_esi + 0x90));
  fVar6 = (float10)random_range(0xbdcccccd,0x3dcccccd);
  frame.local_24.x = (RwReal)fVar6;
  fVar6 = (float10)random_range(0xbdcccccd,0x3dcccccd);
  frame.local_24.y = (RwReal)fVar6;
  fVar6 = (float10)random_range(0xbdcccccd,0x3dcccccd);
  *pfVar1 = frame.local_24.x + *pfVar1;
  *(float *)(in_esi + 0x80) = frame.local_24.y + *(float *)(in_esi + 0x80);
  *(float *)(in_esi + 0x84) = (float)(fVar6 + (float10)*(float *)(in_esi + 0x84));
  *pfVar2 = frame.local_24.x + *pfVar2;
  *(float *)(in_esi + 0x8c) = frame.local_24.y + *(float *)(in_esi + 0x8c);
  *(float *)(in_esi + 0x90) = (float)(fVar6 + (float10)*(float *)(in_esi + 0x90));
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
