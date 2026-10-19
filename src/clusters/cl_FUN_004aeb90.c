// cluster-entries: FUN_004aeb90
/* Installed cluster FUN_004aeb90 (tools/cluster_install.py from output/cluster_pilot_zy/FUN_004aeb90.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*004b3bca*/ extern void _free_004b45fd(void *block);

static void FUN_004aead0_r(void * p0);
extern void FUN_004aead0(void);
/* Scalar deleting destructor: runs the destructor body, frees the object when bit 0 of `flags` is set, returns the object. */
void *FUN_004aeb90_n(void *this_, unsigned char flags)
{
  FUN_004aead0_r(this_);
  if ((flags & 1) != 0) {
    _free_004b45fd(this_);
  }
  return this_;
}

/* FUN_004aead0_r: C -> original FUN_004aead0 (registers ecx) */
__declspec(naked) void FUN_004aead0_r(void * p0)
{
    __asm {
        mov ecx, dword ptr [esp + 4]
        call FUN_004aead0
        ret
    }
}

/* entry thunk at the original address of FUN_004aeb90: register inputs (ecx,s0) -> natural C FUN_004aeb90_n; ret 4 */
__declspec(naked) void FUN_004aeb90(void)
{
    __asm {
        push dword ptr [esp + 4]
        push ecx
        call FUN_004aeb90_n
        add esp, 8
        ret 4
    }
}
