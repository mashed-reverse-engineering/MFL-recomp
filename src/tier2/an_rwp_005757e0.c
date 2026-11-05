/* rwp_capsule_005757e0 (0x005757e0): carve a slot pool (header + n + 2m 8 byte records) from a bump allocator */
#include "mfl_tier2.h"

typedef struct MflPoolRec { unsigned short self, next; int pad; } MflPoolRec;
typedef struct MflSlotPool { unsigned int n, m; MflPoolRec *recs; unsigned short free_head; } MflSlotPool;
typedef struct MflBumpAlloc { int pad[2]; char *cur; } MflBumpAlloc;

/* The pool header sits at the allocator cursor, its records right behind it.  Records 0..n-1 point at themselves; records n.. form
   a free list (next = i + 1) that ends with 0xffff; the free head is record n. */
/* TIER2: unvalidated 2026-11-05 authority=diffemu */
MflSlotPool *rwp_capsule_005757e0(MflBumpAlloc *al, unsigned int n, unsigned int m)
{
  MflSlotPool *p;
  unsigned int i;

  p = (MflSlotPool *)al->cur;
  p->recs = (MflPoolRec *)(p + 1);
  al->cur = al->cur + (n + m * 2 + 2) * 8;
  p->n = n;
  p->m = m;
  for (i = 0; i < n; i++) {
    p->recs[i].self = (unsigned short)i;
    p->recs[i].next = (unsigned short)i;
  }
  for (i = n; i < n + m * 2 - 1; i++)
    p->recs[i].next = (unsigned short)(i + 1);
  p->recs[i].next = 0xffff;
  p->free_head = (unsigned short)n;
  return p;
}
