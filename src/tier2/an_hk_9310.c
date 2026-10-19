#include "mfl_tier2.h"
/* FUN_004af740 */
/* TIER2: unvalidated 2026-10-19 authority=static */
int __stdcall FUN_004af740(int *param_1)
{
  int iVar1;
  
  if (param_1[6] == 0) {
    iVar1 = 1;
  }
  else {
    iVar1 = (*(int (__fastcall **)(int *))(*param_1 + 0x2c))(param_1);
    if (-1 < iVar1) {
      (*(void (__stdcall **)(int *))(*(int *)param_1[6] + 8))((int *)param_1[6]);
      param_1[6] = 0;
      return 0;
    }
  }
  return iVar1;
}
