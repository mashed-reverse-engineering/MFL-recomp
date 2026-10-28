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
/* Queue/submit COM object `sample` (inferred) on `self`: takes the outer lock, forwards to the owner (self[0x1e]), then under the inner lock stores it in the pending slot self[0x1b]. Returns 0 on success, else an HRESULT (E_FAIL 0x80004005, 0x8004022b, 0x8000ffff). (inferred) */
int FUN_004ae5d0_n(int *self, int *sample)

{
  LPCRITICAL_SECTION outer_lock;
  LPCRITICAL_SECTION inner_lock;
  int *owner;
  int result;
  
  outer_lock = (LPCRITICAL_SECTION)(self + 0x1f);
  EnterCriticalSection(outer_lock);
  self[0x2d] = 1;
  result = FUN_004afbc0(self[0x1e] + 0x98,sample);
  if (result != 0) {
    self[0x2d] = 0;
    LeaveCriticalSection(outer_lock);
    return 0x80004005;
  }
  owner = (int *)self[0x1e];
  if (owner[0x33] != 0) {
    result = (*(ThisMethod1 *)(*owner + 0x24))(owner,0,owner[0x33]);
    if (result < 0) {
      self[0x2d] = 0;
      LeaveCriticalSection(outer_lock);
      return result;
    }
  }
  inner_lock = (LPCRITICAL_SECTION)(self + 0x25);
  EnterCriticalSection(inner_lock);
  if (((self[0x1b] == 0) && (self[0x1c] == 0)) && (self[0x18] == 0)) {
    if (self[0x14] != 0) {
      FUN_004b20a0_r(self[0x14],sample);
    }
    if ((self[0x19] == 1) && ((*(ThisMethod1 *)(*self + 0x54))(self,0,(int)sample) == 0)) {
      self[0x2d] = 0;
      LeaveCriticalSection(inner_lock);
      LeaveCriticalSection(outer_lock);
      return 0x8004022b;
    }
    self[0x2e] = *(int *)(self[0x1e] + 0xc0);
    self[0x2f] = *(int *)(self[0x1e] + 0xc4);
    self[0x1b] = (int)sample;
    (*(ComMethod1 *)(*sample + 4))(sample);
    if (self[0x19] == 0) {
      FUN_004acb70_r(self,1);
    }
    LeaveCriticalSection(inner_lock);
    LeaveCriticalSection(outer_lock);
    return 0;
  }
  SetEvent(self[0x17]);
  self[0x2d] = 0;
  LeaveCriticalSection(inner_lock);
  LeaveCriticalSection(outer_lock);
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
