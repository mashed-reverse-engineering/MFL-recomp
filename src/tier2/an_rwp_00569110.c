#include "mfl_tier2.h"

extern float _rwV3dNormalize(float *out, float *in);

/* RtLineSphereIntersectionTest: line = two points (start at 0..2, end at 3..5), sphere = centre (0..2) and radius (3).  The distance along the line
   (b * length of the segment, b = projection of the start-to-centre vector on the unit direction) is stored in *dist.  Succeeds when the start is inside
   the sphere, or the sphere lies ahead (b > 0) and the line reaches it (rr - (cc - b^2) >= 0).  Returns 1 / 0. */
/* TIER2: unvalidated 2026-11-02 authority=diffemu */
int RtLineSphereIntersectionTest(float *line, float *sphere, float *dist)
{
  float d[3];
  float len, rr, cx, cy, cz, b, cc;

  d[0] = line[3] - line[0];
  d[1] = line[4] - line[1];
  d[2] = line[5] - line[2];
  len = _rwV3dNormalize(d, d);
  rr = sphere[3] * sphere[3];
  cx = sphere[0] - line[0];
  cy = sphere[1] - line[1];
  cz = sphere[2] - line[2];
  b = cy * d[1] + cx * d[0] + cz * d[2];
  cc = cy * cy + cx * cx + cz * cz;
  if (cc < rr) {
    *dist = b * len;
    return 1;
  }
  if (b > 0.0f && rr - (cc - b * b) >= 0.0f) {
    *dist = b * len;
    return 1;
  }
  return 0;
}
