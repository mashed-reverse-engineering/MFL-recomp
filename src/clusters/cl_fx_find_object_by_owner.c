// cluster-entries: fx_find_object_by_owner
/* Installed cluster fx_find_object_by_owner (tools/cluster_install.py from output/cluster_pilot10/fx_find_object_by_owner.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*00651250*/ extern undefined g_scene_651250[0x1000];
/*00651aa0*/ extern undefined4 g_fx_object_count;
/* fx_find_object_by_owner */

/* "returns the fx record (0xEC bytes) whose +0x44 matches the owner" */

undefined * fx_find_object_by_owner_n(int in_esi)

{
  undefined *puVar1;
  int iVar2;
  
  iVar2 = 0;
  if (0 < (int)g_fx_object_count) {
    puVar1 = g_scene_651250;
    do {
      if (*(int *)(puVar1 + 0x44) == in_esi) {
        return puVar1;
      }
      iVar2 = iVar2 + 1;
      puVar1 = puVar1 + 0xec;
    } while (iVar2 < (int)g_fx_object_count);
  }
  return (undefined *)0x0;
}

/* entry thunk at the original address of fx_find_object_by_owner: register inputs (esi) -> natural C fx_find_object_by_owner_n; ret 0 */
__declspec(naked) void fx_find_object_by_owner(void)
{
    __asm {
        push esi
        call fx_find_object_by_owner_n
        add esp, 4
        ret
    }
}
