// cluster-entries: car_apply_forces
/* Installed cluster car_apply_forces (tools/cluster_install.py from output/cluster_pilot_tr/car_apply_forces.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
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

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* per car: wheel anchor positions; speed factor from the proximity of other cars (catch-up style)
   and distance warning; adds gravity; wheel grip/drag forces */

static int triangle_normal_r(float * p0, float * p1, float * p2, float * p3);
extern void triangle_normal(void);
void car_apply_forces_n(Car *in_edi, float param_2, RwMatrix *param_3)

{
  uint uVar1;
  float fVar2;
  float fVar3;
  uint uVar4;
  int iVar5;
  int iVar6;
  CourseId CVar7;
  RwV3d *pRVar8;
  float *pfVar9;
  int iVar10;
  float *pfVar11;
  byte bVar12;
  float10 fVar13;
  RwReal RVar14;
  float fVar15;
  float slip3;
  float speed_keep;
  float raw_keep;
  volatile float base_load;      /* the original keeps this value in a stack slot (rounded to float) between its uses */
  int wheels_left;
  struct {
    float local_70;
    float local_6c;
    undefined1 pad_68[4];
    RwV3d local_64;
    RwV3d local_58;
    RwV3d local_4c;
    RwMatrix local_40;
  } frame;
  
  pfVar11 = in_edi->forward;
  RwV3dTransformVectors((RwV3d *)pfVar11,(RwV3d *)&g_weapon_62ac80,1,param_3);
  in_edi->wheels_in_contact = 0.0;
  pRVar8 = (RwV3d *)&g_car_8b06a0;
  pfVar9 = &in_edi->wheels[0].pos_x;
  do {
    if (pfVar9[0xb] != 0.0) {
      in_edi->wheels_in_contact = in_edi->wheels_in_contact + float_1_0;
    }
    pRVar8->x = *pfVar9;
    pRVar8->y = 0.0;
    pRVar8->z = pfVar9[2];
    RwV3dTransformVectors((RwV3d *)(pfVar9 + 5),pRVar8,1,param_3);
    if (pfVar9[0xf] == k_f_0) {
      pfVar9[0x2d] = *pfVar11;
      pfVar9[0x2e] = in_edi->forward[1];
      pfVar9[0x2f] = in_edi->forward[2];
    }
    else {
      RwMatrixRotate(&frame.local_40,(RwV3d *)&g_car_62ac74,pfVar9[0xf],rwCOMBINEREPLACE);
      RwV3dTransformVectors((RwV3d *)(pfVar9 + 0x2d),(RwV3d *)pfVar11,1,&frame.local_40);
    }
    pRVar8 = pRVar8 + 1;
    pfVar9 = pfVar9 + 0x31;
  } while ((int)pRVar8 < (int)&g_car_apply_8b06c8 + 0x8);
  uVar4 = in_edi->velocity_history_index - 1U & 1;
  in_edi->velocity_history_index = uVar4;
  in_edi->velocity_history[uVar4 * 3] = in_edi->velocity[0];
  in_edi->velocity_history[uVar4 * 3 + 1] = in_edi->velocity[1];
  in_edi->velocity_history[uVar4 * 3 + 2] = in_edi->velocity[2];
  fVar15 = in_edi->speed;
  if (in_edi->speed < k_f_1_0en4) {
    fVar15 = k_f_0;
  }
  fVar15 = *(float *)((char *)in_edi + 0x150) * in_edi->speed_scale * *(float *)((char *)in_edi + 0x158)
           * fVar15;
  if (in_edi->wheels_in_contact != 4.0) {
    fVar15 = fVar15 * k_f_0_5;
  }
  uVar4 = 0;
  frame.local_70 = 1.0;
  iVar10 = 0;
  fVar15 = fVar15 * in_edi->time_step_const * param_2 * k_f_3_3333333en4;
  do {
    if ((uVar4 != in_edi->index) &&
       (*(int *)((int)player_car[0].effect_level + iVar10 + -0x1c) == 1)) {
      uVar1 = in_edi->matrix_current;
      iVar5 = *(int *)((int)player_car[0].velocity + iVar10 + -8) * 0x40;
      iVar6 = iVar5 + iVar10;
      frame.local_64.x = *(float *)(iVar5 + 0x8b1038 + iVar10) - in_edi->matrices[uVar1 * 0x10 + 0xc];
      frame.local_64.y = *(float *)((int)player_car[0].matrices + iVar6 + 0x34) -
                   in_edi->matrices[uVar1 * 0x10 + 0xd];
      frame.local_58.x = *(float *)((int)player_car[0].velocity + iVar10);
      frame.local_64.z = *(float *)((int)player_car[0].matrices + iVar6 + 0x38) -
                   in_edi->matrices[uVar1 * 0x10 + 0xe];
      frame.local_58.y = *(float *)((int)player_car[0].velocity + iVar10 + 4);
      frame.local_58.z = *(float *)((int)player_car[0].velocity + iVar10 + 8);
      RVar14 = RwV3dLength(&frame.local_58);
      if (k_f_1_0en4 <= RVar14) {
        RwV3dNormalize(&frame.local_58,&frame.local_58);
        RwV3dNormalize(&frame.local_4c,&frame.local_64);
        if (((k_f_0_99 < frame.local_4c.z * frame.local_58.z + frame.local_4c.x * frame.local_58.x + frame.local_4c.y * frame.local_58.y
             ) && (RVar14 = RwV3dLength(&frame.local_64), RVar14 < k_f_6)) &&
           (fVar2 = float_1_0 - (k_f_6 - RVar14) * k_f_0_125, fVar2 < frame.local_70)) {
          frame.local_70 = fVar2;
        }
      }
    }
    iVar10 = iVar10 + 0xd04;
    uVar4 = uVar4 + 1;
  } while (iVar10 < 0x3410);
  if (game_mode == GAME_MODE_RACE) {
    frame.local_70 = 1.0;
  }
  else if (game_mode == GAME_MODE_CHASE_FUGITIVE) {
    frame.local_70 = 1.0;
  }
  else if ((game_mode == GAME_MODE_SURVIVAL) && (in_edi->index != 0)) {
    frame.local_70 = 0.75;
  }
  iVar10 = get_event_type();
  if ((iVar10 == EVENT_TRAINING) && (in_edi->index == 1)) {
    frame.local_70 = float_1_0;
  }
  fVar2 = k_f_60000 - (float)(int)g_race_clock_b;
  if (k_f_0 <= fVar2) {
    frame.local_6c = 1.0;
  }
  else {
    if (fVar2 < k_f_n30000) {
      fVar2 = k_f_n30000;
    }
    frame.local_6c = float_1_0 - fVar2 * k_f_n3_3333334en5;
  }
  iVar10 = 0;
  if ((player_car[0].active != 0) && (0 < player_car[0].vehicle_id)) {
    iVar10 = player_car[0].vehicle_id;
  }
  if ((player_car[1].active != 0) && (iVar10 < player_car[1].vehicle_id)) {
    iVar10 = player_car[1].vehicle_id;
  }
  if ((player_car[2].active != 0) && (iVar10 < player_car[2].vehicle_id)) {
    iVar10 = player_car[2].vehicle_id;
  }
  if ((player_car[3].active != 0) && (iVar10 < player_car[3].vehicle_id)) {
    iVar10 = player_car[3].vehicle_id;
  }
  fVar2 = k_f_n0_15;
  if ((((iVar10 != 0) && (fVar2 = k_f_0, iVar10 != 1)) && (fVar2 = k_f_0_5, iVar10 != 2)) &&
     (fVar2 = k_f_0, iVar10 == 3)) {
    fVar2 = float_1_0;
  }
  if (frame.local_6c == k_f_0) {
    iVar10 = 3;
    if ((player_car[0].active != 0) && (player_car[0].vehicle_id < 3)) {
      iVar10 = player_car[0].vehicle_id;
    }
    if ((player_car[1].active != 0) && (player_car[1].vehicle_id < iVar10)) {
      iVar10 = player_car[1].vehicle_id;
    }
    if ((player_car[2].active != 0) && (player_car[2].vehicle_id < iVar10)) {
      iVar10 = player_car[2].vehicle_id;
    }
    if ((player_car[3].active != 0) && (player_car[3].vehicle_id < iVar10)) {
      iVar10 = player_car[3].vehicle_id;
    }
    if (iVar10 == 0) {
      frame.local_6c = 1.0;
      fVar2 = k_f_n0_15;
    }
  }
  frame.local_70 = (fVar2 * frame.local_6c + float_1_0) * frame.local_70;
  fVar13 = (float10)camera_get_distance_factor(in_edi->index,frame.local_70,frame.local_6c);
  frame.local_70 = (float)(fVar13 * (float10)frame.local_70);
  if ((k_f_0 < frame.local_6c) && (iVar10 = player_distance_warning(in_edi->index), iVar10 != 0)) {
    bVar12 = player_car[0].active != 0;
    if (player_car[1].active != 0) {
      bVar12 = bVar12 + 1;
    }
    if (player_car[2].active != 0) {
      bVar12 = bVar12 + 1;
    }
    if (player_car[3].active != 0) {
      bVar12 = bVar12 + 1;
    }
    if (2 < bVar12) {
      frame.local_70 = frame.local_70 * k_f_0_5;
    }
  }
  raw_keep = float_1_0 - frame.local_70 * fVar15;
  speed_keep = ((raw_keep < k_f_0) || (float_1_0 < raw_keep)) ? k_f_0 : raw_keep;
  in_edi->velocity[0] = speed_keep * in_edi->velocity[0];
  in_edi->velocity[1] = speed_keep * in_edi->velocity[1];
  in_edi->velocity[2] = speed_keep * in_edi->velocity[2];
  frame.local_64.z = param_2 * in_edi->speed * k_f_1_0en5;
  if (k_f_2 < frame.local_64.z) {
    frame.local_64.z = k_f_2;
  }
  frame.local_64.z = frame.local_64.z * g_cars_801b28;
  frame.local_64.x = g_gravity_vector * frame.local_64.z;
  frame.local_64.y = g_cars_801b20 * frame.local_64.z;
  frame.local_64.z = g_cars_801b24 * frame.local_64.z;
  in_edi->velocity[0] = frame.local_64.x + in_edi->velocity[0];
  in_edi->velocity[1] = frame.local_64.y + in_edi->velocity[1];
  in_edi->velocity[2] = frame.local_64.z + in_edi->velocity[2];
  if (in_edi->wheels_in_contact != k_f_0) {
    iVar10 = in_edi->velocity_history_index;
    uVar4 = iVar10 - 1U & 1;
    frame.local_64.x = in_edi->velocity_history[iVar10 * 3] - in_edi->velocity_history[uVar4 * 3];      /* the original rounds the x and y differences to float, z stays in the register */
    frame.local_64.y = in_edi->velocity_history[iVar10 * 3 + 1] - in_edi->velocity_history[uVar4 * 3 + 1];
    frame.local_64.z = in_edi->velocity_history[iVar10 * 3 + 2] - in_edi->velocity_history[uVar4 * 3 + 2];
    frame.local_64.x = frame.local_64.x * param_2;
    frame.local_64.y = frame.local_64.y * param_2;
    frame.local_64.z = frame.local_64.z * param_2;
    base_load = (in_edi->mass / in_edi->wheels_in_contact) * g_cars_8bd750;
    fVar2 = base_load * k_f_0_33333334;
    if (in_edi->wheels[0].in_contact == 0) {
      in_edi->wheels[0].load = 0.0;
    }
    else {
      fVar3 = (*(float *)((char *)&in_edi->wheels[0] + 0x20) * frame.local_64.z +
              (*(float *)((char *)&in_edi->wheels[0] + 0x18) * frame.local_64.x +
               *(float *)((char *)&in_edi->wheels[0] + 0x1c) * frame.local_64.y)) * k_f_0_002;
      if (fVar2 < fVar3) {
        fVar3 = fVar2;
      }
      if (fVar3 < -fVar2) {
        fVar3 = -fVar2;
      }
      in_edi->wheels[0].load = base_load + fVar3;
      if (fVar3 > k_f_20) {
        g_car_8b06a4 = k_f_20 * k_f_0_0033333334;
      }
      else {
        if (fVar3 < k_f_n20) {
          fVar3 = k_f_n20;
        }
        g_car_8b06a4 = fVar3 * k_f_0_0033333334;
      }
    }
    if (in_edi->wheels[1].in_contact == 0) {
      in_edi->wheels[1].load = 0.0;
    }
    else {
      fVar3 = (*(float *)((char *)&in_edi->wheels[1] + 0x20) * frame.local_64.z +
              (*(float *)((char *)&in_edi->wheels[1] + 0x18) * frame.local_64.x +
               *(float *)((char *)&in_edi->wheels[1] + 0x1c) * frame.local_64.y)) * k_f_0_002;
      if (fVar2 < fVar3) {
        fVar3 = fVar2;
      }
      if (fVar3 < -fVar2) {
        fVar3 = -fVar2;
      }
      in_edi->wheels[1].load = base_load + fVar3;
      if (fVar3 > k_f_20) {
        g_car_8b06b0 = k_f_20 * k_f_0_0033333334;
      }
      else {
        if (fVar3 < k_f_n20) {
          fVar3 = k_f_n20;
        }
        g_car_8b06b0 = fVar3 * k_f_0_0033333334;
      }
    }
    if (in_edi->wheels[2].in_contact == 0) {
      in_edi->wheels[2].load = 0.0;
    }
    else {
      fVar3 =
           (*(float *)((char *)&in_edi->wheels[2] + 0x20) * frame.local_64.z +
              (*(float *)((char *)&in_edi->wheels[2] + 0x18) * frame.local_64.x +
               *(float *)((char *)&in_edi->wheels[2] + 0x1c) * frame.local_64.y)) * k_f_0_002;
      if (fVar2 < fVar3) {
        fVar3 = fVar2;
      }
      if (fVar3 < -fVar2) {
        fVar3 = -fVar2;
      }
      in_edi->wheels[2].load = base_load + fVar3;
      if (fVar3 > k_f_20) {
        g_car_apply_8b06bc = k_f_20 * k_f_0_0033333334;
      }
      else {
        if (fVar3 < k_f_n20) {
          fVar3 = k_f_n20;
        }
        g_car_apply_8b06bc = fVar3 * k_f_0_0033333334;
      }
    }
    if (in_edi->wheels[3].in_contact == 0) {
      in_edi->wheels[3].load = 0.0;
    }
    else {
      slip3 =
           (*(float *)((char *)&in_edi->wheels[3] + 0x20) * frame.local_64.z +
              (*(float *)((char *)&in_edi->wheels[3] + 0x18) * frame.local_64.x +
               *(float *)((char *)&in_edi->wheels[3] + 0x1c) * frame.local_64.y)) * k_f_0_002;
      if (fVar2 < slip3) {
        slip3 = fVar2;
      }
      if (slip3 < -fVar2) {
        slip3 = -fVar2;
      }
      in_edi->wheels[3].load = base_load + slip3;
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
    in_edi->wheels[3].load = in_edi->wheels[3].load * k_f_0_5;
    uVar4 = *(int *)((char *)in_edi + 0xbec) + 1U & 0xf;
    *(uint *)((char *)in_edi + 0xbec) = uVar4;
    triangle_normal_r(&g_car_8b06a0,&g_car_8b06ac,&g_car_apply_8b06b8,
                    in_edi->field_b00 + uVar4 * 3 + 0xb);
    if (in_edi->field_b00[uVar4 * 3 + 0xd] < k_f_0) {
      in_edi->field_b00[uVar4 * 3 + 0xd] = in_edi->field_b00[uVar4 * 3 + 0xd] * k_f_0_25;
    }
  }
  pfVar11 = (float *)((char *)&in_edi->wheels[0] + 0x8c);
  wheels_left = 4;
  do {
    if (pfVar11[-0x17] == 0.0) {
      pfVar11[10] = 0.0;
      pfVar11[9] = 0.0;
      pfVar11[8] = 0.0;
    }
    else {
      fVar15 = pfVar11[-1];
      *pfVar11 = 0.25;
      in_edi->field_b00[0] = 0.0;
      in_edi->field_b00[1] = 0.0;
      in_edi->field_b00[2] = 0.0;
      if (fVar15 == -NAN) {
        fVar15 = in_edi->speed * k_f_5_0en6;
        fVar13 = (float10)random_range(-fVar15,*(int *)&fVar15);
        in_edi->field_b00[0] = (float)fVar13;
        in_edi->field_b00[1] = 0.0;
        fVar13 = (float10)random_range(-fVar15,*(int *)&fVar15);
        in_edi->field_b00[2] = (float)fVar13;
      }
      else if (fVar15 == -NAN) {
        *pfVar11 = 0.1;
      }
      else if (fVar15 == -NAN) {
        *pfVar11 = 0.01;
      }
      else {
        if ((fVar15 == -NAN) || (fVar15 == -2.1068629e+38)) {
          if (in_edi->is_bot == 0) {
            *pfVar11 = 0.01;
            CVar7 = current_course_id();
            if (CVar7 == COURSE_NT1_S) {
              *pfVar11 = 0.1;
            }
          }
          else {
            *pfVar11 = 0.2;
          }
        }
        if (fVar15 == -NAN) {
          *pfVar11 = 0.2;
        }
      }
      if (in_edi->effect_level[3] != 0) {
        *pfVar11 = (float)(int)in_edi->effect_level[3] * *pfVar11 * k_f_0_01;
      }
      fVar15 = pfVar11[-0x1a] * *pfVar11;
      *pfVar11 = fVar15;
      pfVar11[1] = k_f_0_6 * fVar15;
      pfVar11[2] = g_car_apply_6292e8 * fVar15;
      if (in_edi->controls_disabled == 1) {
        pfVar11[1] = fVar15;
        pfVar11[2] = fVar15;
      }
      pfVar11[7] = pfVar11[6] * pfVar11[4];
      if (pfVar11[6] * pfVar11[4] < k_f_0) {
        pfVar11[7] = 0.0;
      }
      if (in_edi->wheels_in_contact == 4.0) {
        pRVar8 = (RwV3d *)(pfVar11 + 8);
        fVar15 = g_car_8bd730 * pfVar11[6] * k_f_0_5;
        pRVar8->x = fVar15 * pfVar11[3];
        pfVar11[9] = (pfVar11[4] - float_1_0) * fVar15;
        pfVar11[10] = fVar15 * pfVar11[5];
        fVar15 = RwV3dLength(pRVar8);
        if (fVar15 != k_f_0) {
          frame.local_64.z = float_1_0 / fVar15;
          frame.local_64.x = frame.local_64.z * pRVar8->x;
          frame.local_64.y = frame.local_64.z * pfVar11[9];
          frame.local_64.z = frame.local_64.z * pfVar11[10];
          fVar2 = (frame.local_64.z * pfVar11[0xd] + frame.local_64.x * pfVar11[0xb] + frame.local_64.y * pfVar11[0xc]
                  ) * fVar15;
          pRVar8->x = fVar2 * pfVar11[0xb];
          pfVar11[9] = fVar2 * pfVar11[0xc];
          fVar2 = fVar2 * pfVar11[0xd];
          goto LAB_0047cad1;
        }
      }
      else {
        fVar15 = (pfVar11[6] / param_2) * g_car_8bd730;
        pfVar11[8] = fVar15 * pfVar11[3];
        pfVar11[9] = fVar15 * pfVar11[4];
        fVar2 = fVar15 * pfVar11[5];
LAB_0047cad1:
        pfVar11[10] = fVar2;
      }
      if ((*(int *)((char *)in_edi + 0xb20) == 0) && (k_f_65536 < fVar15)) {
        *(undefined4 *)((char *)in_edi + 0xb20) = 1;
      }
    }
    fVar15 = g_cars_8bd750;
    pfVar11 = pfVar11 + 0x31;
    wheels_left = wheels_left + -1;
    if (wheels_left == 0) {
      if (in_edi->wheels_in_contact < k_f_3) {
        *(undefined4 *)((char *)in_edi + 0xb20) = 1;
        *(float *)((char *)in_edi + 0xb18) =
             *(float *)((char *)in_edi + 0xb18) - fVar15 * in_edi->mass * k_f_3000;
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
