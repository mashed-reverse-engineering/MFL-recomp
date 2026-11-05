/* rwp_sphere_0056cba0 (0x0056cba0): RwpSphere vtable slot (5f6460): axis-aligned bounding box of a sphere */
#include "mfl_tier2.h"

/* Sphere radius at +0x4c; `frame` (optional) is a matrix whose position row is at +0x30.  box[0..2] = max corner, box[4..6] = min
   corner.  Without a frame the box is centred on the origin.  The third vtable argument is not used by spheres. */
/* TIER2: unvalidated 2026-11-05 authority=diffemu */
void rwp_sphere_0056cba0(float *sphere, float *frame, void *unused, float *box)
{
  float ext[3];

  ext[0] = sphere[19];
  ext[1] = sphere[19];
  ext[2] = sphere[19];
  if (frame != 0) {
    box[0] = frame[12] + ext[0];
    box[1] = frame[13] + ext[1];
    box[2] = frame[14] + ext[2];
    box[4] = frame[12] - ext[0];
    box[5] = frame[13] - ext[1];
    box[6] = frame[14] - ext[2];
  } else {
    box[4] = -ext[0];
    box[5] = -ext[1];
    box[6] = -ext[2];
    box[0] = ext[0];
    box[1] = ext[1];
    box[2] = ext[2];
  }
}
