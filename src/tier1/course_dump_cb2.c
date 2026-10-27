#include "mfl_tier2.h"
/*004f67c0*/ extern struct RpClump * RpClumpRender(struct RpClump *p0);


/* course_dump_cb2 */

/* Course render callback: draws the clumps in slots 1,2,3,9,14,15, then the slot 0xc138/0xc120/0xc150 clumps. */
void course_dump_cb2(char *course)
{
  RpClump *clump;
  uint i;
  int clump_slots [6];
  
  clump_slots[0] = 1;
  clump_slots[1] = 2;
  clump_slots[2] = 3;
  clump_slots[3] = 9;
  clump_slots[4] = 0xe;
  clump_slots[5] = 0xf;
  i = 0;
  do {
    clump = *(RpClump **)(course + 0xc110 + clump_slots[i] * 4);
    if (clump != (RpClump *)0x0) {
      RpClumpRender(clump);
    }
    i = i + 1;
  } while (i < 6);
  func_0x00453560();
  if (*(RpClump **)(course + 0xc138) != (RpClump *)0x0) {
    RpClumpRender(*(RpClump **)(course + 0xc138));
  }
  RpClumpRender(*(RpClump **)(course + 0xc120));
  RpClumpRender(*(RpClump **)(course + 0xc150));
  return;
}
