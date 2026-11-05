/* rwp_capsule_00575670 (0x00575670): hill-climb a convex hull's vertex graph to the vertex furthest along a direction */
#include "mfl_tier2.h"

typedef struct MflV3 { float x, y, z; } MflV3;
typedef struct MflHullGraph {
  int pad0;
  MflV3 *verts;                  /* +0x04 */
  unsigned short *adj_start;     /* +0x08 first adjacency entry of each vertex (n + 1 entries) */
  int pad3, pad4;
  unsigned char *edges;          /* +0x14 12 byte edge records, u16 destination vertex at +6 */
  unsigned short *adj;           /* +0x18 edge index per adjacency entry */
  unsigned short nverts;         /* +0x1c */
} MflHullGraph;

/* dot product in the original's order (y, x, z); t is a double so the sum keeps the x87 register range */
#define HULL_DOT(t, v, d) (t = (double)(v)->y * (d)->y, t += (double)(v)->x * (d)->x, t += (double)(v)->z * (d)->z)

/* Starts at vertex `start` (0 when out of range) and moves to the best neighbour (largest dot product with `dir`) until the vertex
   repeats, the dot product exceeds `stop`, or nverts + 2 steps are done.  *out = the vertex reached; returns its dot product.
   The start vertex's dot product stays unrounded (the original keeps it in an x87 register; double holds it exactly at 24 bit
   precision), a neighbour's is rounded to float before the compare. */
/* TIER2: unvalidated 2026-11-05 authority=diffemu */
double rwp_capsule_00575670(MflHullGraph *h, MflV3 *dir, unsigned short start, float stop, unsigned short *out)
{
  unsigned short cur, prev, from, nb, iter, limit, k;
  unsigned short *p;
  double best, t;
  volatile float d;      /* the original rounds the neighbour's dot product through a frame float (inf past FLT_MAX) before the
                            compare; cl would keep it unrounded on the x87 stack */

  cur = start < h->nverts ? start : 0;
  limit = (unsigned short)(h->nverts + 2);
  HULL_DOT(best, &h->verts[cur], dir);
  prev = limit;
  for (iter = 0; iter < limit; iter++) {
    if (prev == cur || best > stop)
      break;
    from = cur;
    k = (unsigned short)(h->adj_start[from + 1] - h->adj_start[from]);
    for (p = h->adj + h->adj_start[from]; k != 0; k--, p++) {
      nb = *(unsigned short *)(h->edges + *p * 12 + 6);
      if (nb == prev)
        continue;
      HULL_DOT(t, &h->verts[nb], dir);
      d = (float)t;
      if (d > best) {
        best = d;
        cur = nb;
      }
    }
    prev = from;
  }
  *out = cur;
  return best;
}
