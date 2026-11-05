/* rwp_box_0056d3b0 (0x0056d3b0): RwpBox vtable slot (5f64f8): contact face of a box along a direction (vertex, edge or face) */
#include "mfl_tier2.h"

typedef struct MflV3 { float x, y, z; } MflV3;
typedef struct MflV4 { float x, y, z, w; } MflV4;
typedef struct MflSupportFace {
  MflV3 n;                 /* face normal */
  float d;                 /* plane offset: dot(first point, n) */
  MflV4 pt_store[4];       /* +0x10: point storage */
  MflV4 *pts;              /* +0x50 -> pt_store */
  short npts;              /* +0x54 */
} MflSupportFace;

extern float rwp_cylinder_0056fd20(float *out, float *in);

/* The original's frame block, in its order: sh = signed half sizes (reused as the +-1 sign pair of the face loop), c = support point,
   h = half sizes, d = direction components per axis, R = the box axes (identity, 4x4).  With a direction shorter than 0.5 along every
   axis the original's axis pick becomes -1 and it reads the words in front of d / R (h[2] ...); indexing this block reproduces that. */
typedef struct MflBoxFaceWork {
  int parity;
  float sh[3];
  float c[3];
  float h[3];
  float d[3];
  float R[4][4];
} MflBoxFaceWork;

/* An axis whose direction component is above 0.5 in magnitude contributes +-half size (odd number of negative signs = parity);
   the others are "flat".  0 flat axes: the corner; 1: the edge along the flat axis (normal = (axis x dir) x axis, normalised);
   2 or more: the face across the remaining axis, 4 corners (the winding follows `flip` against the side), normal = that axis
   times +-1.  Returns `face`. */
/* TIER2: unvalidated 2026-11-05 authority=diffemu */
MflSupportFace *rwp_box_0056d3b0(float *box, int flip, MflV3 *dir, MflSupportFace *face)
{
  MflBoxFaceWork w;
  int k, sel, nflat, side, i1, i2, bsel, j;
  double dk, e, a, b, A, B, C, g;
  float *row, *r1, *r2;

  sel = 0;
  w.parity = 0;
  nflat = 0;
  w.c[0] = 0.0f;
  w.c[2] = 0.0f;
  w.c[1] = 0.0f;
  face->pts = face->pt_store;
  w.h[0] = box[16];
  w.h[1] = box[17];
  w.h[2] = box[18];
  w.R[2][2] = 1.0f;
  w.R[1][1] = 1.0f;
  w.R[0][0] = 1.0f;
  w.R[1][0] = 0.0f;
  w.R[0][2] = 0.0f;
  w.R[0][1] = 0.0f;
  w.R[2][1] = 0.0f;
  w.R[2][0] = 0.0f;
  w.R[1][2] = 0.0f;
  w.R[3][2] = 0.0f;
  w.R[3][1] = 0.0f;
  w.R[3][0] = 0.0f;
  for (k = 0; k < 3; k++) {
    row = w.R[k];
    dk = (double)dir->y * row[1];
    dk = dk + (double)row[0] * dir->x;
    dk = dk + (double)row[2] * dir->z;
    w.d[k] = (float)dk;
    if (dk * dk > 0.25f) {
      if (dk > 0.0f) {
        w.sh[k] = w.h[k];
      } else {
        w.sh[k] = -w.h[k];
        w.parity ^= 1;
      }
      w.c[0] = w.sh[k] * row[0] + w.c[0];
      w.c[1] = w.sh[k] * row[1] + w.c[1];
      w.c[2] = w.sh[k] * row[2] + w.c[2];
    } else {
      w.sh[k] = 0.0f;
      sel = nflat++ == 0 ? k : 3 - k - sel;
    }
  }
  if (nflat == 0) {
    face->n = *dir;
    face->pt_store[0].x = w.c[0];
    face->pt_store[0].y = w.c[1];
    face->pt_store[0].z = w.c[2];
    face->npts = 1;
  } else if (nflat == 1) {
    row = w.R[sel];
    e = -w.h[sel];
    face->pt_store[0].x = e * row[0] + w.c[0];
    face->pts[0].y = e * row[1] + w.c[1];
    face->pts[0].z = e * row[2] + w.c[2];
    face->pts[1].x = w.h[sel] * row[0] + w.c[0];
    face->pts[1].y = w.h[sel] * row[1] + w.c[1];
    face->pts[1].z = w.h[sel] * row[2] + w.c[2];
    A = (double)row[1] * dir->z - (double)dir->y * row[2];
    B = (double)row[2] * dir->x - (double)dir->z * row[0];
    C = (double)dir->y * row[0] - (double)row[1] * dir->x;
    face->n.x = (float)(row[2] * B - row[1] * C);
    face->n.y = (float)(row[0] * C - row[2] * A);
    face->n.z = (float)(row[1] * A - row[0] * B);
    rwp_cylinder_0056fd20(&face->n.x, &face->n.x);
    face->npts = 2;
  } else {
    side = w.d[sel] > 0.0f;
    i1 = 1 << sel & 3;
    i2 = 1 << i1 & 3;
    w.sh[0] = 1.0f;
    w.sh[1] = 1.0f;
    bsel = flip != side;
    r1 = w.R[i1];
    r2 = w.R[i2];
    for (j = 0; j < 4; j++) {
      a = w.sh[0] * w.h[i1];
      face->pts[j].x = a * r1[0] + w.c[0];
      face->pts[j].y = a * r1[1] + w.c[1];
      face->pts[j].z = a * r1[2] + w.c[2];
      b = w.sh[1] * w.h[i2];
      face->pts[j].x = b * r2[0] + face->pts[j].x;
      face->pts[j].y = b * r2[1] + face->pts[j].y;
      face->pts[j].z = b * r2[2] + face->pts[j].z;
      w.sh[bsel] = w.sh[bsel] * -1.0f;
      bsel ^= 1;
    }
    face->npts = 4;
    g = 1.0f - (double)w.parity * 2.0f;
    row = w.R[sel];
    face->n.x = (float)(row[0] * g);
    face->n.y = (float)(row[1] * g);
    face->n.z = (float)(row[2] * g);
  }
  face->d = face->pts[0].y * face->n.y + face->pts[0].x * face->n.x + face->pts[0].z * face->n.z;
  return face;
}
