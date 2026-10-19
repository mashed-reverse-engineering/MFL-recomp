// cluster-entries: FUN_004ae3c0
/* Installed cluster FUN_004ae3c0 (tools/cluster_install.py from output/cluster_pilot_hb1/FUN_004ae3c0.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
extern __declspec(dllimport) void __stdcall EnterCriticalSection(void *);
extern __declspec(dllimport) void __stdcall LeaveCriticalSection(void *);
/*004b3c0c*/ extern void *operator_new(unsigned int size);
extern char PTR_g_data_6e0049_005e219c[];
typedef int (__fastcall *ThisMethod1)(int *self, int unused_edx, int arg);
/* FUN_004ae3c0 */

static int * FUN_004acbb0_r(void * p0, int p1, int * p2, void * p3);
extern void FUN_004acbb0(void);
int FUN_004ae3c0_n(char *in_ecx, int param_1)

{
  LPCRITICAL_SECTION lpCriticalSection;
  int uVar1;
  void *obj_mem;
  int *piVar3;
  
  lpCriticalSection = (LPCRITICAL_SECTION)(in_ecx + 0xc4);
  EnterCriticalSection(lpCriticalSection);
  if (param_1 == 0) {
    if (*(int *)(in_ecx + 0x78) == 0) {
      param_1 = 0;
      obj_mem = operator_new(0xe0);
      if (obj_mem == (void *)0x0) {
        piVar3 = (int *)0x0;
      }
      else {
        piVar3 = FUN_004acbb0_r(obj_mem,(int)in_ecx,&param_1,PTR_g_data_6e0049_005e219c);
      }
      *(int **)(in_ecx + 0x78) = piVar3;
      if (piVar3 != (int *)0x0) {
        if (-1 < param_1) goto LAB_004ae46f;
        (*(ThisMethod1 *)(*piVar3 + 0xc))(piVar3,0,1);
        *(int *)(in_ecx + 0x78) = 0;
      }
    }
    else {
LAB_004ae46f:
      uVar1 = *(int *)(in_ecx + 0x78);
      LeaveCriticalSection(lpCriticalSection);
      return uVar1;
    }
  }
  LeaveCriticalSection(lpCriticalSection);
  return 0;
}

/* FUN_004acbb0_r: C -> original FUN_004acbb0 (registers ecx,s0,s1,s2) */
__declspec(naked) int * FUN_004acbb0_r(void * p0, int p1, int * p2, void * p3)
{
    __asm {
        push dword ptr [esp + 16]
        push dword ptr [esp + 16]
        push dword ptr [esp + 16]
        mov ecx, dword ptr [esp + 16]
        call FUN_004acbb0
        ret
    }
}

/* entry thunk at the original address of FUN_004ae3c0: register inputs (ecx,s0) -> natural C FUN_004ae3c0_n; ret 4 */
__declspec(naked) void FUN_004ae3c0(void)
{
    __asm {
        push dword ptr [esp + 4]
        push ecx
        call FUN_004ae3c0_n
        add esp, 8
        ret 4
    }
}
