#include "mfl_tier2.h"
/*00467ea0*/ extern int weapon_get_rw_801b14();
/*004c4c70*/ extern int FUN_004c4c70(int p0, float *p1, float *p2);
/*004c51a0*/ extern void FUN_004c51a0(undefined4 p0, void *p1, float *p2);
/*005de490*/ extern float k_f_0_02;
/* particle_spawn_at_weapon_pos */

/* "spawns at weapon pos offset -local_8 along the direction" */

undefined4 __cdecl particle_spawn_at_weapon_pos(float *param_1,undefined4 param_2)

{
  undefined4 uVar1;
  int iVar2;
  struct {
    float local_4c;
    float local_48;
    float local_44;
    float local_40;
    float local_3c;
    float local_38;
    undefined1 pad_34[4];
    undefined1 local_30 [40];
    float local_8;
    undefined1 pad_4[4];
  } frame;
  
  iVar2 = FUN_004c4c70(weapon_get_rw_801b14(),param_1,&frame.local_40);
  if (iVar2 != 0) {
    float10 s = -frame.local_8;
    float10 x = frame.local_40 * s;
    float10 y;
    *(volatile float *)&frame.local_48 = frame.local_3c * s;
    *(volatile float *)&frame.local_44 = frame.local_38 * s;
    x = x + *param_1;
    y = param_1[1] + *(volatile float *)&frame.local_48;
    *(volatile float *)&frame.local_44 = param_1[2] + *(volatile float *)&frame.local_44;
    frame.local_4c = k_f_0_02 * frame.local_40 + x;
    frame.local_48 = k_f_0_02 * frame.local_3c + y;
    frame.local_44 = k_f_0_02 * frame.local_38 + *(volatile float *)&frame.local_44;
    FUN_004c51a0(param_2,frame.local_30,&frame.local_4c);
    return 1;
  }
  return 0;
}
