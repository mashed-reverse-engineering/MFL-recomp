// cluster-entries: FUN_004ada50
/* Installed cluster FUN_004ada50 (tools/cluster_install.py from output/cluster_pilot_uc3/FUN_004ada50.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
extern __declspec(dllimport) void __stdcall EnterCriticalSection(void *section);
extern __declspec(dllimport) void __stdcall LeaveCriticalSection(void *section);
/*004b1160*/ extern longlong __stdcall FUN_004b1160(longlong a, longlong b, longlong c, longlong d);
/*004ad9e0*/ extern int __stdcall FUN_004ad9e0(int value);

/* Sample standard deviation of `count` measurements from their sum of squares `sum_squares` and sum `sum` (64 bit):
   sqrt((sum_squares - sum * sum / count) / (count - 1)) as an integer, written to *out under the object's lock (critical section at +0x7c); 0 when the
   object has no measurements (+0x18 is 0) or count <= 1.  Returns 0 or E_POINTER (0x80004003) when out is NULL.
   (The original registers a compiler generated C++ exception frame for the lock guard; not reproduced.) */
int FUN_004ada50_n(char *this_, int count, int *out, unsigned int sum_squares_low, int sum_squares_high, unsigned int sum_low, int sum_high)
{
  void *lock;
  longlong variance;
  longlong sum_squares = ((longlong)sum_squares_high << 32) | sum_squares_low;
  longlong sum = ((longlong)sum_high << 32) | sum_low;

  if (out == 0) {
    return 0x80004003;
  }
  lock = this_ + 0x7c;
  EnterCriticalSection(lock);
  if (*(int *)(this_ + 0x18) == 0 || count <= 1) {
    *out = 0;
  } else {
    variance = sum_squares - FUN_004b1160(sum, sum, (longlong)count, 0);
    *out = FUN_004ad9e0((int)(variance / (longlong)(count - 1)));
  }
  LeaveCriticalSection(lock);
  return 0;
}

/* entry thunk at the original address of FUN_004ada50: register inputs (ecx,s0,s1,s2,s3,s4,s5) -> natural C FUN_004ada50_n; ret 24 */
__declspec(naked) void FUN_004ada50(void)
{
    __asm {
        push dword ptr [esp + 24]
        push dword ptr [esp + 24]
        push dword ptr [esp + 24]
        push dword ptr [esp + 24]
        push dword ptr [esp + 24]
        push dword ptr [esp + 24]
        push ecx
        call FUN_004ada50_n
        add esp, 28
        ret 24
    }
}
