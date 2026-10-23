#include "mfl_tier2.h"
/*00406d40*/ extern int __cdecl fx_object_timer_subtract_by_id(undefined4 p0, undefined4 p1);
/*00419fa0*/ extern double ai_get_float_8c9980();
/*00419fb0*/ extern int ai_bot_set_cooldown_78();
/*00424760*/ extern undefined __cdecl car_damage_at_world_point(undefined4 p0, undefined4 p1, struct RwV3d *p2);
/*00461cc0*/ extern int weapon_particle_spark_emit();
/*00465f20*/ extern int weapon_effect_get_slot_value_a();
/*00465f40*/ extern int weapon_effect_get_slot_value_b();
/*00465f60*/ extern int weapon_effect_get_slot_record();
/*004726a0*/ extern int sfx_play_at();
/*0047a5b0*/ extern int car_add_velocity();
/*0047b7c0*/ extern int car_set_field_144();
/*00481fd0*/ extern double random_range();
/*00486050*/ extern undefined __cdecl random_vector_in_cone(struct RwV3d *p0, float *p1, float p2);
/*004866f0*/ extern int fx_emitter_spawn_c_default();
/*0048e3f0*/ extern int rwp_object_get_id();
/*00492ca0*/ extern int rwp_object_activate();
/*0049d060*/ extern int fx_spawn_burst_default_scale();
/*004a1680*/ extern int player_rumble();
/*004c45e0*/ extern float rp_random_float_range(undefined4 p0, undefined4 p1);
/*004d0ff0*/ extern RwMatrix * RwFrameGetLTM(struct RwFrame *p0);
/*004d5290*/ extern RwReal RwV3dNormalize(struct RwV3d *p0, struct RwV3d *p1);
/*005dd328*/ extern float k_f_0_6;
/*005dd33c*/ extern float k_f_4;
/*005dd370*/ extern float k_f_0_3;
/*005dd620*/ extern float float_10_0;
/*005dd62c*/ extern float k_f_100;
/*005ddaf8*/ extern float k_f_n0_3;
/*005ddb04*/ extern float k_f_0_023;
/*005ddb20*/ extern float k_f_0_9;
/*005ddc28*/ extern float k_f_0;
/*005de2fc*/ extern float k_f_0_04;
/*005de3e4*/ extern float k_f_40;
/*006a60d0*/ extern struct MachinegunState MachinegunState_ARRAY_006a60d0[4];
/* weapon_machinegun_fire */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void weapon_machinegun_fire(WeaponSlot *param_1)

