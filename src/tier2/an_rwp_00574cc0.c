/* rwp_capsule_00574cc0 (0x00574cc0): RwpCapsule vtable slot (5f68b8): line segment cast against a capsule */
#include "mfl_tier2.h"

typedef struct MflV3 { float x, y, z; } MflV3;
typedef struct MflRayHit {
  void *shape;        /* +0x00 */
  MflV3 p;            /* +0x04 hit point (advanced while the cast steps over the end planes) */
  MflV3 n;            /* +0x10 surface normal */
  float t;            /* +0x1c fraction along the segment */
} MflRayHit;

extern int rwp_ray_sphere_test(float *out, float *center, float *dir, float *origin, float radius);
extern int rwp_line_quadric_intersect(float *out, float *a2, float *v, float *a4, float *dir, float r, int a7, int a8);
extern float rwp_grid_005771f0(float *out, float *in, float *axis, float side);

/* Capsule along the local z axis: half length h (+0x40), radius r (+0x4c).  The cast alternates between the end sphere on the side
   where the point lies beyond an end plane (side s = +-1) and the infinite cylinder (s = 0); when the other part is hit first the
   point steps onto the end plane and the next part is tried, at most 3 steps.  Normals: sphere / cylinder hit = (hit - centre) / r,
   resp. with the axis component taken out; a start inside (t = 0) gets the explicitly normalised direction (5771f0).
   Values the original keeps in x87 registers are doubles here (exact at 24 bit precision), its frame slots are floats.
   Returns 1 on a hit, 0 otherwise. */
/* frame block in the original's order: sphere / cylinder hit fractions, capsule axis, segment, sphere centre, axis base, then the
   float slots of the radial x component and its axis part (in memory, so their float rounding stays: a float scalar would be merged
   into the double by cl) */
typedef struct MflCapsuleCastWork { float ts[2], tc[2], axis[3], d[3], c[3], base[3], slot[2]; } MflCapsuleCastWork;

