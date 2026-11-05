/* rwp_sphere_0056ccc0 (0x0056ccc0): RwpSphere vtable slot (5f6478): line segment cast against a sphere */
#include "mfl_tier2.h"

typedef struct MflV3 { float x, y, z; } MflV3;
typedef struct MflRayHit {
  void *shape;        /* +0x00 */
  MflV3 p;            /* +0x04 hit point */
  MflV3 n;            /* +0x10 surface normal */
  float t;            /* +0x1c fraction along the segment */
} MflRayHit;

extern int rwp_ray_sphere_test(float *out, float *center, float *dir, float *origin, float radius);
extern float rwp_cylinder_0056fd20(float *out, float *in);

/* Segment start -> end against the sphere (radius at +0x4c, centred on the local origin).  rwp_ray_sphere_test gives the hit as the
   fraction t[0] / t[1].  The normal is (hit - centre) / radius, or normalised explicitly when the start lies inside (t[0] <= 0).
   Returns 1 on a hit, 0 otherwise. */
/* TIER2: unvalidated 2026-11-05 authority=diffemu */
int rwp_sphere_0056ccc0(float *sphere, MflV3 *start, MflV3 *end, MflRayHit *hit)
{
  MflV3 d, c;
  float t[2];
  float r, frac, nx, ny, nz, inv;

  hit->shape = sphere;
  d.x = end->x - start->x;
  d.y = end->y - start->y;
  d.z = end->z - start->z;
  r = sphere[19];
  c.z = 0.0f;
  c.y = 0.0f;
  c.x = 0.0f;
  if (rwp_ray_sphere_test(t, &start->x, &d.x, &c.x, r) <= 0)
    return 0;
  frac = t[0] / t[1];
  hit->t = frac;
  hit->p.x = d.x * frac + start->x;
  hit->p.y = d.y * frac + start->y;
  hit->p.z = frac * d.z + start->z;
  nx = hit->p.x - c.x;
  hit->n.x = nx;
  ny = hit->p.y - c.y;
  hit->n.y = ny;
  nz = hit->p.z - c.z;
  hit->n.z = nz;
  if (t[0] > 0.0f) {
    inv = 1.0f / r;
    hit->n.x = inv * nx;
    hit->n.y = ny * inv;
    hit->n.z = inv * nz;
  } else {
    rwp_cylinder_0056fd20(&hit->n.x, &hit->n.x);
  }
  return 1;
}
