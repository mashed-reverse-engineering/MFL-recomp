#include "mfl_tier2.h"

/* A 4x4 matrix record (64 bytes); flags at 0xc as in a RenderWare matrix. */
typedef struct MflAggMat {
  float right[3];
  unsigned int flags;   /* 0xc */
  float up[3];
  unsigned int pad1;
  float at[3];
  unsigned int pad2;
  float pos[3];
  unsigned int pad3;
} MflAggMat;
/* Bounding box record (32 bytes): min at 0, max at 0x10. */
typedef struct MflAggBox {
  float min[3];
  float pad0;
  float max[3];
  float pad1;
} MflAggBox;
/* Handle of an aggregate: the object (or child list) and a 16 bit id (0xffff = single object). */
typedef struct MflAggHandle {
  void *ptr;
  unsigned short id;   /* 0x4 */
  unsigned short pad;
} MflAggHandle;
/* Type data of an object: bounds callback at 0x10, flags at 0x40 (bit 0 = has children, bit 1 = matrix is the entry matrix). */
typedef struct MflAggData {
  unsigned char pad00[0x10];
  void (*bounds)(void *obj, int zero, MflAggMat *mat, MflAggBox *box);   /* 0x10 */
  unsigned char pad14[0x2c];
  unsigned char flags;   /* 0x40 */
} MflAggData;
typedef struct MflAggObj {
  MflAggMat mat;
  unsigned char pad40[0x1c];
  MflAggData *data;   /* 0x5c */
} MflAggObj;
/* Entry of a child list (12 bytes). */
typedef struct MflAggEntry {
  MflAggObj *obj;
  int pad4;
  MflAggMat *mat;   /* 0x8 */
} MflAggEntry;
typedef struct MflAggList {
  unsigned char pad00[0xc];
  unsigned short count;   /* 0xc */
  unsigned short pad0e;
  MflAggEntry *entries;   /* 0x10 */
} MflAggList;
/* Output record (20 bytes). */
typedef struct MflAggRec {
  void *owner;
  unsigned short idx;   /* 0x4 */
  unsigned short idx_hi;
  MflAggObj *obj;   /* 0x8 */
  int zero;   /* 0xc */
  MflAggMat *mat;   /* 0x10 */
} MflAggRec;
typedef struct MflAggCtx {
  unsigned char pad00[0xc];
  MflAggRec *recs;   /* 0xc */
  int nrec;   /* 0x10 */
  int caprec;   /* 0x14 */
  MflAggMat *mats;   /* 0x18 */
  int nmat;   /* 0x1c */
  int capmat;   /* 0x20 */
  MflAggBox *boxes;   /* 0x24, parallel to recs */
  unsigned char pad28[0x48];
  float tol;   /* 0x70 */
  unsigned char pad74[4];
  int overflow;   /* 0x78 */
} MflAggCtx;

extern void rwp_aggregate_00586680(MflAggCtx *ctx, MflAggHandle *h, int id, void *obj, MflAggMat *mat, int zero,
                                   MflAggRec *rec, MflAggBox *src, MflAggBox *ref, void *extra);

/* Collect the objects of an aggregate (a single object, or a child list) into the context's record / matrix / box tables:
   each child matrix is composed with the entry matrix, its bounds (the given src boxes, the box ref when there is a single child,
   or the type's bounds callback) are tested against ref within the tolerance, and children with bit 0 set recurse into 586680.
   Returns the number of records added. */
/* TIER2: unvalidated 2026-10-31 authority=diffemu */
int rwp_aggregate_00586300(MflAggCtx *ctx, MflAggHandle *h, MflAggBox *src, MflAggBox *one, MflAggBox *ref, void *extra)
{
  int start = ctx->nrec;
  MflAggRec *rec = ctx->recs + ctx->nrec;
  MflAggRec *rec_lim = ctx->recs + ctx->caprec;
  MflAggMat *mat = ctx->mats + ctx->nmat;
  MflAggMat *mat_lim = ctx->mats + ctx->capmat;
  MflAggBox *box = ctx->boxes + ctx->nrec;
  float tol = ctx->tol;
  MflAggList *list;
  MflAggEntry *e;
  MflAggObj *obj;
  MflAggData *data;
  unsigned int n, i;

  if (h->id == 0xffff) {
    obj = (MflAggObj *)h->ptr;
    if (obj->data->flags & 2) {
      mat->right[0] = 1.0f; mat->right[1] = 0.0f; mat->right[2] = 0.0f;
      mat->up[0] = 0.0f; mat->up[1] = 1.0f; mat->up[2] = 0.0f;
      mat->at[0] = 0.0f; mat->at[1] = 0.0f; mat->at[2] = 1.0f;
      mat->pos[0] = 0.0f; mat->pos[1] = 0.0f; mat->pos[2] = 0.0f;
      mat->flags |= 0x20003;
    } else {
      *mat = obj->mat;
    }
    *box = *one;
    /* the original passes the id in cx of a register that still holds the record pointer: only the low 16 bits are used by the callee */
    rwp_aggregate_00586680(ctx, h, (int)(((unsigned int)rec & 0xffff0000u) | h->id), h->ptr, mat, 0, rec, src, ref, extra);
    return ctx->nrec - start;
  }
  list = (MflAggList *)h->ptr;
  e = list->entries;
  n = list->count;
  for (i = 0; i < n; i++, e++) {
    if (!(mat < mat_lim && rec < rec_lim)) {
      ctx->overflow = 1;
      break;
    }
    obj = e->obj;
    data = obj->data;
    if (data->flags & 2) *mat = *e->mat;
    else if (!e->mat) *mat = obj->mat;
    else RwMatrixMultiply((RwMatrix *)mat, (RwMatrix *)obj, (RwMatrix *)e->mat);
    if (src) *box = *src;
    else if (n == 1) *box = *one;
    else data->bounds(obj, 0, mat, box);
    if (n != 1 && box != ref) {
      if (!(box->max[0] - ref->min[0] <= tol)) goto next;
      if (!(ref->max[0] - box->min[0] <= tol)) goto next;
      if (!(box->max[1] - ref->min[1] <= tol)) goto next;
      if (!(ref->max[1] - box->min[1] <= tol)) goto next;
      if (!(box->max[2] - ref->min[2] <= tol)) goto next;
      if (!(ref->max[2] - box->min[2] <= tol)) goto next;
    }
    if (data->flags & 1) {
      rwp_aggregate_00586680(ctx, h, (int)i, e->obj, mat, 0, rec, src, ref, extra);
      rec = ctx->recs + ctx->nrec;
      mat = ctx->mats + ctx->nmat;
      box = ctx->boxes + ctx->nrec;
    } else {
      rec->mat = mat;
      rec->owner = h->ptr;
      rec->idx = (unsigned short)i;
      rec->idx_hi = h->pad;
      rec->obj = e->obj;
      rec->zero = 0;
      rec++;
      box++;
      mat++;
    }
next:
    if (src) src++;
  }
  ctx->nrec = rec - ctx->recs;
  ctx->nmat = mat - ctx->mats;
  return ctx->nrec - start;
}
