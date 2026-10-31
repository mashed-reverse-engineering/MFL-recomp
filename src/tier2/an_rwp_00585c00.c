#include "mfl_tier2.h"

struct MflContactLink;
/* A body object: flags at 0x8 (0x20 = skipped, 0x40 = collects contact links) and the head of its contact link list at 0x2c. */
typedef struct MflBodyObj {
  unsigned char pad00[8];
  unsigned int flags;   /* 0x8 */
  unsigned char pad0c[0x20];
  struct MflContactLink *links;   /* 0x2c */
} MflBodyObj;
/* A contact link (20 bytes). */
typedef struct MflContactLink {
  MflBodyObj *obj;
  MflBodyObj *other;   /* 0x4 */
  unsigned int other_id;   /* 0x8 */
  void *hit;   /* 0xc */
  struct MflContactLink *next;   /* 0x10 */
} MflContactLink;
/* A shape pair record (0x120 bytes, see 585950): the two shapes' objects and ids at 0x58 / 0x5c and 0xd8 / 0xdc. */
typedef struct MflPairRec {
  unsigned char pad00[0x58];
  MflBodyObj *obj_a;   /* 0x58 */
  unsigned int id_a;   /* 0x5c: low 16 bits, 0xffff = no object */
  unsigned char pad60[0x78];
  MflBodyObj *obj_b;   /* 0xd8 */
  unsigned int id_b;   /* 0xdc */
  unsigned char pade0[0x40];
} MflPairRec;
typedef struct MflPairCtx {
  unsigned char pad00[0x64];
  MflContactLink *links;   /* 0x64 */
  unsigned int nlinks;   /* 0x68 */
  unsigned int cap;   /* 0x6c */
  float limit;   /* 0x70 */
  int v74;   /* 0x74 */
  int pad78;
  int v7c;   /* 0x7c */
} MflPairCtx;
typedef struct MflHitSlot {
  unsigned char b[0xe0];
} MflHitSlot;

extern int rwp_aggregate_00585950(MflPairRec *p, float limit, MflHitSlot *out);
extern int rwp_generalprimitive_00589690(MflHitSlot *hits1, unsigned int n, int v7c, MflHitSlot *hits, unsigned int cap, int v74);

/* Contact generation for n shape pair records: each pair is run through 585950 into the next hit slot (unless one of the objects has flag
   0x20; then only the link bookkeeping below happens, with no hit), the hits are reduced by 589690 and, for every object with flag 0x40, a link
   (this object, the other one, the other id, the hit) is pushed on the object's link list (and the context's link array, while it has room).
   Returns the number of hits that survived the reduction. */
/* TIER2: unvalidated 2026-10-31 authority=diffemu */
int rwp_aggregate_00585c00(MflPairCtx *ctx, MflPairRec *pairs, unsigned int n, MflHitSlot *hits, unsigned int cap)
{
  unsigned int i, found, k, nk;
  unsigned int fa, fb, ia, ib;
  MflPairRec *p;
  MflHitSlot *out, *hit;
  MflContactLink *l;
  MflBodyObj *o;
  int kept;

  if (!n) return 0;
  p = pairs;
  ia = p->id_a;
  ib = p->id_b;
  fa = (unsigned short)ia != 0xffff ? p->obj_a->flags : 0;
  fb = (unsigned short)ib != 0xffff ? p->obj_b->flags : 0;
  if ((fa & 0x20) || (fb & 0x20)) {
    if (fa & 0x40) {
      if (ctx->nlinks < ctx->cap) {
        o = p->obj_a;
        l = ctx->links + ctx->nlinks++;
        l->obj = o;
        l->other = p->obj_b;
        l->other_id = p->id_b;
        l->hit = 0;
        l->next = o->links;
        o->links = l;
      }
    }
    if (fb & 0x40) {
      if (ctx->nlinks < ctx->cap) {
        o = p->obj_b;
        l = ctx->links + ctx->nlinks++;
        l->obj = o;
        l->other = p->obj_a;
        l->other_id = p->id_a;
        l->hit = 0;
        l->next = o->links;
        o->links = l;
      }
    }
    return 0;
  }
  out = hits + 1;
  found = 0;
  for (i = 0; i < n; i++, p++) {
    if (found >= cap - 1) break;
    if (rwp_aggregate_00585950(p, ctx->limit, out)) {
      found++;
      out++;
    }
  }
  kept = 0;
  if (found) kept = rwp_generalprimitive_00589690(hits + 1, found, ctx->v7c, hits, cap, ctx->v74);
  if (!((fa | fb) & 0x40) || !(unsigned int)kept) return kept;
  p = pairs;
  hit = hits;
  nk = kept;
  do {
    if (fa & 0x40) {
      if (ctx->nlinks < ctx->cap) {
        o = p->obj_a;
        l = ctx->links + ctx->nlinks++;
        l->obj = o;
        l->other = p->obj_b;
        l->other_id = p->id_b;
        l->hit = hit;
        l->next = o->links;
        o->links = l;
      }
    }
    if (fb & 0x40) {
      if (ctx->nlinks < ctx->cap) {
        o = p->obj_b;
        l = ctx->links + ctx->nlinks++;
        l->obj = o;
        l->other = p->obj_a;
        l->other_id = p->id_a;
        l->hit = hit;
        l->next = o->links;
        o->links = l;
      }
    }
    hit++;
  } while (--nk);
  return kept;
}
