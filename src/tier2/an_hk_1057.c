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

/* Initialises an animated-clump object: stores clump/anim_arg, creates its hierarchy (course_warzone_cb0_sub_4c4860), caches atomic bbox/centre values and copies a 16-word matrix from the hierarchy node (+0x40) into entity[0x15..0x24] (inferred). */
void course_warzone_cb0_sub_45ca50(undefined4 *entity,undefined4 clump,undefined4 anim_arg)

{
  int hier_or_bbox;
  undefined4 *src_ptr;
  int words_left;
  undefined4 *dst_ptr;
  struct {
    undefined4 uStack_8;
    undefined4 *puStack_4;
  } frame;
  
  *entity = clump;
  entity[0x13] = anim_arg;
  entity[0x11] = 0;
  hier_or_bbox = course_warzone_cb0_sub_4c4860(clump,anim_arg);
  entity[0x14] = hier_or_bbox;
  entity[0x12] = 0;
  func_0x0053e480(*(undefined4 *)(hier_or_bbox + 0x20),anim_update_hierarchy,entity);
  frame.uStack_8 = 0;
  frame.puStack_4 = entity;
  RpClumpForAllAtomics((struct RpClump *)((RpClump *)*entity),(RpAtomicCallBack)atomic_lookup_error,(void *)&frame.uStack_8);
  entity[4] = *(undefined4 *)(entity[2] + 4);
  hier_or_bbox = atomic_plugin_get_bbox(entity[2]);
  entity[8] = *(undefined4 *)(hier_or_bbox + 0xc);
  entity[9] = *(undefined4 *)(hier_or_bbox + 0x10);
  entity[10] = *(undefined4 *)(hier_or_bbox + 0x14);
  src_ptr = (undefined4 *)atomic_plugin_get_bbox(entity[2]);
  entity[5] = *src_ptr;
  entity[6] = src_ptr[1];
  entity[7] = src_ptr[2];
  RtAnimInterpolatorSetCurrentTime(*(RtAnimInterpolator **)(entity[0x14] + 0x20),0.0);
  RpHAnimHierarchyUpdateMatrices((RpHAnimHierarchy *)entity[0x14]);
  hier_or_bbox = FUN_00544430(entity[0x14]);
  entity[0xe] = *(undefined4 *)(hier_or_bbox + 0x70);
  entity[0xf] = *(undefined4 *)(hier_or_bbox + 0x74);
  entity[0x10] = *(undefined4 *)(hier_or_bbox + 0x78);
  entity[0x25] = (undefined4)((undefined4 *)(hier_or_bbox + 0x40));
  src_ptr = (undefined4 *)(hier_or_bbox + 0x40);
  dst_ptr = entity + 0x15;
  for (words_left = 0x10; words_left != 0; words_left = words_left + -1) {
    *dst_ptr = *src_ptr;
    src_ptr = src_ptr + 1;
    dst_ptr = dst_ptr + 1;
  }
  entity[0x31] = 0;
  return;
}


