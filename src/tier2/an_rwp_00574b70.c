/* rwp_capsule_00574b70 (0x00574b70): RwpCapsule vtable slot (5f68a8): contact face of a capsule along a direction */
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

/* Capsule half length at +0x40 along local z.  A direction pointing mostly along -z gives the bottom end point; mostly along +z
   (or NaN) the top end point; in between (and for a positive half length) the face is the segment between both end points, with
   the normal = direction projected onto the xy plane and normalised.  Returns `face`. */
/* TIER2: unvalidated 2026-11-05 authority=diffemu */
MflSupportFace *rwp_capsule_00574b70(float *cap, void *unused, MflV3 *dir, MflSupportFace *face)
{
  float dz, h, s;
  int segment = 0;

  face->pts = face->pt_store;
  dz = dir->z;
  h = cap[16];
  face->pts[0].x = 0.0f;
  face->pts[0].y = 0.0f;
  if (dz < 0.5f) {
    face->pts[0].z = -h;
    segment = dz > -0.5f && h > 0.0f;
  } else {
    face->pts[0].z = h;
  }
  if (segment) {
    face->pts[1].x = 0.0f;
    face->pts[1].y = 0.0f;
    face->pts[1].z = h;
    face->npts = 2;
    s = 1.0f / (float)sqrt(1.0f - dz * dz);
    face->n.x = s * dir->x;
    face->n.z = 0.0f;
    face->n.y = s * dir->y;
  } else {
    face->npts = 1;
    face->n = *dir;
  }
  face->d = face->pts[0].y * face->n.y + face->pts[0].x * face->n.x + face->pts[0].z * face->n.z;
  return face;
}
