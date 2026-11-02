#include "mfl_tier2.h"
#include "rwp_common.h"

/* An element of the body list of a group: the body is at +0xc, the next element at +0x8. */
typedef struct MflGroupNode {
  unsigned char pad00[8];
  struct MflGroupNode *next;   /* 0x8 */
  void *body;   /* 0xc */
} MflGroupNode;

/* A group as the union-find pass (5785b0) leaves it: three intrusive lists with their counts, 0x28 bytes.  `n_a` bodies hang on `bodies`,
   `n_b` pairs on `pairs` (node + 8 is the pair record), `n_c` joints on `joints` (node - 0xd4 is the joint, whose chain link is at +0xdc). */
typedef struct MflGroupSrc {
  unsigned n_a;   /* 0x0 */
  MflGroupNode *bodies;   /* 0x4 */
  int pad08;
  unsigned n_b;   /* 0xc */
  void **pairs;   /* 0x10 */
  int pad14;
  unsigned n_c;   /* 0x18 */
  void **joints;   /* 0x1c */
  int pad20[2];
} MflGroupSrc;

typedef struct MflGroupSet {
  unsigned char pad00[4];
  unsigned count;   /* 0x4: number of groups */
  MflGroupSrc groups[1];   /* 0x8: the records start at +0xc (the first word is a header), see the offsets below */
} MflGroupSet;

/* The packed group record the island solver reads, 0x28 bytes. */
typedef struct MflIsland {
  void **bodies;   /* 0x0: body pointers of the group */
  char *joint_chain;   /* 0x4: first joint, chained through +0xdc */
  char **pairs;   /* 0x8: pair records */
  unsigned n_bodies;   /* 0xc */
  unsigned n_pairs;   /* 0x10 */
  unsigned weight;   /* 0x14: sum of the u16 at +0xc of the bodies */
  int pad18;   /* 0x18 */
  int pad1c[3];
} MflIsland;

typedef struct MflIslandOut {
  unsigned char pad00[4];
  unsigned count;   /* 0x4 */
  void **body_store;   /* 0x8 */
  char **pair_store;   /* 0xc */
  MflIsland *islands;   /* 0x10 */
} MflIslandOut;

extern void rwp_batch_005785b0(void *table, int *cells, int unused3, int unused4, int unused5, void **pairs, unsigned n_pairs, void *recs,
                               unsigned n_recs);

/* TIER2: unvalidated 2026-11-02 authority=diffemu */
void rwp_batch_00578310(MflIslandOut *out, void *set, int *marks, unsigned nmarks, void **list, unsigned nlist, void **pairs, unsigned npairs,
                        void *recs, unsigned nrecs)
{
  unsigned i, n, m, c, e, nb, nc, na, bodies_done, pairs_done, sum, idx;
  int off, ent;
  char *src, *p, *prev, *node;
  MflIsland *isl;
  MflGroupNode *g;
  void **q;
  unsigned short id;
  unsigned *bits;

  *(unsigned *)((char *)set + 4) = 0;
  for (i = 0; i < nmarks; i++) marks[i * 4] = 0;
  rwp_batch_005785b0(set, marks, nmarks, (int)list, nlist, pairs, npairs, recs, nrecs);
  n = *(unsigned *)((char *)set + 4);
  bodies_done = 0;
  pairs_done = 0;
  src = (char *)set + 0xc;
  off = 0;
  if (n != 0) {
    for (c = 0; c < n; c++) {
      na = *(unsigned *)(src + 0);
      nc = *(unsigned *)(src + 0xc);
      nb = *(unsigned *)(src + 0x18);
      isl = (MflIsland *)((char *)out->islands + off);
      isl->bodies = out->body_store + bodies_done;
      g = *(MflGroupNode **)(src + 4);
      sum = 0;
      for (m = 0; m < na; m++) {
        isl->bodies[m] = g->body;
        sum += *(unsigned short *)((char *)isl->bodies[m] + 0xc);
        g = g->next;
      }
      isl->n_bodies = na;
      bodies_done += na;
      isl->weight = sum;
      isl->pairs = out->pair_store + pairs_done;
      p = *(char **)(src + 0x10);
      for (m = 0; m < nc; m++) {
        isl->pairs[m] = p - 8;
        p = *(char **)p;
      }
      isl->n_pairs = nc;
      pairs_done += nc;
      isl->joint_chain = 0;
      prev = 0;
      p = *(char **)(src + 0x1c);
      for (m = 0; m < nb; m++) {
        if (m == 0) {
          prev = p - 0xd4;
          isl->joint_chain = prev;
        } else {
          node = p - 0xd4;
          *(char **)(prev + 0xdc) = node;
          prev = node;
        }
        p = *(char **)p;
      }
      if (prev != 0) *(char **)(prev + 0xdc) = 0;
      isl->pad18 = 0;
      src += 0x28;
      off += 0x28;
    }
  }
  out->count = n;
  idx = n;
  if (nlist != 0) {
    ent = n * 0x28;
    off = bodies_done * 4;
    for (i = 0; i < nlist; i++) {
      node = (char *)list[i];
      id = *(unsigned short *)(node + 0x20);
      if (*(int *)((char *)marks + id * 16) != 0) {
        *(int *)((char *)marks + id * 16) = 0;
      } else {
        bits = *(unsigned **)(*(char **)(node + 0x24) + 0x60);
        if (bits[id >> 5] & (1u << (id & 0x1f))) {
          isl = (MflIsland *)((char *)out->islands + ent);
          idx++;
          ent += 0x28;
          q = (void **)((char *)out->body_store + off);
          off += 4;
          isl->bodies = q;
          q[0] = node;
          isl->n_bodies = 1;
          isl->n_pairs = 0;
          isl->joint_chain = 0;
          isl->weight = *(unsigned short *)((char *)isl->bodies[0] + 0xc);
          isl->pad18 = 0;
        }
      }
    }
    out->count = idx;
  }
}
