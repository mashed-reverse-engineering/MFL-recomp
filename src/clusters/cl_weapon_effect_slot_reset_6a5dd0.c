// cluster-entries: weapon_effect_slot_reset_6a5dd0
/* Installed cluster weapon_effect_slot_reset_6a5dd0 (tools/cluster_install.py from output/cluster_pilot10/weapon_effect_slot_reset_6a5dd0.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*00484660*/ extern undefined __cdecl clump_set_flag_on_atomics(struct RpClump *p0, undefined4 p1);
/*004f46d0*/ extern struct RpWorld * RpWorldRemoveClump(struct RpWorld *p0, struct RpClump *p1);
/*004f4920*/ extern RpWorld * clump_get_world(RpClump * p0);
/* weapon_effect_slot_reset_6a5dd0 */

/* WARNING: Unknown calling convention */
/* "clears the state, atomic flags and removes the clump from its world" */

void weapon_effect_slot_reset_6a5dd0_n(undefined4 *in_esi)

{
  RpWorld *world;
  RpClump *clump;
  undefined4 uVar1;
  
  clump = (RpClump *)in_esi[6];
  uVar1 = 0;
  in_esi[1] = 0;
  in_esi[2] = 1;
  *in_esi = 0;
  in_esi[10] = 0;
  in_esi[7] = 0;
  clump_set_flag_on_atomics(clump, 0x0);
  world = (RpWorld *)clump_get_world(in_esi[6]);
  if (world != (RpWorld *)0x0) {
    RpWorldRemoveClump(world,(RpClump *)in_esi[6]);
  }
  return;
}

/* entry thunk at the original address of weapon_effect_slot_reset_6a5dd0: register inputs (esi) -> natural C weapon_effect_slot_reset_6a5dd0_n; ret 0 */
__declspec(naked) void weapon_effect_slot_reset_6a5dd0(void)
{
    __asm {
        push esi
        call weapon_effect_slot_reset_6a5dd0_n
        add esp, 4
        ret
    }
}
