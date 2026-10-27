#include "mfl_tier2.h"
/*005bf4e0*/ extern int __cdecl FUN_005bf4e0(int a1, int a2, void *a3, void *a4);
/* gap_5c3020 */

/* no context */

uint gap_5c3020(int param_1,char *param_2,int param_3)

{
  int iVar1;
  int iVar2;
  int iVar3;
  uint uVar4;
  
  iVar3 = *(int *)(*(char **)(param_1 + 0x118) + 0x18);
  iVar1 = iVar3 + param_3 * 0x28;
  iVar2 = *(int *)(param_2 + 4);
  iVar3 = (*(uint *)(iVar1 + 0x20) / (uint)*(ushort *)(iVar3 + 0x1a + param_3 * 0x28)) *
          *(int *)(iVar1 + 0xc);
  FUN_005bf4e0(*(undefined4 *)(param_2 + 8),iVar3,&param_1,&param_3);
  uVar4 = 1;
  if ((*(byte *)(*(char **)(iVar1 + 0x14) + 0x18) & 4) == 0) {
    uVar4 = (uint)*(byte *)(*(char **)(iVar1 + 0x14) + 0xd);
  }
  return (uint)(iVar2 * iVar3 + param_1) / uVar4;
}


