// cluster-entries: course_warzone_cb2_sub_45d9f0
/* Installed cluster course_warzone_cb2_sub_45d9f0 (tools/cluster_install.py from output/cluster_pilot_md/course_warzone_cb2_sub_45d9f0.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*004a5d70*/ extern void __cdecl object_render(undefined4 p0);
/* course_warzone_cb2_sub_45d9f0 */

/* "single caller course_warzone_cb2: renders the object stored at +8 of its argument" */
void course_warzone_cb2_sub_45d9f0(int param_1)
{
  object_render(*(undefined4 *)(param_1 + 8));
}

