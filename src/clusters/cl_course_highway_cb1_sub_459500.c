// cluster-entries: course_highway_cb1_sub_459500
/* Installed cluster course_highway_cb1_sub_459500 (tools/cluster_install.py from output/cluster_pilot_k/course_highway_cb1_sub_459500.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*008c3f60*/ extern undefined4 g_course_highway_8c3f60;
/*008c3f64*/ extern undefined4 g_course_8c3f64;
/*008c3f68*/ extern undefined4 g_course_highway_8c3f68;
/* course_highway_cb1_sub_459500 */

/* single caller course_highway_cb1 */

/* Fills box+0x68..0x70 with centre_src(+0x40..0x48) + a per-index offset vector and box+0x74..0x7c with centre_src - the same vector, i.e. a min/max pair around a centre (inferred). Offset vectors are 12-byte entries at g_course_highway_8c3f60[corner_index*3]. */
void course_highway_cb1_sub_459500_n(int box)

{
  int corner_index;
  char *centre_src;
  
  corner_index = *(int *)(box + 0xac);
  centre_src = *(char **)(*(char **)(box + 0xa0) + 4);
  *(float *)(box + 0x68) =
       *(float *)&(&g_course_highway_8c3f60)[corner_index * 3] + *(float *)(centre_src + 0x40);
  *(float *)(box + 0x6c) = *(float *)&(&g_course_8c3f64)[corner_index * 3] + *(float *)(centre_src + 0x44);
  *(float *)(box + 0x70) =
       *(float *)(centre_src + 0x48) + *(float *)&(&g_course_highway_8c3f68)[corner_index * 3];
  *(float *)(box + 0x74) =
       *(float *)(centre_src + 0x40) - *(float *)&(&g_course_highway_8c3f60)[corner_index * 3];
  *(float *)(box + 0x78) = *(float *)(centre_src + 0x44) - *(float *)&(&g_course_8c3f64)[corner_index * 3];
  *(float *)(box + 0x7c) =
       *(float *)(centre_src + 0x48) - *(float *)&(&g_course_highway_8c3f68)[corner_index * 3];
  return;
}

/* entry thunk at the original address of course_highway_cb1_sub_459500: register inputs (edx) -> natural C course_highway_cb1_sub_459500_n; ret 0 */
__declspec(naked) void course_highway_cb1_sub_459500(void)
{
    __asm {
        push edx
        push edx
        call course_highway_cb1_sub_459500_n
        add esp, 4
        pop edx
        ret
    }
}
