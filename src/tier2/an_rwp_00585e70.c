#include "mfl_tier2.h"

typedef struct MflBodyRef {
  int id0, id1;
  void *shape;   /* 0x8 */
  int id3;
  void *xf;   /* 0x10 */
} MflBodyRef;

/* Contact manifold of a pair: the contact normal dir at 0x100, a point buffer at 0x80, the first point slot base at 0x50 and the number
   of points at 0x54; 0xd0 receives the slot to start from. */
typedef struct MflManifold {
  unsigned char pad00[0x50];
  unsigned int base;   /* 0x50 */
  unsigned short count;   /* 0x54 */
  unsigned char pad56[0x2a];
  unsigned char points[0x50];   /* 0x80 */
  unsigned int start;   /* 0xd0 */
  unsigned char padd4[0x2c];
  float dir[3];   /* 0x100 */
} MflManifold;

extern int rwp_sphere_0056c800(void *shape, void *xf, int mode, float *dir, void *ctx);

/* Collects the contact points of a pair: the shape of body 0 is queried against the reversed normal (mode 0) and, when that finds
   something, the start slot is set (beyond the first 4 points: base + 16 * count) and the shape of body 1 is queried along the normal
   (mode 1) into the point buffer. */
/* TIER2: unvalidated 2026-10-31 authority=diffemu */
void rwp_aggregate_00585e70(MflBodyRef **pair, MflManifold *m)
{
  float neg[3];
  unsigned int base;

  neg[0] = -m->dir[0];
  neg[1] = -m->dir[1];
  base = m->base;
  neg[2] = -m->dir[2];
  if (rwp_sphere_0056c800(pair[0]->shape, pair[0]->xf, 0, neg, m) != 0) {
    m->start = base;
    if (m->count > 4) m->start = ((m->count & 0xffff) << 4) + base;
    rwp_sphere_0056c800(pair[1]->shape, pair[1]->xf, 1, m->dir, m->points);
  }
}
