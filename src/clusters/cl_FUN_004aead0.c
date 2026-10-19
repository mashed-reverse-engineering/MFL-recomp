// cluster-entries: FUN_004aead0
/* Installed cluster FUN_004aead0 (tools/cluster_install.py from output/cluster_pilot_hb1/FUN_004aead0.c); entries are patched at their original addresses by build_overlay.py --clusters. */
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
extern char PTR_LAB_005e23a0[];
extern char PTR_FUN_005e0e80[];
extern char PTR_LAB_005e238c[];
extern char PTR_LAB_005e2368[];
extern char PTR_LAB_005e2354[];
/* FUN_004aead0 */

static void FUN_004adda0_r(int * p0);
extern void FUN_004adda0(void);
void FUN_004aead0_n(int *in_ecx)

{
  *in_ecx = (int)PTR_LAB_005e23a0;
  in_ecx[3] = (int)PTR_FUN_005e0e80;
  in_ecx[4] = (int)PTR_LAB_005e238c;
  in_ecx[0x38] = (int)PTR_LAB_005e2368;
  in_ecx[0x39] = (int)PTR_LAB_005e2354;
  FUN_004adda0_r(in_ecx);
  return;
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

/* entry thunk at the original address of FUN_004aead0: register inputs (ecx) -> natural C FUN_004aead0_n; ret 0 */
__declspec(naked) void FUN_004aead0(void)
{
    __asm {
        push eax
        push ecx
        push edx
        push ecx
        call FUN_004aead0_n
        add esp, 4
        pop edx
        pop ecx
        pop eax
        ret
    }
}
