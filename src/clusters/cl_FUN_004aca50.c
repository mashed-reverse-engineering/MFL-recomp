// cluster-entries: FUN_004aca50
/* Installed cluster FUN_004aca50 (tools/cluster_install.py from output/cluster_pilot_hb1/FUN_004aca50.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
extern __declspec(dllimport) void __stdcall EnterCriticalSection(void *);
extern __declspec(dllimport) void __stdcall LeaveCriticalSection(void *);
extern __declspec(dllimport) int __stdcall SetEvent(int);
extern __declspec(dllimport) int __stdcall ResetEvent(int);
extern __declspec(dllimport) int __stdcall timeBeginPeriod(int);
extern __declspec(dllimport) int __stdcall timeEndPeriod(int);
typedef int (__stdcall *ComMethod1)(int *self);
typedef int (__fastcall *ThisMethod0)(int *self, int unused_edx);
typedef int (__fastcall *ThisMethod1)(int *self, int unused_edx, int arg);
/* FUN_004aca50 */

/* Starts the object (inferred): under the critical section at obj+0x94, if state (obj[0x19]) != 1 sets it to 1, timeBeginPeriod(1), calls vtable +0x40, then +0x60, or +0x54 with obj[0x1b] (signals event obj[0x15] on success). */
int FUN_004aca50_n(int *obj)

{
  LPCRITICAL_SECTION lpCriticalSection;
  int result;
  int hr;
  
  lpCriticalSection = (LPCRITICAL_SECTION)(obj + 0x25);
  EnterCriticalSection(lpCriticalSection);
  if (obj[0x19] != 1) {
    obj[0x19] = 1;
    timeBeginPeriod(1);
    (*(ThisMethod0 *)(*obj + 0x40))(obj,0);
    if (obj[0x1b] == 0) {
      result = (*(ThisMethod0 *)(*obj + 0x60))(obj,0);
      LeaveCriticalSection(lpCriticalSection);
      return result;
    }
    hr = (*(ThisMethod1 *)(*obj + 0x54))(obj,0,obj[0x1b]);
    if (hr == 0) {
      SetEvent(obj[0x15]);
    }
  }
  LeaveCriticalSection(lpCriticalSection);
  return 0;
}

/* entry thunk at the original address of FUN_004aca50: register inputs (ecx) -> natural C FUN_004aca50_n; ret 0 */
__declspec(naked) void FUN_004aca50(void)
{
    __asm {
        push ecx
        call FUN_004aca50_n
        add esp, 4
        ret
    }
}
