// cluster-entries: weapon_fx_clump_remove_b
/* Installed cluster weapon_fx_clump_remove_b (tools/cluster_install.py from output/cluster_pilot11/weapon_fx_clump_remove_b.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*00484660*/ extern undefined __cdecl clump_set_flag_on_atomics(struct RpClump *p0, undefined4 p1);
/*004f46d0*/ extern struct RpWorld * RpWorldRemoveClump(struct RpWorld *p0, struct RpClump *p1);
/*004f4920*/ extern RpWorld * clump_get_world(RpClump * p0);
/* weapon_fx_clump_remove_b */

/* "ids = -1, clump flags cleared and clump at +0x5C removed from its world" */

void weapon_fx_clump_remove_b_n(int in_esi)

{
  RpWorld *world;
  RpClump *clump;
  undefined4 uVar1;
  
  *(undefined4 *)(in_esi + 0x54) = 0xffffffff;
  *(undefined4 *)(in_esi + 0x58) = 0xffffffff;
  clump = *(RpClump **)(in_esi + 0x5c);
  uVar1 = 0;
  *(undefined4 *)(in_esi + 0x50) = 0;
  *(undefined4 *)(in_esi + 0x60) = 0;
  clump_set_flag_on_atomics(clump, 0x0);
  world = (RpWorld *)clump_get_world(*(undefined4 *)(in_esi + 0x5c));
  if (world != (RpWorld *)0x0) {
    RpWorldRemoveClump(world,*(RpClump **)(in_esi + 0x5c));
  }
  return;
}

/* entry thunk at the original address of weapon_fx_clump_remove_b: register inputs (esi) -> natural C weapon_fx_clump_remove_b_n; ret 0 */
__declspec(naked) void weapon_fx_clump_remove_b(void)
{
    __asm {
        push esi
        call weapon_fx_clump_remove_b_n
        add esp, 4
        ret
    }
}
