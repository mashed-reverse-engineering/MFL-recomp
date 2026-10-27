#include "mfl_tier2.h"
/* gap_5c6990 */

/* no context */

int gap_5c6990(byte *param_1,int param_2,byte *param_3)

{
  byte bVar1;
  int iVar2;
  int iVar3;
  
  iVar2 = (int)(param_2 + 0xf + (param_2 + 0xf >> 0x1f & 0xfU)) >> 4;
  param_2 = iVar2;
  if (0 < iVar2) {
    do {
      bVar1 = *param_1;
      param_1 = param_1 + 2;
      iVar3 = 2;
      do {
        *param_3 = 0xc - (bVar1 & 0xf) | bVar1 & 0xf0;
        param_3[1] = *param_1 >> 4 | *param_1 << 4;
        param_3[2] = param_1[1] >> 4 | param_1[1] << 4;
        param_3[3] = param_1[2] >> 4 | param_1[2] << 4;
        param_3[4] = param_1[3] >> 4 | param_1[3] << 4;
        param_3[5] = param_1[4] >> 4 | param_1[4] << 4;
        param_3[6] = param_1[5] >> 4 | param_1[5] << 4;
        param_3[7] = param_1[6] >> 4 | param_1[6] << 4;
        param_1 = param_1 + 7;
        param_3 = param_3 + 8;
        iVar3 = iVar3 + -1;
      } while (iVar3 != 0);
      param_2 = param_2 + -1;
    } while (param_2 != 0);
  }
  return iVar2 << 4;
}


