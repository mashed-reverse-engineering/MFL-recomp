// cluster-entries: car_integrate_step
/* Installed cluster car_integrate_step (tools/cluster_install.py from output/cluster_pilot_h3/car_integrate_step.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*0040fdc0*/ extern int race_get_phase();
/*0047b780*/ extern int car_get_vec3_9c8(RwV3d *p0, uint p1);
/*004d3d50*/ extern int MatrixOrthoNormalize_004d3d50(float *p0, float *p1);
/*005dd330*/ extern float float_1_0;
/*005dd338*/ extern float k_f_0_01;
/*005dd33c*/ extern float k_f_4;
/*005dd340*/ extern float k_f_0_5;
/*005dd40c*/ extern float k_f_1_5;
/*005dd9ac*/ extern float k_f_255;
/*005dda1c*/ extern float k_f_3_3333333en4;
/*005dde3c*/ extern float k_f_3000;
/*005de19c*/ extern float k_f_1_0en4;
/*005de270*/ extern float k_f_n0_1;
/*005df2b4*/ extern float k_f_5_0en4;
/*005df2c8*/ extern float k_f_0_002;
/*005df498*/ extern float k_f_6_6666666en4;
/*005dfda0*/ extern float k_f_0_0027778;
/*005e64a4*/ extern float k_f_0;
/*006292b8*/ extern float g_car_handling_param_b8;
/* car_integrate_step */

/* "applies velocity and rotation to the car RwMatrix (old matrix in edi -> new in ebx) then MatrixOrthoNormalize"
   car = the Car object (esi); the dwords are viewed as floats / ints by index */

void car_integrate_step_n(Car *car, float *old_m, float *new_m, float dt, unsigned char *input)

