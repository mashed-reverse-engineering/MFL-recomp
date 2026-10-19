// cluster-entries: FUN_004acde0
/* Installed cluster FUN_004acde0 (tools/cluster_install.py from output/cluster_pilot_hb1/FUN_004acde0.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
extern __declspec(dllimport) void __stdcall EnterCriticalSection(void *);
extern __declspec(dllimport) void __stdcall LeaveCriticalSection(void *);
extern __declspec(dllimport) int __stdcall SetEvent(int);
extern __declspec(dllimport) int __stdcall ResetEvent(int);
extern __declspec(dllimport) unsigned int __stdcall timeGetTime(void);
typedef int (__stdcall *ComMethod1)(int *self);
typedef int (__stdcall *ComMethod2)(int *self, int a);
typedef int (__fastcall *ThisMethod0)(int *self, int unused_edx);
typedef int (__fastcall *ThisMethod1)(int *self, int unused_edx, int arg);
typedef int (__fastcall *ThisMethod2)(int *self, int unused_edx, int a, int b);
/* FUN_004acde0 */

static int FUN_004afb80_r(int * p0);
extern void FUN_004afb80(void);
int FUN_004acde0_n(int *in_ecx)

{
  int *target;
  int iVar1;
  
  target = (int *)in_ecx[0x36];
  iVar1 = (*(ThisMethod0 *)(*target + 0x8c))(target,0);
  if (iVar1 < 0) {
    return iVar1;
  }
  return FUN_004afb80_r(in_ecx);
}

/* FUN_004afb80_r: C -> original FUN_004afb80 (registers ecx) */
__declspec(naked) int FUN_004afb80_r(int * p0)
{
    __asm {
        mov ecx, dword ptr [esp + 4]
        call FUN_004afb80
        ret
    }
}

/* entry thunk at the original address of FUN_004acde0: register inputs (ecx) -> natural C FUN_004acde0_n; ret 0 */
__declspec(naked) void FUN_004acde0(void)
{
    __asm {
        push ecx
        call FUN_004acde0_n
        add esp, 4
        ret
    }
}
