#include "mfl_tier2.h"
#include "rwp_common.h"

/* A shape pair record (0x120 bytes, see 585950): the two shapes' objects and ids at 0x58 / 0x5c and 0xd8 / 0xdc. */
typedef struct MflPairRec {
  unsigned char pad00[0x58];
  void *obj_a;   /* 0x58 */
  unsigned int id_a;   /* 0x5c: low 16 bits, 0xffff = no object */
  unsigned char pad60[0x78];
  void *obj_b;   /* 0xd8 */
  unsigned int id_b;   /* 0xdc */
  unsigned char pade0[0x40];
} MflPairRec;
/* One handle of the broadphase list (20 bytes): the object (or child list), a 16 bit id at 4 (0xffff = single object) for each of the two
   sides (0x0 / 0x4 and 0x8 / 0xc), and a pair filter at 0x10. */
typedef struct MflPairHandle {
  void *a;   /* 0x0 */
  unsigned short a_id;   /* 0x4 */
  unsigned short pad06;
  void *b;   /* 0x8 */
  unsigned short b_id;   /* 0xc */
  unsigned short pad0e;
  void *filter;   /* 0x10 */
} MflPairHandle;
/* The narrow phase state (lives at +0x37c of the pipeline object): pair collector output (0x0 / 0x4), the pair record buffer (0x2c / 0x30),
   the tolerance (0x70) and the 'collector full' flag (0x78). */
typedef struct MflNarrow {
  unsigned char **pairs;   /* 0x0: item pointer pairs written by 586ce0 */
  unsigned count;   /* 0x4 */
  unsigned char pad08[8];
  int f10;   /* 0x10 */
  unsigned char pad14[8];
  int f1c;   /* 0x1c */
  unsigned char pad20[0xc];
  MflPairRec *recs;   /* 0x2c */
  unsigned nrecs;   /* 0x30 */
  unsigned char pad34[8];
  int pad3c;
  unsigned char pad40[0x1c];
  int f5c;   /* 0x5c */
  unsigned char pad60[0x10];
  float tol;   /* 0x70 */
  float v74;   /* 0x74 */
  int full;   /* 0x78 */
} MflNarrow;

extern void rwp_sphere_0056c350(void *obj, int a, int b, float *box);
extern void rwp_volume_0056b180(void *owner, void *shape, float *box);
extern int rwp_aggregate_00586300(void *ctx, void *h, void *src, float *one, float *ref, void *extra);
extern unsigned rwp_aggregate_00586ce0(void *ctx, unsigned split, unsigned n_inner, void *f_id, void *f_obj);
extern int rwp_aggregate_00585f20(void *cs, void *query, void *pair, float limit);
extern int rwp_aggregate_00585c00(void *ctx, MflPairRec *pairs, unsigned n, void *hits, unsigned cap);

/* TIER2: unvalidated 2026-11-10 authority=static */
unsigned rwp_batch_00579040(char *ctx, MflPairHandle *handles, unsigned nhandles, char *hits, unsigned cap)
{
  MflNarrow *nw;
  char *w;
  void *filter_obj;
  float box_a[8], box_b[8];
  unsigned total, j, j0, n, k, b, run, rem, got, r1, r2;
  MflPairHandle *h;
  MflPairRec *rec, *r;
  unsigned short ca, cb;

  total = 0;
  if (nhandles == 0 || cap == 0) return 0;
  nw = (MflNarrow *)(ctx + 0x37c);
  w = **(char ***)(ctx + 0x70);
  nw->tol = *(float *)(*(char **)w + 0xc014);
  nw->v74 = *(float *)(w + 0xc);
  filter_obj = *(void **)(*(char **)w + 0xc018);
  j = 0;
  while (j < nhandles) {
    if (total >= cap) break;
    nw->nrecs = 0;
    nw->pad3c = 0;
    nw->full = 0;
    j0 = j;
    h = handles + j;
    while (j < nhandles && nw->full == 0) {
      if (h->a_id == 0xffff)
        rwp_sphere_0056c350(h->a, 0, 0, box_a);
      else
        rwp_volume_0056b180(w, h->a, box_a);
      if (h->b_id == 0xffff)
        rwp_sphere_0056c350(h->b, 0, 0, box_b);
      else
        rwp_volume_0056b180(w, h->b, box_b);
      nw->count = 0;
      nw->f1c = 0;
      nw->f10 = 0;
      nw->f5c = 0;
      r1 = rwp_aggregate_00586300(nw, h, 0, box_a, box_b, &h->b);
      if (r1 != 0) {
        r2 = rwp_aggregate_00586300(nw, &h->b, 0, box_b, box_a, h);
        if (r2 != 0) {
          n = rwp_aggregate_00586ce0(nw, r1, r2, h->filter, filter_obj);
          if (nw->full != 0 && j > j0) break;
          for (b = 0; b < n; b++) rwp_aggregate_00585f20(nw, ctx, &nw->pairs[b * 2], nw->tol);
        }
      }
      j++;
      h++;
    }
    k = 0;
    rec = nw->recs;
    if (nw->nrecs != 0) {
      while (total < cap) {
        run = 1;
        rem = nw->nrecs - k;
        if (rem > 1) {
          r = rec + 1;
          while (1) {
            ca = (unsigned short)rec->id_a;
            if (ca == 0xffff) {
              if ((unsigned short)r->id_a != ca) {
                if (rec->obj_a != r->obj_a || ca != (unsigned short)r->id_a) break;
              }
            } else if (rec->obj_a != r->obj_a || ca != (unsigned short)r->id_a)
              break;
            cb = (unsigned short)rec->id_b;
            if (cb == 0xffff) {
              if ((unsigned short)r->id_b != cb) {
                if (rec->obj_b != r->obj_b || cb != (unsigned short)r->id_b) break;
              }
            } else if (rec->obj_b != r->obj_b || cb != (unsigned short)r->id_b)
              break;
            run++;
            r++;
            if (run >= rem) break;
          }
        }
        got = rwp_aggregate_00585c00(nw, rec, run, hits + total * 0xe0, cap - total);
        total += got;
        k += run;
        rec += run;
        if (k >= nw->nrecs) break;
      }
    }
  }
  return total;
}
