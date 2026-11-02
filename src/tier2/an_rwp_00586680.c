#include "mfl_tier2.h"
#include "rwp_common.h"

/* One level of the shape-tree walk of rwp_aggregate_00586300: collects the leaf records of an aggregate object into the context's tables.
   A leaf (type data without flag 1) becomes one record.  A compound (type 8) recurses into its child list (0x60 byte child objects, each
   with its matrix composed with the given one, the bounds from the callback or the given box, children farther than the tolerance from
   `ref` skipped).  Any other shape with flag 1 is queried with `ref` shrunk by the tolerance: flag 4 shapes list faces (+0x48), which are
   copied into the 0x60 byte plane table with their plane (normal from the three vertices when the context says so) and one record each;
   other shapes append records themselves (+0x44), each of them recursed into when its type has flag 1.  `path` collects the child indices
   (16 bits per level, index + 1) and ends in the `zero` word of the records.  Returns the number of records handled at this level (the
   child count for a compound), 0 when the tables overflowed (ctx->overflow is set) or nothing fitted. */

typedef struct MflAggMat {
  float right[3]; unsigned int flags;
  float up[3]; unsigned int pad1;
  float at[3]; unsigned int pad2;
  float pos[3]; unsigned int pad3;
} MflAggMat;
typedef struct MflAggObj MflAggObj;
/* Type data: bounds callback at 0x10, flags at 0x40 (1 = has children, 2 = the given matrix is used as is, 4 = face query). */
typedef struct MflAggData {
  unsigned short type;   /* 0x0: 0 = empty, 8 = compound */
  unsigned char pad02[0xe];
  void (*bounds)(MflAggObj *obj, MflAggMat *mat, int zero, MflAggBox *box);   /* 0x10 */
  unsigned char pad14[0x2c];
  unsigned char flags;   /* 0x40 */
} MflAggData;
typedef struct MflAggChildren {
  MflAggObj *items;   /* 0x0 */
  unsigned int count;   /* 0x4 */
} MflAggChildren;
/* A face handed back by the +0x48 callback (0x4c bytes): three vertices 16 bytes apart, the plane normal at 0x30. */
typedef struct MflAggFace {
  float v0[3]; float pad0c;
  float v1[3]; float pad1c;
  float v2[3]; float pad2c;
  float n[3];   /* 0x30 */
  unsigned char pad3c[8];
  int index;   /* 0x44 */
  unsigned char flags;   /* 0x48: 0x10 = skip */
  unsigned char pad49[3];
} MflAggFace;
/* Plane record (0x60 bytes): the face with the plane offset at 0x1c, three words and the id of the object, and a tag pointer. */
typedef struct MflAggPlane {
  float v0[3]; float pad0c;
  float v1[3]; float d;   /* 0x1c */
  float v2[3]; float pad2c;
  float n[3];   /* 0x30 */
  unsigned char pad3c[8];
  int index;   /* 0x44 */
  unsigned char flags;   /* 0x48 */
  unsigned char pad49[3];
  int w4c, w50, w54;
  unsigned char pad58[2];
  unsigned short id5a;
  void *tag;   /* 0x5c */
} MflAggPlane;
typedef struct MflW19 { int w[19]; } MflW19;
typedef struct MflAggRec {
  void *owner;
  unsigned short idx;   /* 0x4 */
  unsigned short idx_hi;
  MflAggObj *obj;   /* 0x8 */
  unsigned int zero;   /* 0xc */
  MflAggMat *mat;   /* 0x10 */
} MflAggRec;
struct MflAggObj {
  MflAggMat mat;   /* 0x0 */
  MflAggChildren *children;   /* 0x40 */
  int (*query)(MflAggObj *obj, MflAggBox *q, void *extra, MflAggRec **recs);   /* 0x44 */
  int (*faces)(MflAggObj *obj, MflAggMat *mat, MflAggBox *q, void *extra, MflAggFace **out);   /* 0x48 */
  int w4c, w50, w54;
  unsigned char pad58[2];
  unsigned short id5a;   /* 0x5a */
  MflAggData *data;   /* 0x5c */
};
typedef struct MflAggCtx {
  unsigned char pad00[0xc];
  MflAggRec *recs;   /* 0xc */
  unsigned int nrec;   /* 0x10 */
  unsigned int caprec;   /* 0x14 */
  MflAggMat *mats;   /* 0x18 */
  unsigned int nmat;   /* 0x1c */
  unsigned int capmat;   /* 0x20 */
  MflAggBox *boxes;   /* 0x24, parallel to recs */
  unsigned char pad28[0x1c];
  MflAggFace *faces;   /* 0x44 */
  int nfaces;   /* 0x48 */
  unsigned char pad4c[4];
  unsigned char flags;   /* 0x50: 2 = compute the plane normals */
  unsigned char pad51[7];
  MflAggPlane *planes;   /* 0x58 */
  unsigned int nplanes;   /* 0x5c */
  unsigned int capplanes;   /* 0x60 */
  unsigned char pad64[0xc];
  float tol;   /* 0x70 */
  unsigned char pad74[4];
  int overflow;   /* 0x78 */
} MflAggCtx;

