// cluster-entries: course_highway_lod_atomic_clone_b
/* Installed cluster course_highway_lod_atomic_clone_b (tools/cluster_install.py from output/cluster_pilot_hw/course_highway_lod_atomic_clone_b.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*004d0c50*/ extern struct RwFrame * RwFrameCreate();
/*004d1160*/ extern struct RwFrame * RwFrameAddChild(struct RwFrame *p0, struct RwFrame *p1);
/*004f6ae0*/ extern struct RpAtomic * RpAtomicClone(struct RpAtomic *p0);
/*004f7f70*/ extern struct RpAtomic * RpAtomicSetFrame(struct RpAtomic *p0, struct RwFrame *p1);
/* course_highway_lod_atomic_clone_b */

/* "clones the atomic into +0xA4 and adds its frame as child of the +0xA0 frame" */

void course_highway_lod_atomic_clone_b_n(int in_esi, RpAtomic *in_eax)

{
  RpAtomic *pRVar1;
  RwFrame *frame;
  
  pRVar1 = RpAtomicClone(in_eax);
  *(RpAtomic **)(in_esi + 0xa4) = pRVar1;
  frame = RwFrameCreate();
  RpAtomicSetFrame(*(RpAtomic **)(in_esi + 0xa4),frame);
  RwFrameAddChild(*(RwFrame **)(*(char **)(in_esi + 0xa0) + 4),frame);
  return;
}

/* entry thunk at the original address of course_highway_lod_atomic_clone_b: register inputs (esi,eax) -> natural C course_highway_lod_atomic_clone_b_n; ret 0 */
__declspec(naked) void course_highway_lod_atomic_clone_b(void)
{
    __asm {
        push eax
        push esi
        call course_highway_lod_atomic_clone_b_n
        add esp, 8
        ret
    }
}
