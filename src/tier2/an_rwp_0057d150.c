#include "mfl_tier2.h"

/* Half of a body record: three floats, a pad word, three floats, a pad word (32 bytes). */
typedef union MflHalf {
  float f[8];
  int w[8];   /* the same bits as integers (word copies of the slot) */
} MflHalf;
/* A 64 byte record: two halves. */
typedef struct MflPair64 {
  MflHalf lo;
  MflHalf hi;
} MflPair64;
typedef struct MflPtrTable {
  MflPair64 **items;
  unsigned int count;   /* 0x4 */
} MflPtrTable;
typedef struct MflPairTable {
  MflPair64 *items;
  unsigned int count;   /* 0x4 */
} MflPairTable;
typedef struct MflHalfTable {
  MflHalf *items;
  unsigned int count;   /* 0x4 */
} MflHalfTable;
typedef struct MflSlotMap {
  int *index;   /* slot of body j */
  unsigned int count;   /* 0x4 */
  int pad8;
} MflSlotMap;
/* A joint row (0x100 bytes): body A index at 0x1c when the word at 0xc is not -1, body B likewise at 0x3c / 0x2c. */
typedef struct MflJointRow {
  int pad00[3];
  int has_a;   /* 0xc */
  int pad10[3];
  int body_a;   /* 0x1c */
  int pad20[3];
  int has_b;   /* 0x2c */
  int pad30[3];
  int body_b;   /* 0x3c */
  unsigned char pad40[0xc0];
} MflJointRow;
/* Pair of body indices (-1 = none) of a joint. */
typedef struct MflBodyPair {
  int a;
  int b;
} MflBodyPair;

extern void rwp_batch_0057cf90(MflPair64 *rec, MflJointRow *row, float *w, int i);

/* Rebuilds the per-slot accumulators of a joint solve step: clears the first n body records and matrices, lets 57cf90 add the impulse of every
   joint row to the records of its (up to) two bodies, copies the n3 source slots into acc and clears the same number of entries of out, and finally
   adds the halves of the body records and matrices of every body to the slot accumulators acc / out. */
