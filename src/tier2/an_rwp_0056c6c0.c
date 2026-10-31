#include "mfl_tier2.h"

/* A convex hull shape: centre of mass (and plane offset w) at 0, vertices (16 bytes each) at 0x50, vertex count at 0x54. */
typedef struct MflHull {
  float c[4];   /* 0x0 */
  unsigned char pad10[0x40];
  float (*verts)[4];   /* 0x50 */
  unsigned short count;   /* 0x54 */
} MflHull;

/* Moves a hull by the matrix m (rotation columns m[0..2] / m[4..6] / m[8..10], translation m[12..14]): every vertex is rotated and
   translated in place, the centre is rotated (without translation), and c[3] receives the dot product of the first vertex with the
   new centre (the translation with the centre when the hull has no vertices). */
/* TIER2: unvalidated 2026-10-31 authority=diffemu */
void rwp_sphere_0056c6c0(MflHull *h, float *m)
{
  double x, y, z, acc;
  volatile float zs;
  float cx, cy, cz[2];
  unsigned int i;

  for (i = 0; i < h->count; i++) {
    float *v = h->verts[i];
    x = v[1] * m[4];
    x = x + v[0] * m[0];
    x = x + v[2] * m[8];
    y = m[5] * v[1];
    y = y + v[2] * m[9];
    y = y + v[0] * m[1];
    z = v[2] * m[10];
    z = z + v[1] * m[6];
    z = z + m[2] * v[0];
    zs = z;
    h->verts[i][0] = m[12] + x;
    h->verts[i][1] = m[13] + y;
    h->verts[i][2] = m[14] + zs;
  }
  cx = h->c[0];
  cy = h->c[1];
  cz[0] = h->c[2];
  acc = cz[0] * m[8];
  acc = acc + cy * m[4];
  acc = acc + cx * m[0];
  h->c[0] = acc;
  acc = cz[0] * m[9];
  acc = acc + cy * m[5];
  acc = acc + cx * m[1];
  h->c[1] = acc;
  z = cz[0] * m[10];
  z = z + cy * m[6];
  z = z + cx * m[2];
  h->c[2] = z;
  if (h->count) {
    float *v0 = h->verts[0];
    acc = v0[1] * h->c[1];
    acc = acc + v0[0] * h->c[0];
    acc = acc + v0[2] * z;
    h->c[3] = acc;
  } else {
    acc = m[13] * h->c[1];
    acc = acc + m[12] * h->c[0];
    acc = acc + m[14] * z;
    h->c[3] = acc;
  }
}
