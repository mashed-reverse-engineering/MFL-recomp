// cluster-entries: course_highway_cb1_sub_453c20
/* Installed cluster course_highway_cb1_sub_453c20 (tools/cluster_install.py from output/cluster_pilot_md/course_highway_cb1_sub_453c20.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/* course_highway_cb1_sub_453c20 */

static void SUB_00453aa0_r(undefined4 p0);
extern void SUB_00453aa0(void);
/* "single caller course_highway_cb1: forwards its argument to SUB_00453aa0 (which takes it in edi)" */
void course_highway_cb1_sub_453c20(undefined4 param_1)
{
  SUB_00453aa0_r(param_1);
}

/* SUB_00453aa0_r: C -> original SUB_00453aa0 (registers edi) */
__declspec(naked) void SUB_00453aa0_r(undefined4 p0)
{
    __asm {
        push edi
        mov edi, dword ptr [esp + 8]
        call SUB_00453aa0
        pop edi
        ret
    }
}
