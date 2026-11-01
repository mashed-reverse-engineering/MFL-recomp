#include "mfl_tier2.h"

/* A (value, pointer) pair of the row tables; the copy handed to the solver points every pair at the one output window. */
typedef struct MflRowPair {
  int a;
  void *p;   /* 0x4 */
} MflRowPair;

typedef struct MflRowSet {
  MflRowPair r0;   /* 0x0 */
  MflRowPair r1;   /* 0x8 */
  MflRowPair r2;   /* 0x10 */
  MflRowPair r3;   /* 0x18 */
  MflRowPair r4;   /* 0x20 */
  MflRowPair r5;   /* 0x28 */
  MflRowPair r6;   /* 0x30 */
  int pad38[2];
  MflRowPair r8;   /* 0x40 */
  MflRowPair r9;   /* 0x48 */
  MflRowPair r10;   /* 0x50 */
} MflRowSet;

typedef struct MflRowTables {
  MflRowSet set;
  int *base;   /* 0x58: row storage */
  unsigned int used;   /* 0x5c: rows used */
  unsigned int limit;   /* 0x60: rows available */
} MflRowTables;

/* The output window of one solver run: where to write, how many rows were produced, how many fit. */
typedef struct MflRowWin {
  int *ptr;
  int rows;   /* 0x4: out */
  int count;   /* 0x8 */
} MflRowWin;

typedef struct MflRowBatch {
  int c4;   /* 0x0 */
  unsigned int n;   /* 0x4: groups in the batch */
  int pad8[2];
  void *groups;   /* 0x10 */
} MflRowBatch;

typedef struct MflRowOwner {
  unsigned char pad00[0xc];
  int v0c;   /* 0xc */
  MflRowTables *tables;   /* 0x10 */
} MflRowOwner;

typedef struct MflRowGroup {
  unsigned char pad00[0x1c];
  unsigned int nrows;   /* 0x1c */
  unsigned char pad20[8];
} MflRowGroup;

typedef struct MflRowWorld {
  unsigned char pad00[0x48];
  unsigned int cap;   /* 0x48: rows per batch */
  unsigned char pad4c[0x24];
  MflRowOwner **owner;   /* 0x70 */
  unsigned char pad74[0x28];
  unsigned char b9c[0xc];   /* 0x9c */
  unsigned char ba8[0x1c];   /* 0xa8 */
  int pad_c4;
  unsigned int ngroups;   /* 0xc8 */
  unsigned char padcc[8];
  MflRowGroup *groups;   /* 0xd4 */
  unsigned char padd8[0x1c];
  int fnf4;   /* 0xf4 */
  int fnf8;   /* 0xf8 */
  unsigned char bfc[0x6c];   /* 0xfc */
  unsigned char b168[0x15c];   /* 0x168 */
  unsigned char b2c4[0xa0];   /* 0x2c4 */
  unsigned char b364[0xa0];   /* 0x364 */
  int fn404;   /* 0x404 */
  unsigned char b408[4];   /* 0x408 */
} MflRowWorld;

extern int rwp_pipeline_00571c60(MflRowSet *rs, void *a1, void *a2, void *a3, void *a4, void *a5, void *a6, MflRowBatch *b, MflRowWorld *w,
                                 int a9, int a10, int a11, int a12, void *a13);

/* Runs the solver row builder (571c60) over the groups in batches whose row counts (group +0x1c) add up to at most w->cap; each run writes into
   the row storage after the rows already used and advances `used`. Returns w, or 0 when a single group is already above the cap. */
/* TIER2: unvalidated 2026-11-02 authority=diffemu */
MflRowWorld *rwp_pipeline_00572a40(MflRowWorld *w)
{
  MflRowTables *t;
  struct { MflRowSet rs; MflRowWin win; MflRowBatch b; } f;      /* one block: the original keeps the three in this order in its frame (the solver gets pointers into it) */
  unsigned int done = 0, n, batch, acc, j;

  t = (*w->owner)->tables;
  t->used = 0;
  f.rs.r0 = t->set.r0;
  f.rs.r0.p = &f.win;
  f.rs.r1 = t->set.r1;
  f.rs.r1.p = &f.win;
  f.rs.r2 = t->set.r2;
  f.rs.r2.p = &f.win;
  f.rs.r8 = t->set.r8;
  f.rs.r8.p = &f.win;
  f.rs.r10 = t->set.r10;
  f.rs.r10.p = &f.win;
  f.rs.r3 = t->set.r3;
  f.rs.r3.p = &f.win;
  f.rs.r4 = t->set.r4;
  f.rs.r4.p = &f.win;
  f.rs.r5 = t->set.r5;
  f.rs.r5.p = &f.win;
  f.rs.r6 = t->set.r6;
  f.rs.r6.p = &f.win;
  f.rs.r9 = t->set.r9;
  f.rs.r9.p = &f.win;
  n = w->ngroups;
  if (n == 0) return w;
  do {
    batch = 0;
    acc = 0;
    if (done == n) return 0;
    for (j = done; j != n; j++) {
      acc += w->groups[j].nrows;
      if (acc > w->cap) break;
      batch++;
    }
    if (batch == 0) return 0;
    f.b.groups = &w->groups[done];
    f.b.c4 = w->pad_c4;
    f.b.n = batch;
    f.win.ptr = t->base + t->used;
    f.win.count = t->limit - t->used;
    rwp_pipeline_00571c60(&f.rs, w->b9c, w->b168, w->bfc, w->b2c4, w->b364, w->ba8, &f.b, w, (*w->owner)->v0c, w->fnf4, w->fnf8, w->fn404, w->b408);
    t->used += f.win.rows;
    done += batch;
  } while (done != n);
  return w;
}
