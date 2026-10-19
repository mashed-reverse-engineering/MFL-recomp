// cluster-entries: debris_body_remove_if_in_world
/* Installed cluster debris_body_remove_if_in_world (tools/cluster_install.py from output/cluster_pilot11/debris_body_remove_if_in_world.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/* debris_body_remove_if_in_world */

/* WARNING: Unknown calling convention */
/* "removes the body from the world when it has one" */

static void rwp_world_current_deactivate_r(int p0);
extern void rwp_world_current_deactivate(void);
void debris_body_remove_if_in_world_n(int in_eax)

{
  if ((*(int *)(in_eax + 0x10) != 0) && (*(int *)(*(int *)(in_eax + 0x10) + 0x24) != 0)) {
    rwp_world_current_deactivate_r(*(int *)(in_eax + 0x10));
    return;
  }
  return;
}

/* rwp_world_current_deactivate_r: C -> original rwp_world_current_deactivate (registers eax) */
__declspec(naked) void rwp_world_current_deactivate_r(int p0)
{
    __asm {
        mov eax, dword ptr [esp + 4]
        call rwp_world_current_deactivate
        ret
    }
}

/* entry thunk at the original address of debris_body_remove_if_in_world: register inputs (eax) -> natural C debris_body_remove_if_in_world_n; ret 0 */
__declspec(naked) void debris_body_remove_if_in_world(void)
{
    __asm {
        push edx
        push eax
        call debris_body_remove_if_in_world_n
        add esp, 4
        pop edx
        ret
    }
}
