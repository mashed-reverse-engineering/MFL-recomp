// cluster-entries: machinegun_state_clear
/* Installed cluster machinegun_state_clear (tools/cluster_install.py from output/cluster_pilot10/machinegun_state_clear.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*00467e90*/ extern int course_get_rpworld_00467e90();
/*00484660*/ extern undefined __cdecl clump_set_flag_on_atomics(struct RpClump *p0, undefined4 p1);
/*004f46d0*/ extern struct RpWorld * RpWorldRemoveClump(struct RpWorld *p0, struct RpClump *p1);
/*004f4920*/ extern RpWorld * clump_get_world(RpClump * p0);
/* machinegun_state_clear */

/* "removes the clump from the course world and clears the state" */

void machinegun_state_clear_n(undefined4 *in_esi)

{
  int iVar1;
  RpWorld *world;
  RpClump *clump;
  
  *in_esi = 0;
  iVar1 = clump_get_world(in_esi[2]);
  if (iVar1 != 0) {
    clump = (RpClump *)in_esi[2];
    world = (RpWorld *)course_get_rpworld_00467e90();
    RpWorldRemoveClump(world,clump);
  }
  clump_set_flag_on_atomics((RpClump *)in_esi[2], 0x0);
  in_esi[3] = 0;
  in_esi[5] = 0;
  in_esi[6] = 0;
  in_esi[7] = 0;
  in_esi[10] = 0;
  in_esi[9] = 0;
  in_esi[8] = 0;
  in_esi[4] = 0x32;
  return;
}

/* entry thunk at the original address of machinegun_state_clear: register inputs (esi) -> natural C machinegun_state_clear_n; ret 0 */
__declspec(naked) void machinegun_state_clear(void)
{
    __asm {
        push esi
        call machinegun_state_clear_n
        add esp, 4
        ret
    }
}
