#include "mfl_tier2.h"

enum { BOX_CORNER_COUNT = 8 };

/* Returns 1 if none of the box's 8 corners lies on or in front of the plane (classify 0 or 1), else 0.
   box layout: [0..2] = max corner, [3..5] = min corner; bit 0/1/2 of the corner index picks x/y/z max. */
undefined4 __cdecl bbox_plane_corner_test(float *plane,float *box)
{
  int corner_index;
  int side;
  float corner[3];
  for (corner_index = 0; corner_index < BOX_CORNER_COUNT; corner_index++) {
    corner[0] = (corner_index & 1) ? box[0] : box[3];
    corner[1] = (corner_index & 2) ? box[1] : box[4];
    corner[2] = (corner_index & 4) ? box[2] : box[5];
    side = plane_classify_point(plane,corner,0);
    if (side == 0 || side == 1) return 0;
  }
  return 1;
}
