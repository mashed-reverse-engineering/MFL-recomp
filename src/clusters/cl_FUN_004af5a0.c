// cluster-entries: FUN_004af5a0
/* Installed cluster FUN_004af5a0 (tools/cluster_install.py from output/cluster_pilot_uc3/FUN_004af5a0.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"

static int thunk_FUN_004b2a00_r(void * p0, int p1);
extern void thunk_FUN_004b2a00(void);
/* Forwards `arg` to the member object at +0x34 (thunk_FUN_004b2a00 through its thunk); returns the result when it is negative (failure code), else 0. */
int FUN_004af5a0_n(char *this_, int arg)
{
  int result = thunk_FUN_004b2a00_r(this_ + 0x34, arg);
  return result < 0 ? result : 0;
}

/* thunk_FUN_004b2a00_r: C -> original thunk_FUN_004b2a00 (registers ecx,s0) */
__declspec(naked) int thunk_FUN_004b2a00_r(void * p0, int p1)
{
    __asm {
        push dword ptr [esp + 8]
        mov ecx, dword ptr [esp + 8]
        call thunk_FUN_004b2a00
        ret
    }
}

/* entry thunk at the original address of FUN_004af5a0: register inputs (ecx,s0) -> natural C FUN_004af5a0_n; ret 4 */
__declspec(naked) void FUN_004af5a0(void)
{
    __asm {
        push dword ptr [esp + 4]
        push ecx
        call FUN_004af5a0_n
        add esp, 8
        ret 4
    }
}
