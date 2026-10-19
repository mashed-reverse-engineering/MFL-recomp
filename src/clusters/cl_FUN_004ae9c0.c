// cluster-entries: FUN_004ae9c0
/* Installed cluster FUN_004ae9c0 (tools/cluster_install.py from output/cluster_pilot_zy/FUN_004ae9c0.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*004b3bca*/ extern void _free_004b45fd(void *block);

static void thunk_FUN_004af9c0_r(void * p0);
extern void thunk_FUN_004af9c0(void);
/* Scalar deleting destructor: runs the destructor body, frees the object when bit 0 of `flags` is set, returns the object. */
void *FUN_004ae9c0_n(void *this_, unsigned char flags)
{
  thunk_FUN_004af9c0_r(this_);
  if ((flags & 1) != 0) {
    _free_004b45fd(this_);
  }
  return this_;
}

/* thunk_FUN_004af9c0_r: C -> original thunk_FUN_004af9c0 (registers ecx) */
__declspec(naked) void thunk_FUN_004af9c0_r(void * p0)
{
    __asm {
        mov ecx, dword ptr [esp + 4]
        call thunk_FUN_004af9c0
        ret
    }
}

/* entry thunk at the original address of FUN_004ae9c0: register inputs (ecx,s0) -> natural C FUN_004ae9c0_n; ret 4 */
__declspec(naked) void FUN_004ae9c0(void)
{
    __asm {
        push dword ptr [esp + 4]
        push ecx
        call FUN_004ae9c0_n
        add esp, 8
        ret 4
    }
}
