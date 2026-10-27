// cluster-entries: course_highway_cb1_sub_459fc0
/* Installed cluster course_highway_cb1_sub_459fc0 (tools/cluster_install.py from output/cluster_pilot_k/course_highway_cb1_sub_459fc0.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*0048e720*/ extern undefined __cdecl rwp_object_set_matrix(int p0, RwMatrix *p1);
/*004d4870*/ extern RwMatrix * RwMatrixTranslate(RwMatrix *p0, struct RwV3d *p1, enum RwOpCombineType p2);
/*006a2680*/ extern undefined4 g_course_objects_6a2680;
/* course_highway_cb1_sub_459fc0 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* WARNING: Unknown calling convention */
/* single caller course_highway_cb1 */

void course_highway_cb1_sub_459fc0_n(int in_eax)

{
  RwV3d *translation;
  RwMatrix RStack_40;
  
  g_course_objects_6a2680 = g_course_objects_6a2680 + -1;
  translation = *(RwV3d **)(in_eax + 0xf4);
  translation[1].z = 0.0;
  *(undefined4 *)(in_eax + 0xf4) = 0;
  RwMatrixTranslate(&RStack_40,translation,rwCOMBINEREPLACE);
  rwp_object_set_matrix(((int *)translation)[6],&RStack_40);
  return;
}

/* entry thunk at the original address of course_highway_cb1_sub_459fc0: register inputs (eax) -> natural C course_highway_cb1_sub_459fc0_n; ret 0 */
__declspec(naked) void course_highway_cb1_sub_459fc0(void)
{
    __asm {
        push eax
        call course_highway_cb1_sub_459fc0_n
        add esp, 4
        ret
    }
}
