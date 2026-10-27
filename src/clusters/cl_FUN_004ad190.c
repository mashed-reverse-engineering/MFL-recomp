// cluster-entries: FUN_004ad190
/* Installed cluster FUN_004ad190 (tools/cluster_install.py from output/cluster_pilot_hb1/FUN_004ad190.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
extern __declspec(dllimport) int __stdcall timeGetTime(void);
typedef int (__fastcall *ThisMethod2)(int *self, int unused_edx, int a, int b);
/* FUN_004ad190 */

void FUN_004ad190_n(int *in_ecx, undefined4 unused_1)

{
  DWORD DVar1;
  
  (*(ThisMethod2 *)(*in_ecx + 0xb4))(in_ecx,0,in_ecx[0x54],in_ecx[0x55]);
  DVar1 = timeGetTime();
  in_ecx[0x3f] = DVar1;
  return;
}

/* entry thunk at the original address of FUN_004ad190: register inputs (ecx,s0) -> natural C FUN_004ad190_n; ret 4 */
__declspec(naked) void FUN_004ad190(void)
{
    __asm {
        push dword ptr [esp + 4]
        push ecx
        call FUN_004ad190_n
        add esp, 8
        ret 4
    }
}