{
  float fbits1;
  float fbits2;
  float fbits3;
  float fVar1;
  WeaponSlot *pWVar2;
  int iVar3;
  RwV3d *pointsIn;
  RwMatrix *pRVar4;
  int iVar5;
  undefined4 slot_record;
  uint uVar6;
  bool bVar7;
  double fVar8;
  /* one contiguous frame: rwp_object_activate reads the two vectors at RStack_30 / fStack_24 together */
  struct {
    undefined1 uStack_34;
    undefined1 uStack_33;
    undefined1 uStack_32;
    undefined1 uStack_31;
    RwV3d RStack_30;
    float fStack_24;
    float fStack_20;
    float fStack_1c;
    RwV3d RStack_18;
    undefined4 uStack_c;
    undefined4 uStack_8;
    undefined4 uStack_4;
  } frame;
  
  pWVar2 = param_1;
  frame.uStack_c = 0;
  frame.uStack_8 = 0;
  frame.uStack_4 = 0;
  frame.fStack_24 = 1.0;
  frame.fStack_20 = 1.5;
  frame.fStack_1c = 1.0;
  frame.uStack_34 = 0;
  frame.uStack_33 = 0;
  frame.uStack_32 = 0;
  frame.uStack_31 = 0xff;
  iVar3 = weapon_effect_get_slot_value_b(param_1->player);
  pointsIn = (RwV3d *)weapon_effect_get_slot_record(pWVar2->player);
  uVar6 = pWVar2->player;
  if (MachinegunState_ARRAY_006a60d0[uVar6].spin < k_f_40) {
    MachinegunState_ARRAY_006a60d0[uVar6].spin = MachinegunState_ARRAY_006a60d0[uVar6].spin + k_f_4;
  }
  uVar6 = pWVar2->player;
  if ((MachinegunState_ARRAY_006a60d0[uVar6].cooldown == k_f_0) &&
     (0 < MachinegunState_ARRAY_006a60d0[uVar6].ammo)) {
    MachinegunState_ARRAY_006a60d0[uVar6].cooldown = 0.06;
    MachinegunState_ARRAY_006a60d0[pWVar2->player].ammo =
         MachinegunState_ARRAY_006a60d0[pWVar2->player].ammo + -1;
    weapon_particle_spark_emit(pointsIn,0x3f19999a);
    MachinegunState_ARRAY_006a60d0[pWVar2->player].fired = 1;
    uVar6 = MachinegunState_ARRAY_006a60d0[pWVar2->player].ammo & 0x80000001;
    bVar7 = uVar6 == 0;
    if ((int)uVar6 < 0) {
      bVar7 = (uVar6 - 1 | 0xfffffffe) == 0xffffffff;
    }
    if (bVar7) {
      param_1 = (WeaponSlot *)0xff2184ff;
      pRVar4 = RwFrameGetLTM(*(RwFrame **)
                              (*(int *)&MachinegunState_ARRAY_006a60d0[pWVar2->player].field_0x44 +
                              4));
      frame.RStack_18.x = (pRVar4->pos).x;
      frame.RStack_18.y = (pRVar4->pos).y;
      frame.RStack_18.z = (pRVar4->pos).z;
      frame.RStack_30.x = pointsIn->x - (pRVar4->pos).x;
      frame.RStack_30.y = pointsIn->y - (pRVar4->pos).y;
      frame.RStack_30.z = pointsIn->z - (pRVar4->pos).z;
      RwV3dNormalize(&frame.RStack_30,&frame.RStack_30);
      frame.fStack_24 = frame.RStack_30.x * k_f_0_6;
      frame.fStack_20 = frame.RStack_30.y * k_f_0_6;
      frame.fStack_1c = frame.RStack_30.z * k_f_0_6;
      frame.RStack_18.x = frame.RStack_30.x + frame.RStack_30.x + frame.RStack_18.x;
      frame.RStack_18.y = frame.RStack_30.y + frame.RStack_30.y + frame.RStack_18.y;
      frame.RStack_18.z = frame.RStack_30.z + frame.RStack_30.z + frame.RStack_18.z;
      fx_spawn_burst_default_scale(&frame.RStack_18,1,&frame.fStack_24,&param_1,&frame.uStack_c,0x40a00000);
    }
    if (iVar3 == -1) {
      iVar3 = weapon_effect_get_slot_value_a(pWVar2->player);
      if (iVar3 != -1) {
        fVar8 = (double)ai_get_float_8c9980();
        iVar5 = fx_object_timer_subtract_by_id
                          (iVar3,(fbits1 = (float)((double)k_f_0_9 - fVar8 * (double)k_f_0_04), *(int *)&fbits1));
        if (iVar5 == 0) {
          frame.RStack_30.x = pWVar2->aim_vector[0];
          frame.RStack_30.y = pWVar2->aim_vector[1];
          frame.RStack_30.z = pWVar2->aim_vector[2];
          fVar8 = (double)rp_random_float_range(0x3ca3d70a,0x3d75c28f);
          fx_emitter_spawn_c_default(pointsIn,0,0x43b40000,(fbits2 = (float)fVar8, *(int *)&fbits2));
          iVar5 = rwp_object_get_id(iVar3);
          if (iVar5 < 150000) {
            frame.RStack_30.y = frame.RStack_30.y * float_10_0;
            fVar1 = float_10_0;
          }
          else {
            frame.RStack_30.y = 100.0;
            fVar1 = k_f_100;
          }
          frame.RStack_30.x = frame.RStack_30.x * fVar1;
          frame.RStack_30.z = frame.RStack_30.z * fVar1;
          slot_record = weapon_effect_get_slot_record(pWVar2->player);
          rwp_object_activate(iVar3,&frame.RStack_30,slot_record);
        }
      }
      frame.fStack_24 = 0.0;
      frame.fStack_20 = 1.0;
      frame.fStack_1c = 0.0;
      iVar3 = 4;
      do {
        random_vector_in_cone(&frame.RStack_18,&frame.fStack_24,10.0);
        frame.RStack_18.x = frame.RStack_18.x * k_f_0_023;
        frame.RStack_18.y = frame.RStack_18.y * k_f_0_023;
        frame.RStack_18.z = frame.RStack_18.z * k_f_0_023;
        fx_spawn_burst_default_scale(pointsIn,1,&frame.RStack_18,&frame.uStack_34,&frame.uStack_c,0x40000000);
        iVar3 = iVar3 + -1;
      } while (iVar3 != 0);
    }
    else {
      fVar8 = (double)rp_random_float_range(0xbf800000,0x3f800000);
      frame.fStack_24 = (float)fVar8;
      fVar8 = (double)rp_random_float_range(0xbf800000,0x3f800000);
      frame.fStack_20 = (float)fVar8;
      fVar8 = (double)rp_random_float_range(0xbf800000,0x3f800000);
      frame.fStack_1c = (float)fVar8;
      iVar5 = 4;
      do {
        random_vector_in_cone(&frame.RStack_18,&frame.fStack_24,10.0);
        frame.RStack_18.x = frame.RStack_18.x * k_f_0_023;
        frame.RStack_18.y = frame.RStack_18.y * k_f_0_023;
        frame.RStack_18.z = frame.RStack_18.z * k_f_0_023;
        fx_spawn_burst_default_scale(pointsIn,1,&frame.RStack_18,0,&frame.uStack_c,0x40800000);
        iVar5 = iVar5 + -1;
      } while (iVar5 != 0);
      fVar8 = (double)rp_random_float_range(0x3d75c28f,0x3dcccccd);
      fx_emitter_spawn_c_default(pointsIn,0,0x43b40000,(fbits3 = (float)fVar8, *(int *)&fbits3));
      player_rumble(iVar3,1,2,0xff);
      fVar8 = (double)random_range(0xc3480000,0x43480000);
      frame.RStack_30.x = (RwReal)fVar8;
      frame.RStack_30.y = 0.0;
      fVar8 = (double)random_range(0xc3480000,0x43480000);
      frame.RStack_30.z = (RwReal)fVar8;
      car_add_velocity(&frame.RStack_30,iVar3);
      frame.RStack_30.x = 0.0;
      fVar8 = (double)random_range(0xbf4ccccd,0x3f4ccccd);
      frame.RStack_30.y = (RwReal)fVar8;
      if (((double)k_f_0_3 <= fVar8) || (frame.RStack_30.y <= k_f_0)) {
        if ((k_f_n0_3 < frame.RStack_30.y) && (frame.RStack_30.y < k_f_0)) {
          frame.RStack_30.y = -0.3;
        }
      }
      else {
        frame.RStack_30.y = 0.3;
      }
      frame.RStack_30.z = 0.0;
      car_set_field_144(&frame.RStack_30,iVar3);
      ai_bot_set_cooldown_78(iVar3);
      car_damage_at_world_point(iVar3,0x3f400000,pointsIn);
    }
    sfx_play_at(0x13,pWVar2->muzzle_position);
  }
  return;
}


