// cluster-entries: weapon_fx_slot_clear_b
/* Installed cluster weapon_fx_slot_clear_b (tools/cluster_install.py from output/cluster_pilot10/weapon_fx_slot_clear_b.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*004f46d0*/ extern struct RpWorld * RpWorldRemoveClump(struct RpWorld *p0, struct RpClump *p1);
/*004f4920*/ extern RpWorld * clump_get_world(RpClump * p0);
/* weapon_fx_slot_clear_b */

/* "resets the matrix based fx slot to identity / defaults" */

void weapon_fx_slot_clear_b_n(undefined4 *in_esi)

{
  RpWorld *world;
  
  in_esi[10] = 0x3f800000;
  in_esi[5] = 0x3f800000;
  *in_esi = 0x3f800000;
  in_esi[4] = 0;
  in_esi[2] = 0;
  in_esi[1] = 0;
  in_esi[9] = 0;
  in_esi[8] = 0;
  in_esi[6] = 0;
  in_esi[0xe] = 0;
  in_esi[0xd] = 0;
  in_esi[0xc] = 0;
  in_esi[3] = in_esi[3] | 0x20003;
  in_esi[0x18] = 0;
  in_esi[0x17] = 0;
  in_esi[0x19] = 0;
  in_esi[0x10] = 0;
  in_esi[0x11] = 0;
  in_esi[0x12] = 0;
  in_esi[0x13] = 0;
  in_esi[0x1a] = 0;
  world = (RpWorld *)clump_get_world(in_esi[0x16]);
  if (world != (RpWorld *)0x0) {
    RpWorldRemoveClump(world,(RpClump *)in_esi[0x16]);
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
