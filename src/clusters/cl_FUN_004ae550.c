// cluster-entries: FUN_004ae550
/* Installed cluster FUN_004ae550 (tools/cluster_install.py from output/cluster_pilot_zy/FUN_004ae550.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"

typedef int (__fastcall *ThisCall0)(void *this_);

/* Stops the object: releases the COM interface at +0xac, fails with 0x80040224 when the sub object (+0x78) is not initialised, otherwise
   FUN_004acb70(this, 0), virtual calls +0x64 and +0x70, clears +0x60, and +0x80 when the state (+0x14) is 2.  Returns 1 (no source), 0x80040224 or 0. */
static void FUN_004acb70_r(void * p0, int p1);
extern void FUN_004acb70(void);
int FUN_004ae550_n(int *this_)
{
  int *com;

  com = (int *)this_[0x2b];
  if (com != 0) {
    ((int (__stdcall *)(void *))(*(int **)com)[2])(com);
    this_[0x2b] = 0;
  }
  if (*(int *)(this_[0x1e] + 0x18) == 0) {
    return 1;
  }
  if (this_[5] != 0 && *(char *)(this_[0x1e] + 0x25) == 0) {
    return 0x80040224;
  }
  FUN_004acb70_r(this_, 0);
  ((ThisCall0)(*(int **)this_)[0x19])(this_);
  ((ThisCall0)(*(int **)this_)[0x1c])(this_);
  this_[0x18] = 0;
  if (this_[5] == 2) {
    ((ThisCall0)(*(int **)this_)[0x20])(this_);
  }
  return 0;
}

/* FUN_004acb70_r: C -> original FUN_004acb70 (registers ecx,s0) */
__declspec(naked) void FUN_004acb70_r(void * p0, int p1)
{
    __asm {
        push dword ptr [esp + 8]
        mov ecx, dword ptr [esp + 8]
        call FUN_004acb70
        ret
    }
}

/* entry thunk at the original address of FUN_004ae550: register inputs (ecx) -> natural C FUN_004ae550_n; ret 0 */
__declspec(naked) void FUN_004ae550(void)
{
    __asm {
        push ecx
        call FUN_004ae550_n
        add esp, 4
        ret
    }
}
