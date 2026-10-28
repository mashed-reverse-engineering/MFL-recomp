#include "mfl_tier2.h"
/*005543d0*/ extern int rw_first_field(int * p0);
/*0058c8a0*/ extern int rwp_owner_object_get(int p0);
/*00701c9c*/ extern char *g_rw_701c9c;
/* course_neustein_cb0_sub_454650 */

/* single caller course_neustein_cb0_sub_4546e0 */

/* Builds a small direction/offset probe, resolves two owner objects from msg[0], msg[1], calls sub_454460 and stores the result in msg[2] (inferred). */
void course_neustein_cb0_sub_454650(undefined4 *msg)

{
  undefined4 first_field;
  undefined4 owner_a;
  undefined4 owner_b;
  struct {
    undefined4 offset_x;
    undefined4 offset_y;
    undefined4 offset_z;
    undefined4 extent_x;
    undefined4 extent_y;
    undefined4 extent_z;
  } probe;
  
  probe.offset_x = 0;
  probe.offset_y = 0xbdcccccd;
  probe.offset_z = 0;
  probe.extent_x = 0;
  probe.extent_y = 0x3fffe910;
  probe.extent_z = 0;
  first_field = rw_first_field((int *)g_rw_701c9c);
  owner_a = rwp_owner_object_get(*msg);
  owner_b = rwp_owner_object_get(msg[1]);
  owner_a = course_neustein_cb0_sub_454460(owner_a,owner_b,&probe.extent_x,&probe.offset_x);
  msg[2] = owner_a;
  func_0x0056a5c0(first_field,owner_a,0xffffffff);
  return;
}


