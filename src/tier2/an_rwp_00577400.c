#include "mfl_tier2.h"
#include <math.h>

/* Ray (origin, dir) against the sphere (center, radius).  Returns 1 and the hit (out[0] = entry distance,
   out[1] = |dir|^2) when the ray hits it, 0 when it misses, -1 when it points away from it; a start inside the
   sphere gives out = (0, 1) and 1.  With out == 0 only "starts inside" is returned. */
/* TIER2: unvalidated 2026-10-31 authority=diffemu */
int rwp_grid_00577400(float *out, float *center, float *dir, float *origin, float radius)
{
  double acc, a, t;
  float mx, my, mz, c, b, disc;
  int inside;

  mx = origin[0] - center[0];
  my = origin[1] - center[1];
  mz = origin[2] - center[2];
  acc = my * my;
  acc = acc + mx * mx;
  acc = acc + mz * mz;
  c = acc - radius * radius;
  inside = c < 0.0f;
  if (out) {
    if (inside) {
      out[0] = 0.0f;
      out[1] = 1.0f;
      return 1;
    }
    acc = dir[1] * my;
    acc = acc + mx * dir[0];
    acc = acc + dir[2] * mz;
    b = acc;
    if (b <= 0.0f)
      return -1;
    a = dir[0] * dir[0];
    a = a + dir[1] * dir[1];
    a = a + dir[2] * dir[2];
    out[1] = a;
    disc = b * b - a * c;
    if (disc < 0.0f)
      return 0;
    t = b - a;
    if (t > 0.0f && t * t > disc)
      return 0;
    out[0] = b - sqrt(disc);
    return 1;
  }
  return inside;
}
