#include "mfl_tier2.h"
/*004b1100*/ extern int __stdcall FUN_004b1100(int *p0, int *p1);
/*005e219c*/ extern int PTR_g_data_6e0049_005e219c[4];
/* FUN_004ac600 */

/* "QueryInterface of a sub object at this-0xc: asks the outer object (vtable +0x1c) and hands out the sub object at +0xc" */

int __stdcall FUN_004ac600(int param_1,int *param_2,int *param_3)

{
  int iVar1;
  int *piVar2;
  int *piNull;
  
  if (param_3 == (int *)0x0) {
    return 0x80004003;
  }
  iVar1 = FUN_004b1100(param_2,PTR_g_data_6e0049_005e219c);
  if (iVar1 == 0) {
    iVar1 = (*(int (__fastcall **)(int, int, int))(*(int *)(param_1 + -0xc) + 0x1c))(param_1 + -0xc,*(int *)(param_1 + -0xc),0);
    if (iVar1 != 0) {
      piVar2 = (int *)(iVar1 + 0xc);
      *param_3 = (int)piVar2;
      (*(int (__stdcall **)(int *))(*piVar2 + 4))(piVar2);
      return 0;
    }
    piNull = (int *)iVar1;
    *param_3 = 0;
    (*(int (__stdcall **)(int *))(*piNull + 4))(piNull);
    return 0;
  }
  *param_3 = 0;
  return 0x80040216;
}
