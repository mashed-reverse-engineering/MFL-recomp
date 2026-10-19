// cluster-entries: FUN_004ae490
/* Installed cluster FUN_004ae490 (tools/cluster_install.py from output/cluster_pilot_tr/FUN_004ae490.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
extern __declspec(dllimport) int __stdcall SetEvent(int);
/* FUN_004ae490 */

undefined4 FUN_004ae490_n(int *param_1)

{
  if ((param_1[5] != 0) && (param_1[0x1c] = 1, param_1[0x1b] == 0)) {
    SetEvent((HANDLE)param_1[0x17]);
    if (param_1[0x19] != 0) {
      (*(void (__fastcall **)(int *self, int unused_edx))(*param_1 + 0x60))(param_1, 0);      /* thiscall: this in ecx */
    }
  }
  return 0;
}

/* entry thunk at the original address of FUN_004ae490: register inputs (ecx) -> natural C FUN_004ae490_n; ret 0 */
__declspec(naked) void FUN_004ae490(void)
{
    __asm {
        push ecx
        call FUN_004ae490_n
        add esp, 4
        ret
    }
}
