/* rwp_trilist_edge_callback (0x00573ab0): collision world triangle callback of rwp_sector_triangles_query: clears the edge flags of a
   triangle whose edges are shared with a neighbouring triangle */
#include "mfl_tier2.h"

extern float rwp_cylinder_0056fd20(float *out, float *in);   /* normalise, returns the length */

/* RpCollisionTriangle: the triangle the world query found. */
typedef struct MflCollTriangle {
  float normal[3];
  float point[3];   /* 0xc */
  int index;   /* 0x18 */
  float *vertices[3];   /* 0x1c */
} MflCollTriangle;

/* The query triangle (rwp_sector_triangles_query): index in its sector, the sector's flag bits (4 per triangle: bit 0 the face, bits 1..3
   the edges v[k] -> v[(k + 1) % 3]), its sector, vertices, distance margin and normal. */
typedef struct MflTriangle {
  int index;
  unsigned int *bits;   /* 0x4 */
  void *sector;   /* 0x8 */
  float *v[3];   /* 0xc */
  float margin;   /* 0x18 */
  float n[3];   /* 0x1c */
} MflTriangle;

#define BIT_CLR(bits, id) ((bits)[(id) >> 5] &= ~(1 << ((id) & 0x1f)))
#define BIT_TEST(bits, id) ((bits)[(id) >> 5] & (1 << ((id) & 0x1f)))
#define DIST(a, b) sqrt(((a)[0] - (b)[0]) * ((a)[0] - (b)[0]) + ((a)[1] - (b)[1]) * ((a)[1] - (b)[1]) + ((a)[2] - (b)[2]) * ((a)[2] - (b)[2]))
#define DOT(a, b) ((a)[0] * (b)[0] + (a)[1] * (b)[1] + (a)[2] * (b)[2])

/* For every edge of the found triangle ct that coincides (within t->margin, opposite direction) with an edge k of t: a convex or flat
   neighbour (the edge direction against n(ct) x n(t) >= 0, or > -0.2 with normals within 60 degrees) clears t's edge flag k; a concave
   one whose normal faces away (cos < -0.2) clears the face flag.  Stops at the first shared edge unless the triangles are back to back
   (cos <= -0.99); back to back triangles clear the face flag when ct lies on or below t's plane.  Returns ct. */
/* TIER2: unvalidated 2026-11-05 authority=diffemu */
MflCollTriangle *rwp_trilist_edge_callback(void *isect, void *sector, MflCollTriangle *ct, float distance, MflTriangle *t)
{
  unsigned int base, i, prev, k, next;
  float cosn, s;
  float c[3], e[3];
  float *a, *b;

  if (sector == t->sector && ct->index == t->index)
    return ct;
  base = t->index * 4;
  cosn = DOT(ct->normal, t->n);
  for (i = 0, prev = 2; i < 3; prev = i, i++) {
    for (k = 0; k < 3; k++) {
      a = ct->vertices[i];
      b = t->v[k];
      if (!(DIST(a, b) < t->margin))
        continue;
      next = (1 << k) & 3;
      if (!(DIST(ct->vertices[prev], t->v[next]) < t->margin))
        continue;
      c[0] = ct->normal[1] * t->n[2] - t->n[1] * ct->normal[2];
      c[1] = t->n[0] * ct->normal[2] - t->n[2] * ct->normal[0];
      c[2] = t->n[1] * ct->normal[0] - t->n[0] * ct->normal[1];
      e[0] = t->v[next][0] - b[0];
      e[1] = t->v[next][1] - b[1];
      e[2] = t->v[next][2] - b[2];
      rwp_cylinder_0056fd20(e, e);
      s = DOT(e, c);
      if (s > 0.0f || (s > -0.2f && cosn > 0.5f))
        BIT_CLR(t->bits, base + k + 1);
      else if (s < 0.0f && cosn < -0.2f)
        BIT_CLR(t->bits, base);
      if (cosn > -0.99f)
        return ct;
    }
  }
  if (cosn < -0.99f && BIT_TEST(t->bits, base)) {
    if (DOT(ct->vertices[0], t->n) - DOT(t->v[0], t->n) <= 0.0f)
      BIT_CLR(t->bits, base);
  }
  return ct;
}
