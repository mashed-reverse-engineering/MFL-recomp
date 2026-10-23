#include "mfl_tier2.h"
/*0056b1d0*/ extern void rwp_world_set_bit_5c(int p0, void *p1, int p2);
/*0056b300*/ extern void FUN_0056b300(void *p0, void *p1);
/*0056bc20*/ extern void FUN_0056bc20(void *p0, void *p1, undefined4 p2);
/*0058c8a0*/ extern int rwp_owner_object_get(int p0);
/* debris_apply_velocity_b */

/* "(body, vec3 or zero, flag): same with extra param" */

void __cdecl
debris_apply_velocity_b
          (undefined4 param_1,undefined4 *param_2,int param_3,char *param_4,char *param_5,
          char *param_6,char *param_7,char *param_8,char *param_9,
          char *param_10,char *param_11,char *param_12,char *param_13,
          char *param_14,char *param_15,char *param_16,char *param_17)

{
  char *iVar1;
  int *piVar2;
  char *iVar3;
  struct {
    undefined4 local_c;
    undefined4 local_8;
    undefined4 local_4;
  } frame;
  
  frame.local_c = 0;
  frame.local_8 = 0;
  frame.local_4 = 0;
  if (param_2 == (undefined4 *)0x0) {
    param_2 = &frame.local_c;
  }
  iVar3 = rwp_owner_object_get(param_1);
  iVar1 = *(char **)(iVar3 + 0x24);
  if (iVar1 != 0) {
    if ((*(uint *)(*(char **)(iVar1 + 0x60) + (uint)(*(ushort *)(iVar3 + 0x20) >> 5) * 4) &
        1 << ((byte)*(ushort *)(iVar3 + 0x20) & 0x1f)) != 0) goto LAB_00424998;
  }
  FUN_0056b300(iVar1,iVar3);
LAB_00424998:
  piVar2 = *(int **)(*(char **)(iVar3 + 0x10) + 4);
  if (param_3 == 0) {
    param_3 = **(int **)(*piVar2 + 0x10) + 0x30 + piVar2[1] * 0x40;
  }
  FUN_0056bc20(piVar2,param_2,param_3);
  rwp_world_set_bit_5c(*(undefined4 *)(iVar3 + 0x24),iVar3,1);
  return;
}


