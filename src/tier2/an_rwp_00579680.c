#include "mfl_tier2.h"

/* An entry of the surface pair table: the two ids, a direction (0x14), a result value (0x20) and the next entry of the bucket. */
typedef struct MflPairEnt {
  int pad00;
  unsigned short id_a;   /* 0x4 */
  unsigned short pad06[3];
  unsigned short id_b;   /* 0xc */
  unsigned short pad0e[3];
  float dir[3];   /* 0x14 */
  int value;   /* 0x20 */
  struct MflPairEnt *next;   /* 0x24 */
} MflPairEnt;
/* The hash table: buckets and the bucket count (a power of two). */
typedef struct MflPairHash {
  MflPairEnt **buckets;
  unsigned int size;   /* 0x4 */
} MflPairHash;
typedef struct MflMat {
  unsigned char pad00[0x20];
  unsigned short id;   /* 0x20 */
} MflMat;
/* A contact object: normal, the two surface materials with their ids, the chosen pair value and the next object of the chain. */
typedef struct MflContactObj {
  float n[3];
  unsigned char pad0c[0xa8];
  MflMat *mat_a;   /* 0xb4 */
  unsigned short id_a;   /* 0xb8 */
  unsigned short pad_ba;
  MflMat *mat_b;   /* 0xbc */
  unsigned short id_b;   /* 0xc0 */
  unsigned short pad_c2;
  unsigned char padc4[0xc];
  int value;   /* 0xd0 */
  unsigned char padd4[8];
  struct MflContactObj *next;   /* 0xdc */
} MflContactObj;
/* A contact record of 0x28 bytes: flags at 0x18, the contact object chain at 4. */
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

/* For every contact record that is not flagged (flags & 3) and every object of its chain: finds, among the pair table entries of the
   hash bucket of the object's two surface ids (in either order), the one whose direction has the largest projection on the object's
   normal and stores its value in the object (0 when none). */
/* TIER2: unvalidated 2026-10-31 authority=diffemu */
MflPairHash *rwp_batch_00579680(MflPairHash *h, MflContactSet *cs)
{
  unsigned int i, e1, e2, k;
  MflContactRec *rec;
  MflContactObj *o;
  MflPairEnt *e;
  unsigned short di, si;
  float best[2];
  double d;

  for (i = 0; i < cs->count; i++) {
    rec = &cs->recs[i];
    if (rec->flags & 3) continue;
    o = rec->obj;
    if (!o) continue;
    do {
      di = o->id_a;
      best[0] = -3.4028235e38f;
      o->value = 0;
      if (di != 0xffff) e1 = o->mat_a->id; else e1 = 0xffff;
      si = o->id_b;
      k = e1 * 32 + di;
      if (si != 0xffff) e2 = o->mat_b->id; else e2 = 0xffff;
      k = k + (k * 4095) * 4 + e2 * 32 + si;
      k = k & (h->size - 1);
      for (e = h->buckets[k]; e; e = e->next) {
        if ((e->id_a == di && e->id_b == si) || (e->id_a == si && e->id_b == di)) {
          d = e->dir[1] * o->n[1];
          d = d + e->dir[0] * o->n[0];
          d = d + e->dir[2] * o->n[2];
          if (d > best[0]) {
            best[0] = d;
            o->value = e->value;
          }
        }
      }
      o = o->next;
    } while (o);
  }
  return h;
}
