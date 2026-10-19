// cluster-entries: car_ground_contact_forces
/* Installed cluster car_ground_contact_forces (tools/cluster_install.py from output/cluster_pilot_h3/car_ground_contact_forces.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*004d5290*/ extern RwReal RwV3dNormalize(struct RwV3d *p0, struct RwV3d *p1);
/*004d53a0*/ extern RwReal RwV3dLength(struct RwV3d *p0);
/*005dd32c*/ extern float k_f_3;
/*005dd330*/ extern float float_1_0;
/*005dd33c*/ extern float k_f_4;
/*005dd340*/ extern float k_f_0_5;
/*005dd364*/ extern float k_f_n1;
/*005dd378*/ extern float k_f_0_1;
/*005dd628*/ extern float k_f_0_25;
/*005dda24*/ extern float k_f_0_75;
/*005ddad4*/ extern float k_f_90;
/*005ddb20*/ extern float k_f_0_9;
/*005ddb50*/ extern float k_f_8;
/*005ddc28*/ extern float k_f_0;
/*005de19c*/ extern float k_f_1_0en4;
/*005de244*/ extern float k_f_500;
/*005de290*/ extern float float_50_0;
/*005de6a8*/ extern float k_f_n0_5;
/*005dfd2c*/ extern float k_f_50_24;
/*005dfda0*/ extern float k_f_0_0027778;
/* car_ground_contact_forces */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* WARNING: Unknown calling convention */
/* "counts wheels in contact (+0x198 / +0x25C / +0x320 / +0x3E4) and builds the contact forces
   (inferred)" */

void car_ground_contact_forces_n(Car *car)

