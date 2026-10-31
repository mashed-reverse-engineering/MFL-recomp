#include "mfl_tier2.h"
#include <math.h>

extern float rwp_cylinder_0056fd20(float *a, float *b);
extern int rwp_grid_00577550(float *hit, float *center, float *dir, float *p0, float *d, float radius, int flag0, int flag1);
extern int rwp_grid_00577400(float *hit, float *center, float *dir, float *origin, float radius);

/* Segment p0..p1 against the sphere (center, radius) swept along dir: first the closest-approach query rwp_grid_00577550
   against the segment direction; the foot point s along the segment is clamped to the end points (s < 0: p0, s*s > len2:
   p1) and the sphere ray test rwp_grid_00577400 is run against that end point; otherwise out = foot point.
   Returns the hit time (distance ratio) or 2.0 for no hit. */
/* TIER2: unvalidated 2026-10-31 authority=diffemu */
float rwp_null_0058c5c0(float *out, float *center, float *dir, float *p0, float *p1, float radius)
{
  double acc, t, px, py, vy;
  float d[3], hit[2], len2, s;
  volatile float sp[2];      /* pz and vx: the original rounds them to frame floats (volatile keeps the rounding) */

  d[0] = p1[0] - p0[0];
  d[1] = p1[1] - p0[1];
  d[2] = p1[2] - p0[2];
  acc = d[1] * d[1];
  acc = acc + d[0] * d[0];
  acc = acc + d[2] * d[2];
  len2 = acc;
  rwp_cylinder_0056fd20(d, d);
  if (rwp_grid_00577550(hit, center, dir, p0, d, radius, 0, 0) > 0) {
    t = hit[0] / hit[1];
    px = t * dir[0] + center[0];
    py = t * dir[1] + center[1];
    sp[0] = t * dir[2] + center[2];
    sp[1] = px - p0[0];
    vy = py - p0[1];
    acc = vy * d[1];
    acc = acc + sp[1] * d[0];
    acc = acc + (sp[0] - p0[2]) * d[2];
    s = acc;
    if (s < 0.0f) {
      out[0] = p0[0];
      out[1] = p0[1];
      out[2] = p0[2];
    } else if (s * s > len2) {
      out[0] = p1[0];
      out[1] = p1[1];
      out[2] = p1[2];
    } else {
      out[0] = s * d[0] + p0[0];
      out[1] = s * d[1] + p0[1];
      out[2] = s * d[2] + p0[2];
      return t;
    }
    if (rwp_grid_00577400(hit, center, dir, out, radius) > 0)
      return hit[0] / hit[1];
  }
  return 2.0f;
}
