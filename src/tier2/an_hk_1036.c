#include "mfl_tier2.h"
/*004d43f0*/ extern RwMatrix * RwMatrixRotate(RwMatrix *p0, struct RwV3d *p1, RwReal p2, enum RwOpCombineType p3);
/*004d4e40*/ extern RwBool RwMatrixDestroy(RwMatrix *p0);
/*004d4e70*/ extern RwMatrix * RwMatrixCreate();
/*005543d0*/ extern int rw_first_field(int * p0);
/*00557140*/ extern RwBool RtQuatConvertFromMatrix(struct RtQuat *p0, RwMatrix *p1);
/*0056e7c0*/ extern void FUN_0056e7c0(undefined4 st_arg1, undefined4 st_arg2);
/*0056e890*/ extern int __cdecl FUN_0056e890(int a1, int a2, void *a3, void *a4);
/*0062ac80*/ extern undefined g_weapon_62ac80;
/*00701c9c*/ extern char *g_rw_701c9c;
/* course_neustein_cb0_sub_454460 */

/* single caller course_neustein_cb0_sub_454650 */

undefined4 course_neustein_cb0_sub_454460(undefined4 param_1,undefined4 param_2,undefined4 param_3,undefined4 param_4)

{
  undefined4 sample_map;
  undefined4 channel_or_result;
  undefined4 channel_item;
  RwMatrix *matrix;
  struct {
    undefined1 start_key [8];
    undefined1 end_key [8];
  } frame;
  RtQuat identity_quat;
  
  /* func_0x0056e780 takes (list, index) but the original pushes a third value (ignored by it) that stays on the stack as the second argument of the setter that is called next (56e9b0 / 56e990 copy a quaternion / vec3 from it): the decompiler lost it and the setters read stack garbage (same class as course_superg_cb4_sub_45c1f0) */
  sample_map = func_0x0056e640();
  func_0x0056e770(sample_map,0x3e);
  channel_or_result = func_0x0056e7a0(sample_map);
  func_0x0056e7f0(channel_or_result,7);
  func_0x0056e7d0(channel_or_result,0x3f800000);
  func_0x0056e830(channel_or_result,0x4020d97c);
  func_0x0056e850(channel_or_result,0x4020d97c);
  func_0x0056e870(channel_or_result,0x4020d97c);
  channel_or_result = func_0x0056e7b0(sample_map);
  func_0x0056e7f0(channel_or_result,1);
  func_0x0056e7d0(channel_or_result,0x3f800000);
  func_0x0056e830(channel_or_result,0x4020d97c);
  func_0x0056e850(channel_or_result,0x4020d97c);
  func_0x0056e870(channel_or_result,0x4020d97c);
  channel_item = func_0x0056e800(channel_or_result);
  FUN_0056e7c0(channel_item,0x358637bd);
  func_0x0056e7d0(channel_item,0x38d1b717);
  func_0x0056e7e0(channel_item,0x7f7fffff);
  channel_item = func_0x0056e810(channel_or_result);
  FUN_0056e7c0(channel_item,0x358637bd);
  func_0x0056e7d0(channel_item,0x38d1b717);
  func_0x0056e7e0(channel_item,0x7f7fffff);
  channel_or_result = func_0x0056e820(channel_or_result);
  FUN_0056e7c0(channel_or_result,0x358637bd);
  func_0x0056e7d0(channel_or_result,0x38d1b717);
  func_0x0056e7e0(channel_or_result,0x7f7fffff);
  channel_or_result = rw_first_field((int *)g_rw_701c9c);
  matrix = RwMatrixCreate();
  RwMatrixRotate(matrix,(RwV3d *)&g_weapon_62ac80,0.0,rwCOMBINEREPLACE);
  RtQuatConvertFromMatrix(&identity_quat,matrix);
  channel_item = func_0x0056e780(sample_map,1,&identity_quat);
  course_neustein_cb0_sub_56e9b0(channel_item,&identity_quat);
  channel_item = func_0x0056e780(sample_map,0,&identity_quat);
  course_neustein_cb0_sub_56e9b0(channel_item,&identity_quat);
  channel_item = func_0x0056e780(sample_map,1,param_4);
  course_neustein_cb0_sub_56e990(channel_item,param_4);
  channel_item = func_0x0056e780(sample_map,0,param_3);
  course_neustein_cb0_sub_56e990(channel_item,param_3);
  course_neustein_cb0_sub_56e9e0(frame.end_key,param_2,0);
  course_neustein_cb0_sub_56e9e0(frame.start_key,param_1,0);
  channel_or_result = FUN_0056e890(channel_or_result,sample_map,frame.end_key,frame.start_key);
  rpLtMapSampleMapDestroy(sample_map);
  RwMatrixDestroy(matrix);
  return channel_or_result;
}


