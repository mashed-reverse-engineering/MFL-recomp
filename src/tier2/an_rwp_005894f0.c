/* rwp_pair_test_separating_axis (0x005894f0): pair test with the separating-axis query, collecting contacts */
#include "mfl_tier2.h"

/* Contact manifold of a pair (see rwp_contact_points_collect): the separating direction at 0x100 with its offset at 0x10c and depth at 0x110. */
typedef struct MflManifold {
  unsigned char pad00[0x100];
  float dir[3];   /* 0x100 */
  float offset;   /* 0x10c */
  float depth;   /* 0x110 */
} MflManifold;

extern int rwp_aggregate_00588cb0(void *a1, void *pair, float t, float *dir, float *offset, float *depth, void *a5);
extern void rwp_contact_points_collect(void *pair, MflManifold *m);

/* Pair test: runs the separation query rwp_aggregate_00588cb0 with limit t into the manifold and, when the depth found is below t, collects
   the contact points (rwp_contact_points_collect).  Returns 1 in that case, else 0. */
/* TIER2: unvalidated 2026-10-31 authority=diffemu */
int rwp_pair_test_separating_axis(void *a1, void *pair, float t, MflManifold *m, void *a5)
{
  int hit = 0;

  if (rwp_aggregate_00588cb0(a1, pair, t, m->dir, &m->offset, &m->depth, a5)) {
    if (m->depth < t) {
      hit = 1;
      rwp_contact_points_collect(pair, m);
      return hit;
    }
  }
  return 0;
}
