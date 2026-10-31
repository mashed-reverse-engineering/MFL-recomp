#include "mfl_tier2.h"

/* Contact manifold of a pair (see rwp_aggregate_00585e70): the separating direction at 0x100 with its offset at 0x10c and depth at 0x110. */
typedef struct MflManifold {
  unsigned char pad00[0x100];
  float dir[3];   /* 0x100 */
  float offset;   /* 0x10c */
  float depth;   /* 0x110 */
} MflManifold;

extern int rwp_gjk_0058b040(int unused, void *pair, float margin, float *dir, float *pa, float *pb, float step);
extern void rwp_aggregate_00585e70(void *pair, MflManifold *m);

/* Pair test with the distance search rwp_gjk_0058b040 (limit t, step size step): when it finds a depth below t, the contact points are
   collected (rwp_aggregate_00585e70).  Returns 1 in that case, else 0.  Same flow as rwp_aggregate_005894f0. */
/* TIER2: unvalidated 2026-10-31 authority=diffemu */
int rwp_gjk_0058b450(int unused, void *pair, float t, MflManifold *m, float step)
{
  int hit = 0;

  if (rwp_gjk_0058b040(unused, pair, t, m->dir, &m->offset, &m->depth, step)) {
    if (m->depth < t) {
      hit = 1;
      rwp_aggregate_00585e70(pair, m);
      return hit;
    }
  }
  return 0;
}
