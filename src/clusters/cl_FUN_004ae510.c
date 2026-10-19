// cluster-entries: FUN_004ae510
/* Installed cluster FUN_004ae510 (tools/cluster_install.py from output/cluster_pilot_zy/FUN_004ae510.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"

static int FUN_004ac5b0_r(void * p0);
extern void FUN_004ac5b0(void);
static void FUN_004acb70_r(void * p0, int p1);
extern void FUN_004acb70(void);
/* Clears the +0x60 field, then depending on the state of FUN_004ac5b0 (== 2 or not) calls virtual method +0x7c / FUN_004acb70(this, 1).  `unused` is never read.  Returns 0 or the failed HRESULT. */
int FUN_004ae510_n(int *this_, int unused)
{
  int result;

  this_[0x18] = 0;
  result = FUN_004ac5b0_r(this_);
  if (result == 2) {
    result = ((int (__fastcall *)(void *))(*(int **)this_)[0x1f])(this_);
    if (result >= 0) {
      FUN_004acb70_r(this_, 0);
      return 0;
    }
  } else {
    FUN_004acb70_r(this_, 1);
    result = 0;
  }
  return result;
}

/* FUN_004ac5b0_r: C -> original FUN_004ac5b0 (registers ecx) */
__declspec(naked) int FUN_004ac5b0_r(void * p0)
{
    __asm {
        mov ecx, dword ptr [esp + 4]
        call FUN_004ac5b0
        ret
    }
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

/* entry thunk at the original address of FUN_004ae510: register inputs (ecx,s0) -> natural C FUN_004ae510_n; ret 4 */
__declspec(naked) void FUN_004ae510(void)
{
    __asm {
        push dword ptr [esp + 4]
        push ecx
        call FUN_004ae510_n
        add esp, 8
        ret 4
    }
}
