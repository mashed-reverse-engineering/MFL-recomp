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

void course_highway_lod_objects_setup_n(int in_ebx, char *param_2)

{
  int iVar1;
  RpAtomic *pRVar2;
  RwFrame *pRVar3;
  int iVar4;
  int *piVar5;
  
  iVar1 = *(int *)((char *)((undefined *)&g_course_highway_8c4470) + *(int *)(in_ebx + 0xac) * 4);
  iVar4 = 0;
  *(int *)(in_ebx + 100) = iVar1;
  if (0 < iVar1) {
    piVar5 = (int *)(in_ebx + 0xc);
    do {
      iVar1 = (iVar4 + *(int *)(in_ebx + 0xac) * 4) * 0xc;
      piVar5[-3] = *(int *)((char *)((undefined *)&g_course_highway_8c4720) + iVar1);
      piVar5[-2] = *(int *)((char *)((undefined *)&g_course_highway_8c4724) + iVar1);
      piVar5[-1] = *(int *)((char *)((undefined *)&g_course_highway_8c4728) + iVar1);
      pRVar2 = RpAtomicClone(*(RpAtomic **)(param_2 + iVar4 * 4));
      *piVar5 = (int)pRVar2;
      pRVar3 = RwFrameCreate();
      RpAtomicSetFrame((RpAtomic *)*piVar5,pRVar3);
      pRVar3 = *(RwFrame **)(*piVar5 + 4);
      RwFrameAddChild(*(RwFrame **)(*(char **)(in_ebx + 0xa0) + 4),pRVar3);
      RwFrameTranslate(pRVar3,(RwV3d *)(((undefined *)&g_course_highway_8c4720) +
                                       (iVar4 + *(int *)(in_ebx + 0xac) * 4) * 0xc),
                       rwCOMBINEREPLACE);
      iVar4 = iVar4 + 1;
      piVar5 = piVar5 + 6;
    } while (iVar4 < *(int *)(in_ebx + 100));
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
