#include "mfl_tier2.h"
/*004b10c0*/ extern int __stdcall FUN_004b10c0(int a1, int a2, int a3);
/* COM method (stdcall): the callee pops the this pointer */
typedef int (__stdcall *ComMethod)(int *self);
#define COM_VTBL_ADDREF (0x4)
/* FUN_004aee20 */

undefined4 __stdcall FUN_004aee20(int param_1, undefined2 *param_2)

{
  int *piVar1;
  
  if (param_2 == (undefined2 *)0x0) {
    return 0x80004003;
  }
  if (*(int *)(param_1 + 0x30) == 0) {
    *param_2 = 0;
  }
  else {
    FUN_004b10c0((int)param_2,*(int *)(param_1 + 0x30),0x80);
  }
  *(undefined4 *)(param_2 + 0x80) = *(undefined4 *)(param_1 + 0x34);
  piVar1 = *(int **)(param_1 + 0x34);
  if (piVar1 != (int *)0x0) {
    (*(ComMethod *)(*piVar1 + COM_VTBL_ADDREF))(piVar1);
  }
  return 0;
}


