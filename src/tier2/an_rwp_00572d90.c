/* rwp_pipeline_00572d90 (0x00572d90): one pipeline pass over the item groups - slot assignment, body integration, link hits, broad and narrow phase */
#include "mfl_tier2.h"
#include "rwp_common.h"
#include <string.h>

extern float RwV3dLength(float *v);
extern int rwp_sphere_0056c140(void *shape, void *arg, float *hit);
extern float *rwp_pipeline_00572c80(void *pl, int index, float *hit);
extern int rwp_math_0056f500(void *d, void *a, float *p, int dist);
extern int rwp_math_0056f450(void *out, void *a, void *b);
extern int rwp_volume_0056b100(void *owner, void *obj, void *acc);
extern int rwp_obj_0056e610(void *obj);
extern int rwp_volume_0056af70(void *scan, void *list, unsigned int count, void *out, unsigned int max_out, unsigned int mask);
extern int rwp_convexhullmassprops_00583d20(void *w, void *a, int b, void *c, void *d, void *e, int f);
extern int rwp_batch_0057c080(void *w, void *list, unsigned int n, void *a, void *b, void *c, int d);
extern int rwp_batch_0057c530(void *tables, int index, float dt);
extern void rwp_batch_0057c750(void *a, void *b, void *c, void *d, void *e, void *f, void *g, void *h, void *i, void *j, float dt);

/* A body slot of an item link: owner world and index in its tables (per-body arrays), then a vector. */
typedef struct MflQuat16 { float v[4]; } MflQuat16;
typedef struct MflBox32 { int w[8]; } MflBox32;

typedef struct MflPipeBody {
  MflDynWorld *owner;
  int index;   /* 0x4 */
  float v[3];   /* 0x8 */
} MflPipeBody;

typedef struct MflPipeLink {
  void *shape;
  MflPipeBody *body;   /* 0x4 */
  int pad8;
} MflPipeLink;

typedef struct MflPipeTables {
  void *mats;   /* 0x0: 64 byte matrices */
  int a4;   /* 0x4 */
  char *vel;   /* 0x8: 32 byte velocity records */
  int ac;   /* 0xc */
  float *quats;   /* 0x10: 16 byte quaternions */
  int a14, a18, a1c;
  unsigned char pad20[0x30];
  int a50, a54;
} MflPipeTables;

typedef struct MflPipeItem {
  int (**vtbl)();   /* 0x0: [7] after-step hook */
  int pad4;
  unsigned int flags;   /* 0x8 */
  unsigned short nlinks;   /* 0xc */
  unsigned short pad0e;
  MflPipeLink *links;   /* 0x10 */
  unsigned char pad14[0xc];
  unsigned short id;   /* 0x20 */
  unsigned short pad22;
  MflDynWorld *owner;   /* 0x24 */
} MflPipeItem;

typedef struct MflPipeShapeType {
  unsigned char pad00[0x24];
  float (*weight)();   /* 0x24 */
} MflPipeShapeType;

typedef struct MflPipeShape {
  unsigned char pad00[0x4c];
  float mass;   /* 0x4c */
  unsigned char pad50[0xc];
  MflPipeShapeType *type;   /* 0x5c */
} MflPipeShape;

typedef struct MflPipeGroup {
  MflPipeItem **items;   /* 0x0 */
  int pad4, pad8;
  unsigned int count;   /* 0xc */
  unsigned char pad10[8];
  unsigned int flags;   /* 0x18 */
  unsigned char pad1c[0xc];
} MflPipeGroup;

typedef struct MflPipeWorld {
  float scale;   /* 0x0 */
  unsigned char pad04[0x18];
  float ratio;   /* 0x1c */
  unsigned int cap;   /* 0x20 */
  unsigned char pad24[8];
  MflPipeItem **sel;   /* 0x2c: items with a slot range */
  float *pos;   /* 0x30: 3 floats per slot */
  float *quat;   /* 0x34: 4 floats per slot */
  char *box;   /* 0x38: 32 bytes per slot */
  float *dt;   /* 0x3c: 1 float per slot */
  unsigned short *slotof;   /* 0x40: item id -> first slot */
  unsigned char pad44[0x2c];
  MflDynWorld **dyn;   /* 0x70 */
  MflPipeItem **list;   /* 0x74 */
  unsigned int nlist;   /* 0x78 */
  unsigned char pad7c[4];
  void *near_ids;   /* 0x80 */
  int result;   /* 0x84 */
  void *visited;   /* 0x88 */
  unsigned char pad8c[0x3c];
  unsigned int ngroups;   /* 0xc8 */
  unsigned char padcc[8];
  MflPipeGroup *groups;   /* 0xd4 */
} MflPipeWorld;

