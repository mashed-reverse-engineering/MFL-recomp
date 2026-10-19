// cluster-entries: FUN_004aeb00
/* Installed cluster FUN_004aeb00 (tools/cluster_install.py from output/cluster_pilot_hb1/FUN_004aeb00.c); entries are patched at their original addresses by build_overlay.py --clusters. */
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
/*004b3bca*/ extern void __cdecl _free_004b45fd(void *p0);
/* FUN_004aeb00 */

static void FUN_004adda0_r(int * p0);
extern void FUN_004adda0(void);
int *FUN_004aeb00_n(int *in_ecx, unsigned int flags)

{
  FUN_004adda0_r(in_ecx);
  if ((flags & 1) != 0) {
    _free_004b45fd(in_ecx);
  }
  return in_ecx;
}

/* FUN_004adda0_r: C -> original FUN_004adda0 (registers ecx) */
__declspec(naked) void FUN_004adda0_r(int * p0)
{
    __asm {
        mov ecx, dword ptr [esp + 4]
        call FUN_004adda0
        ret
    }
}

/* entry thunk at the original address of FUN_004aeb00: register inputs (ecx,s0) -> natural C FUN_004aeb00_n; ret 4 */
__declspec(naked) void FUN_004aeb00(void)
{
    __asm {
        push dword ptr [esp + 4]
        push ecx
        call FUN_004aeb00_n
        add esp, 8
        ret 4
    }
}
