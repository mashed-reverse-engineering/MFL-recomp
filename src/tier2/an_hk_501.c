#include "mfl_tier2.h"
/*00482070*/ extern float dot_product_3d_clamped();
/*004b2b48*/ extern int _ftol();
/*004d53a0*/ extern RwReal RwV3dLength(struct RwV3d *p0);
/*005ddafc*/ extern float k_f_0_05;
/*005ddb00*/ extern float k_f_0_025;
/* hud_player_marker_position_calc_with_facing */

/* "as hud_player_marker_position_calc but also computes the facing dot product" */

void __cdecl
hud_player_marker_position_calc_with_facing
          (float *param_1,char *param_2,char *param_3,char *param_4,float param_5,float param_6,
          undefined1 *param_7,int *param_8,char *param_9,char *param_10,
          char *param_11)

{
  float fVar1;
  volatile float zprod;      /* rounds the z product to float like the original's stack temp (it is stored, then reloaded for the sum) */
  float fdot;
  undefined1 uVar2;
  RwReal RVar3;
  struct {
    RwV3d local_24;
    undefined4 local_18;
    undefined4 local_14;
    undefined4 local_10;
    undefined4 local_c;
    undefined4 local_8;
    undefined4 local_4;
  } frame;
  
  if (*param_8 < 0x7f) {
    frame.local_24.x = *(float *)(param_2 + 0x30) - *(float *)(param_3 + 0x30);
    frame.local_24.y = *(float *)(param_2 + 0x34) - *(float *)(param_3 + 0x34);
    frame.local_24.z = *(float *)(param_2 + 0x38) - *(float *)(param_3 + 0x38);
    RVar3 = RwV3dLength(&frame.local_24);
    frame.local_18 = *(undefined4 *)(param_3 + 0x20);
    frame.local_14 = *(undefined4 *)(param_3 + 0x24);
    frame.local_10 = *(undefined4 *)(param_3 + 0x28);
    frame.local_c = *(undefined4 *)(param_2 + 0x20);
    frame.local_8 = *(undefined4 *)(param_2 + 0x24);
    frame.local_4 = *(undefined4 *)(param_2 + 0x28);
    fdot = dot_product_3d_clamped(&frame.local_c,&frame.local_18);
    zprod = frame.local_24.z * param_5;
    *param_1 = frame.local_24.x * param_5 + *(float *)(param_2 + 0x30);
    param_1[1] = frame.local_24.y * param_5 + *(float *)(param_2 + 0x34);
    fVar1 = *(float *)(param_2 + 0x38);
    param_1[5] = 1.0;
    param_1[7] = 1.0;
    param_1[2] = zprod + fVar1;
    param_1[6] = 0.0;
    param_1[8] = 0.0;
    param_1[3] = RVar3 * k_f_0_025 + k_f_0_05;
    *(undefined1 *)(param_1 + 9) = *param_7;
    *(undefined1 *)((char *)param_1 + 0x25) = param_7[1];
    *(undefined1 *)((char *)param_1 + 0x26) = param_7[2];
    uVar2 = (int)(fdot * fdot * 255.0f * *(float *)&param_4);
    *(undefined1 *)((char *)param_1 + 0x27) = uVar2;
    param_1[10] = param_6;
    *param_8 = *param_8 + 1;
  }
  return;
}


