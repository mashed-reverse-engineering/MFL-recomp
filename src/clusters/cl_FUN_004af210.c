// cluster-entries: FUN_004af210
/* Installed cluster FUN_004af210 (tools/cluster_install.py from output/cluster_pilot_hb1/FUN_004af210.c); entries are patched at their original addresses by build_overlay.py --clusters. */
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
/*004b0f00*/ extern void FUN_004b0f00(void);
/* FUN_004af210 */

static void FUN_004b29e0_r(int * p0);
extern void FUN_004b29e0(void);
void FUN_004af210_n(int *in_ecx)

{
  _free_004b45fd((void *)in_ecx[5]);
  FUN_004b29e0_r(in_ecx + 0xd);
  FUN_004b0f00();
  return;
}

/* FUN_004b29e0_r: C -> original FUN_004b29e0 (registers ecx) */
__declspec(naked) void FUN_004b29e0_r(int * p0)
{
    __asm {
        mov ecx, dword ptr [esp + 4]
        call FUN_004b29e0
        ret
    }
}

/* entry thunk at the original address of FUN_004af210: register inputs (ecx) -> natural C FUN_004af210_n; ret 0 */
__declspec(naked) void FUN_004af210(void)
{
    __asm {
        push ecx
        call FUN_004af210_n
        add esp, 4
        ret
    }
}
