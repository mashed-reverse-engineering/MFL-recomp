#include "mfl_tier2.h"

/* The surface pair table (see rwp_batch_00579680): buckets, bucket count (a power of two) and the entry pool (40 bytes each, from 0xc). */
typedef struct MflPairEnt {
  struct MflMat *mat_a;   /* 0x0 */
  int id_a;   /* 0x4 */
  struct MflMat *mat_b;   /* 0x8 */
  int id_b;   /* 0xc */
  int pad10;
  float dir[3];   /* 0x14 */
  float value;   /* 0x20 */
  struct MflPairEnt *next;   /* 0x24 */
} MflPairEnt;
typedef struct MflPairHash {
  MflPairEnt **buckets;
  unsigned int size;   /* 0x4 */
  int pad8;
  MflPairEnt *pool;   /* 0xc */
} MflPairHash;
/* A surface material: a flag set pointer (through 0x24) and a 16 bit index at 0x20. */
typedef struct MflMatFlags { unsigned char pad00[0x60]; unsigned int *bits; } MflMatFlags;
typedef struct MflMat {
  unsigned char pad00[0x20];
  unsigned short idx;   /* 0x20 */
  unsigned char pad22[2];
  MflMatFlags *flags;   /* 0x24 */
} MflMat;
/* A contact object: normal, the two surface materials with their ids, a plane / direction pair at 0xb0 and the next object of the chain. */
typedef struct MflContactObj {
  float n[3];
  unsigned char pad0c[0xa4];
  float *planes;   /* 0xb0: [0..2] and [8..10] */
  MflMat *mat_a;   /* 0xb4 */
  unsigned short id_a;   /* 0xb8 */
  unsigned short pad_ba;
  MflMat *mat_b;   /* 0xbc */
  unsigned short id_b;   /* 0xc0 */
  unsigned short pad_c2;
  unsigned char padc4[0x18];
  struct MflContactObj *next;   /* 0xdc */
} MflContactObj;
typedef struct MflContactRec {
  int pad00;
  MflContactObj *obj;   /* 0x4 */
  unsigned char pad08[0x10];
  unsigned char flags;   /* 0x18 */
  unsigned char pad19[0xf];
} MflContactRec;
typedef struct MflContactSet {
  int pad00;
  unsigned int count;   /* 0x4 */
  unsigned char pad08[8];
  MflContactRec *recs;   /* 0x10 */
} MflContactSet;

/* Rebuilds the pair table from the contact set: clears the buckets, then for every unflagged contact record and every object of its chain
   fills the next pool entry (materials, ids, normal, and the signed distance along the object's plane, clamped at 0) and links it at the
   head of the bucket of the object's two surface ids (see rwp_batch_00579680 for the hash).  Returns the table. */
/* TIER2: unvalidated 2026-10-31 authority=static */
MflPairHash *rwp_batch_00579480(MflPairHash *h, MflContactSet *cs)
{
  unsigned int i, k, n, e1, e2, key;
  MflContactRec *rec;
  MflContactObj *o;
  MflPairEnt *e, **bucket;
  unsigned short di, si;
  double d;
  float v;

  for (i = 0; i < h->size; i++) h->buckets[i] = 0;
  n = 0;
  for (k = 0; k < cs->count; k++) {
    rec = &cs->recs[k];
    if (rec->flags & 3) continue;
    o = rec->obj;
    if (!o) continue;
    do {
      e = &h->pool[n];
      n++;
      di = o->id_a;
      if (di != 0xffff) e1 = o->mat_a->idx; else e1 = 0xffff;
      key = e1 * 32 + di;
      si = o->id_b;
      if (si != 0xffff) e2 = o->mat_b->idx; else e2 = 0xffff;
      key = key + (key * 4095) * 4 + e2 * 32 + si;
      key = key & (h->size - 1);
      bucket = &h->buckets[key];
      *(MflMat **)&e->mat_a = o->mat_a;
      e->id_a = *(int *)&o->id_a;
      e->mat_b = o->mat_b;
      e->id_b = *(int *)&o->id_b;
      *(int *)&e->dir[0] = *(int *)&o->n[0];
      *(int *)&e->dir[1] = *(int *)&o->n[1];
      *(int *)&e->dir[2] = *(int *)&o->n[2];
      if (o->id_a != 0xffff && o->mat_a->flags && (o->mat_a->flags->bits[o->mat_a->idx >> 5] & (1 << (o->mat_a->idx & 0x1f)))) {
        d = o->planes[1] * o->n[1];
        d = d + o->planes[0] * o->n[0];
        d = d + o->planes[2] * o->n[2];
        e->value = d;
      } else {
        d = o->planes[9] * o->n[1];
        d = d + o->planes[8] * o->n[0];
        d = d + o->planes[10] * o->n[2];
        e->value = -d;
      }
      if (e->value < 0.0f) *(int *)&e->value = 0;
      e->next = *bucket;
      *bucket = e;
      o = o->next;
    } while (o);
  }
  return h;
}
