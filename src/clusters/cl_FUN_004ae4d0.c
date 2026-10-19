// cluster-entries: FUN_004ae4d0
/* Installed cluster FUN_004ae4d0 (tools/cluster_install.py from output/cluster_pilot_hb1/FUN_004ae4d0.c); entries are patched at their original addresses by build_overlay.py --clusters. */
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
/* FUN_004ae4d0 */

static void FUN_004ac540_r(int * p0);
extern void FUN_004ac540(void);
int FUN_004ae4d0_n(int *in_ecx)

{
  if (in_ecx[5] == 1) {
    ResetEvent(in_ecx[0x17]);
  }
  (*(ThisMethod1 *)(*in_ecx + 0x28))(in_ecx,0,0);
  (*(ThisMethod0 *)(*in_ecx + 0x6c))(in_ecx,0);
  (*(ThisMethod0 *)(*in_ecx + 0x70))(in_ecx,0);
  FUN_004ac540_r(in_ecx);
  return 0;
}

/* FUN_004ac540_r: C -> original FUN_004ac540 (registers ecx) */
__declspec(naked) void FUN_004ac540_r(int * p0)
{
    __asm {
        mov ecx, dword ptr [esp + 4]
        call FUN_004ac540
        ret
    }
}

/* entry thunk at the original address of FUN_004ae4d0: register inputs (ecx) -> natural C FUN_004ae4d0_n; ret 0 */
__declspec(naked) void FUN_004ae4d0(void)
{
    __asm {
        push ecx
        call FUN_004ae4d0_n
        add esp, 4
        ret
    }
}
