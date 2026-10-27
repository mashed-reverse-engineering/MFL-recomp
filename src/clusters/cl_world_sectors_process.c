// cluster-entries: world_sectors_process
/* Installed cluster world_sectors_process (tools/cluster_install.py from output/cluster_pilot8/world_sectors_process.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
extern char LAB_004890e0[];
/*004f5e10*/ extern struct RpWorld * RpWorldForAllWorldSectors(struct RpWorld *p0, RpWorldSectorCallBack p1, void *p2);
/* world_sectors_process */

/* "RpWorldForAllWorldSectors(world, LAB_004890e0)" */

void world_sectors_process_n(RpWorld *in_eax)

{
  RpWorldForAllWorldSectors(in_eax,(RpWorldSectorCallBack)&LAB_004890e0,(void *)0x0);
  return;
}

/* entry thunk at the original address of world_sectors_process: register inputs (eax) -> natural C world_sectors_process_n; ret 0 */
__declspec(naked) void world_sectors_process(void)
{
    __asm {
        push eax
        call world_sectors_process_n
        add esp, 4
        ret
    }
}
