// cluster-entries: critical_section_leave
/* Installed cluster critical_section_leave (tools/cluster_install.py from output/cluster_pilot9/critical_section_leave.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
extern __declspec(dllimport) int __stdcall LeaveCriticalSection(int);
/* critical_section_leave */

/* WARNING: Unknown calling convention */
/*  */

void critical_section_leave_n(undefined4 *in_ecx)

{
  LeaveCriticalSection((LPCRITICAL_SECTION)*in_ecx);
  return;
}

/* entry thunk at the original address of critical_section_leave: register inputs (ecx) -> natural C critical_section_leave_n; ret 0 */
__declspec(naked) void critical_section_leave(void)
{
    __asm {
        push ecx
        call critical_section_leave_n
        add esp, 4
        ret
    }
}
