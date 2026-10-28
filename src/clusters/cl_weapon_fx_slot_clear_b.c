// cluster-entries: weapon_fx_slot_clear_b
/* Installed cluster weapon_fx_slot_clear_b (tools/cluster_install.py from output/cluster_pilot10/weapon_fx_slot_clear_b.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*004f46d0*/ extern struct RpWorld * RpWorldRemoveClump(struct RpWorld *p0, struct RpClump *p1);
/*004f4920*/ extern RpWorld * clump_get_world(RpClump * p0);
/* weapon_fx_slot_clear_b */

/* "resets the matrix based fx slot to identity / defaults" */

void weapon_fx_slot_clear_b_n(undefined4 *fx_slot)

{
  RpWorld *world;
  
  /* Diagonal of a 3x3 matrix at dwords 0..10 = identity (1.0f at 0, 5, 10), off-diagonals 0 (inferred). */
  fx_slot[10] = 0x3f800000;
  fx_slot[5] = 0x3f800000;
  *fx_slot = 0x3f800000;
  fx_slot[4] = 0;
  fx_slot[2] = 0;
  fx_slot[1] = 0;
  fx_slot[9] = 0;
  fx_slot[8] = 0;
  fx_slot[6] = 0;
  fx_slot[0xe] = 0;
  fx_slot[0xd] = 0;
  fx_slot[0xc] = 0;
  fx_slot[3] = fx_slot[3] | 0x20003;
  fx_slot[0x18] = 0;
  fx_slot[0x17] = 0;
  fx_slot[0x19] = 0;
  fx_slot[0x10] = 0;
  fx_slot[0x11] = 0;
  fx_slot[0x12] = 0;
  fx_slot[0x13] = 0;
  fx_slot[0x1a] = 0;
  /* Detach the clump (slot dword 0x16) from its world, if it is in one. */
  world = (RpWorld *)clump_get_world(fx_slot[0x16]);
  if (world != (RpWorld *)0x0) {
    RpWorldRemoveClump(world,(RpClump *)fx_slot[0x16]);
  }
  return;
}

/* entry thunk at the original address of weapon_fx_slot_clear_b: register inputs (esi) -> natural C weapon_fx_slot_clear_b_n; ret 0 */
__declspec(naked) void weapon_fx_slot_clear_b(void)
{
    __asm {
        push esi
        call weapon_fx_slot_clear_b_n
        add esp, 4
        ret
    }
}
