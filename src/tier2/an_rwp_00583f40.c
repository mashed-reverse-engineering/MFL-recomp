/* rwp_hull_mass_props (0x00583f40): mass properties of a convex hull from its integrals */
#include "mfl_tier2.h"
extern float rwp_convexhullmassprops_00584080();

/* rwp_hull_mass_props: mass properties of a convex hull from the integrals of 584080.
   *mass = volume, com = centre of mass, inertia = the 3x3 inertia tensor (row major, per unit mass) about the centre of mass. */
/* TIER2: unvalidated 2026-11-01 authority=diffemu */
int rwp_hull_mass_props(int hull, float *com, float *inertia, float *mass)
{
  float cross[3];      /* integral of xy, yz, zx */
  float first[3];      /* integral of x, y, z */
  float second[3];     /* integral of x^2, y^2, z^2 */
  float m, inv;
  float ixx, iyy, izz, ixy, iyz, izx;

  m = rwp_convexhullmassprops_00584080(hull, first, second, cross);
  *mass = m;
  inv = 1.0f / m;
  com[0] = first[0] * inv;
  com[1] = first[1] * inv;
  com[2] = first[2] * inv;
  ixx = (second[2] + second[1]) * inv;
  inertia[0] = ixx;
  iyy = (second[0] + second[2]) * inv;
  inertia[4] = iyy;
  izz = (second[0] + second[1]) * inv;
  inertia[8] = izz;
  ixy = -(cross[0] * inv);
  inertia[1] = inertia[3] = ixy;
  iyz = -(cross[1] * inv);
  inertia[5] = inertia[7] = iyz;
  izx = -(cross[2] * inv);
  inertia[2] = inertia[6] = izx;
  /* parallel axis shift to the centre of mass */
  inertia[0] = ixx - (com[1] * com[1] + com[2] * com[2]);
  inertia[4] = iyy - (com[0] * com[0] + com[2] * com[2]);
  inertia[8] = izz - (com[0] * com[0] + com[1] * com[1]);
  inertia[1] = inertia[3] = com[1] * com[0] + ixy;
  inertia[5] = inertia[7] = com[2] * com[1] + iyz;
  inertia[2] = inertia[6] = com[2] * com[0] + izx;
  return 1;
}