{
  float fVar1;
  float fVar2;
  float fVar3;
  float fVar4;
  float *pfVar5;
  int iVar6;
  float *pfVar7;
  int cVar8;
  RwReal RVar9;
  struct {
    float pad_bot;
    float local_78;
    float local_74;
    undefined1 pad_70[4];
    RwV3d local_6c;
    float local_60;
    float local_5c;
    float local_58;
    RwV3d local_54;
    RwV3d local_48;
    RwV3d local_3c;
    RwV3d local_30;
    RwV3d local_24;
    float local_18;
    float local_14;
    float local_10;
    float local_c;
    float local_8;
  } frame;
  
  iVar6 = 0;
  cVar8 = car->wheels[0].in_contact != 0;
  frame.local_78 = 0.0;
  frame.local_6c.z = 0.0;
  frame.local_6c.y = 0.0;
  frame.local_6c.x = 0.0;
  frame.local_58 = 0.0;
  frame.local_5c = 0.0;
  frame.local_60 = 0.0;
  frame.local_54.z = 0.0;
  frame.local_54.y = 0.0;
  frame.local_54.x = 0.0;
  frame.local_74 = 1.0;
  if (car->wheels[1].in_contact != 0) {
    cVar8 = cVar8 + 1;
  }
  if (car->wheels[2].in_contact != 0) {
    cVar8 = cVar8 + 1;
  }
  if (car->wheels[3].in_contact != 0) {
    cVar8 = cVar8 + 1;
  }
  pfVar5 = (float *)((char *)&car->contacts[0] + 8);
  do {
    pfVar7 = pfVar5;
    if (*(int *)&pfVar7[-2] != -1) {
      frame.local_3c.z = pfVar7[0xb];
      if (iVar6 < 4) {
        frame.local_3c.z = frame.local_3c.z * k_f_n1;
        if (*pfVar7 < k_f_0_9) {
          frame.local_3c.z = frame.local_3c.z * k_f_4;
        }
      }
      else if (cVar8 == 4) {
        frame.local_3c.z = frame.local_3c.z * k_f_n1;
      }
      else {
        frame.local_3c.z = frame.local_3c.z * k_f_n0_5;
      }
      fVar1 = pfVar7[0xb] / car->speed;
      if (fVar1 < k_f_0) {
        fVar1 = -fVar1;
        if (float_1_0 < fVar1) {
          fVar1 = float_1_0;
        }
        frame.local_74 = float_1_0 - fVar1;
      }
      if ((*(int *)((char *)car + 0xcfc) != 0) && (frame.local_3c.z < k_f_500)) {
        frame.local_3c.z = k_f_500;
      }
      frame.local_3c.z = frame.local_3c.z * k_f_0_75;
      frame.local_3c.x = frame.local_3c.z * pfVar7[-1];
      frame.local_3c.y = frame.local_3c.z * *pfVar7;
      frame.local_3c.z = frame.local_3c.z * pfVar7[1];
      frame.local_54.x = frame.local_3c.x + frame.local_54.x;
      frame.local_54.y = frame.local_3c.y + frame.local_54.y;
      frame.local_54.z = frame.local_3c.z + frame.local_54.z;
      frame.local_60 = frame.local_3c.x + frame.local_60;
      frame.local_5c = frame.local_3c.y + frame.local_5c;
      frame.local_58 = frame.local_3c.z + frame.local_58;
      if (*(int *)((char *)car + 0xcfc) == 0) {
        frame.local_48.x = pfVar7[-1] * k_f_90;
        frame.local_48.y = *pfVar7 * k_f_90;
        frame.local_48.z = pfVar7[1] * k_f_90;
      }
      else {
        frame.local_48.x = pfVar7[-1] * k_f_500;
        frame.local_48.y = *pfVar7 * k_f_500;
        frame.local_48.z = pfVar7[1] * k_f_500;
      }
      frame.local_54.x = frame.local_48.x + frame.local_54.x;
      frame.local_54.y = frame.local_48.y + frame.local_54.y;
      frame.local_54.z = frame.local_48.z + frame.local_54.z;
      frame.local_60 = frame.local_48.x + frame.local_60;
      frame.local_5c = frame.local_48.y + frame.local_5c;
      frame.local_58 = frame.local_48.z + frame.local_58;
      if (3 < iVar6) {
        RwV3dNormalize(&frame.local_24,(RwV3d *)(pfVar7 + 8));
        fVar1 = frame.local_24.x * pfVar7[-1] + frame.local_24.y * *pfVar7 + frame.local_24.z * pfVar7[1];
        RVar9 = RwV3dLength(&frame.local_3c);
        fVar1 = RVar9 * fVar1;
        frame.local_c = frame.local_24.x * fVar1;
        frame.local_8 = frame.local_24.y * fVar1;
        fVar1 = fVar1 * frame.local_24.z;
        frame.local_18 = frame.local_3c.x - frame.local_c;
        frame.local_14 = frame.local_3c.y - frame.local_8;
        frame.local_10 = frame.local_3c.z - fVar1;
        frame.local_30.x = frame.local_10 * frame.local_8 - frame.local_14 * fVar1;
        frame.local_30.y = frame.local_18 * fVar1 - frame.local_10 * frame.local_c;
        frame.local_30.z = frame.local_14 * frame.local_c - frame.local_18 * frame.local_8;
        if (k_f_0 < pfVar7[10] * frame.local_3c.z +
                    frame.local_3c.x * ((RwV3d *)(pfVar7 + 8))->x + pfVar7[9] * frame.local_3c.y) {
          frame.local_30.x = -frame.local_30.x;
          frame.local_30.y = -frame.local_30.y;
          frame.local_30.z = -frame.local_30.z;
        }
        frame.local_54.x = frame.local_c + frame.local_54.x;
        frame.local_54.y = frame.local_8 + frame.local_54.y;
        frame.local_54.z = fVar1 + frame.local_54.z;
        frame.local_60 = frame.local_3c.x + frame.local_60;
        frame.local_5c = frame.local_3c.y + frame.local_5c;
        frame.local_58 = frame.local_3c.z + frame.local_58;
        frame.local_48.z = pfVar7[2];
        frame.local_48.x = frame.local_18 * frame.local_48.z;
        frame.local_48.y = frame.local_14 * frame.local_48.z;
        frame.local_48.z = frame.local_10 * frame.local_48.z;
        RVar9 = RwV3dLength(&frame.local_48);
        fVar1 = RVar9 * k_f_50_24;
        frame.local_78 = fVar1 + frame.local_78;
        RVar9 = RwV3dLength(&frame.local_30);
        if (k_f_1_0en4 < RVar9) {
          RwV3dNormalize(&frame.local_30,&frame.local_30);
          frame.local_24.x = frame.local_30.x * fVar1;
          frame.local_24.y = frame.local_30.y * fVar1;
          frame.local_24.z = frame.local_30.z * fVar1;
          frame.local_6c.x = frame.local_6c.x - frame.local_24.x;
          frame.local_6c.y = frame.local_6c.y - frame.local_24.y;
          frame.local_6c.z = frame.local_6c.z - frame.local_24.z;
        }
      }
    }
    iVar6 = iVar6 + 1;
    pfVar5 = pfVar7 + 0x10;
  } while (iVar6 < 0x12);
  fVar1 = float_1_0 / (float)*(int *)((char *)car + 0x9ec);
  frame.local_60 = frame.local_60 * fVar1;
  frame.local_5c = frame.local_5c * fVar1;
  frame.local_58 = frame.local_58 * fVar1;
  frame.local_54.x = frame.local_54.x * fVar1;
  frame.local_54.y = frame.local_54.y * fVar1;
  frame.local_54.z = frame.local_54.z * fVar1;
  frame.local_6c.x = frame.local_6c.x * fVar1;
  frame.local_6c.y = frame.local_6c.y * fVar1;
  frame.local_6c.z = frame.local_6c.z * fVar1;
  frame.local_78 = frame.local_78 * fVar1;
  RVar9 = RwV3dLength(&frame.local_6c);
  if (frame.local_78 - RVar9 != k_f_0) {
    frame.local_48.x = frame.local_60 - frame.local_54.x;
    frame.local_48.y = frame.local_5c - frame.local_54.y;
    frame.local_48.z = frame.local_58 - frame.local_54.z;
  }
  fVar1 = car->inertia_inverse * k_f_0_0027778;
  *(undefined4 *)((char *)car + 0xb20) = 1;
  fVar1 = fVar1 * k_f_8;
  frame.local_6c.x = frame.local_6c.x * fVar1;
  frame.local_6c.y = frame.local_6c.y * fVar1;
  frame.local_6c.z = fVar1 * frame.local_6c.z;
  RVar9 = RwV3dLength(&frame.local_6c);
  if ((RVar9 != k_f_0) && (RVar9 < k_f_0_25)) {
    fVar1 = k_f_0_25 / RVar9;
    if (float_50_0 < k_f_0_25 / RVar9) {
      fVar1 = float_50_0;
    }
    frame.local_6c.x = frame.local_6c.x * fVar1;
    frame.local_6c.y = frame.local_6c.y * fVar1;
    frame.local_6c.z = fVar1 * frame.local_6c.z;
  }
  frame.local_6c.x = frame.local_6c.x * k_f_0_5;
  pfVar5 = car->velocity;
  frame.local_6c.y = frame.local_6c.y * k_f_0_5;
  frame.local_6c.z = frame.local_6c.z * k_f_0_5;
  car->velocity_input[0] = frame.local_6c.x + car->velocity_input[0];
  car->velocity_input[1] = frame.local_6c.y + car->velocity_input[1];
  car->velocity_input[2] = frame.local_6c.z + car->velocity_input[2];
  fVar1 = *pfVar5;
  *pfVar5 = frame.local_54.x + fVar1;
  fVar2 = frame.local_54.y + car->velocity[1];
  car->velocity[1] = fVar2;
  fVar3 = frame.local_54.z + car->velocity[2];
  car->velocity[2] = fVar3;
  if (*(int *)&pfVar7[-2] != -1) {
    fVar4 = frame.local_74 * k_f_3;
    if (k_f_0_9 < frame.local_74 * k_f_3) {
      fVar4 = k_f_0_9;
    }
    *pfVar5 = (frame.local_54.x + fVar1) * fVar4;
    car->velocity[1] = fVar2 * fVar4;
    car->velocity[2] = fVar3 * fVar4;
  }
  if ((*(int *)&car->wheels_in_contact == 0x40800000) && (RVar9 = RwV3dLength((RwV3d *)pfVar5), k_f_1_0en4 < RVar9))
  {
    RwV3dNormalize(&frame.local_48,(RwV3d *)pfVar5);
    fVar1 = (frame.local_48.z * car->up[2] + frame.local_48.x * car->up[0] + frame.local_48.y * car->up[1]) * k_f_0_1;
    frame.local_48.z = fVar1 * RVar9;
    frame.local_48.x = frame.local_48.z * car->up[0];
    frame.local_48.y = frame.local_48.z * car->up[1];
    frame.local_48.z = frame.local_48.z * car->up[2];
    *pfVar5 = *pfVar5 - frame.local_48.x;
    car->velocity[1] = car->velocity[1] - frame.local_48.y;
    car->velocity[2] = car->velocity[2] - frame.local_48.z;
    RwV3dLength(&frame.local_54);
  }
  return;
}

/* entry thunk at the original address of car_ground_contact_forces: register inputs (edi) -> natural C car_ground_contact_forces_n; ret 0 */
__declspec(naked) void car_ground_contact_forces(void)
{
    __asm {
        push edi
        call car_ground_contact_forces_n
        add esp, 4
        ret
    }
}
