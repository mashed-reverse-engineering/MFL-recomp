// cluster-entries: FUN_004af0c0
/* Installed cluster FUN_004af0c0 (tools/cluster_install.py from output/cluster_pilot_zy/FUN_004af0c0.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"

static void FUN_004b2630_r(void * p0);
extern void FUN_004b2630(void);
/* Refreshes the cached values of the object: asks its sub object (+0xc) virtual method +0x14 -> +0x10 and +0x18 -> +0x8, clears +4 and resets the member at +0x18. Returns 0. */
int FUN_004af0c0_n(int *object)
{
  object[4] = ((int (__fastcall *)(void *))(*(int **)object[3])[5])((void *)object[3]);
  object[2] = ((int (__fastcall *)(void *))(*(int **)object[3])[6])((void *)object[3]);
  object[1] = 0;
  FUN_004b2630_r(object + 6);
  return 0;
}

/* FUN_004b2630_r: C -> original FUN_004b2630 (registers ecx) */
__declspec(naked) void FUN_004b2630_r(void * p0)
{
    __asm {
        mov ecx, dword ptr [esp + 4]
        call FUN_004b2630
        ret
    }
}

/* entry thunk at the original address of FUN_004af0c0: register inputs (s0) -> natural C FUN_004af0c0_n; ret 4 */
__declspec(naked) void FUN_004af0c0(void)
{
    __asm {
        push dword ptr [esp + 4]
        call FUN_004af0c0_n
        add esp, 4
        ret 4
    }
}
