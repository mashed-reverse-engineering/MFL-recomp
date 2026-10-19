// cluster-entries: FUN_004ae5d0
/* Installed cluster FUN_004ae5d0 (tools/cluster_install.py from output/cluster_pilot_hb1/FUN_004ae5d0.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
extern __declspec(dllimport) void __stdcall EnterCriticalSection(void *);
extern __declspec(dllimport) void __stdcall LeaveCriticalSection(void *);
extern __declspec(dllimport) int __stdcall SetEvent(int);
/*004afbc0*/ extern int __stdcall FUN_004afbc0(int a, int *b);
typedef int (__stdcall *ComMethod1)(int *self);
typedef int (__fastcall *ThisMethod1)(int *self, int unused_edx, int arg);
/* FUN_004ae5d0 */

static void FUN_004b20a0_r(int p0, int * p1);
extern void FUN_004b20a0(void);
static void FUN_004acb70_r(int * p0, int p1);
extern void FUN_004acb70(void);
int FUN_004ae5d0_n(int *in_ecx, int *param_1)

{
  LPCRITICAL_SECTION lpCriticalSection;
  LPCRITICAL_SECTION lpCriticalSection_00;
  int *owner;
  int iVar1;
  
  lpCriticalSection = (LPCRITICAL_SECTION)(in_ecx + 0x1f);
  EnterCriticalSection(lpCriticalSection);
  in_ecx[0x2d] = 1;
  iVar1 = FUN_004afbc0(in_ecx[0x1e] + 0x98,param_1);
  if (iVar1 != 0) {
    in_ecx[0x2d] = 0;
    LeaveCriticalSection(lpCriticalSection);
    return 0x80004005;
  }
  owner = (int *)in_ecx[0x1e];
  if (owner[0x33] != 0) {
    iVar1 = (*(ThisMethod1 *)(*owner + 0x24))(owner,0,owner[0x33]);
    if (iVar1 < 0) {
      in_ecx[0x2d] = 0;
      LeaveCriticalSection(lpCriticalSection);
      return iVar1;
    }
  }
  lpCriticalSection_00 = (LPCRITICAL_SECTION)(in_ecx + 0x25);
  EnterCriticalSection(lpCriticalSection_00);
  if (((in_ecx[0x1b] == 0) && (in_ecx[0x1c] == 0)) && (in_ecx[0x18] == 0)) {
    if (in_ecx[0x14] != 0) {
      FUN_004b20a0_r(in_ecx[0x14],param_1);
    }
    if ((in_ecx[0x19] == 1) && ((*(ThisMethod1 *)(*in_ecx + 0x54))(in_ecx,0,(int)param_1) == 0)) {
      in_ecx[0x2d] = 0;
      LeaveCriticalSection(lpCriticalSection_00);
      LeaveCriticalSection(lpCriticalSection);
      return 0x8004022b;
    }
    in_ecx[0x2e] = *(int *)(in_ecx[0x1e] + 0xc0);
    in_ecx[0x2f] = *(int *)(in_ecx[0x1e] + 0xc4);
    in_ecx[0x1b] = (int)param_1;
    (*(ComMethod1 *)(*param_1 + 4))(param_1);
    if (in_ecx[0x19] == 0) {
      FUN_004acb70_r(in_ecx,1);
    }
    LeaveCriticalSection(lpCriticalSection_00);
    LeaveCriticalSection(lpCriticalSection);
    return 0;
  }
  SetEvent(in_ecx[0x17]);
  in_ecx[0x2d] = 0;
  LeaveCriticalSection(lpCriticalSection_00);
  LeaveCriticalSection(lpCriticalSection);
  return 0x8000ffff;
}

/* FUN_004b20a0_r: C -> original FUN_004b20a0 (registers ecx,s0) */
__declspec(naked) void FUN_004b20a0_r(int p0, int * p1)
{
    __asm {
        push dword ptr [esp + 8]
        mov ecx, dword ptr [esp + 8]
        call FUN_004b20a0
        ret
    }
}

/* FUN_004acb70_r: C -> original FUN_004acb70 (registers ecx,s0) */
__declspec(naked) void FUN_004acb70_r(int * p0, int p1)
{
    __asm {
        push dword ptr [esp + 8]
        mov ecx, dword ptr [esp + 8]
        call FUN_004acb70
        ret
    }
}

/* entry thunk at the original address of FUN_004ae5d0: register inputs (ecx,s0) -> natural C FUN_004ae5d0_n; ret 4 */
__declspec(naked) void FUN_004ae5d0(void)
{
    __asm {
        push dword ptr [esp + 4]
        push ecx
        call FUN_004ae5d0_n
        add esp, 8
        ret 4
    }
}
