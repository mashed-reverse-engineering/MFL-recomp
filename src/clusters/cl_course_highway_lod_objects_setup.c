// cluster-entries: course_highway_lod_objects_setup
/* Installed cluster course_highway_lod_objects_setup (tools/cluster_install.py from output/cluster_pilot_hw/course_highway_lod_objects_setup.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*004d0c50*/ extern struct RwFrame * RwFrameCreate();
/*004d1160*/ extern struct RwFrame * RwFrameAddChild(struct RwFrame *p0, struct RwFrame *p1);
/*004d1460*/ extern struct RwFrame * RwFrameTranslate(struct RwFrame *p0, struct RwV3d *p1, enum RwOpCombineType p2);
/*004f6ae0*/ extern struct RpAtomic * RpAtomicClone(struct RpAtomic *p0);
/*004f7f70*/ extern struct RpAtomic * RpAtomicSetFrame(struct RpAtomic *p0, struct RwFrame *p1);
/*008c4470*/ extern undefined4 g_course_highway_8c4470;
/*008c4720*/ extern undefined4 g_course_highway_8c4720;
/*008c4724*/ extern undefined4 g_course_highway_8c4724;
/*008c4728*/ extern undefined4 g_course_highway_8c4728;
/* course_highway_lod_objects_setup */

/* WARNING: Unknown calling convention */
/* "clones the atomics of the highway object set and creates their frames" */

void course_highway_lod_objects_setup_n(int object_set, char *source_atomics)

{
  int count_or_row;
  RpAtomic *clone;
  RwFrame *clone_frame;
  int i;
  int *entry;
  
  count_or_row = *(int *)((char *)((undefined *)&g_course_highway_8c4470) + *(int *)(object_set + 0xac) * 4);
  i = 0;
  *(int *)(object_set + 100) = count_or_row;
  if (0 < count_or_row) {
    entry = (int *)(object_set + 0xc);
    do {
      count_or_row = (i + *(int *)(object_set + 0xac) * 4) * 0xc;
      entry[-3] = *(int *)((char *)((undefined *)&g_course_highway_8c4720) + count_or_row);
      entry[-2] = *(int *)((char *)((undefined *)&g_course_highway_8c4724) + count_or_row);
      entry[-1] = *(int *)((char *)((undefined *)&g_course_highway_8c4728) + count_or_row);
      clone = RpAtomicClone(*(RpAtomic **)(source_atomics + i * 4));
      *entry = (int)clone;
      clone_frame = RwFrameCreate();
      RpAtomicSetFrame((RpAtomic *)*entry,clone_frame);
      clone_frame = *(RwFrame **)(*entry + 4);
      RwFrameAddChild(*(RwFrame **)(*(char **)(object_set + 0xa0) + 4),clone_frame);
      RwFrameTranslate(clone_frame,(RwV3d *)(((undefined *)&g_course_highway_8c4720) +
                                       (i + *(int *)(object_set + 0xac) * 4) * 0xc),
                       rwCOMBINEREPLACE);
      i = i + 1;
      entry = entry + 6;
    } while (i < *(int *)(object_set + 100));
  }
  return;
}

/* entry thunk at the original address of course_highway_lod_objects_setup: register inputs (ebx,s0) -> natural C course_highway_lod_objects_setup_n; ret 0 */
__declspec(naked) void course_highway_lod_objects_setup(void)
{
    __asm {
        push dword ptr [esp + 4]
        push ebx
        call course_highway_lod_objects_setup_n
        add esp, 8
        ret
    }
}
