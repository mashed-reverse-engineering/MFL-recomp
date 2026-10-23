#include "mfl_tier2.h"
/* Float indices of the bounding box (inferred): max xyz at 0..2, index 3 unused, min xyz at 4..6 */
#define BOUNDS_MAX_X (0)
#define BOUNDS_MAX_Y (1)
#define BOUNDS_MAX_Z (2)
#define BOUNDS_MIN_X (4)
#define BOUNDS_MIN_Y (5)
#define BOUNDS_MIN_Z (6)
/* FUN_0056f900 */

/*
 * Grows the axis-aligned bounding box so that it contains point (inferred).
 * bounds: 7-float box (layout above), updated in place.
 * point: xyz position, read only.
 * Returns nothing.
 */
void __cdecl FUN_0056f900(float *bounds,float *point)

{
  /* X: if the lower bound is at or below point.x, only the upper bound can grow; otherwise lower it to point.x */
  if (!(bounds[BOUNDS_MIN_X] > *point)) {      /* `<=` is also taken for NaN in the original (and eax,0x4100; jne) */
    if (*bounds < *point) {
      *bounds = *point;
    }
  }
  else {
    bounds[BOUNDS_MIN_X] = *point;
  }
  /* Y: same rule as X */
  if (!(bounds[BOUNDS_MIN_Y] > point[1])) {
    if (bounds[BOUNDS_MAX_Y] < point[1]) {
      bounds[BOUNDS_MAX_Y] = point[1];
    }
  }
  else {
    bounds[BOUNDS_MIN_Y] = point[1];
  }
  /* Z: point below the lower bound only moves the lower bound and skips the upper-bound check (inferred: upper bound is already above it) */
  if (point[2] < bounds[BOUNDS_MIN_Z]) {
    bounds[BOUNDS_MIN_Z] = point[2];
    return;
  }
  if (bounds[BOUNDS_MAX_Z] < point[2]) {
    bounds[BOUNDS_MAX_Z] = point[2];
  }
  return;
}


