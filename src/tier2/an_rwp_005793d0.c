/* rwp_batch_005793d0 (0x005793d0): carve a pair hash table out of a work area */
#include "mfl_tier2.h"

extern int rwp_capsule_00575af0(void *p, int v, int n);   /* memset */

/* A bump allocator: the next free address at +8. */
typedef struct MflArena {
  int pad00[2];
  unsigned int cur;   /* 0x8 */
} MflArena;

/* The table: bucket heads, bucket count (a power of two), entry capacity and the 40 byte entries. */
typedef struct MflPairHash {
  unsigned int *buckets;
  unsigned int nbuckets;   /* 0x4 */
  unsigned int capacity;   /* 0x8 */
  unsigned int entries;   /* 0xc */
} MflPairHash;

#define ALIGN16(x) (((x) + 15) & ~15u)

/* Allocates the table header, `capacity` zeroed 40 byte entries and the largest power of two <= capacity (at least 1) of empty buckets,
   each piece 16 byte aligned.  Returns the table. */
/* TIER2: unvalidated 2026-11-05 authority=diffemu */
MflPairHash *rwp_batch_005793d0(MflArena *ar, unsigned int capacity)
{
  MflPairHash *t;
  unsigned int n, i;

  t = (MflPairHash *)ALIGN16(ar->cur);
  ar->cur = ALIGN16((unsigned int)t + sizeof(MflPairHash));
  t->entries = ar->cur;
  rwp_capsule_00575af0((void *)ar->cur, 0, capacity * 40);
  ar->cur = ALIGN16(ar->cur + capacity * 40);
  t->capacity = capacity;
  t->nbuckets = 1;
  for (n = capacity >> 1; n; n >>= 1)
    t->nbuckets += t->nbuckets;
  ar->cur = ALIGN16(ar->cur);
  t->buckets = (unsigned int *)ar->cur;
  rwp_capsule_00575af0((void *)ar->cur, 0, t->nbuckets * 4);
  ar->cur = ALIGN16(ar->cur + t->nbuckets * 4);
  for (i = 0; i < t->nbuckets; i++)
    t->buckets[i] = 0;
  return t;
}
