#include "mfl_tier2.h"
/*004f67c0*/ extern struct RpClump * RpClumpRender(struct RpClump *p0);


/* course_training_cb2 */

/* Training course render callback: renders each of the course's optional clumps that is set (names unknown). */
void course_training_cb2(char *course)
{
  /* course+0xc134, +0xc11c, +0xc120, +0xc124, +0xc128, +0xc12c, +0xc130: optional clumps */
  if (*(RpClump **)(course + 0xc134) != (RpClump *)0x0) {
    RpClumpRender(*(RpClump **)(course + 0xc134));
  }
  if (*(RpClump **)(course + 0xc11c) != (RpClump *)0x0) {
    RpClumpRender(*(RpClump **)(course + 0xc11c));
  }
  if (*(RpClump **)(course + 0xc120) != (RpClump *)0x0) {
    RpClumpRender(*(RpClump **)(course + 0xc120));
  }
  if (*(RpClump **)(course + 0xc124) != (RpClump *)0x0) {
    RpClumpRender(*(RpClump **)(course + 0xc124));
  }
  if (*(RpClump **)(course + 0xc128) != (RpClump *)0x0) {
    RpClumpRender(*(RpClump **)(course + 0xc128));
  }
  if (*(RpClump **)(course + 0xc12c) != (RpClump *)0x0) {
    RpClumpRender(*(RpClump **)(course + 0xc12c));
  }
  if (*(RpClump **)(course + 0xc130) != (RpClump *)0x0) {
    RpClumpRender(*(RpClump **)(course + 0xc130));
  }
  return;
}