/* TIER2: unvalidated 2026-11-05 authority=diffemu */
int rwp_capsule_00574cc0(float *cap, MflV3 *start, MflV3 *end, MflRayHit *hit)
{
  float r, num, den, s, h, dzv, py, pz, vzf, wf, inv_f;
  MflCapsuleCastWork w;
  int left, nh, plane;
  double q, f, px, vx, vy, k, nx, ny, m, inv;

  w.base[0] = 0.0f;
  w.base[1] = 0.0f;
  w.base[2] = 0.0f;
  hit->shape = cap;
  w.axis[0] = 0.0f;
  w.axis[1] = 0.0f;
  w.axis[2] = 1.0f;
  w.d[0] = end->x - start->x;
  left = 3;
  w.d[1] = end->y - start->y;
  w.d[2] = end->z - start->z;
  r = cap[19];
  h = cap[16];
  *(MflV3 *)w.c = *start;
  if ((double)h * h < (double)w.c[2] * w.c[2])      /* beyond an end plane */
    s = w.c[2] > 0.0f ? 1.0f : -1.0f;
  else
    s = 0.0f;
  hit->p = *start;
  hit->t = 0.0f;
  for (;;) {
    dzv = w.d[2];
    if (s != 0.0f) {
      /* end sphere */
      w.c[2] = s * h;
      w.c[0] = 0.0f;
      w.c[1] = 0.0f;
      nh = rwp_ray_sphere_test(w.ts, &hit->p.x, w.d, w.c, r);
      if (nh > 0 && w.ts[0] == 0.0f) {
        hit->n.x = hit->p.x - w.c[0];
        hit->n.y = hit->p.y - w.c[1];
        hit->n.z = hit->p.z - w.c[2];
        rwp_grid_005771f0(&hit->n.x, &hit->n.x, w.axis, s);
        return 1;
      }
      q = (double)dzv * s;
      plane = 0;
      if (q < 0.0f) {                                  /* moving towards the end plane */
        num = (float)(-(double)h - -(double)hit->p.z * s);
        den = (float)-q;
        plane = num <= den;
      }
      if (nh > 0 && (plane <= 0 || (double)w.ts[1] * num > (double)den * w.ts[0]))
        goto sphere_hit;
      if (plane <= 0)
        return 0;
      s = 0.0f;
    } else {
      /* cylinder */
      if (rwp_line_quadric_intersect(w.tc, &hit->p.x, w.d, w.base, w.axis, r, 0, 0) <= 0)
        return 0;
      if (w.tc[0] == 0.0f)
        goto cylinder_inside;
      s = -1.0f;
      if (!(dzv < 0.0f))
        s = 1.0f;
      k = (double)hit->p.y * w.axis[1];
      k = k + (double)hit->p.x * w.axis[0];
      k = k + (double)hit->p.z * w.axis[2];
      k = -k;
      num = (float)(k * s + h);
      den = dzv * s;
      if (!(num <= den) || !((double)w.tc[0] * den > (double)w.tc[1] * num))
        goto cylinder_hit;
    }
    /* step onto the end plane */
    f = (double)num / den;
    left--;
    hit->t = (float)(f + hit->t);
    px = w.d[0] * f + hit->p.x;
    hit->p.x = (float)px;
    py = (float)(f * w.d[1] + hit->p.y);
    hit->p.y = py;
    pz = (float)(f * w.d[2] + hit->p.z);
    hit->p.z = pz;
    w.d[0] = (float)(end->x - px);
    w.d[1] = end->y - py;
    w.d[2] = end->z - pz;
    if (left == 0)
      return 0;
  }

sphere_hit:
  f = (double)w.ts[0] / w.ts[1];
  hit->t = (float)(f + hit->t);
  px = w.d[0] * f + hit->p.x;
  hit->p.x = (float)px;
  py = (float)(f * w.d[1] + hit->p.y);
  hit->p.y = py;
  pz = (float)(f * w.d[2] + hit->p.z);
  hit->p.z = pz;
  inv_f = 1.0f / r;
  hit->n.x = (float)((px - w.c[0]) * inv_f);
  hit->n.y = (py - w.c[1]) * inv_f;
  hit->n.z = (pz - w.c[2]) * inv_f;
  return 1;

cylinder_hit:
  f = (double)w.tc[0] / w.tc[1];
  hit->t = (float)(f + hit->t);
  px = w.d[0] * f + hit->p.x;
  hit->p.x = (float)px;
  py = (float)(f * w.d[1] + hit->p.y);
  hit->p.y = py;
  pz = (float)(f * w.d[2] + hit->p.z);
  hit->p.z = pz;
  vx = px - w.base[0];
  vy = (double)py - w.base[1];
  vzf = pz - w.base[2];
  k = (double)w.axis[1] * vy;
  wf = vzf * w.axis[2];
  k = vx * w.axis[0] + k;
  k = k + wf;
  k = -k;
  nx = k * w.axis[0] + vx;
  w.slot[0] = (float)nx;
  m = nx * w.axis[0];
  w.slot[1] = (float)m;
  k = m + (double)w.axis[1] * vy;
  k = k + wf;
  k = -k;
  ny = k * w.axis[1] + vy;
  inv = 1.0 / (double)r;
  hit->n.x = (float)(w.slot[0] * inv);
  hit->n.y = (float)(inv * ny);
  k = ny * w.axis[1];
  k = k + w.slot[1];
  k = k + wf;
  k = -k;
  hit->n.z = (float)((k * w.axis[2] + vzf) * inv);
  return 1;

cylinder_inside:
  vx = (double)hit->p.x - w.base[0];
  vy = (double)hit->p.y - w.base[1];
  vzf = hit->p.z - w.base[2];
  k = (double)w.axis[1] * vy;
  wf = vzf * w.axis[2];
  q = vx * w.axis[0] + k;
  q = q + wf;
  q = -q;
  nx = q * w.axis[0] + vx;
  hit->n.x = (float)nx;
  w.slot[1] = (float)(nx * w.axis[0]);
  q = k + w.slot[1];
  q = q + wf;
  q = -q;
  ny = q * w.axis[1] + vy;
  hit->n.y = (float)ny;
  q = ny * w.axis[1];
  q = q + w.slot[1];
  q = q + wf;
  q = -q;
  hit->n.z = (float)(q * w.axis[2] + vzf);
  rwp_grid_005771f0(&hit->n.x, &hit->n.x, w.axis, 0.0f);
  return 1;
}
