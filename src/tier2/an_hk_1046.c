#include "mfl_tier2.h"
/*004f46d0*/ extern struct RpWorld * RpWorldRemoveClump(struct RpWorld *p0, struct RpClump *p1);
/*004f4920*/ extern RpWorld * clump_get_world(RpClump * p0);
/*004f6f40*/ extern RwBool RpClumpDestroy(struct RpClump *p0);
/* course_rouabout_cb5_sub_4573f0 */

/* single caller course_rouabout_cb5 */

void course_rouabout_cb5_sub_4573f0(undefined4 *param_1)

{
  RpWorld *world;
  int iVar1;
  
  world = (RpWorld *)clump_get_world((RpClump *)param_1[1]);
  if (world != (RpWorld *)0x0) {
    RpWorldRemoveClump(world,(RpClump *)param_1[1]);
  }
  RpClumpDestroy((RpClump *)param_1[1]);
  for (iVar1 = 0x16; iVar1 != 0; iVar1 = iVar1 + -1) {
    *param_1 = 0;
    param_1 = param_1 + 1;
  }
  return;
}


