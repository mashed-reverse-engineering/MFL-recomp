// cluster-entries: course_highway_cb1_sub_459f70
/* Installed cluster course_highway_cb1_sub_459f70 (tools/cluster_install.py from output/cluster_pilot_k/course_highway_cb1_sub_459f70.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*0056b220*/ extern void rwp_world_set_bit_60_64(int p0, int p1, int p2);
/*006a2680*/ extern undefined4 g_course_objects_6a2680;
/*008c47c0*/ extern undefined4 g_course_8c47c0;
/* course_highway_cb1_sub_459f70 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* WARNING: Unknown calling convention */
/* single caller course_highway_cb1 */

void course_highway_cb1_sub_459f70_n(int in_edx)

{
  undefined4 *puVar1;
  int iVar2;
  
  iVar2 = 0;
  puVar1 = &g_course_8c47c0 + *(int *)(in_edx + 0xac) * 0x46;
  do {
    if (puVar1[5] == 0) {
      *(undefined4 **)(in_edx + 0xf4) = puVar1;
      puVar1[5] = in_edx;
      g_course_objects_6a2680 = g_course_objects_6a2680 + 1;
      rwp_world_set_bit_60_64(*(undefined4 *)(puVar1[4] + 0x24),puVar1[4],1);
      return;
    }
    iVar2 = iVar2 + 1;
    puVar1 = puVar1 + 7;
  } while (iVar2 < 10);
  return;
}

/* entry thunk at the original address of course_highway_cb1_sub_459f70: register inputs (edx) -> natural C course_highway_cb1_sub_459f70_n; ret 0 */
__declspec(naked) void course_highway_cb1_sub_459f70(void)
{
    __asm {
        push edx
        call course_highway_cb1_sub_459f70_n
        add esp, 4
        ret
    }
}
