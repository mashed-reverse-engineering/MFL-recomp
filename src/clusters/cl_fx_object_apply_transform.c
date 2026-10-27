// cluster-entries: fx_object_apply_transform
/* Installed cluster fx_object_apply_transform (tools/cluster_install.py from output/cluster_pilot11/fx_object_apply_transform.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*004c40c0*/ extern int FUN_004c40c0(undefined4 p0);
/*004c5360*/ extern void atomic_set_render_flag(RpAtomic * p0, int p1);
/*004d15a0*/ extern struct RwFrame * RwFrameTransform(struct RwFrame *p0, RwMatrix *p1, enum RwOpCombineType p2);
/* fx_object_apply_transform */

/* "transforms the frame from the matrix and updates the plugin colour" */

void fx_object_apply_transform_n(int *in_esi)

{
  int iVar1;
  
  if (*in_esi != 0) {
    RwFrameTransform(*(RwFrame **)(*in_esi + 4),(RwMatrix *)(in_esi + 4),rwCOMBINEREPLACE);
    iVar1 = FUN_004c40c0(*in_esi);
    if (iVar1 != 0) {
      *(undefined4 *)(iVar1 + 4) = 0xffffffff;
    }
    atomic_set_render_flag(*in_esi,0);
  }
  return;
}

/* entry thunk at the original address of fx_object_apply_transform: register inputs (esi) -> natural C fx_object_apply_transform_n; ret 0 */
__declspec(naked) void fx_object_apply_transform(void)
{
    __asm {
        push esi
        call fx_object_apply_transform_n
        add esp, 4
        ret
    }
}
