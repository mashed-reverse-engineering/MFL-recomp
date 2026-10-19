// cluster-entries: weapon_fx_clump_remove_a
/* Installed cluster weapon_fx_clump_remove_a (tools/cluster_install.py from output/cluster_pilot10/weapon_fx_clump_remove_a.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*004f46d0*/ extern struct RpWorld * RpWorldRemoveClump(struct RpWorld *p0, struct RpClump *p1);
/*004f4920*/ extern RpWorld * clump_get_world(RpClump * p0);
/* weapon_fx_clump_remove_a */

/* WARNING: Unknown calling convention */
/* "clears the counters and removes the clump at +0x10 from its world" */

void weapon_fx_clump_remove_a_n(int in_esi)

{
  RpWorld *world;
  
  *(undefined4 *)(in_esi + 8) = 0;
  *(undefined4 *)(in_esi + 0xc) = 0;
  world = (RpWorld *)clump_get_world(*(undefined4 *)(in_esi + 0x10));
  if (world != (RpWorld *)0x0) {
    RpWorldRemoveClump(world,*(RpClump **)(in_esi + 0x10));
  }
  return;
}

/* entry thunk at the original address of weapon_fx_clump_remove_a: register inputs (esi) -> natural C weapon_fx_clump_remove_a_n; ret 0 */
__declspec(naked) void weapon_fx_clump_remove_a(void)
{
    __asm {
        push esi
        call weapon_fx_clump_remove_a_n
        add esp, 4
        ret
    }
}
