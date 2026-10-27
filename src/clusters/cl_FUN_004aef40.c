// cluster-entries: FUN_004aef40
/* Installed cluster FUN_004aef40 (tools/cluster_install.py from output/cluster_pilot_hb1/FUN_004aef40.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
typedef int (__stdcall *ComMethod4)(int *self, int a, int b, int c);
/* FUN_004aef40 */

undefined4 FUN_004aef40_n(char *self, int command, undefined4 arg_a, undefined4 arg_b)

{
  int *com_object;
  undefined4 result;
  
  com_object = *(int **)(self + 0x44);
  if (com_object == (int *)0x0) {
    return 0x80004001;
  }
  if (command == 1) {
    result = (*(ComMethod4 *)(*com_object + 0xc))(com_object,1,arg_a,self + 0xc);
    return result;
  }
  result = (*(ComMethod4 *)(*com_object + 0xc))(com_object,command,arg_a,arg_b);
  return result;
}

/* entry thunk at the original address of FUN_004aef40: register inputs (ecx,s0,s1,s2) -> natural C FUN_004aef40_n; ret 12 */
__declspec(naked) void FUN_004aef40(void)
{
    __asm {
        push dword ptr [esp + 12]
        push dword ptr [esp + 12]
        push dword ptr [esp + 12]
        push ecx
        call FUN_004aef40_n
        add esp, 16
        ret 12
    }
}
