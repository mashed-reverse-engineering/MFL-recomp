#include "mfl_tier2.h"
/*0040e000*/ extern undefined __cdecl skid_mark_spawn_from_wheel(undefined4 p0, undefined4 p1, undefined4 p2, undefined4 p3);
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
  int *piVar1;
  float fVar2;
  int iVar3;
  int iVar4;
  uint uVar5;
  int *piVar6;
  uint uVar7;
  uint uVar8;
  undefined4 *puVar9;
  int fx_flag;      /* 1 when the random draw is above 0.2, later whether the race phase is 6 (the original compares it with 2 in between) */
  float10 fVar11;
  uint uVar12;
  struct {
    int *local_a8;
    undefined1 local_a4 [24];
    undefined4 *local_8c;
    int local_88;
    float local_84;
    undefined4 *local_80;
    undefined4 local_7c;
    int local_78;
    int local_74;
    undefined4 local_70;
    uint local_6c;
    uint local_68;
    RwV3d *local_64;
    undefined4 local_60;
    undefined1 local_5c [4];
    undefined4 local_58;
    RwReal local_54;
    undefined4 local_50;
    RwV3d local_4c;
    undefined1 local_40 [48];
    undefined1 local_10 [16];
  } frame;
  
  uVar8 = 4;
  frame.local_88 = race_get_phase();
  frame.local_8c = g_ai_editor_mode;
  frame.local_a8 = (int *)&g_race_7eebb4;
  puVar9 = (undefined4 *)&player_car[4].field_0xcfc;
  do {
    piVar1 = frame.local_8c + -4;
    frame.local_8c = frame.local_8c + -4;
    puVar9 = puVar9 + -0x341;
    piVar6 = frame.local_a8 + -3;
    uVar8 = uVar8 - 1;
    if (-1 < *piVar1) {
      frame.local_80 = puVar9;
      fVar11 = (float10)random_range(0,0x3f800000);
      fx_flag = (float10)k_f_0_2 < fVar11;
      uVar7 = 0;
      car_get_int_from_float_field(&frame.local_78,uVar8);
      if ((1 < frame.local_78) && (iVar3 = car_is_active(uVar8), iVar3 != 0)) {
        car_get_velocity_any_mode(&frame.local_4c,uVar8);
        vehicle_part_ltm(uVar8,0x13);
        frame.local_6c = uVar8;
        car_get_field_b0c(&frame.local_60,uVar8);
        car_get_drift_angle((float *)frame.local_5c,uVar8);
        frame.local_58 = *puVar9;
        frame.local_64 = &frame.local_4c;
        frame.local_54 = RwV3dLength(&frame.local_4c);
        iVar3 = sound_get_8d9960(uVar8);
        iVar4 = vehicle_is_active(uVar8);
        if ((iVar4 == 1) && (iVar3 != 0)) {
          fx_explosion_spawn(uVar8,5,0x3e4ccccd,0x3dcccccd);
          fx_explosion_spawn(uVar8,6,0x3e4ccccd,0x3dcccccd);
        }
        if (*piVar6 == 0) {
LAB_0040e42d:
          if (fx_flag == 2) goto LAB_0040e434;
        }
        else {
          iVar3 = *piVar6 + -0x18;
          *piVar6 = iVar3;
          if (iVar3 < 0) {
            *piVar6 = 0;
          }
          if (frame.local_a8[-2] != 2) {
            fVar2 = (float)*piVar6 * k_f_5_0en5 + k_f_0_1;
            frame.local_84 = fVar2;
            fx_explosion_spawn(uVar8,5,0x3f4ccccd,*(int *)&fVar2);
            fx_explosion_spawn(uVar8,6,0x3f4ccccd,*(int *)&fVar2);
            frame.local_a4[0] = 0xff;
            frame.local_a4[1] = 0;
            frame.local_a4[2] = 0;
            frame.local_a4[3] = 200;
            frame.local_a4[4] = 0;
            frame.local_a4[5] = 0;
            frame.local_a4[6] = 0xff;
            frame.local_a4[7] = 200;
            frame.local_a4[8] = 0;
            frame.local_a4[9] = 0xff;
            frame.local_a4[10] = 0;
            frame.local_a4[0xb] = 200;
            frame.local_a4[0xc] = 0xff;
            frame.local_a4[0xd] = 0xff;
            frame.local_a4[0xe] = 0;
            frame.local_a4[0xf] = 200;
            frame.local_a4[0x10] = 0xff;
            frame.local_a4[0x11] = 0;
            frame.local_a4[0x12] = 0xff;
            frame.local_a4[0x13] = 200;
            frame.local_a4[0x14] = 0;
            frame.local_a4[0x15] = 0;
            frame.local_a4[0x16] = 0;
            frame.local_a4[0x17] = 200;
            if ((*(int *)((char *)((undefined *)&g_effects_reset_652dc0) + uVar8 * 4) == 0) && (0x76b < *piVar6)) {
              vehicle_get_matrix_local(uVar8,frame.local_40);
              hud_label_add_world_text
                        (0x32c,frame.local_10,frame.local_a4 + frame.local_8c[2] * 4,frame.local_a4 + frame.local_8c[2] * 4,
                         0x41c00000,0x3f800000);
              *(undefined4 *)((char *)((undefined *)&g_effects_reset_652dc0) + uVar8 * 4) = 1;
            }
            goto LAB_0040e42d;
          }
          frame.local_60 = 0x43480000;
LAB_0040e434:
          fx_flag = frame.local_88 == 6;
          frame.local_54 = 500.0;
        }
        iVar3 = get_viewport_camera_matrix(0);
        if ((k_f_n0_33 < *(float *)(iVar3 + 0x24)) && (*(float *)(iVar3 + 0x24) < k_f_0_33)) {
          uVar5 = g_tick_number & 0x80000003;
          if ((int)uVar5 < 0) {
            uVar5 = (uVar5 - 1 | 0xfffffffc) + 1;
          }
          fx_flag = uVar5 == 0;
        }
        frame.local_50 = *frame.local_8c;
        do {
          if (((((fx_flag != 0) && (uVar7 == (g_tick_number & 3))) || (frame.local_88 == 8)) || (frame.local_88 == 7))
             && (iVar3 = vehicle_part_ltm(uVar8,uVar7 + 0x1d), iVar3 != 0)) {
            puVar9 = &frame.local_7c;
            uVar5 = uVar8;
            uVar12 = uVar7;
            car_wheel_get_field_88_b(&frame.local_7c, uVar5, uVar12);
            frame.local_70 = frame.local_7c;
            frame.local_74 = iVar3;
            frame.local_68 = uVar7;
            skid_mark_spawn_from_wheel((undefined4)&frame.local_74,(undefined4)puVar9,uVar5,uVar12);
          }
          uVar7 = uVar7 + 1;
          puVar9 = frame.local_80;
        } while ((int)uVar7 < 4);
      }
    }
    frame.local_a8 = piVar6;
    if (puVar9 == (undefined4 *)&player_car[0].field_0xcfc) {
      return;
    }
  } while( true );
}


