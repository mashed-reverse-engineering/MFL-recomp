#include "mfl_tier2.h"
/*005543d0*/ extern int rw_first_field(int * p0);
/*0058c8a0*/ extern int rwp_owner_object_get(int p0);
/*00701c9c*/ extern char *g_rw_701c9c;
/* course_neustein_cb0_sub_454650 */

/* single caller course_neustein_cb0_sub_4546e0 */

void course_neustein_cb0_sub_454650(undefined4 *param_1)

{
  undefined4 uVar1;
  undefined4 uVar2;
  undefined4 uVar3;
  struct {
    undefined4 uStack_18;
    undefined4 uStack_14;
    undefined4 uStack_10;
    undefined4 uStack_c;
    undefined4 uStack_8;
    undefined4 uStack_4;
  } frame;
  
  frame.uStack_18 = 0;
  frame.uStack_14 = 0xbdcccccd;
  frame.uStack_10 = 0;
  frame.uStack_c = 0;
  frame.uStack_8 = 0x3fffe910;
  frame.uStack_4 = 0;
  uVar1 = rw_first_field((int *)g_rw_701c9c);
  uVar2 = rwp_owner_object_get(*param_1);
  uVar3 = rwp_owner_object_get(param_1[1]);
  uVar2 = course_neustein_cb0_sub_454460(uVar2,uVar3,&frame.uStack_c,&frame.uStack_18);
  param_1[2] = uVar2;
  func_0x0056a5c0(uVar1,uVar2,0xffffffff);
  return;
}