/* TIER2: unvalidated 2026-10-31 authority=diffemu */
void rwp_batch_0057d150(MflHalf *acc, int a2, MflHalfTable *out, MflPtrTable *bodies, MflPairTable *mats, MflHalf *src,
                        MflSlotMap *slots, MflJointRow *rows, int rows_bytes, int a10, int a11, MflBodyPair *pairs, int n,
                        int a14, float *vec_a, int a16, int a17, float *vec_b)
{
  int i, j, k, t;
  unsigned int n2, n3;
  MflHalf loc;
  MflHalf *d;
  int ia, ib;

  for (i = 0; i != n; i++) {
    bodies->items[i]->lo.f[0] = 0.0f;
    bodies->items[i]->lo.f[1] = 0.0f;
    bodies->items[i]->lo.f[2] = 0.0f;
    bodies->items[i]->lo.f[4] = 0.0f;
    bodies->items[i]->lo.f[5] = 0.0f;
    bodies->items[i]->lo.f[6] = 0.0f;
    bodies->items[i]->hi.f[0] = 0.0f;
    bodies->items[i]->hi.f[1] = 0.0f;
    bodies->items[i]->hi.f[2] = 0.0f;
    bodies->items[i]->hi.f[4] = 0.0f;
    bodies->items[i]->hi.f[5] = 0.0f;
    bodies->items[i]->hi.f[6] = 0.0f;
    mats->items[i].lo.f[0] = 0.0f;
    mats->items[i].lo.f[1] = 0.0f;
    mats->items[i].lo.f[2] = 0.0f;
    mats->items[i].lo.f[4] = 0.0f;
    mats->items[i].lo.f[5] = 0.0f;
    mats->items[i].lo.f[6] = 0.0f;
    mats->items[i].hi.f[0] = 0.0f;
    mats->items[i].hi.f[1] = 0.0f;
    mats->items[i].hi.f[2] = 0.0f;
    mats->items[i].hi.f[4] = 0.0f;
    mats->items[i].hi.f[5] = 0.0f;
    mats->items[i].hi.f[6] = 0.0f;
  }
  bodies->count = n;
  mats->count = n;
  n2 = (unsigned int)rows_bytes >> 2;
  if (n2) {
    for (k = 0; k != (int)n2; k++) {
      ia = rows[k].has_a;
      ib = rows[k].has_b;
      if (ia != -1) {
        rwp_batch_0057cf90(bodies->items[rows[k].body_a], &rows[k], vec_a + 4 * k, 0);
        rwp_batch_0057cf90(mats->items + rows[k].body_a, &rows[k], vec_b + 4 * k, 0);
      }
      if (ib != -1) {
        rwp_batch_0057cf90(bodies->items[rows[k].body_b], &rows[k], vec_a + 4 * k, 1);
        rwp_batch_0057cf90(mats->items + rows[k].body_b, &rows[k], vec_b + 4 * k, 1);
      }
    }
  }
  n3 = slots->count;
  if (n3) {
    j = 0;
    do {
      t = slots->index[j];
      acc[t].w[0] = src[t].w[0];
      acc[t].w[1] = src[t].w[1];
      acc[t].w[3] = 0;
      acc[t].w[2] = src[t].w[2];
      acc[t].w[4] = src[t].w[4];
      acc[t].w[5] = src[t].w[5];
      acc[t].w[7] = 0;
      acc[t].w[6] = src[t].w[6];
      out->items[j].w[0] = 0;
      out->items[j].w[1] = 0;
      out->items[j].w[2] = 0;
      out->items[j].w[3] = 0;
      out->items[j].w[4] = 0;
      out->items[j].w[5] = 0;
      out->items[j].w[6] = 0;
      out->items[j].w[7] = 0;
      j++;
    } while (j != (int)n3);
  }
  out->count = n3;
  if (n) {
    for (i = 0; i != n; i++) {
      int p0 = pairs[i].a, p1 = pairs[i].b;
      if (p0 != -1) {
        loc = bodies->items[i]->lo;
        d = acc + slots->index[p0];
        d->f[0] = loc.f[0] + d->f[0]; d->f[1] = loc.f[1] + d->f[1]; d->f[2] = loc.f[2] + d->f[2];
        d->f[4] = loc.f[4] + d->f[4]; d->f[5] = loc.f[5] + d->f[5]; d->f[6] = loc.f[6] + d->f[6];
        loc = mats->items[i].lo;
        d = out->items + p0;
        d->f[0] = loc.f[0] + d->f[0]; d->f[1] = loc.f[1] + d->f[1]; d->f[2] = loc.f[2] + d->f[2];
        d->f[4] = loc.f[4] + d->f[4]; d->f[5] = loc.f[5] + d->f[5]; d->f[6] = loc.f[6] + d->f[6];
      }
      if (p1 != -1) {
        loc = bodies->items[i]->hi;
        d = acc + slots->index[p1];
        d->f[0] = loc.f[0] + d->f[0]; d->f[1] = loc.f[1] + d->f[1]; d->f[2] = loc.f[2] + d->f[2];
        d->f[4] = loc.f[4] + d->f[4]; d->f[5] = loc.f[5] + d->f[5]; d->f[6] = loc.f[6] + d->f[6];
        loc = mats->items[i].hi;
        d = out->items + p1;
        d->f[0] = loc.f[0] + d->f[0]; d->f[1] = loc.f[1] + d->f[1]; d->f[2] = loc.f[2] + d->f[2];
        d->f[4] = loc.f[4] + d->f[4]; d->f[5] = loc.f[5] + d->f[5]; d->f[6] = loc.f[6] + d->f[6];
      }
    }
  }
}
