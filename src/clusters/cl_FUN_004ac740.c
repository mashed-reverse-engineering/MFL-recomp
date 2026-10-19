// cluster-entries: FUN_004ac740
/* Installed cluster FUN_004ac740 (tools/cluster_install.py from output/cluster_pilot_hb1/FUN_004ac740.c); entries are patched at their original addresses by build_overlay.py --clusters. */
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
/* FUN_004ac740 */

static void FUN_004ac730_r(int * p0);
extern void FUN_004ac730(void);
int FUN_004ac740_n(int *in_ecx)

{
  int iVar1;
  int *owner;
  
  iVar1 = in_ecx[0x1a];
  if (iVar1 != 0) {
    owner = (int *)in_ecx[6];
    (*(ComMethod2 *)(*owner + 0x18))(owner,iVar1);
    FUN_004ac730_r(in_ecx);
  }
  ResetEvent(in_ecx[0x15]);
  return iVar1 == 0;
}

/* FUN_004ac730_r: C -> original FUN_004ac730 (registers ecx) */
__declspec(naked) void FUN_004ac730_r(int * p0)
{
    __asm {
        mov ecx, dword ptr [esp + 4]
        call FUN_004ac730
        ret
    }
}

/* entry thunk at the original address of FUN_004ac740: register inputs (ecx) -> natural C FUN_004ac740_n; ret 0 */
__declspec(naked) void FUN_004ac740(void)
{
    __asm {
        push ecx
        call FUN_004ac740_n
        add esp, 4
        ret
    }
}
