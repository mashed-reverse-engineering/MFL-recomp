/* rwp_capsule_00574a80 (0x00574a80): RwpCapsule vtable slot (5f68a0): axis-aligned bounding box of a capsule */
#include "mfl_tier2.h"

/* Capsule half length at +0x40 (along the local z axis), radius at +0x4c.  `frame` (optional) is a matrix: at axis row +0x20, position
   row +0x30.  box[0..2] = max corner, box[4..6] = min corner. */
/* TIER2: unvalidated 2026-11-05 authority=diffemu */
void rwp_capsule_00574a80(float *cap, float *frame, void *unused, float *box)
{
  float h, ex, ey, ez;

  if (frame != 0) {
    h = cap[16];
    ex = (float)fabs(frame[8]) * h + cap[19];
    ey = (float)fabs(frame[9]) * h + cap[19];
    ez = (float)fabs(frame[10]) * h + cap[19];
    box[0] = ex + frame[12];
    box[1] = ey + frame[13];
    box[2] = ez + frame[14];
    box[4] = frame[12] - ex;
    box[5] = frame[13] - ey;
    box[6] = frame[14] - ez;
  } else {
    box[0] = cap[19];
    box[1] = cap[19];
    box[2] = cap[16] + cap[19];
    box[4] = -box[0];
    box[5] = -box[1];
    box[6] = -box[2];
  }
}
