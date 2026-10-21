#include "mfl_tier2.h"
/*004943c0*/ extern int rwp_world_clear_arena();
/*00494460*/ extern int physics_buffer_size();
/*004b2b48*/ extern int _ftol();
/*005dd634*/ extern float k_f_2;
/*005dd7e8*/ extern float k_f_60;
/*007d2128*/ extern char *RwEngineInstance;
/* physics_buffer_create */

/* "allocates and clears 0x19C bytes" */

undefined4 * physics_buffer_create(float param_1,int param_2,undefined4 param_3)

{
  int fbits;
  int ft1;
  int bytes;
  undefined4 *puVar1;
  undefined4 uVar2;
  void *pvVar3;
  int iVar4;
  undefined4 *puVar5;

  puVar1 = RwMalloc(0x19c,0);
  puVar5 = puVar1;
  for (iVar4 = 0x67; iVar4 != 0; iVar4 = iVar4 + -1) {
    *puVar5 = 0;
    puVar5 = puVar5 + 1;
  }
  fbits = *(int *)&param_1;
  puVar1[3] = fbits;
  puVar1[4] = param_2;
  uVar2 = physics_buffer_size(fbits,param_2);
  puVar1[1] = uVar2;
  puVar1[2] = param_3;
  ft1 = (int)(param_1 * 60.0f);
  iVar4 = (int)((float10)ft1 / param_2 + k_f_2);
  bytes = iVar4 * 0x24;
  puVar1[0x5b] = iVar4 + -2;
  pvVar3 = RwMalloc(bytes,0);
  puVar1[5] = (undefined4)pvVar3;
  *puVar1 = 0x10b;
  rwp_world_clear_arena(puVar1);
  return puVar1;
}
