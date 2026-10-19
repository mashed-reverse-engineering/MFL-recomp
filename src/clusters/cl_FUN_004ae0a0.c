// cluster-entries: FUN_004ae0a0
/* Installed cluster FUN_004ae0a0 (tools/cluster_install.py from output/cluster_pilot_hb1/FUN_004ae0a0.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
extern __declspec(dllimport) void __stdcall EnterCriticalSection(void *);
extern __declspec(dllimport) void __stdcall LeaveCriticalSection(void *);
extern __declspec(dllimport) int __stdcall SetEvent(int);
extern __declspec(dllimport) int __stdcall ResetEvent(int);
typedef int (__stdcall *ComMethod1)(int *self);
typedef int (__fastcall *ThisMethod0)(int *self, int unused_edx);
typedef int (__fastcall *ThisMethod1)(int *self, int unused_edx, int arg);
/* FUN_004ae0a0 */

undefined4 FUN_004ae0a0_n(int *in_ecx, int param_1)

{
  int iVar1;
  
  if (*(int *)(in_ecx[0x1e] + 0x18) != 0) {
    if (in_ecx[0x1c] == 1) {
      SetEvent(in_ecx[0x17]);
      return 0;
    }
    iVar1 = (*(ThisMethod0 *)(*in_ecx + 0xa0))(in_ecx,0);
    if ((iVar1 != 1) || (param_1 == 0)) {
      ResetEvent(in_ecx[0x17]);
      return 1;
    }
  }
  SetEvent(in_ecx[0x17]);
  return 0;
}

/* entry thunk at the original address of FUN_004ae0a0: register inputs (ecx,s0) -> natural C FUN_004ae0a0_n; ret 4 */
__declspec(naked) void FUN_004ae0a0(void)
{
    __asm {
        push dword ptr [esp + 4]
        push ecx
        call FUN_004ae0a0_n
        add esp, 8
        ret 4
    }
}
