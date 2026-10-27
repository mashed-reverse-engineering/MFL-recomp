#include "mfl_tier2.h"
/*004f67c0*/ extern struct RpClump * RpClumpRender(struct RpClump *p0);


/* course_dump_cb2 */

void course_dump_cb2(char *param_1)
{
  RpClump *clump;
  uint uVar1;
  int aiStack_18 [6];
  
  aiStack_18[0] = 1;
  aiStack_18[1] = 2;
  aiStack_18[2] = 3;
  aiStack_18[3] = 9;
  aiStack_18[4] = 0xe;
  aiStack_18[5] = 0xf;
  uVar1 = 0;
  do {
    clump = *(RpClump **)(param_1 + 0xc110 + aiStack_18[uVar1] * 4);
    if (clump != (RpClump *)0x0) {
      RpClumpRender(clump);
    }
    uVar1 = uVar1 + 1;
  } while (uVar1 < 6);
  func_0x00453560();
  if (*(RpClump **)(param_1 + 0xc138) != (RpClump *)0x0) {
    RpClumpRender(*(RpClump **)(param_1 + 0xc138));
  }
  RpClumpRender(*(RpClump **)(param_1 + 0xc120));
  RpClumpRender(*(RpClump **)(param_1 + 0xc150));
  return;
}
