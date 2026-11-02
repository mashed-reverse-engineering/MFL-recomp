/* rwp_pairs_emit_near_neighbours (0x0056af70): pair generator: emits (source, neighbour) body records for each listed object */
#include "mfl_tier2.h"

typedef struct MflPairScan {
  unsigned char pad00[4];
  unsigned int words;   /* 0x4: number of 32 bit words in the visited bitmap */
  unsigned int flags;   /* 0x8 */
  unsigned char pad0c[0x44];
  int *slot;   /* 0x50: id -> slot index */
  void *graph;   /* 0x54: adjacency table walked by 575a30 */
  unsigned int *bits_a;   /* 0x58 */
  unsigned int *bits_b;   /* 0x5c */
  unsigned char pad60[4];
  unsigned int *bits_c;   /* 0x64 */
  unsigned char pad68[8];
  unsigned int *visited;   /* 0x70 */
  unsigned char pad74[0xc];
  unsigned short *near_ids;   /* 0x80 */
} MflPairScan;

typedef struct MflPairRec {
  int slot_a;
  short kind_a;   /* 0x4 */
  short pad6;
  int slot_b;   /* 0x8 */
  short kind_b;   /* 0xc */
  short pad0e;
  int zero;   /* 0x10 */
} MflPairRec;

typedef struct MflPairSrc {
  unsigned char pad00[0x20];
  unsigned short id;   /* 0x20 */
} MflPairSrc;

extern int rwp_capsule_00575a30(void *graph, unsigned short id, unsigned short *out, unsigned int *visited, int sort);

/* Pair generator: for every source object in list[0..count) mark its id as visited, gather its neighbour ids (575a30 fills
   near_ids, skipping visited ones) and emit a record (slot of the source, slot of the neighbour, both kind 0x8000) for each
   neighbour that is not yet visited and passes the filter `mask`.  mask 0xf accepts everything; otherwise the neighbour's
   class is (bits_c set ? 1 : 2) << (bits_b set ? 0 : 2) and must be a bit of `mask`.  Stops after max_out records and returns
   the number written (0 for an empty mask). */
/* TIER2: unvalidated 2026-11-01 authority=diffemu */
int rwp_pairs_emit_near_neighbours(MflPairScan *s, MflPairSrc **list, unsigned int count, MflPairRec *out, unsigned int max_out, unsigned int mask)
{
  unsigned int i, j, k, n, cnt, bit;
  unsigned int nid, id;
  unsigned int sel;

  n = 0;
  if (mask == 0)
    return 0;
  for (i = 0; i < s->words; i++)
    s->visited[i] = 0;
  for (k = 0; k < count; k++) {
    id = list[k]->id;
    cnt = rwp_capsule_00575a30(s->graph, (unsigned short)id, s->near_ids, s->visited, s->flags & 1);
    s->visited[id >> 5] |= 1 << (id & 31);
    for (j = 0; j < cnt; j++) {
      nid = s->near_ids[j];
      bit = 1 << (nid & 31);
      if (s->visited[nid >> 5] & bit)
        continue;
      if (mask != 0xf) {
        sel = (s->bits_c[nid >> 5] & bit) ? 1 : 2;
        sel <<= (s->bits_b[nid >> 5] & bit) ? 0 : 2;
        if (!(mask & sel))
          continue;
      }
      out[n].slot_a = s->slot[id];
      out[n].kind_a = (short)0x8000;
      out[n].slot_b = s->slot[nid];
      out[n].kind_b = (short)0x8000;
      out[n].zero = 0;
      n++;
      if (n == max_out)
        return n;
    }
  }
  return n;
}
