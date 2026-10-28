#include "mfl_tier2.h"
/*005543d0*/ extern int rw_first_field(int * p0);
/*0058c8a0*/ extern int rwp_owner_object_get(int p0);
/*00701c9c*/ extern char *g_rw_701c9c;
/* course_neustein_cb0_sub_454650 */

/* single caller course_neustein_cb0_sub_4546e0 */

/* Builds a small direction/offset frame, resolves two owner objects from msg[0], msg[1], calls sub_454460 and stores the result in msg[2] (inferred). */
void course_neustein_cb0_sub_454650(undefined4 *msg)

{
  undefined4 first_field;
  undefined4 owner_a;
  undefined4 owner_b;
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
  first_field = rw_first_field((int *)g_rw_701c9c);
  owner_a = rwp_owner_object_get(*msg);
  owner_b = rwp_owner_object_get(msg[1]);
  owner_a = course_neustein_cb0_sub_454460(owner_a,owner_b,&frame.uStack_c,&frame.uStack_18);
  msg[2] = owner_a;
  func_0x0056a5c0(first_field,owner_a,0xffffffff);
  return;
}