extern char g_data_5f72b0[];
extern float rwp_cylinder_0056fd20(float *out, float *in);
extern void rwp_box_extent_in_frame(float *out, float *box, float *m);
extern void rwp_sphere_0056c340(MflAggPlane *pl, MflAggMat *mat, int zero, MflAggBox *box);

/* TIER2: unvalidated 2026-11-02 authority=diffemu */
int rwp_aggregate_00586680(MflAggCtx *ctx, MflAggHandle *h, int id, MflAggObj *obj, MflAggMat *mat, unsigned int path, MflAggRec *rec,
                           MflAggBox *src, MflAggBox *ref, void *extra)
{
  MflAggData *data, *cd;
  MflAggBox q, *bx, *box;
  MflAggMat *dm;
  MflAggObj *item, *child;
  MflAggRec *rn, *rp, *rr;
  MflAggFace *face;
  MflAggPlane *pl;
  float tol;
  double ss;
  unsigned int n, i, j, total;

  ctx->nrec = rec - ctx->recs;
  ctx->nmat = mat - ctx->mats;
  if (ctx->nmat >= ctx->capmat || ctx->nrec >= ctx->caprec) {
    ctx->overflow = 1;
    return 0;
  }
  ctx->nmat++;
  data = obj->data;
  if (!(data->flags & 1)) {
    if (data->type == 0) return 0;
    rec->mat = mat;
    rec->owner = h->ptr;
    rec->idx = id;
    rec->idx_hi = h->pad;
    rec->obj = obj;
    rec->zero = path;
    ctx->nrec++;
    return 1;
  }
  tol = ctx->tol;
  path <<= 16;
  if (data->type == 8) {
    item = obj->children->items;
    n = obj->children->count;
    for (i = 0; i < n; i++, item++) {
      cd = item->data;
      if (ctx->nmat >= ctx->capmat || ctx->nrec >= ctx->caprec) {
        ctx->overflow = 1;
        return n;
      }
      dm = ctx->mats + ctx->nmat;
      box = ctx->boxes + ctx->nrec;
      if (cd->flags & 2) *dm = *mat;
      else if (!mat) *dm = item->mat;
      else RwMatrixMultiply((RwMatrix *)dm, (RwMatrix *)item, (RwMatrix *)mat);
      if (src) *box = *src;
      else {
        cd->bounds(item, dm, 0, box);
        if (n > 1 && box != ref) {
          if (!(box->max[0] - ref->min[0] <= tol)) continue;
          if (!(ref->max[0] - box->min[0] <= tol)) continue;
          if (!(box->max[1] - ref->min[1] <= tol)) continue;
          if (!(ref->max[1] - box->min[1] <= tol)) continue;
          if (!(box->max[2] - ref->min[2] <= tol)) continue;
          if (!(ref->max[2] - box->min[2] <= tol)) continue;
        }
      }
      rwp_aggregate_00586680(ctx, h, id, item, dm, path + i + 1, rec, src, ref, extra);
      rec = ctx->recs + ctx->nrec;
    }
    return n;
  }
  q.min[0] = tol + ref->min[0];
  q.min[1] = ref->min[1] + tol;
  q.min[2] = ref->min[2] + tol;
  q.max[0] = ref->max[0] - tol;
  q.max[1] = ref->max[1] - tol;
  q.max[2] = ref->max[2] - tol;
  if (data->flags & 4) {
    if (!obj->faces(obj, mat, &q, extra, &ctx->faces)) return 0;
    face = ctx->faces;
    total = 0;
    for (j = 0; j < (unsigned int)ctx->nfaces; j++, face++) {
      if (face->flags & 0x10) continue;
      if (ctx->nplanes >= ctx->capplanes || ctx->nrec >= ctx->caprec) {
        ctx->overflow = 1;
        return total;
      }
      pl = ctx->planes + ctx->nplanes;
      ctx->nplanes++;
      *(MflW19 *)pl = *(MflW19 *)face;
      pl->tag = g_data_5f72b0;
      pl->w4c = obj->w4c;
      pl->w50 = obj->w50;
      pl->w54 = obj->w54;
      pl->id5a = obj->id5a;
      if (ctx->flags & 2) {
        ss = (face->v1[2] - face->v2[2]) * face->v0[1];
        ss += (face->v0[2] - face->v1[2]) * face->v2[1];
        ss += (face->v2[2] - face->v0[2]) * face->v1[1];
        pl->n[0] = ss;
        ss = (face->v1[0] - face->v2[0]) * face->v0[2];
        ss += (face->v2[0] - face->v0[0]) * face->v1[2];
        ss += (face->v0[0] - face->v1[0]) * face->v2[2];
        pl->n[1] = ss;
        ss = (face->v0[1] - face->v1[1]) * face->v2[0];
        ss += (face->v1[1] - face->v2[1]) * face->v0[0];
        ss += (face->v2[1] - face->v0[1]) * face->v1[0];
        pl->n[2] = ss;
        rwp_cylinder_0056fd20(pl->n, pl->n);
      }
      ss = pl->n[1] * face->v0[1];
      ss += pl->n[0] * face->v0[0];
      ss += pl->n[2] * face->v0[2];
      pl->d = ss;
      rr = ctx->recs + ctx->nrec;
      rr->owner = h->ptr;
      rr->idx = id;
      rr->idx_hi = h->pad;
      rr->obj = (MflAggObj *)pl;
      rr->mat = mat;
      rwp_sphere_0056c340(pl, mat, 0, ctx->boxes + ctx->nrec);
      rr->zero = face->index + path;
      ctx->nrec++;
      total++;
    }
    return total;
  }
  rwp_box_extent_in_frame(&q.min[0], &q.min[0], (float *)mat);
  n = obj->query(obj, &q, extra, &ctx->recs);
  rn = rec;
  bx = ctx->boxes + (rn - ctx->recs);
  rp = ctx->recs + ctx->nrec;
  for (i = 0; i < n; i++, rn++, bx++) {
    if (ctx->nmat >= ctx->capmat) {
      ctx->overflow = 1;
      return n;
    }
    child = rn->obj;
    dm = ctx->mats + ctx->nmat;
    cd = child->data;
    rn->owner = h->ptr;
    rn->idx = id;
    rn->mat = dm;
    rn->zero += path + 1;
    if (cd->flags & 2) *dm = *mat;
    else if (!mat) *dm = child->mat;
    else RwMatrixMultiply((RwMatrix *)dm, (RwMatrix *)child, (RwMatrix *)mat);
    cd->bounds(child, dm, 0, bx);
    if (cd->flags & 1) {
      rwp_aggregate_00586680(ctx, h, id, child, dm, rn->zero, rp, 0, ref, extra);
      rn->obj = (MflAggObj *)1;
      rp = ctx->recs + ctx->nrec;
    } else {
      ctx->nmat++;
    }
  }
  return n;
}