{
  float t;
  float f;
  float fVar1;
  float fVar2;
  int iVar3;
  float vx, vy, vz;
  struct { float lo[5]; RwV3d dir; float tmp[3]; } fr;      /* frame layout of the original: dir at esp+0x18 below the return address */

  t = dt * k_f_3_3333333en4;
  f = dt * k_f_3_3333333en4 * k_f_0_0027778;
  vx = f * car->velocity[0];
  vy = f * car->velocity[1];
  vz = f * car->velocity[2];
  iVar3 = race_get_phase();
  if (((iVar3 != 6) && (iVar3 = race_get_phase(), iVar3 != 0xb)) &&
     (iVar3 = race_get_phase(), iVar3 != RACE_PHASE_WINNER_DONE)) {
    vz = 0.0f;
    vy = 0.0f;
    vx = 0.0f;
  }
  new_m[0xc] = vx + old_m[0xc];
  new_m[0xd] = old_m[0xd] + vy;
  new_m[0xe] = old_m[0xe] + vz;
  f = t * k_f_0_5;
  vx = f * car->angular_velocity[0];
  vy = car->angular_velocity[1] * f;
  vz = f * car->angular_velocity[2];
  car_get_vec3_9c8(&fr.dir, car->index);
  if (car->controls_disabled == 0) {
    vz = 0.0f;
    vy = 0.0f;
    vx = 0.0f;
    fVar1 = car->speed * k_f_6_6666666en4;
    if (car->speed * k_f_6_6666666en4 > float_1_0) {
      fVar1 = float_1_0;
    }
    car->field_0xcfc = 0;
    if ((input[4] != 0) && (input[5] != 0)) {
      car->field_0xcfc = 1;
      fVar1 = float_1_0;
    }
    if (car->wheels_in_contact != k_f_0) {
      fVar2 = (float)input[0];
      if ((car->effect_level[0] != 0) && (fVar2 = (float)input[0] + (float)(int)car->effect_level[0], fVar2 > k_f_255)) {
        fVar2 = k_f_255;
      }
      iVar3 = car->effect_level[4];
      if (iVar3 != 0) {
        t = float_1_0 - (float)iVar3 * k_f_0_01;
        fVar2 = t * fVar2;
      }
      if (fVar2 != k_f_0) {
        t = fVar2 * dt * g_car_handling_param_b8 * fVar1 * k_f_1_0en4 * k_f_3_3333333en4;
        vx = fr.dir.x * t;
        vy = fr.dir.y * t;
        vz = fr.dir.z * t;
      }
      fVar2 = (float)input[1];
      if ((car->effect_level[1] != 0) && (fVar2 = (float)input[1] + (float)(int)car->effect_level[1], fVar2 > k_f_255)) {
        fVar2 = k_f_255;
      }
      if (iVar3 != 0) {
        t = float_1_0 - (float)iVar3 * k_f_0_01;
        fVar2 = t * fVar2;
      }
      if (fVar2 != k_f_0) {
        t = fVar2 * dt * g_car_handling_param_b8 * fVar1 * k_f_1_0en4 * k_f_3_3333333en4;
        fVar1 = -t;
        fr.tmp[1] = fVar1 * fr.dir.y;
        fr.tmp[2] = fVar1 * fr.dir.z;
        vx = fr.dir.x * fVar1 + vx;
        vy = fr.tmp[1] + vy;
        vz = fr.tmp[2] + vz;
      }
    }
    if ((input[4] != 0) && (input[5] != 0)) {
      vx = vx * k_f_1_5;
      vy = vy * k_f_1_5;
      vz = vz * k_f_1_5;
    }
    if (*(int *)&car->wheels_in_contact != 0x40800000) {
      vx = t * car->angular_velocity[0] + vx;
      vz = t * car->angular_velocity[2] + vz;
    }
    if ((car->forward[2] * car->velocity[2] + car->forward[0] * car->velocity[0] + car->forward[1] * car->velocity[1] < k_f_n0_1) &&
       ((input[4] == 0 || (input[5] == 0)))) {
      vx = -vx;
      vy = -vy;
      vz = -vz;
    }
  }
  if (car->wheels_in_contact == k_f_0) {
    car->velocity_input[0] = vx + car->velocity_input[0];
    car->velocity_input[1] = vy + car->velocity_input[1];
    car->velocity_input[2] = vz + car->velocity_input[2];
  }
  iVar3 = race_get_phase();
  if ((iVar3 == 7) && (car->active == 1)) {
    f = (dt + dt) * k_f_3_3333333en4 * car->speed * k_f_5_0en4;
    vy = fr.dir.y * f;
    vz = fr.dir.z * f;
    vx = fr.dir.x * f;
  }
  fVar1 = (k_f_3000 - dt * k_f_4) * k_f_3_3333333en4;
  car->velocity_input[0] = (vx + car->velocity_input[0]) * fVar1;
  car->velocity_input[1] = (vy + car->velocity_input[1]) * fVar1;
  car->velocity_input[2] = (vz + car->velocity_input[2]) * fVar1;
  fVar1 = dt * k_f_0_002;
  fr.dir.x = fVar1 * car->velocity_input[0];
  fr.dir.y = fVar1 * car->velocity_input[1];
  fr.dir.z = fVar1 * car->velocity_input[2];
  if ((input[1] == 0) && (input[0] == 0)) {
    vx = fr.dir.x + vx;
    vy = fr.dir.y + vy;
    vz = vz + fr.dir.z;
  }
  fVar1 = old_m[0];
  fVar2 = old_m[2];
  fr.dir.z = vx * old_m[1] - vy * old_m[0];
  new_m[0] = (vy * old_m[2] - vz * old_m[1]) + old_m[0];
  new_m[1] = (vz * fVar1 - vx * fVar2) + old_m[1];
  new_m[2] = fr.dir.z + old_m[2];
  fVar1 = old_m[4];
  fVar2 = old_m[6];
  fr.dir.z = vx * old_m[5] - vy * old_m[4];
  new_m[4] = (vy * old_m[6] - vz * old_m[5]) + old_m[4];
  new_m[5] = (vz * fVar1 - vx * fVar2) + old_m[5];
  new_m[6] = fr.dir.z + old_m[6];
  fr.dir.x = vy * old_m[10] - vz * old_m[9];
  fVar1 = old_m[8];
  fVar2 = old_m[10];
  fr.dir.z = vx * old_m[9] - vy * old_m[8];
  new_m[8] = fr.dir.x + old_m[8];
  new_m[9] = (vz * fVar1 - vx * fVar2) + old_m[9];
  new_m[10] = fr.dir.z + old_m[10];
  MatrixOrthoNormalize_004d3d50(new_m, new_m);
}

/* entry thunk at the original address of car_integrate_step: register inputs (esi,edi,ebx,s0,s1) -> natural C car_integrate_step_n; ret 0 */
__declspec(naked) void car_integrate_step(void)
{
    __asm {
        push dword ptr [esp + 8]
        push dword ptr [esp + 8]
        push ebx
        push edi
        push esi
        call car_integrate_step_n
        add esp, 20
        ret
    }
}
