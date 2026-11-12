/* rwp_capsule_00575a30 (0x00575a30): collect the values of a slot pool ring that are not masked out, optionally sorted */
#include "mfl_tier2.h"

typedef struct MflPoolRec { unsigned short self, next, value, pad; } MflPoolRec;
typedef struct MflSlotPool { unsigned int n, m; MflPoolRec *recs; unsigned short free_head; } MflSlotPool;

/* Walks the ring that starts behind record `head` (the slot pool of rwp_capsule_005757e0) and appends every record value whose bit
   in `mask` is clear to `out`.  With `sort` set the result is bubble-sorted ascending.  Returns the number of values. */
/* TIER2: unvalidated 2026-11-10 authority=static */
unsigned int rwp_capsule_00575a30(MflSlotPool *pool, unsigned short head, unsigned short *out, unsigned int *mask, int sort)
{
  unsigned int n = 0, j, k;
  unsigned short i, v, t;

  for (i = pool->recs[head].next; i != head; i = pool->recs[i].next) {
    v = pool->recs[i].value;
    if ((mask[v >> 5] & 1 << (v & 31)) == 0)
      out[n++] = v;
  }
  if (sort != 0 && n >= 2) {
    for (k = n - 1; k >= 1; k--) {
      for (j = 0; j < k; j++) {
        if (out[j] > out[j + 1]) {
          t = out[j];
          out[j] = out[j + 1];
          out[j + 1] = t;
        }
      }
    }
  }
  return n;
}
