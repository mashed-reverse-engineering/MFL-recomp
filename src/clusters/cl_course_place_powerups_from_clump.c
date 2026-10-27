// cluster-entries: course_place_powerups_from_clump
/* Installed cluster course_place_powerups_from_clump (tools/cluster_install.py from output/cluster_pilot_kc/course_place_powerups_from_clump.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*00464e00*/ extern undefined __cdecl powerup_place_with_respawn(undefined4 p0, undefined4 p1, undefined4 p2);
/*004c3f90*/ extern int clump_get_atomics(RpClump * p0, RpAtomic ** p1);
/*004c52b0*/ extern int FUN_004c52b0(undefined4 p0, undefined4 p1, undefined4 p2);
/* course_place_powerups_from_clump */

/* WARNING: Removing unreachable block (ram,0x0042a955) */
/* WARNING: Unknown calling convention */
/* "for each frame of the clump places a powerup with respawn" */

void course_place_powerups_from_clump_n(undefined4 clump)

{
  float fbits1;
  int frame;
  int atomic_count;
  uint atomic_id;
  int i;
  int atomics [32];
  
  atomic_count = clump_get_atomics(clump,atomics);
  i = 0;
  if (0 < atomic_count) {
    do {
      frame = *(int *)(atomics[i] + 4);
      atomic_id = FUN_004c52b0(atomics[i],0,0);
      powerup_place_with_respawn(frame + 0x40,atomic_id & 0xff,(fbits1 = (float)(atomic_id >> 8), *(int *)&fbits1));
      i = i + 1;
    } while (i < atomic_count);
  }
  return;
}

/* entry thunk at the original address of course_place_powerups_from_clump: register inputs (ecx) -> natural C course_place_powerups_from_clump_n; ret 0 */
__declspec(naked) void course_place_powerups_from_clump(void)
{
    __asm {
        push ecx
        call course_place_powerups_from_clump_n
        add esp, 4
        ret
    }
}
