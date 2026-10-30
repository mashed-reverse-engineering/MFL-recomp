// cluster-entries: course_dem_cb0
/* Installed cluster course_dem_cb0 (tools/cluster_install.py from output/cluster_pilot_te5d/course_dem_cb0.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*00452ea0*/ extern void course_dem_state_reset(void);

static void SUB_00452ee0_r(unsigned int p0, unsigned char * p1);
extern void SUB_00452ee0(void);
/* course_dem_cb0: the course set-up callback of the demolition course: resets the state, then runs the first update (ticks 0) */
void course_dem_cb0(unsigned char *world)
{
  course_dem_state_reset();
  SUB_00452ee0_r(0, world);
}

/* SUB_00452ee0_r: C -> original SUB_00452ee0 (registers edx,s0) */
__declspec(naked) void SUB_00452ee0_r(unsigned int p0, unsigned char * p1)
{
    __asm {
        push dword ptr [esp + 8]
        mov edx, dword ptr [esp + 8]
        call SUB_00452ee0
        add esp, 4
        ret
    }
}
