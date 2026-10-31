#include "mfl_tier2.h"
#include <math.h>

extern void rwp_math_0056f900(float *bounds, float *point);
extern void *rwp_math_0056f0d0(float *out, float *in, float *p);

/* One edge of the hull: its length (later the inverse length) and the two vertex indices. */
typedef struct MflEdge {
  float len;
  unsigned short v0, v1;
  int pad8;
} MflEdge;

/* A convex hull with edges: vertices (12 bytes) at 4, edges at 0x14, vertex / edge counts, the bounding box {max[3], pad, min[3]} at
   0x24, the bounding sphere centre at 0x44 (radius at 0x50) and a flag word at 0x88. */
typedef struct MflHullData {
  int pad00;
  float (*verts)[3];   /* 0x4 */
  unsigned char pad08[0xc];
  MflEdge *edges;   /* 0x14 */
  unsigned char pad18[4];
  unsigned short vcount;   /* 0x1c */
  unsigned short pad1e;
  unsigned short ecount;   /* 0x20 */
  unsigned short pad22;
  float maxv[3];   /* 0x24 */
  float pad30;
  float minv[3];   /* 0x34 */
  float pad40;
  float centre[3];   /* 0x44 */
  float radius;   /* 0x50 */
  unsigned char pad54[0x34];
  unsigned int flags;   /* 0x88 */
} MflHullData;

/* Prepares a hull: inverse edge lengths (0 stays 0), the bounding box of the vertices, the bounding sphere (box centre, half of the
   largest box extent, then grown over every vertex by rwp_math_0056f0d0), and marks the hull as prepared (flags |= 1). */
/* TIER2: unvalidated 2026-10-31 authority=diffemu */
void rwp_capsule_00575330(MflHullData *h)
{
  double acc, len, dx, dy, dz, ex, m;
  float ext[2];
  unsigned short e, k;
  MflEdge *edge;
  float *p, *q;

  for (e = 0; e < h->ecount; e++) {
    edge = &h->edges[e];
    p = h->verts[edge->v0];
    q = h->verts[edge->v1];
    dz = p[2] - q[2];
    dy = p[1] - q[1];
    dx = p[0] - q[0];
    acc = dx * dx;
    acc = acc + dy * dy;
    acc = acc + dz * dz;
    len = sqrt(acc);
    edge->len = len;
    if (len != 0.0) edge->len = 1.0f / len;
  }
  *(int *)&h->minv[0] = *(int *)&h->verts[0][0];
  *(int *)&h->minv[1] = *(int *)&h->verts[0][1];
  *(int *)&h->minv[2] = *(int *)&h->verts[0][2];
  *(int *)&h->maxv[0] = *(int *)&h->verts[0][0];
  *(int *)&h->maxv[1] = *(int *)&h->verts[0][1];
  *(int *)&h->maxv[2] = *(int *)&h->verts[0][2];
  for (k = 1; k < h->vcount; k++) rwp_math_0056f900(h->maxv, h->verts[k]);
  ex = h->maxv[0] - h->minv[0];
  ext[0] = h->maxv[1] - h->minv[1];
  ext[1] = h->maxv[2] - h->minv[2];
  h->centre[0] = 0.5f * ex + h->minv[0];
  h->centre[1] = ext[0] * 0.5f + h->minv[1];
  h->centre[2] = ext[1] * 0.5f + h->minv[2];
  m = ext[0] < ext[1] ? ext[1] : ext[0];
  m = ex < m ? m : ex;
  h->radius = m * 0.5f;
  for (k = 0; k < h->vcount; k++) rwp_math_0056f0d0(h->centre, h->centre, h->verts[k]);
  h->flags = h->flags | 1;
}
