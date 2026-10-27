#include "mfl_tier2.h"
/*00429bc0*/ extern int atomic_lookup_error();
/*0045ca10*/ extern undefined anim_update_hierarchy(undefined4 p1, int p2);
/*004817b0*/ extern int atomic_plugin_get_bbox();
/*004f6810*/ extern struct RpClump * RpClumpForAllAtomics(struct RpClump *p0, RpAtomicCallBack p1, void *p2);
/*0053e820*/ extern RwBool RtAnimInterpolatorSetCurrentTime(struct RtAnimInterpolator *p0, RwReal p1);
/*00544430*/ extern int FUN_00544430(undefined4 p0);
/*00544450*/ extern RwBool RpHAnimHierarchyUpdateMatrices(struct RpHAnimHierarchy *p0);
/* course_warzone_cb0_sub_45ca50 */

/* single caller course_warzone_cb0 */

void course_warzone_cb0_sub_45ca50(undefined4 *object,undefined4 clump,undefined4 anim_arg)

{
  int iVar1;
  undefined4 *src_cursor;
  int copy_left;
  undefined4 *dst_cursor;
  struct {
    undefined4 uStack_8;
    undefined4 *puStack_4;
  } frame;
  
  *object = clump;
  object[0x13] = anim_arg;
  object[0x11] = 0;
  iVar1 = course_warzone_cb0_sub_4c4860(clump,anim_arg);
  object[0x14] = iVar1;
  object[0x12] = 0;
  func_0x0053e480(*(undefined4 *)(iVar1 + 0x20),anim_update_hierarchy,object);
  frame.uStack_8 = 0;
  frame.puStack_4 = object;
  RpClumpForAllAtomics((struct RpClump *)((RpClump *)*object),(RpAtomicCallBack)atomic_lookup_error,(void *)&frame.uStack_8);
  object[4] = *(undefined4 *)(object[2] + 4);
  iVar1 = atomic_plugin_get_bbox(object[2]);
  object[8] = *(undefined4 *)(iVar1 + 0xc);
  object[9] = *(undefined4 *)(iVar1 + 0x10);
  object[10] = *(undefined4 *)(iVar1 + 0x14);
  src_cursor = (undefined4 *)atomic_plugin_get_bbox(object[2]);
  object[5] = *src_cursor;
  object[6] = src_cursor[1];
  object[7] = src_cursor[2];
  RtAnimInterpolatorSetCurrentTime(*(RtAnimInterpolator **)(object[0x14] + 0x20),0.0);
  RpHAnimHierarchyUpdateMatrices((RpHAnimHierarchy *)object[0x14]);
  iVar1 = FUN_00544430(object[0x14]);
  object[0xe] = *(undefined4 *)(iVar1 + 0x70);
  object[0xf] = *(undefined4 *)(iVar1 + 0x74);
  object[0x10] = *(undefined4 *)(iVar1 + 0x78);
  object[0x25] = (undefined4)((undefined4 *)(iVar1 + 0x40));
  src_cursor = (undefined4 *)(iVar1 + 0x40);
  dst_cursor = object + 0x15;
  for (copy_left = 0x10; copy_left != 0; copy_left = copy_left + -1) {
    *dst_cursor = *src_cursor;
    src_cursor = src_cursor + 1;
    dst_cursor = dst_cursor + 1;
  }
  object[0x31] = 0;
  return;
}


