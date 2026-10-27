// cluster-entries: debris_update_twice
/* Installed cluster debris_update_twice (tools/cluster_install.py from output/cluster_pilot10/debris_update_twice.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/* debris_update_twice */

/* "calls race_update_common_sub_4254e0 twice" */

static int vehicle_debris_update_one_r(int p0);
extern void vehicle_debris_update_one(void);
void debris_update_twice_n(int in_eax)

{
  int iVar1;
  
  iVar1 = 2;
  do {
    vehicle_debris_update_one_r(in_eax);
    in_eax = in_eax + 0x100;
    iVar1 = iVar1 + -1;
  } while (iVar1 != 0);
  return;
}

/* vehicle_debris_update_one_r: C -> original vehicle_debris_update_one (registers ebx) */
__declspec(naked) int vehicle_debris_update_one_r(int p0)
{
    __asm {
        push ebx
        mov ebx, dword ptr [esp + 8]
        call vehicle_debris_update_one
        pop ebx
        ret
    }
}

/* entry thunk at the original address of debris_update_twice: register inputs (eax) -> natural C debris_update_twice_n; ret 0 */
__declspec(naked) void debris_update_twice(void)
{
    __asm {
        push eax
        call debris_update_twice_n
        add esp, 4
        ret
    }
}
