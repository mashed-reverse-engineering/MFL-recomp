// cluster-entries: FUN_004acc00
/* Installed cluster FUN_004acc00 (tools/cluster_install.py from output/cluster_pilot_zy/FUN_004acc00.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*004ac6b0*/ extern int __stdcall FUN_004ac6b0(void *object);
extern __declspec(dllimport) void __stdcall EnterCriticalSection(void *section);
extern __declspec(dllimport) void __stdcall LeaveCriticalSection(void *section);

typedef int (__fastcall *ThisCall0)(void *this_);

/* Under the two locks of the shared sub object (+0xcc: sections at +0x7c and +0x94): virtual method +0x38 of the object's base part (this - 0xc); when it returns 0, virtual method +0x68 of the sub object and, on success, FUN_004ac6b0(object).
   `ecx_slot` is the push ecx frame slot of the original (the register is never read). (The original also registers a compiler generated C++ exception frame for the lock guards; not reproduced.) */
int FUN_004acc00_n(int ecx_slot, char *object)
{
  void *lock_a;
  void *lock_b;
  int result;

  lock_a = *(char **)(object + 0xcc) + 0x7c;
  EnterCriticalSection(lock_a);
  lock_b = *(char **)(object + 0xcc) + 0x94;
  EnterCriticalSection(lock_b);
  result = ((ThisCall0)(*(int **)(object - 0xc))[0xe])(object - 0xc);
  if (result == 0) {
    result = ((ThisCall0)(**(int ***)(object + 0xcc))[0x1a])(*(void **)(object + 0xcc));
    if (result >= 0) {
      result = FUN_004ac6b0(object);
    }
  }
  LeaveCriticalSection(lock_b);
  LeaveCriticalSection(lock_a);
  return result;
}

/* entry thunk at the original address of FUN_004acc00: register inputs (ecx,s0) -> natural C FUN_004acc00_n; ret 4 */
__declspec(naked) void FUN_004acc00(void)
{
    __asm {
        push dword ptr [esp + 4]
        push ecx
        call FUN_004acc00_n
        add esp, 8
        ret 4
    }
}
