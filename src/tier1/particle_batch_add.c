#include "mfl_tier2.h"


/* particle_batch_add */

/* "appends a particle (position, colour) to the batch if it is not full" */

void particle_batch_add(int *param_1,undefined4 *param_2,undefined1 *param_3)
{
  undefined4 *puVar1;
  int iVar2;
  
  if (param_1[2] >= param_1[1]) {
    param_1[2] = param_1[1];
    return;
  }
  iVar2 = param_1[2] * 0x30;
  puVar1 = (undefined4 *)(*param_1 + 4 + iVar2);
  *puVar1 = *param_2;
  puVar1[1] = param_2[1];
  puVar1[2] = param_2[2];
  *(undefined4 *)(*param_1 + 0x1c + iVar2) = 0;
  *(undefined1 *)(iVar2 + *param_1) = *param_3;
  *(undefined1 *)(*param_1 + 1 + iVar2) = param_3[1];
  *(undefined1 *)(*param_1 + 2 + iVar2) = param_3[2];
  *(undefined1 *)(*param_1 + 3 + iVar2) = param_3[3];
  param_1[2] = param_1[2] + 1;
  param_1[0x15] = 1;
  return;
}
