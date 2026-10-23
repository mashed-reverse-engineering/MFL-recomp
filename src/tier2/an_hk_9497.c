#include "mfl_tier2.h"
/*0044a470*/ extern int camera_apply_rotation();
/*00481fd0*/ extern double random_range();
/*004d53a0*/ extern RwReal RwV3dLength(struct RwV3d *p0);
/*004d5410*/ extern RwReal _rwSqrt(RwReal p0);
/*005dd330*/ extern float float_1_0;
/*005dd340*/ extern float k_f_0_5;
/*005dd378*/ extern float k_f_0_1;
/*005ddacc*/ extern float k_f_360;
/*005ddad0*/ extern float k_f_180;
/*005ddad4*/ extern float k_f_90;
/*005ddafc*/ extern float k_f_0_05;
/*005e64a4*/ extern float k_f_0;
/*005de1ac*/ extern float k_f_0_125;
/*005de1b4*/ extern float k_f_0_0625;
/*005de4ac*/ extern float k_f_270;
/*005de8c4*/ extern float k_f_0_016666668;
/*005df2f4*/ extern float k_f_0_012048192;
/*005df2f8*/ extern float k_f_0_0042918455;
/*005df2fc*/ extern float k_f_0_012820513;
/*007ef804*/ extern undefined4 g_tick_number;
extern double atan2(double, double);
extern double sin(double);
/* camera_record_set_path */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* "fills a camera path record with start / end points and deltas" */

void __cdecl camera_record_set_path(char *param_1,float param_2,float param_3,float param_4,float param_5,float param_6,
          float param_7,float param_8,float param_9)

{
  float fVar1;
  float fVar2;
  double fVar3;
  double fVar4;
  double fVar5;
  double fVar6;
  double fVar7;
  RwReal RVar8;
  float num;
  RwV3d d;
  float sumf;
  
  num = param_2 - param_5;
  *(float *)(param_1 + 0x964) = param_5;
  fVar1 = param_3 - param_6;
  *(float *)(param_1 + 0x968) = param_6;
  *(float *)(param_1 + 0x96c) = param_7;
  fVar2 = param_4 - param_7;
  *(float *)(param_1 + 0x97c) = param_2;
  *(float *)(param_1 + 0x980) = param_3;
  *(float *)(param_1 + 0x984) = param_4;
  *(float *)(param_1 + 0x44) = param_6;
  *(float *)(param_1 + 0x40) = param_5;
  *(float *)(param_1 + 0x48) = param_7;
  d.x = num;
  d.y = fVar1;
  d.z = fVar2;
  RwV3dLength(&d);
  fVar3 = (double)sin((double)(int)g_tick_number * (double)k_f_0_05);
  *(float *)(param_1 + 0x58) =
       (float)((fVar3 + (double)float_1_0) * (double)param_8 * (double)param_9 *
               (double)k_f_0_05 + (double)*(volatile float *)&param_8);
  if (fVar2 == k_f_0) {
    if (k_f_0 <= (double)num) {
      fVar3 = (double)k_f_90;
    }
    else {
      fVar3 = (double)k_f_270;
    }
  }
  else {
    fVar3 = (double)atan2((double)num / (double)fVar2,(double)1);
    fVar3 = (double)k_f_180 - -(fVar3 * (double)(57.295799255371094));
    if (k_f_0 < fVar2) {
      fVar3 = fVar3 + (double)k_f_180;
    }
  }
  *(float *)(param_1 + 0x38) = (float)fVar3;
  num = num * num + fVar2 * fVar2;
  if (num != k_f_0) {
    RVar8 = _rwSqrt(num);
    if (RVar8 != k_f_0) {
      fVar3 = (double)atan2((double)fVar1 / (double)RVar8,(double)1);
      fVar3 = (double)k_f_180 - -(fVar3 * (double)(57.295799255371094));
      if (k_f_0 < RVar8) {
        fVar3 = fVar3 + (double)k_f_180;
      }
      goto LAB_0044c474;
    }
  }
  if (k_f_0 <= fVar1) {
    fVar3 = (double)k_f_90;
  }
  else {
    fVar3 = (double)k_f_270;
  }
LAB_0044c474:
  fVar7 = (double)k_f_360;
  *(float *)(param_1 + 0x34) = (float)(fVar7 - fVar3);
  fVar4 = (double)(int)g_tick_number;
  fVar5 = (double)sin((double)k_f_0_05 * fVar4);
  fVar6 = (double)sin(fVar4 * (double)k_f_0_012820513);
  fVar4 = (double)sin(fVar4 * (double)k_f_0_0042918455);
  *(volatile float *)&sumf = fVar6 + fVar5;
  *(float *)(param_1 + 0x34) =
       (float)(((double)*(volatile float *)&sumf * (double)k_f_0_5 + fVar4) * (double)k_f_0_5 *
               (double)param_9 + (fVar7 - fVar3));
  fVar3 = (double)sin((double)k_f_0_1 * (double)(int)g_tick_number);
  fVar7 = (double)sin((double)(int)g_tick_number * (double)k_f_0_012048192);
  param_8 = (float)(fVar7 + fVar3 * (double)k_f_0_125);
  fVar7 = (double)random_range(0,0x42700000);
  fVar3 = (double)(int)g_tick_number;
  *(undefined4 *)(param_1 + 0x3c) = 0;
  fVar3 = (double)sin((fVar7 + fVar3) * (double)k_f_0_016666668);
  *(float *)(param_1 + 0x38) =
       (float)((fVar3 * (double)k_f_0_0625 + (double)param_8) * (double)param_9 +
              (double)*(float *)(param_1 + 0x38));
  camera_apply_rotation(param_1);
  return;
}


