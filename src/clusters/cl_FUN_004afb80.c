// cluster-entries: FUN_004afb80
/* Installed cluster FUN_004afb80 (tools/cluster_install.py from output/cluster_pilot_hb1/FUN_004afb80.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
typedef int (__stdcall *ComMethod1)(int *self);
/* FUN_004afb80 */

int FUN_004afb80_n(int this_ptr)

{
  int *com_object;
  int hr;
  
  com_object = *(int **)(this_ptr + 0x9c);
  if (com_object != (int *)0x0) {
    hr = (*(ComMethod1 *)(*com_object + 0x18))(com_object);
    if (hr < 0) {
      return hr;
    }
    (*(ComMethod1 *)(**(int **)(this_ptr + 0x9c) + 8))(*(int **)(this_ptr + 0x9c));
    *(undefined4 *)(this_ptr + 0x9c) = 0;
  }
  return 0;
}

/* entry thunk at the original address of FUN_004afb80: register inputs (ecx) -> natural C FUN_004afb80_n; ret 0 */
__declspec(naked) void FUN_004afb80(void)
{
    __asm {
        push ecx
        call FUN_004afb80_n
        add esp, 4
        ret
    }
}
