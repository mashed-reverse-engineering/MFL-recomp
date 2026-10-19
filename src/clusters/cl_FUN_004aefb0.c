// cluster-entries: FUN_004aefb0
/* Installed cluster FUN_004aefb0 (tools/cluster_install.py from output/cluster_pilot_hb1/FUN_004aefb0.c); entries are patched at their original addresses by build_overlay.py --clusters. */
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
extern char PTR_LAB_005e2464[];
/* FUN_004aefb0 */

static void FUN_004b27b0_r(int * p0);
extern void FUN_004b27b0(void);
void FUN_004aefb0_n(int *in_ecx)

{
  int obj;
  
  *in_ecx = (int)PTR_LAB_005e2464;
  obj = in_ecx[3];
  (*(ComMethod1 *)(*(int *)(obj + 0xc) + 8))((int *)(obj + 0xc));
  FUN_004b27b0_r(in_ecx + 6);
  return;
}

/* FUN_004b27b0_r: C -> original FUN_004b27b0 (registers ecx) */
__declspec(naked) void FUN_004b27b0_r(int * p0)
{
    __asm {
        mov ecx, dword ptr [esp + 4]
        call FUN_004b27b0
        ret
    }
}

/* entry thunk at the original address of FUN_004aefb0: register inputs (ecx) -> natural C FUN_004aefb0_n; ret 0 */
__declspec(naked) void FUN_004aefb0(void)
{
    __asm {
        push ecx
        call FUN_004aefb0_n
        add esp, 4
        ret
    }
}
