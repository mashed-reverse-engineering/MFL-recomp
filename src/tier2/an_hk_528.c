#include "mfl_tier2.h"
/*00497fa0*/ extern int fx_trail_particle_spawn();
/* fx_smoke_particle_spawn_grey */

/* "spawns a grey (0x20) type 6 particle with scale 1.4" */

void __cdecl
fx_smoke_particle_spawn_grey
          (undefined4 param_1,undefined4 param_2,char *param_3,char *param_4,
          char *param_5,char *param_6,char *param_7)

{
  char *iVar1;
  
  iVar1 = fx_trail_particle_spawn(param_1,0,0,0);
  *(undefined1 *)(iVar1 + 0xc) = 0x20;
  *(undefined1 *)(iVar1 + 0xd) = 0x20;
  *(undefined1 *)(iVar1 + 0xe) = 0x20;
  *(undefined4 *)(iVar1 + 0x14) = param_2;
  *(undefined4 *)(iVar1 + 0x24) = 0;
  *(undefined4 *)(iVar1 + 0x28) = 0;
  *(undefined4 *)(iVar1 + 0x2c) = 0;
  *(undefined4 *)(iVar1 + 0x18) = 0;
  *(undefined4 *)(iVar1 + 0x20) = 0;
  *(undefined1 *)(iVar1 + 0x45) = 6;
  *(undefined4 *)(iVar1 + 0x1c) = 0x40000000;
  *(undefined1 *)(iVar1 + 0xf) = 0xff;
  *(undefined4 *)(iVar1 + 0x38) = 0x3fb33333;
  *(undefined4 *)(iVar1 + 0x10) = param_2;
  *(uint *)(iVar1 + 0x40) = *(uint *)(iVar1 + 0x40) | 0x1000;
  return;
}


