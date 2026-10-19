#include "mfl_tier2.h"
/*0040fe20*/ extern int __cdecl race_setup_slot_used(undefined4 p0);
/*00422350*/ extern int vehicle_part_ltm();
/*00424290*/ extern undefined __cdecl car_damage_smoke_spawn(float *position, int car);
/*0047abb0*/ extern int car_get_velocity_any_mode();
/*00481fd0*/ extern double random_range();
/*004846e0*/ extern int point_lerp_to_aux_camera();
/*004867f0*/ extern int particles_set_position();
/*00486820*/ extern int particles_set_size();
/*00486840*/ extern int particles_set_color();
/*00486880*/ extern int gfx_set_gfx_6c50c8();
/*00486b50*/ extern int particles_add();
/*00486c40*/ extern int particles_flush();
/*004b2b48*/ extern int _ftol();
/*004d53a0*/ extern RwReal RwV3dLength(struct RwV3d *p0);
/*005dd328*/ extern float k_f_0_6;
/*005dd330*/ extern float float_1_0;
/*005dd41c*/ extern float float_5_0;
/*005dd338*/ extern float k_f_0_01;
/*005dd340*/ extern float k_f_0_5;
/*005dd374*/ extern float k_f_0_2;
/*005ddc28*/ extern float k_f_0;
/*005de430*/ extern float k_f_6_2831855;
/*0065a9d8*/ extern undefined1 g_car_damage_65a9d8;
/*0065a9e8*/ extern undefined4 car_damage;
/*0065a9ec*/ extern undefined4 g_car_damage_65a9ec;
/*0065aa88*/ extern undefined g_car_damage_65aa88;
/*0065aae4*/ extern undefined4 g_car_damage_65aae4;
/*007ef7f4*/ extern float g_tick_seconds;
extern double cos(double);
extern double sin(double);
/* car_damage_smoke_update */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* "per car: emits smoke particles for the damaged front / back (inferred)" */

void car_damage_smoke_update(void)

{
  byte bVar1;
  float fVar2;
  undefined1 uVar3;
  int slot_used;
  int iVar4;
  float *pfVar5;
  int iVar6;
  float *pfVar7;
  float10 fVar8;
  float strength;
  float10 angle;
  float10 cos_offset;
  float10 alpha;
  struct {
    uint local_30;      /* particle colour */
    float local_2c;
    uint local_28;
    float local_24;
    float local_20;
    float local_1c;
    float local_18;
    float local_14;
    float local_10;
    RwV3d local_c;
  } frame;
  
  iVar6 = 0;
  pfVar5 = (float *)&car_damage;
  pfVar7 = (float *)&g_car_damage_65a9ec;
  do {
    car_get_velocity_any_mode(&frame.local_c,iVar6);
    RwV3dLength(&frame.local_c);
    strength = (*pfVar7 + *pfVar5) * k_f_0_01;
    if (k_f_0 <= strength) {
      slot_used = race_setup_slot_used(iVar6);
      if (slot_used != 0) {
        fVar8 = (float10)random_range(0,0x3f800000);
        if (fVar8 < (float10)strength * (float10)k_f_0_5) {
          iVar4 = vehicle_part_ltm(iVar6, 0x33);
          frame.local_24 = *(float *)(iVar4 + 0x30);
          frame.local_20 = *(float *)(iVar4 + 0x34);
          frame.local_1c = *(float *)(iVar4 + 0x38);
          /* random offset of the smoke position around the part: x +-0.07, y +-0.1, z +-0.07 */
          fVar8 = (float10)random_range(0xbd8f5c29,0x3d8f5c29);
          frame.local_24 = (float)(fVar8 + (float10)frame.local_24);
          fVar8 = (float10)random_range(0xbdcccccd,0x3dcccccd);
          frame.local_20 = (float)(fVar8 + (float10)frame.local_20);
          fVar8 = (float10)random_range(0xbd8f5c29,0x3d8f5c29);
          frame.local_1c = (float)(fVar8 + (float10)frame.local_1c);
          car_damage_smoke_spawn((undefined4)&frame.local_24,iVar6);
        }
      }
    }
    pfVar7 = pfVar7 + 10;
    iVar6 = iVar6 + 1;
    pfVar5 = pfVar5 + 10;
  } while ((int)pfVar7 < (int)&g_car_damage_65aa88 + 0x4);
  iVar6 = 0;
  pfVar5 = (float *)&g_car_damage_65aae4;
  do {
    fVar2 = *pfVar5;
    if ((float_1_0 <= fVar2) || (fVar2 < k_f_0)) {
      (&g_car_damage_65a9d8)[iVar6 >> 3] =
           (&g_car_damage_65a9d8)[iVar6 >> 3] & ~('\x01' << ((byte)iVar6 & 7));
    }
    else {
      frame.local_30 = *(uint *)&pfVar5[2];
      angle = (float10)fVar2 * (float10)k_f_6_2831855;
      fVar8 = (float10)sin(angle);
      frame.local_2c = (float)(fVar8 * (float10)(0.05000000074505806));
      cos_offset = (float10)cos(angle) * (float10)(0.05000000074505806);
      *pfVar5 = g_tick_seconds + *pfVar5;
      /* alpha of the particle colour: fades in over the first 0.2 of its life and out afterwards */
      if (fVar2 < k_f_0_2) {
        alpha = (float10)(int)*(byte *)((int)pfVar5 + 0xb) * ((float10)fVar2 * (float10)float_5_0);
      }
      else {
        alpha = (float10)(int)*(byte *)((int)pfVar5 + 0xb) * ((float10)float_1_0 - (float10)fVar2);
      }
      frame.local_24 = pfVar5[-7] * pfVar5[1];
      frame.local_30 = (frame.local_30 & 0xffffff) | ((uint)(byte)(int)alpha << 0x18);
      frame.local_20 = pfVar5[-6] * pfVar5[1];
      pfVar5[-2] = pfVar5[-1] + pfVar5[-2];
      frame.local_14 = fVar2 * k_f_0_6 + pfVar5[-4];
      if (pfVar5[-1] <= k_f_0) {
        frame.local_18 = pfVar5[-5] - frame.local_2c;
        fVar8 = (float10)pfVar5[-3] - cos_offset;
      }
      else {
        frame.local_18 = frame.local_2c + pfVar5[-5];
        fVar8 = cos_offset + (float10)pfVar5[-3];
      }
      frame.local_10 = (float)fVar8;
      point_lerp_to_aux_camera(&frame.local_18,&frame.local_18,0x3d4ccccd);
      particles_set_position(&frame.local_18);
      particles_set_size(&frame.local_24);
      gfx_set_gfx_6c50c8(*(int *)&pfVar5[-2]);
      particles_set_color(&frame.local_30);
      particles_add(&g_car_damage_65aa88);
    }
    pfVar5 = pfVar5 + 0xb;
    iVar6 = iVar6 + 1;
  } while ((int)pfVar5 < (int)&g_car_damage_65aae4 + 0x1600);
  particles_flush(&g_car_damage_65aa88);
  return;
}


