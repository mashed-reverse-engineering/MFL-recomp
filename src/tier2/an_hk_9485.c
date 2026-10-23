#include "mfl_tier2.h"
/*0049b890*/ extern int particle_batch_add(int p0, void *p1, void *p2);
/*0049b910*/ extern int particle_batch_set_uv_a(int p0, void *p1);
/*0049b940*/ extern int particle_batch_set_size(int p0, int p1);
/*0049bd50*/ extern int fx_system_flush(int p0);
/*005dd330*/ extern float float_1_0;
/*005dd328*/ extern float k_f_0_6;
/*005dd370*/ extern float k_f_0_3;
/* fx_ribbon_add_point */

/* "appends a point (pos, optional vel, colour) to a ribbon ring and fades the older points" */

void fx_ribbon_add_point(int *param_1,undefined4 *param_2,undefined4 *param_3,undefined4 *param_4)

{
  float *pfVar1;
  int *piVar2;
  int iVar4;
  undefined4 *puVar5;
  int iVar6;
  double fVar7;
  double fVar8;
  uint alpha;
  struct {
    float local_14;
    float local_10;
    undefined4 local_c;
    undefined4 local_8;
    undefined4 local_4;
  } frame;
  
  piVar2 = param_1;
  puVar5 = (undefined4 *)(*param_1 + param_1[9] * 0xc);
  *puVar5 = *param_2;
  puVar5[1] = param_2[1];
  puVar5[2] = param_2[2];
  if (param_3 == (undefined4 *)0x0) {
    *(undefined4 *)(param_1[1] + param_1[9] * 0xc) = 0;
    *(undefined4 *)(param_1[1] + 4 + param_1[9] * 0xc) = 0;
    *(undefined4 *)(param_1[1] + 8 + param_1[9] * 0xc) = 0;
  }
  else {
    puVar5 = (undefined4 *)(param_1[1] + param_1[9] * 0xc);
    *puVar5 = *param_3;
    puVar5[1] = param_3[1];
    puVar5[2] = param_3[2];
  }
  if (param_4 == (undefined4 *)0x0) {
    *(undefined1 *)(param_1[2] + param_1[9] * 4) = 0xff;
    *(undefined1 *)(param_1[2] + 1 + param_1[9] * 4) = 0xff;
    *(undefined1 *)(param_1[2] + 2 + param_1[9] * 4) = 0xff;
    *(undefined1 *)(param_1[2] + 3 + param_1[9] * 4) = 0xff;
  }
  else {
    *(undefined4 *)(param_1[2] + param_1[9] * 4) = *param_4;
  }
  iVar6 = param_1[9];
  iVar4 = param_1[3];
  param_1[9] = iVar6 + 1;
  if (iVar4 <= iVar6 + 1) {
    param_1[9] = 0;
  }
  iVar6 = iVar4 - param_1[4];
  if (iVar6 < iVar4) {
    do {
      iVar4 = piVar2[9] + iVar6;
      fVar8 = float_1_0 - (double)iVar6 / piVar2[3];
      if (piVar2[3] <= iVar4) {
        iVar4 = (piVar2[9] - piVar2[3]) + iVar6;
      }
      *(uint *)&param_2 = *(uint *)(piVar2[2] + iVar4 * 4);
      alpha = *(uint *)&param_2 >> 0x18;
      *((char *)&param_2 + 3) = (char)(int)((double)(int)alpha - (double)(int)(alpha - 10) * fVar8);
      iVar4 = iVar4 * 0xc;
      puVar5 = (undefined4 *)(*piVar2 + iVar4);
      frame.local_c = *puVar5;
      fVar7 = ((*(float *)&piVar2[7] - *(float *)&piVar2[6]) * fVar8 + (double)*(float *)&piVar2[6]) * (double)k_f_0_3;
      frame.local_8 = puVar5[1];
      frame.local_4 = puVar5[2];
      frame.local_10 = (float)fVar7;
      frame.local_14 = (float)(fVar7 * (double)k_f_0_6);
      *(float *)&param_3 = (float)(fVar8 * (double)*(float *)&piVar2[8]);
      particle_batch_add(piVar2[10],&frame.local_c,&param_2);
      particle_batch_set_uv_a(piVar2[10],&frame.local_14);
      particle_batch_set_size(piVar2[10],*(int *)&param_3);
      iVar6 = iVar6 + 1;
      *(float *)(*piVar2 + iVar4) = *(float *)(iVar4 + piVar2[1]) + *(float *)(*piVar2 + iVar4);
      pfVar1 = (float *)(*piVar2 + 4 + iVar4);
      *pfVar1 = *(float *)(iVar4 + 4 + piVar2[1]) + *pfVar1;
      pfVar1 = (float *)(*piVar2 + 8 + iVar4);
      *pfVar1 = *(float *)(iVar4 + 8 + piVar2[1]) + *pfVar1;
    } while (iVar6 < piVar2[3]);
    fx_system_flush(piVar2[10]);
  }
  return;
}
