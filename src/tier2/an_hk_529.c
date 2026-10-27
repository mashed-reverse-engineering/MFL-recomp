#include "mfl_tier2.h"
/*00497fa0*/ extern int fx_trail_particle_spawn();
/*005dd330*/ extern float float_1_0;
/* fx_smoke_particle_spawn_tinted */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* "spawns a type 6 particle with a given colour and size+1" */

void fx_smoke_particle_spawn_tinted
               (undefined4 param_1,undefined4 param_2,undefined4 *param_3,char *unused_4,
               float param_5)

{
  undefined4 uVar1;
  char *iVar2;
  
  iVar2 = fx_trail_particle_spawn(param_1,0,0,0);
  param_5 = param_5 + float_1_0;
  *(undefined4 *)(iVar2 + 0x24) = 0;
  *(undefined4 *)(iVar2 + 0x28) = 0;
  *(undefined4 *)(iVar2 + 0x2c) = 0;
  *(undefined4 *)(iVar2 + 0x18) = 0;
  *(undefined4 *)(iVar2 + 0x20) = 0;
  *(undefined1 *)(iVar2 + 0x45) = 6;
  *(undefined4 *)(iVar2 + 0x1c) = 0x3f800000;
  uVar1 = *param_3;
  *(float *)(iVar2 + 0x38) = param_5;
  *(undefined4 *)(iVar2 + 0xc) = uVar1;
  *(undefined4 *)(iVar2 + 0x14) = param_2;
  *(undefined4 *)(iVar2 + 0x10) = param_2;
  *(uint *)(iVar2 + 0x40) = *(uint *)(iVar2 + 0x40) | 0x1000;
  return;
}


