// cluster-entries: course_highway_cb1_sub_459500
/* Installed cluster course_highway_cb1_sub_459500 (tools/cluster_install.py from output/cluster_pilot_k/course_highway_cb1_sub_459500.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*008c3f60*/ extern undefined4 g_course_highway_8c3f60;
/*008c3f64*/ extern undefined4 g_course_8c3f64;
/*008c3f68*/ extern undefined4 g_course_highway_8c3f68;
/* course_highway_cb1_sub_459500 */

/* single caller course_highway_cb1 */

void course_highway_cb1_sub_459500_n(int in_edx)

{
  int iVar1;
  char *iVar2;
  
  iVar1 = *(int *)(in_edx + 0xac);
  iVar2 = *(char **)(*(char **)(in_edx + 0xa0) + 4);
  *(float *)(in_edx + 0x68) =
       *(float *)&(&g_course_highway_8c3f60)[iVar1 * 3] + *(float *)(iVar2 + 0x40);
  *(float *)(in_edx + 0x6c) = *(float *)&(&g_course_8c3f64)[iVar1 * 3] + *(float *)(iVar2 + 0x44);
  *(float *)(in_edx + 0x70) =
       *(float *)(iVar2 + 0x48) + *(float *)&(&g_course_highway_8c3f68)[iVar1 * 3];
  *(float *)(in_edx + 0x74) =
       *(float *)(iVar2 + 0x40) - *(float *)&(&g_course_highway_8c3f60)[iVar1 * 3];
  *(float *)(in_edx + 0x78) = *(float *)(iVar2 + 0x44) - *(float *)&(&g_course_8c3f64)[iVar1 * 3];
  *(float *)(in_edx + 0x7c) =
       *(float *)(iVar2 + 0x48) - *(float *)&(&g_course_highway_8c3f68)[iVar1 * 3];
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
