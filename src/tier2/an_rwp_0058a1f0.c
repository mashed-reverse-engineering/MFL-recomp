#include "mfl_tier2.h"
#include <math.h>

typedef struct MflContact {
  int pad0[3];
  float pos[3];   /* 0xc */
  float extent;   /* 0x18 */
  int pad1c[3];
} MflContact;
typedef struct MflContactList {
  MflContact c[4];
  int pad_a0[3];
  int count;   /* 0xac */
} MflContactList;

/* Tests a plane (normal plane[0..2] through the point plane[3..5], extent plane[6]) against the first contact of the list: returns 1 when
   the list is empty or the contact lies within half the larger extent plus a tenth of tol of the plane, else 0. */
/* TIER2: unvalidated 2026-10-31 authority=diffemu */
int rwp_generalprimitive_0058a1f0(float *plane, MflContactList *list, float tol)
{
  float d[3];
  float ea, eb, e;
  double dist;

  if (!list->count) return 1;
  d[0] = plane[3] - list->c[0].pos[0];
  d[1] = plane[4] - list->c[0].pos[1];
  d[2] = plane[5] - list->c[0].pos[2];
  ea = fabs(plane[6]);
  eb = fabs(list->c[0].extent);
  if (ea < eb) e = eb;
  else e = ea;
  dist = plane[1] * d[1];
  dist = dist + d[0] * plane[0];
  dist = dist + plane[2] * d[2];
  if (e * 0.5f + tol * 0.1f > fabs(dist)) return 1;
  return 0;
}
