// cluster-entries: FUN_004ac670
/* Installed cluster FUN_004ac670 (tools/cluster_install.py from output/cluster_pilot_hb1/FUN_004ac670.c); entries are patched at their original addresses by build_overlay.py --clusters. */
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
/* FUN_004ac670 */

static void FUN_004b2210_r(int p0);
extern void FUN_004b2210(void);
int FUN_004ac670_n(int *in_ecx)

{
  if (in_ecx[0x14] != 0) {
    FUN_004b2210_r(in_ecx[0x14]);
  }
  (*(ThisMethod1 *)(*in_ecx + 0x28))(in_ecx,0,1);
  return 0;
}

/* FUN_004b2210_r: C -> original FUN_004b2210 (registers ecx) */
__declspec(naked) void FUN_004b2210_r(int p0)
{
    __asm {
        mov ecx, dword ptr [esp + 4]
        call FUN_004b2210
        ret
    }
}

/* entry thunk at the original address of FUN_004ac670: register inputs (ecx) -> natural C FUN_004ac670_n; ret 0 */
__declspec(naked) void FUN_004ac670(void)
{
    __asm {
        push ecx
        call FUN_004ac670_n
        add esp, 4
        ret
    }
}
