#include "mfl_tier2.h"
/*0047b520*/ extern int car_current_matrix();
/*00490c60*/ extern int __cdecl physics_body_get_velocity(undefined4 p0, undefined4 p1);
/*004d53a0*/ extern RwReal RwV3dLength(struct RwV3d *p0);
/*004d56d0*/ extern struct RwV3d * RwV3dTransformVectors(struct RwV3d *p0, struct RwV3d *p1, RwInt32 p2, RwMatrix *p3);
/*0056bc20*/ extern void FUN_0056bc20(undefined4 p0, undefined4 p1, undefined4 p2);
/*0058c8a0*/ extern int rwp_owner_object_get(int p0);
/*005dd330*/ extern float float_1_0;
/*005dd340*/ extern float k_f_0_5;
/*005dd62c*/ extern float k_f_100;
/*005e04d8*/ extern float k_f_n6_5;
/*005e04dc*/ extern float k_f_n0_22222222;
/*005e04e0*/ extern float k_f_0_22222222;
/*005e04e4*/ extern float k_f_n0_11111111;
/*005e04e8*/ extern float k_f_0_11111111;
/*005e04ec*/ extern float k_f_n6_6666666en4;
/*0062ac68*/ extern float g_car_62ac68;
/*006fd498*/ extern undefined4 g_rw_6fd498;
/*008b06e0*/ extern struct Car player_car[16];
/* rwp_car_apply_forces */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* "applies the physics object forces from the car matrix" */

void rwp_car_apply_forces(int param_1,undefined4 param_2)

