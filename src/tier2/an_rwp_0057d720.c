#include "mfl_tier2.h"
#include "rwp_common.h"

extern void rwp_batch_0057d640(unsigned *out, unsigned *m, int n);

/* Index tables of the batched solver (the object the pipeline builds per step).  Only the members this builder touches are named. */
typedef struct MflBatchTables {
  unsigned *rows;   /* 0x0: 24 word rows (see 57d640) */
  int *count4;   /* 0x4: per body: number of rows */
  int n_bodies_a;   /* 0x8 */
  int pad0c;
  int *row_local;   /* 0x10: per body: first row inside its group */
  int n_bodies_b;   /* 0x14 */
  int pad18;
  int *row_slot;   /* 0x1c: per row: position inside the group */
  int rows_built;   /* 0x20 */
  int pad24;
  int *row_block;   /* 0x28: per row: block (4 constraints) it came from */
  int n_rows;   /* 0x2c */
  int pad30;
  int *row_first;   /* 0x34: per body: first row */
  int n_bodies_c;   /* 0x38 */
  int pad3c;
  int *count40;   /* 0x40: per body: rows counted / filled */
  int n_bodies_d;   /* 0x44 */
  int pad48;
  int *group_rows;   /* 0x4c: per group: rows */
  int n_groups;   /* 0x50 */
  int pad54;
  int *group_lens;   /* 0x58: the group size array and its count / extra argument as passed in */
  int group_count;   /* 0x5c */
  int extra;   /* 0x60 */
  int *pair;   /* 0x64: per block: body a, body b */
  int n_blocks_a;   /* 0x68 */
  int pad6c;
  int *rel;   /* 0x70: per block: row of body a, body b relative to the group start */
  int n_blocks_b;   /* 0x74 */
  int pad78;
  int n_bodies_e;   /* 0x7c */
  int total_rows;   /* 0x80 */
  int n_blocks_c;   /* 0x84 */
} MflBatchTables;

/* rwp_batch_0057d720: builds the index tables of the batched solver from the blocks (0x100 bytes each, four constraints; the body
   indices of the pair are at +0xc and +0x2c, -1 = none).  Pass 1 counts the rows of every body, pass 2 gives each body its first row (running
   sum over everything and over the group), pass 3 walks the groups (ng, with the number of constraints of each in lens) and for every block
   gathers the rows of its two bodies (57d640) into the next free row of the body, recording block, slot and the row relative to the group. */
/* TIER2: unvalidated 2026-11-10 authority=static */
void rwp_batch_0057d720(MflBatchTables *t, char *recs, unsigned nrec, int n, int a4, int *lens, int ng, int a7, int *cnt, int ncnt, int a10)
{
  int i, a, b, e, g, run, grp, s, idx, tot, k;
  unsigned q;
  char *p;
  int *c;

  t->group_lens = cnt;
  t->group_count = ncnt;
  t->extra = a10;
  for (i = 0; i != n; i++) {
    t->count40[i] = 0;
    t->count4[i] = 0;
  }
  t->n_bodies_d = n;
  t->n_bodies_a = n;
  q = nrec >> 2;
  p = recs;
  for (g = 0; g != (int)q; g++) {
    a = *(int *)(p + 0xc);
    b = *(int *)(p + 0x2c);
    if (a != -1) {
      t->count40[a]++;
      t->count4[a]++;
    }
    if (b != -1) {
      t->count40[b]++;
      t->count4[b]++;
    }
    p += 0x100;
  }
  tot = 0;
  e = 0;
  if (ncnt != 0) {
    c = cnt;
    for (k = ncnt; k != 0; k--) {
      run = 0;
      if (*c != 0) {
        i = 0;
        do {
          t->row_first[e] = tot;
          t->row_local[e] = run;
          tot += t->count40[e];
          run += t->count40[e];
          e++;
          i++;
        } while (i != *c);
      }
      c++;
    }
  }
  t->n_blocks_c = q;
  t->n_bodies_b = n;
  t->total_rows = tot;
  t->n_bodies_e = n;
  t->n_bodies_c = n;
  for (i = 0; i != n; i++) t->count40[i] = 0;
  t->n_rows = t->total_rows;
  t->n_blocks_b = t->n_blocks_c;
  t->n_blocks_a = t->n_blocks_c;
  tot = 0;
  grp = 0;
  if (ng != 0) {
    for (g = 0; g != ng; g++) {
      t->group_rows[g] = 0;
      s = 0;
      if (((unsigned)lens[g] & 0xfffffffc) != 0) {
        p = recs + grp * 0x100;
        do {
          a = *(int *)(p + 0xc);
          b = *(int *)(p + 0x2c);
          t->pair[grp * 2] = a;
          t->pair[grp * 2 + 1] = b;
          if (a != -1) {
            idx = t->row_first[a] + t->count40[a];
            rwp_batch_0057d640((unsigned *)((char *)t->rows + idx * 0x60), (unsigned *)p, 0);
            t->rel[grp * 2] = idx - tot;
            t->count40[a]++;
            t->row_block[idx] = grp;
            t->row_slot[idx] = s;
            t->group_rows[g]++;
            t->rows_built++;
          }
          if (b != -1) {
            idx = t->row_first[b] + t->count40[b];
            rwp_batch_0057d640((unsigned *)((char *)t->rows + idx * 0x60), (unsigned *)p, 1);
            t->rel[grp * 2 + 1] = idx - tot;
            t->count40[b]++;
            t->row_block[idx] = grp;
            t->row_slot[idx] = s;
            t->group_rows[g]++;
            t->rows_built++;
          }
          grp++;
          p += 0x100;
          s++;
        } while (s != (int)((unsigned)lens[g] >> 2));
      }
      t->n_groups++;
      tot += t->group_rows[g];
    }
  }
}
