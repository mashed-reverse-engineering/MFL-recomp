// cluster-entries: FUN_004ae3c0
/* Installed cluster FUN_004ae3c0 (tools/cluster_install.py from output/cluster_pilot_hb1/FUN_004ae3c0.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
extern __declspec(dllimport) void __stdcall EnterCriticalSection(void *);
extern __declspec(dllimport) void __stdcall LeaveCriticalSection(void *);
/*004b3c0c*/ extern void *operator_new(unsigned int size);
extern char PTR_g_data_6e0049_005e219c[];
typedef int (__fastcall *ThisMethod1)(int *self, int unused_edx, int arg);
/* FUN_004ae3c0 */
#define CHILD_OBJECT_BYTES (0xe0) /* sizeof the child object built by FUN_004acbb0 (inferred) */

static int * FUN_004acbb0_r(void * p0, int p1, int * p2, void * p3);
extern void FUN_004acbb0(void);
/* Lazily creates the child object stored at obj+0x78 (inferred): when arg_or_hr == 0 and none exists, allocates 0xe0 bytes, constructs it, destroys it again on a negative HRESULT; returns the child pointer or 0, under the critical section at obj+0xc4. */
int FUN_004ae3c0_n(char *obj, int arg_or_hr)

{
  LPCRITICAL_SECTION obj_cs;
  int child;
  void *obj_mem;
  int *new_child;
  
  obj_cs = (LPCRITICAL_SECTION)(obj + 0xc4);
  EnterCriticalSection(obj_cs);
  if (arg_or_hr == 0) {
    if (*(int *)(obj + 0x78) == 0) {
      arg_or_hr = 0;
      obj_mem = operator_new(CHILD_OBJECT_BYTES);
      if (obj_mem == (void *)0x0) {
        new_child = (int *)0x0;
      }
      else {
        new_child = FUN_004acbb0_r(obj_mem,(int)obj,&arg_or_hr,PTR_g_data_6e0049_005e219c);
      }
      *(int **)(obj + 0x78) = new_child;
      if (new_child != (int *)0x0) {
        if (-1 < arg_or_hr) goto LAB_004ae46f;
        (*(ThisMethod1 *)(*new_child + 0xc))(new_child,0,1);
        *(int *)(obj + 0x78) = 0;
      }
    }
    else {
LAB_004ae46f:
      child = *(int *)(obj + 0x78);
      LeaveCriticalSection(obj_cs);
      return child;
    }
  }
  LeaveCriticalSection(obj_cs);
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
