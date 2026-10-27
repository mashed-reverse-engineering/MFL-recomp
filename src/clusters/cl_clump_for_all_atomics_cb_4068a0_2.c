// cluster-entries: clump_for_all_atomics_cb_4068a0_2
/* Installed cluster clump_for_all_atomics_cb_4068a0_2 (tools/cluster_install.py from output/cluster_pilot_kb/clump_for_all_atomics_cb_4068a0_2.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*004065c0*/ extern int course_materials_fixup_cb_4065c0();
/*00406690*/ extern int course_materials_fixup_cb_406690();
/*00481a60*/ extern int object_set_pipeline_6c();
/*004f8b60*/ extern struct RpGeometry * RpGeometryForAllMaterials(struct RpGeometry *p0, RpMaterialCallBack p1, void *p2);
/* clump_for_all_atomics_cb_4068a0_2 */

/* function pointer pushed/stored by clump_for_all_atomics_cb_4068a0 (PUSH 0x4068a0) (name existed
   on another function) */

int clump_for_all_atomics_cb_4068a0_2(int param_1, void *param_2)

{
  int iStack_4;
  
  iStack_4 = 0;
  RpGeometryForAllMaterials
            (*(RpGeometry **)(param_1 + 0x18),course_materials_fixup_cb_4065c0,&iStack_4);
  if (iStack_4 != 0) {
    object_set_pipeline_6c(param_1);
    RpGeometryForAllMaterials
              (*(RpGeometry **)(param_1 + 0x18),course_materials_fixup_cb_406690,param_2);
  }
  return param_1;
}

