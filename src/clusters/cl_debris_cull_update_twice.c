// cluster-entries: debris_cull_update_twice
/* Installed cluster debris_cull_update_twice (tools/cluster_install.py from output/cluster_pilot10/debris_cull_update_twice.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/* debris_cull_update_twice */

/* "debris_cull_update called twice" */

static int debris_cull_update_r(int p0);
extern void debris_cull_update(void);
void debris_cull_update_twice_n(int in_eax)

{
  int iVar1;
  
  iVar1 = 2;
  do {
    debris_cull_update_r(in_eax);
    in_eax = in_eax + 0x100;
    iVar1 = iVar1 + -1;
  } while (iVar1 != 0);
  return;
}

/* debris_cull_update_r: C -> original debris_cull_update (registers ebx) */
__declspec(naked) int debris_cull_update_r(int p0)
{
    __asm {
        push ebx
        mov ebx, dword ptr [esp + 8]
        call debris_cull_update
        pop ebx
        ret
    }
}

/* entry thunk at the original address of debris_cull_update_twice: register inputs (eax) -> natural C debris_cull_update_twice_n; ret 0 */
__declspec(naked) void debris_cull_update_twice(void)
{
    __asm {
        push eax
        call debris_cull_update_twice_n
        add esp, 4
        ret
    }
}
