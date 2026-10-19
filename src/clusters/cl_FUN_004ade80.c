// cluster-entries: FUN_004ade80
/* Installed cluster FUN_004ade80 (tools/cluster_install.py from output/cluster_pilot_hb1/FUN_004ade80.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
extern __declspec(dllimport) void __stdcall EnterCriticalSection(void *);
extern __declspec(dllimport) void __stdcall LeaveCriticalSection(void *);
/*004b3c0c*/ extern void *operator_new(unsigned int size);
typedef int (__stdcall *ComMethod3)(int *self, int a, int b);
typedef int (__fastcall *ThisMethod1)(int *self, int unused_edx, int arg);
typedef int (__fastcall *ThisMethod2)(int *self, int unused_edx, int a, int b);
/* FUN_004ade80 */

static int FUN_004b2520_r(void * p0, int p1, int p2, int * p3, int p4);
extern void FUN_004b2520(void);
int FUN_004ade80_n(int *in_ecx, int param_1, int param_2)

{
  LPCRITICAL_SECTION lpCriticalSection;
  int uVar1;
  int iVar2;
  int *piVar3;
  void *obj_mem;
  int status;
  
  lpCriticalSection = (LPCRITICAL_SECTION)(in_ecx + 0x31);
  EnterCriticalSection(lpCriticalSection);
  iVar2 = in_ecx[0x14];
  if (iVar2 == 0) {
    status = 0;
    obj_mem = operator_new(0x50);
    if (obj_mem == (void *)0x0) {
      iVar2 = 0;
    }
    else {
      piVar3 = (int *)(*(ThisMethod1 *)(*in_ecx + 0x1c))(in_ecx,0,0);
      if (piVar3 == (int *)0x0) {
        iVar2 = 0;
      }
      else {
        iVar2 = (int)piVar3 + 0xc;
      }
      iVar2 = FUN_004b2520_r(obj_mem,0,in_ecx[1],&status,iVar2);
    }
    in_ecx[0x14] = iVar2;
    if (iVar2 == 0) {
      LeaveCriticalSection(lpCriticalSection);
      return 0x8007000e;
    }
    if (status < 0) {
      (*(ThisMethod1 *)(*(int *)(iVar2 + 8) + 0xc))((int *)(iVar2 + 8),0,1);
      in_ecx[0x14] = 0;
      LeaveCriticalSection(lpCriticalSection);
      return 0x80004002;
    }
    uVar1 = (*(ThisMethod2 *)(*in_ecx + 0x24))(in_ecx,0,param_1,param_2);
  }
  else {
    uVar1 = (*(ComMethod3 *)(*(int *)(iVar2 + 8)))((int *)(iVar2 + 8),param_1,param_2);
  }
  LeaveCriticalSection(lpCriticalSection);
  return uVar1;
}

/* FUN_004b2520_r: C -> original FUN_004b2520 (registers ecx,s0,s1,s2,s3) */
__declspec(naked) int FUN_004b2520_r(void * p0, int p1, int p2, int * p3, int p4)
{
    __asm {
        push dword ptr [esp + 20]
        push dword ptr [esp + 20]
        push dword ptr [esp + 20]
        push dword ptr [esp + 20]
        mov ecx, dword ptr [esp + 20]
        call FUN_004b2520
        ret
    }
}

/* entry thunk at the original address of FUN_004ade80: register inputs (ecx,s0,s1) -> natural C FUN_004ade80_n; ret 8 */
__declspec(naked) void FUN_004ade80(void)
{
    __asm {
        push dword ptr [esp + 8]
        push dword ptr [esp + 8]
        push ecx
        call FUN_004ade80_n
        add esp, 12
        ret 8
    }
}