{
  int *piVar1;
  int iVar2;
  int iVar3;
  float *pfVar4;
  float fVar5;
  struct {
    RwV3d local_24;
    float local_18;
    float local_14;
    float local_10;
    float local_c;
    float local_8;
    float local_4;
  } frame;
  
  iVar2 = param_1;
  car_current_matrix(&param_1,param_1);
  iVar3 = rwp_owner_object_get((&g_rw_6fd498)[iVar2]);
  piVar1 = *(int **)(*(int *)(iVar3 + 0x10) + 4);
  if (piVar1 == (int *)0x0) {
    return;
  }
  switch(param_2) {
  case 0:
    frame.local_18 = *(float *)(param_1 + 0x30);
    frame.local_14 = *(float *)(param_1 + 0x34);
    frame.local_10 = *(float *)(param_1 + 0x38);
    pfVar4 = (float *)(*(int *)(*(int *)(*piVar1 + 0x10) + 8) + piVar1[1] * 0x20);
    frame.local_24.x = *pfVar4 * k_f_n6_6666666en4;
    frame.local_24.y = pfVar4[1] * k_f_n6_6666666en4;
    frame.local_8 = 350.0;
    frame.local_24.z = pfVar4[2] * k_f_n6_6666666en4;
    break;
  case 1:
    pfVar4 = (float *)(*(int *)(*(int *)(*piVar1 + 0x10) + 8) + piVar1[1] * 0x20);
    frame.local_24.x = *pfVar4 * k_f_n6_6666666en4;
    frame.local_24.y = pfVar4[1] * k_f_n6_6666666en4;
    frame.local_24.z = pfVar4[2] * k_f_n6_6666666en4;
    frame.local_c = 0.0;
    frame.local_8 = 350.0;
    frame.local_4 = 0.0;
    frame.local_18 = frame.local_24.x + *(float *)(param_1 + 0x30);
    frame.local_14 = frame.local_24.y + *(float *)(param_1 + 0x34);
    frame.local_10 = frame.local_24.z + *(float *)(param_1 + 0x38);
    FUN_0056bc20((undefined4)piVar1,(undefined4)&frame.local_c,(undefined4)&frame.local_18);
    RwV3dTransformVectors(&frame.local_24,(RwV3d *)&g_car_62ac68,1,(RwMatrix *)player_car[iVar2].matrices)
    ;
    frame.local_24.x = frame.local_24.x * k_f_0_11111111;
    frame.local_24.y = frame.local_24.y * k_f_0_11111111;
    frame.local_24.z = frame.local_24.z * k_f_0_11111111;
    frame.local_18 = frame.local_24.x + frame.local_18;
    frame.local_14 = frame.local_24.y + frame.local_14;
    frame.local_10 = frame.local_24.z + frame.local_10;
    frame.local_c = 0.0;
    frame.local_8 = 150.0;
    frame.local_4 = 0.0;
    FUN_0056bc20((undefined4)piVar1,(undefined4)&frame.local_c,(undefined4)&frame.local_18);
    return;
  case 2:
    pfVar4 = (float *)(*(int *)(*(int *)(*piVar1 + 0x10) + 8) + piVar1[1] * 0x20);
    frame.local_24.x = *pfVar4 * k_f_n6_6666666en4;
    frame.local_24.y = pfVar4[1] * k_f_n6_6666666en4;
    frame.local_24.z = pfVar4[2] * k_f_n6_6666666en4;
    frame.local_c = 0.0;
    frame.local_8 = 350.0;
    frame.local_4 = 0.0;
    frame.local_18 = frame.local_24.x + *(float *)(param_1 + 0x30);
    frame.local_14 = frame.local_24.y + *(float *)(param_1 + 0x34);
    frame.local_10 = frame.local_24.z + *(float *)(param_1 + 0x38);
    FUN_0056bc20((undefined4)piVar1,(undefined4)&frame.local_c,(undefined4)&frame.local_18);
    RwV3dTransformVectors(&frame.local_24,(RwV3d *)&g_car_62ac68,1,(RwMatrix *)player_car[iVar2].matrices)
    ;
    frame.local_24.x = frame.local_24.x * k_f_n0_11111111;
    frame.local_24.y = frame.local_24.y * k_f_n0_11111111;
    frame.local_24.z = frame.local_24.z * k_f_n0_11111111;
    frame.local_18 = frame.local_24.x + frame.local_18;
    frame.local_14 = frame.local_24.y + frame.local_14;
    frame.local_10 = frame.local_24.z + frame.local_10;
    frame.local_c = 0.0;
    frame.local_8 = 150.0;
    frame.local_4 = 0.0;
    FUN_0056bc20((undefined4)piVar1,(undefined4)&frame.local_c,(undefined4)&frame.local_18);
    return;
  case 3:
    pfVar4 = (float *)(*(int *)(*(int *)(*piVar1 + 0x10) + 8) + piVar1[1] * 0x20);
    frame.local_24.x = *pfVar4 * k_f_n6_6666666en4;
    frame.local_24.y = pfVar4[1] * k_f_n6_6666666en4;
    frame.local_24.z = pfVar4[2] * k_f_n6_6666666en4;
    frame.local_18 = frame.local_24.x + *(float *)(param_1 + 0x30);
    frame.local_14 = frame.local_24.y + *(float *)(param_1 + 0x34);
    frame.local_10 = frame.local_24.z + *(float *)(param_1 + 0x38);
    RwV3dTransformVectors(&frame.local_24,(RwV3d *)&g_car_62ac68,1,(RwMatrix *)player_car[iVar2].matrices)
    ;
    frame.local_24.x = frame.local_24.x * k_f_0_11111111;
    frame.local_c = 0.0;
    frame.local_8 = 180.0;
    frame.local_4 = 0.0;
    frame.local_24.y = frame.local_24.y * k_f_0_11111111;
    frame.local_24.z = frame.local_24.z * k_f_0_11111111;
    frame.local_18 = frame.local_24.x + frame.local_18;
    frame.local_14 = frame.local_24.y + frame.local_14;
    frame.local_10 = frame.local_24.z + frame.local_10;
    FUN_0056bc20((undefined4)piVar1,(undefined4)&frame.local_c,(undefined4)&frame.local_18);
    return;
  case 4:
    pfVar4 = (float *)(*(int *)(*(int *)(*piVar1 + 0x10) + 8) + piVar1[1] * 0x20);
    frame.local_24.x = *pfVar4 * k_f_n6_6666666en4;
    frame.local_24.y = pfVar4[1] * k_f_n6_6666666en4;
    frame.local_24.z = pfVar4[2] * k_f_n6_6666666en4;
    frame.local_18 = frame.local_24.x + *(float *)(param_1 + 0x30);
    frame.local_14 = frame.local_24.y + *(float *)(param_1 + 0x34);
    frame.local_10 = frame.local_24.z + *(float *)(param_1 + 0x38);
    RwV3dTransformVectors(&frame.local_24,(RwV3d *)&g_car_62ac68,1,(RwMatrix *)player_car[iVar2].matrices)
    ;
    frame.local_24.x = frame.local_24.x * k_f_n0_11111111;
    frame.local_c = 0.0;
    frame.local_8 = 180.0;
    frame.local_4 = 0.0;
    frame.local_24.y = frame.local_24.y * k_f_n0_11111111;
    frame.local_24.z = frame.local_24.z * k_f_n0_11111111;
    frame.local_18 = frame.local_24.x + frame.local_18;
    frame.local_14 = frame.local_24.y + frame.local_14;
    frame.local_10 = frame.local_24.z + frame.local_10;
    FUN_0056bc20((undefined4)piVar1,(undefined4)&frame.local_c,(undefined4)&frame.local_18);
    return;
  case 5:
    pfVar4 = (float *)(*(int *)(*(int *)(*piVar1 + 0x10) + 8) + piVar1[1] * 0x20);
    frame.local_24.x = *pfVar4 * k_f_n6_6666666en4;
    frame.local_24.y = pfVar4[1] * k_f_n6_6666666en4;
    frame.local_24.z = pfVar4[2] * k_f_n6_6666666en4;
    frame.local_18 = frame.local_24.x + *(float *)(param_1 + 0x30);
    frame.local_14 = frame.local_24.y + *(float *)(param_1 + 0x34);
    frame.local_10 = frame.local_24.z + *(float *)(param_1 + 0x38);
    frame.local_24.x = player_car[iVar2].forward[0] * k_f_0_22222222;
    frame.local_8 = 425.0;
    frame.local_24.y = player_car[iVar2].forward[1] * k_f_0_22222222;
    frame.local_24.z = player_car[iVar2].forward[2] * k_f_0_22222222;
    goto LAB_004929af;
  case 6:
    pfVar4 = (float *)(*(int *)(*(int *)(*piVar1 + 0x10) + 8) + piVar1[1] * 0x20);
    frame.local_24.x = *pfVar4 * k_f_n6_6666666en4;
    frame.local_24.y = pfVar4[1] * k_f_n6_6666666en4;
    frame.local_24.z = pfVar4[2] * k_f_n6_6666666en4;
    frame.local_18 = frame.local_24.x + *(float *)(param_1 + 0x30);
    frame.local_14 = frame.local_24.y + *(float *)(param_1 + 0x34);
    frame.local_10 = frame.local_24.z + *(float *)(param_1 + 0x38);
    frame.local_24.x = player_car[iVar2].forward[0] * k_f_n0_22222222;
    frame.local_8 = 425.0;
    frame.local_24.y = player_car[iVar2].forward[1] * k_f_n0_22222222;
    frame.local_24.z = player_car[iVar2].forward[2] * k_f_n0_22222222;
    break;
  case 7:
    pfVar4 = (float *)(*(int *)(*(int *)(*piVar1 + 0x10) + 8) + piVar1[1] * 0x20);
    frame.local_24.x = *pfVar4 * k_f_n6_6666666en4;
    frame.local_24.y = pfVar4[1] * k_f_n6_6666666en4;
    frame.local_24.z = pfVar4[2] * k_f_n6_6666666en4;
    frame.local_18 = frame.local_24.x + *(float *)(param_1 + 0x30);
    frame.local_14 = frame.local_24.y + *(float *)(param_1 + 0x34);
    frame.local_10 = frame.local_24.z + *(float *)(param_1 + 0x38);
    frame.local_24.x = player_car[iVar2].forward[0] * k_f_n0_22222222;
    frame.local_8 = 125.0;
    frame.local_24.y = player_car[iVar2].forward[1] * k_f_n0_22222222;
    frame.local_24.z = player_car[iVar2].forward[2] * k_f_n0_22222222;
LAB_004929af:
    frame.local_18 = frame.local_24.x + frame.local_18;
    frame.local_14 = frame.local_24.y + frame.local_14;
    frame.local_c = 0.0;
    frame.local_4 = 0.0;
    frame.local_10 = frame.local_24.z + frame.local_10;
    FUN_0056bc20((undefined4)piVar1,(undefined4)&frame.local_c,(undefined4)&frame.local_18);
    return;
  case 8:
    iVar3 = physics_body_get_velocity((undefined4)&frame.local_24,iVar2);
    fVar5 = k_f_100;
    if (iVar3 != 0) {
      fVar5 = RwV3dLength(&frame.local_24);
    }
    if ((6000 < *(int *)&player_car[iVar2].field_0xc) || (fVar5 < float_1_0)) {
      frame.local_18 = *(float *)(param_1 + 0x30);
      frame.local_14 = *(float *)(param_1 + 0x34) + k_f_0_5;
      frame.local_10 = *(float *)(param_1 + 0x38);
      frame.local_c = player_car[iVar2].up[0] * k_f_n6_5;
      frame.local_8 = player_car[iVar2].up[1] * k_f_n6_5;
      frame.local_4 = player_car[iVar2].up[2] * k_f_n6_5;
      FUN_0056bc20((undefined4)piVar1,(undefined4)&frame.local_c,(undefined4)&frame.local_18);
    }
  default:
    return;
  }
  frame.local_18 = frame.local_24.x + frame.local_18;
  frame.local_14 = frame.local_24.y + frame.local_14;
  frame.local_c = 0.0;
  frame.local_4 = 0.0;
  frame.local_10 = frame.local_24.z + frame.local_10;
  FUN_0056bc20((undefined4)piVar1,(undefined4)&frame.local_c,(undefined4)&frame.local_18);
  return;
}


