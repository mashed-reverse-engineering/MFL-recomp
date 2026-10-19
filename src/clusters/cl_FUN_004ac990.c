// cluster-entries: FUN_004ac990
/* Installed cluster FUN_004ac990 (tools/cluster_install.py from output/cluster_pilot_zy/FUN_004ac990.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
extern __declspec(dllimport) void __stdcall EnterCriticalSection(void *section);
extern __declspec(dllimport) void __stdcall LeaveCriticalSection(void *section);

/* Under the object's lock (critical section at +0x94): returns 0 when the flag at +0x64 is clear; otherwise clears +0xc0, calls FUN_004b2240 on the member at +0x50 (when set), sets +0x74 and returns FUN_004aef40(this, 1, 0, this + 0xc).
   (The original registers a compiler generated C++ exception frame for the lock guard; not reproduced.) */
static int FUN_004aef40_r(void * p0, int p1, int p2, void * p3);
extern void FUN_004aef40(void);
static void FUN_004b2240_r(void * p0);
extern void FUN_004b2240(void);
int FUN_004ac990_n(char *this_)
{
  void *lock = this_ + 0x94;
  int result;

  EnterCriticalSection(lock);
  if (*(int *)(this_ + 0x64) == 0) {
    LeaveCriticalSection(lock);
    return 0;
  }
  *(int *)(this_ + 0xc0) = 0;
  if (*(int *)(this_ + 0x50) != 0) {
    FUN_004b2240_r(*(void **)(this_ + 0x50));
  }
  *(int *)(this_ + 0x74) = 1;
  result = FUN_004aef40_r(this_, 1, 0, this_ + 0xc);
  LeaveCriticalSection(lock);
  return result;
}

/* FUN_004aef40_r: C -> original FUN_004aef40 (registers ecx,s0,s1,s2) */
__declspec(naked) int FUN_004aef40_r(void * p0, int p1, int p2, void * p3)
{
    __asm {
        push dword ptr [esp + 16]
        push dword ptr [esp + 16]
        push dword ptr [esp + 16]
        mov ecx, dword ptr [esp + 16]
        call FUN_004aef40
        ret
    }
}

/* FUN_004b2240_r: C -> original FUN_004b2240 (registers ecx) */
__declspec(naked) void FUN_004b2240_r(void * p0)
{
    __asm {
        mov ecx, dword ptr [esp + 4]
        call FUN_004b2240
        ret
    }
}

/* entry thunk at the original address of FUN_004ac990: register inputs (ecx) -> natural C FUN_004ac990_n; ret 0 */
__declspec(naked) void FUN_004ac990(void)
{
    __asm {
        push ecx
        call FUN_004ac990_n
        add esp, 4
        ret
    }
}