/* One pass of the pipeline over the item groups: gives every active item (flags bit 2, or bit 3 with a link whose shape passes the energy test against
   the world's thresholds) a range of slots (one per link; slotof maps the item id to its first slot), gathers position / orientation / step time of
   the linked bodies into the per-slot arrays, integrates the bodies (57c750) and steps their items; then for every selected item runs its links
   (572c80 hit + 56f500, the rotation matrix of the slot's quaternion written back to the body tables, the boxes combined by 56f450, 56b100), and the
   broad / narrow phase calls (56af70, 583d20, 57c080, 57c530).  Returns w. */
/* TIER2: unvalidated 2026-11-02 authority=diffemu */
MflPipeWorld *rwp_pipeline_00572d90(MflPipeWorld *w)
{
  struct {
    float scan[4];      /* hit of the energy test (position, distance) */
    int gap[3];
    float hit[4];       /* hit of the link loop */
    char acc[32];       /* the combined box of the item */
  } f;
  unsigned int gi, bi, li, j, k;
  unsigned int found, nextslot, nl, slot, accn, nsel;
  MflPipeGroup *g;
  MflPipeItem **p, **dst, **sel;
  MflPipeItem *it;
  MflPipeLink *lk;
  MflPipeBody *bd;
  MflPipeShape *sh;
  MflPipeTables *tb;
  volatile float L1, L2, val;
  float thr;
  unsigned short *slotof;
  float *A, *B, *E;
  char *D;
  float q0, q1, q2, q3, *Q;
  double s, sa, sc, sac, sab;
  int hits;
  volatile float t50, t54, t58, t5c, t60, t64, t6c, t80;
  float *M;

  slotof = w->slotof;
  memset(slotof, 0, (*w->dyn)->idcap * 2);
  found = 0;
  nextslot = 1;
  for (gi = 0; gi < w->ngroups; gi++) {
    g = &w->groups[gi];
    if (g->flags & 3) continue;
    p = g->items;
    for (bi = 0; bi < g->count; bi++, p++) {
      it = *p;
      if (!(it->flags & 4)) {
        if (!(it->flags & 8)) continue;
        for (li = 0; li < it->nlinks; li++) {
          lk = &it->links[li];
          sh = (MflPipeShape *)lk->shape;
          bd = lk->body;
          if (!rwp_sphere_0056c140(sh, 0, f.scan)) continue;
          tb = (MflPipeTables *)bd->owner->tables;
          L1 = RwV3dLength((float *)(tb->vel + bd->index * 32 + 0x10));
          L2 = RwV3dLength((float *)(tb->vel + bd->index * 32));
          val = ((RwV3dLength(bd->v) + f.scan[3]) * L1 + L2) * w->scale;
          thr = (sh->type->weight(sh) + sh->mass * 2.0f) * 0.5f;
          if (val > thr && f.scan[3] * w->ratio < val) goto assign;
        }
        continue;
      }
    assign:
      if (it->nlinks + nextslot - 1 > w->cap) continue;
      slotof[it->id] = (unsigned short)nextslot;
      found++;
      nextslot += it->nlinks;
    }
  }
  if (found != 0) {
    A = w->pos;
    B = w->quat;
    D = w->box;
    E = w->dt;
    dst = w->sel;
    for (k = 0; k < w->nlist; k++) {
      it = w->list[k];
      if (slotof[it->id] != 0) *dst++ = it;
    }
    if (found > 0) {
      p = w->sel;
      for (nsel = found; nsel; nsel--, p++) {
        it = *p;
        nl = it->nlinks;
        slot = slotof[it->id];
        if (nl != 0) {
          float *a = A + slot * 3;
          float *b = B + slot * 4;
          float *e = E + slot;
          for (j = 0; j < nl; j++) {
            bd = it->links[j].body;
            *(MflV3 *)a = *(MflV3 *)((char *)((MflPipeTables *)bd->owner->tables)->mats + bd->index * 64 + 0x30);
            a += 3;
            *(MflQuat16 *)b = *(MflQuat16 *)(((MflPipeTables *)bd->owner->tables)->quats + bd->index * 4);
            b += 4;
            *e++ = w->scale;
          }
        }
      }
    }
  }
  tb = (MflPipeTables *)(*w->dyn)->tables;
  rwp_batch_0057c750(tb->mats, (void *)tb->a4, tb->quats, (void *)tb->a14, tb->vel, (void *)tb->ac, (void *)tb->a50, (void *)tb->a54, (void *)tb->a18, (void *)tb->a1c, w->scale);
  for (gi = 0; gi < w->ngroups; gi++) {
    g = &w->groups[gi];
    if (!(g->flags & 1)) {
      for (bi = 0; bi < g->count; bi++) {
        it = g->items[bi];
        it->vtbl[7](it);
        rwp_obj_0056e610(it);
      }
    }
  }
  if (found == 0)
    return w;
  /* per selected item: hit of every link in world space, the orientation matrix of the slot's quaternion written back to the body tables, boxes combined */
  p = w->sel;
  for (nsel = found; nsel; nsel--, p++) {
    it = *p;
    nl = it->nlinks;
    slot = slotof[it->id];
    accn = 0;
    if (nl != 0) {
      A = w->pos + slot * 3;
      B = w->quat + slot * 4;
      D = w->box + slot * 32;
      for (j = 0; j < nl; j++, A += 3, B += 4, D += 32) {
        if (!rwp_pipeline_00572c80(it, j, f.hit))
          continue;
        rwp_math_0056f500(D, A, f.hit, *(int *)&f.hit[3]);
        q0 = B[0];
        q1 = B[1];
        q2 = B[2];
        q3 = B[3];
        s = q0 * q0;
        s = s + q1 * q1;
        s = s + q2 * q2;
        s = s + q3 * q3;
        s = 2.0f / s;
        sa = q0 * s;
        t80 = q1 * s;
        sc = q2 * s;
        t5c = q3 * sa;
        t60 = q3 * t80;
        t64 = q3 * sc;
        t50 = q0 * sa;
        t54 = t80 * q1;
        t58 = sc * q2;
        t6c = sc * q1;
        sac = sa * q2;
        sab = t80 * q0;
        bd = it->links[j].body;
        M = (float *)((char *)((MflPipeTables *)bd->owner->tables)->mats + bd->index * 64);
        M[0] = 1.0f - (t58 + t54);
        M[1] = t64 + sab;
        M[2] = sac - t60;
        M[4] = sab - t64;
        M[5] = 1.0f - (t58 + t50);
        M[6] = t6c + t5c;
        M[8] = sac + t60;
        M[9] = t6c - t5c;
        M[10] = 1.0f - (t54 + t50);
        M[12] = 0.0f;
        M[13] = 0.0f;
        M[14] = 0.0f;
        ((int *)M)[3] = 3;
        M[12] = A[0];
        M[13] = A[1];
        M[14] = A[2];
        Q = ((MflPipeTables *)bd->owner->tables)->quats + bd->index * 4;
        Q[0] = q0;
        Q[1] = q1;
        Q[2] = q2;
        Q[3] = q3;
        if (accn++ == 0)
          *(MflBox32 *)f.acc = *(MflBox32 *)D;
        else
          rwp_math_0056f450(f.acc, f.acc, D);
      }
    }
    it->vtbl[7](it);
    rwp_volume_0056b100(it->owner, it, f.acc);
  }
  hits = rwp_volume_0056af70(*w->dyn, w->sel, found, w->near_ids, w->visited, 0xf);
  w->result = hits;
  rwp_convexhullmassprops_00583d20(w, w->near_ids, hits, slotof, w->box, w->dt, nextslot);
  rwp_batch_0057c080(w, w->sel, found, slotof, w->box, w->dt, nextslot);
  p = w->sel;
  for (nsel = found; nsel; nsel--, p++) {
    it = *p;
    nl = it->nlinks;
    slot = slotof[it->id];
    for (j = 0; j < nl; j++)
      rwp_batch_0057c530((*w->dyn)->tables, it->links[j].body->index, w->dt[slot]);
    it->vtbl[7](it);
    rwp_obj_0056e610(it);
  }
  return w;
}
