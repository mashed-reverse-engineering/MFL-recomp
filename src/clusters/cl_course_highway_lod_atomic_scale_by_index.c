// cluster-entries: course_highway_lod_atomic_scale_by_index
/* Installed cluster course_highway_lod_atomic_scale_by_index (tools/cluster_install.py from output/cluster_pilot_hw/course_highway_lod_atomic_scale_by_index.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*004d0c50*/ extern struct RwFrame * RwFrameCreate();
/*004d1160*/ extern struct RwFrame * RwFrameAddChild(struct RwFrame *p0, struct RwFrame *p1);
/*004d1500*/ extern struct RwFrame * RwFrameScale(struct RwFrame *p0, struct RwV3d *p1, enum RwOpCombineType p2);
/*004f6ae0*/ extern struct RpAtomic * RpAtomicClone(struct RpAtomic *p0);
/*004f7f70*/ extern struct RpAtomic * RpAtomicSetFrame(struct RpAtomic *p0, struct RwFrame *p1);
/* course_highway_lod_atomic_scale_by_index */

/* "clones the atomic, adds the frame as child, scale 1.27/1.24 by index" */

void course_highway_lod_atomic_scale_by_index_n(RpAtomic *in_eax, int in_ecx)

{
  int iVar1;
  RpAtomic *pRVar2;
  RwFrame *frame;
  RwV3d local_c;
  
  local_c.x = 1.0;
  local_c.y = 1.0;
  local_c.z = 1.0;
  pRVar2 = RpAtomicClone(in_eax);
  *(RpAtomic **)(in_ecx + 0xa8) = pRVar2;
  frame = RwFrameCreate();
  RpAtomicSetFrame(*(RpAtomic **)(in_ecx + 0xa8),frame);
  RwFrameAddChild(*(RwFrame **)(*(char **)(in_ecx + 0xa0) + 4),frame);
  iVar1 = *(int *)(in_ecx + 0xac);
  if (iVar1 == 0) {
    local_c.x = 1.27;
  }
  else if (iVar1 == 1) {
    local_c.x = 1.24;
  }
  else if (iVar1 == 2) {
    local_c.x = 1.12;
  }
  RwFrameScale(frame,&local_c,rwCOMBINEREPLACE);
  return;
}

/* entry thunk at the original address of course_highway_lod_atomic_scale_by_index: register inputs (eax,ecx) -> natural C course_highway_lod_atomic_scale_by_index_n; ret 0 */
__declspec(naked) void course_highway_lod_atomic_scale_by_index(void)
{
    __asm {
        push ecx
        push eax
        call course_highway_lod_atomic_scale_by_index_n
        add esp, 8
        ret
    }
}
