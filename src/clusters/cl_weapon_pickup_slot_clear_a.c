// cluster-entries: weapon_pickup_slot_clear_a
/* Installed cluster weapon_pickup_slot_clear_a (tools/cluster_install.py from output/cluster_pilot10/weapon_pickup_slot_clear_a.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*004f46d0*/ extern struct RpWorld * RpWorldRemoveClump(struct RpWorld *p0, struct RpClump *p1);
/*004f4920*/ extern RpWorld * clump_get_world(RpClump * p0);
/* weapon_pickup_slot_clear_a */

/* WARNING: Unknown calling convention */
/* "clears fields, removes the clump from its world" */

void weapon_pickup_slot_clear_a_n(int in_esi)

{
  RpWorld *world;
  
  *(undefined4 *)(in_esi + 8) = 0;
  *(undefined4 *)(in_esi + 0x10) = 0;
  *(undefined4 *)(in_esi + 4) = 0xffffffff;
  world = (RpWorld *)clump_get_world(*(undefined4 *)(in_esi + 0x14));
  if (world != (RpWorld *)0x0) {
    RpWorldRemoveClump(world,*(RpClump **)(in_esi + 0x14));
  }
  return;
}

/* entry thunk at the original address of weapon_pickup_slot_clear_a: register inputs (esi) -> natural C weapon_pickup_slot_clear_a_n; ret 0 */
__declspec(naked) void weapon_pickup_slot_clear_a(void)
{
    __asm {
        push esi
        call weapon_pickup_slot_clear_a_n
        add esp, 4
        ret
    }
}
