// cluster-entries: FUN_004ad150
/* Installed cluster FUN_004ad150 (tools/cluster_install.py from output/cluster_pilot_hb1/FUN_004ad150.c); entries are patched at their original addresses by build_overlay.py --clusters. */
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
/* FUN_004ad150 */

static void FUN_004ad120_r(int * p0);
extern void FUN_004ad120(void);
void FUN_004ad150_n(int *in_ecx, int param_1)

{
  in_ecx[0x3d] = 0;
  in_ecx[0x3e] = 0x4c4b40;
  (*(ThisMethod2 *)(*in_ecx + 0xb4))(in_ecx,0,in_ecx[0x54],in_ecx[0x55]);
  FUN_004ad120_r(in_ecx);
  return;
}

/* FUN_004ad120_r: C -> original FUN_004ad120 (registers ecx) */
__declspec(naked) void FUN_004ad120_r(int * p0)
{
    __asm {
        mov ecx, dword ptr [esp + 4]
        call FUN_004ad120
        ret
    }
}

/* entry thunk at the original address of FUN_004ad150: register inputs (ecx,s0) -> natural C FUN_004ad150_n; ret 4 */
__declspec(naked) void FUN_004ad150(void)
{
    __asm {
        push dword ptr [esp + 4]
        push ecx
        call FUN_004ad150_n
        add esp, 8
        ret 4
    }
}
