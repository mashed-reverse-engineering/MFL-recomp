/* Rwp articulated object (several bodies linked by joints, optional matrix hierarchy): destroy (0x005b5fa0), definition release (0x005b63f0,
   0x005b6420), disable (0x005b61c0), bounding box (0x005b6240), matrix sync (0x005b62e0) */
#include "mfl_tier2.h"
#include "rwp_common.h"

/*007d2128*/ extern struct RwGlobalsRelease *RwEngineInstance;
/*0056e550*/ extern void rwp_obj_0056e550(void *obj);
/*00575b80*/ extern void rwp_capsule_00575b80(void *p);
/*0056bf00*/ extern void rwp_sphere_0056bf00(void *body);
/*00573f50*/ extern void RtAnimAnimationDestroy(void *anim);
/*0056b630*/ extern void rwp_volume_0056b630(void *sim);
/*0056e8c0*/ extern void rwp_obj_0056e8c0(void *joint);
/*0056c350*/ extern int rwp_sphere_0056c350(void *shape, void *matrix, int mode, MflAggBox *box);
/*0056f450*/ extern void rwp_math_0056f450(MflAggBox *out, MflAggBox *a, MflAggBox *b);
/*004d4870*/ extern RwMatrix *RwMatrixTranslate(RwMatrix *matrix, RwV3d *translation, RwOpCombineType combineOp);
/*004d3cc0*/ extern RwMatrix *RwMatrixUpdate(RwMatrix *matrix);
/*004d49c0*/ extern RwMatrix *RwMatrixTransform(RwMatrix *matrix, RwMatrix *transform, RwOpCombineType combineOp);

#define RWP_ARTIC_HIERARCHY 0x10000   /* flags: the parts follow a matrix hierarchy */

typedef struct MflRwpClass {
  unsigned char pad00[8];
  unsigned int max;   /* 0x8 */
  unsigned int count;   /* 0xc */
} MflRwpClass;

/* shared definition of an articulated object (reference counted) */
typedef struct MflArticDef {
  int refs;   /* 0x0 */
  unsigned short nbodies;   /* 0x4 */
  unsigned short pad06;
  unsigned short nparts;   /* 0x8 */
  unsigned short njoints;   /* 0xa */
  void *anim;   /* 0xc */
  void *shape;   /* 0x10 */
  char *bodies;   /* 0x14: nbodies records of 0x4c bytes */
  void *joint_defs;   /* 0x18 */
  void *joint_parts;   /* 0x1c */
  unsigned int nnodes;   /* 0x20: hierarchy nodes */
  void *buf24;   /* 0x24 */
  void *buf28;   /* 0x28 */
  int *parent;   /* 0x2c: parent node per node, -1 = root (may be 0) */
} MflArticDef;

typedef struct MflRwpSimSet {
  unsigned char pad00[0x10];
  RwMatrix **matrices;   /* 0x10: [0] = the simulated matrices */
} MflRwpSimSet;

typedef struct MflRwpSim {
  MflRwpSimSet *set;   /* 0x0 */
  int index;   /* 0x4 */
  RwV3d com;   /* 0x8: centre of mass in part space */
} MflRwpSim;

typedef struct MflArticPart {
  void *shape;   /* 0x0 */
  MflRwpSim *sim;   /* 0x4 */
  RwMatrix *matrix;   /* 0x8 */
} MflArticPart;

typedef struct MflArtic {
  MflRwpClass *cls;   /* 0x0 */
  MflArticDef *def;   /* 0x4 */
  unsigned int flags;   /* 0x8 */
  unsigned short nparts;   /* 0xc: parts in the simulation */
  unsigned short pad0e;
  MflArticPart *parts;   /* 0x10 */
  unsigned short njoints;   /* 0x14: joints in the simulation */
  unsigned short pad16;
  void **joints;   /* 0x18 */
  unsigned char pad1c[8];
  void *space;   /* 0x24 */
  unsigned char pad28[0x10];
  RwMatrix *local;   /* 0x38: per node matrices relative to the parent */
  RwMatrix *world;   /* 0x3c: per node matrices */
} MflArtic;

/* Drops one reference of the definition; the last one releases the bodies, the shape, the tables and the animation.  Returns TRUE. */
/* TIER2: unvalidated 2026-11-05 authority=diffemu */
int rwp_artic_def_release(MflArticDef *d)
{
  int i;

  if (--d->refs == 0) {
    for (i = 0; i < d->nbodies; i++) rwp_sphere_0056bf00(d->bodies + i * 0x4c);
    rwp_capsule_00575b80(d->shape);
    RwFree(d->bodies);
    RwFree(d->joint_defs);
    RwFree(d->joint_parts);
    RwFree(d->buf28);
    RwFree(d->buf24);
    RtAnimAnimationDestroy(d->anim);
    if (d->parent) RwFree(d->parent);
  }
  return 1;
}

/* Releases one reference of the definition and frees it when none is left.  Returns TRUE. */
/* TIER2: unvalidated 2026-11-05 authority=diffemu */
int rwp_artic_def_destroy(MflArticDef *d)
{
  rwp_artic_def_release(d);
  if (d->refs == 0) RwFree(d);
  return 1;
}

/* Destroys an articulated object: object base (rwp_obj_0056e550), local matrix block, definition reference, the object itself.  Returns TRUE. */
/* TIER2: unvalidated 2026-11-05 authority=diffemu */
int rwp_artic_destroy(MflArtic *o)
{
  rwp_obj_0056e550(o);
  if (o->local) rwp_capsule_00575b80(o->local);
  rwp_artic_def_destroy(o->def);
  RwFree(o);
  return 1;
}

/* Takes every part and joint of the object out of the simulation and uncounts the object.  Returns o. */
/* TIER2: unvalidated 2026-11-10 authority=static */
MflArtic *rwp_artic_disable(MflArtic *o)
{
  int i;

  for (i = 0; i < o->def->nparts; i++) rwp_volume_0056b630(o->parts[i].sim);
  for (i = 0; i < o->def->njoints; i++) rwp_obj_0056e8c0(o->joints[i]);
  o->njoints = 0;
  o->cls->count--;
  return o;
}

/* Bounding box of all parts (rwp_sphere_0056c350 per part, merged by rwp_math_0056f450).  Returns box, or 0 when no part gave a box. */
/* TIER2: unvalidated 2026-11-10 authority=static */
MflAggBox *rwp_artic_bounding_box(MflArtic *o, MflAggBox *box)
{
  MflAggBox part;
  unsigned int i;
  int found = 0;

  for (i = 0; i < o->def->nparts; i++) {
    if (rwp_sphere_0056c350(o->parts[i].shape, o->parts[i].matrix, 1, &part)) {
      found++;
      if (found == 1) *box = part;
      else rwp_math_0056f450(box, box, &part);
    }
  }
  return found ? box : 0;
}

/* Copies the simulated matrix of every part to the matrix the part follows (moved back by the centre of mass offset), then, for a
   hierarchy, rebuilds the world matrix of every node with a parent from its local matrix and the parent's world matrix.  Returns o. */
/* TIER2: unvalidated 2026-11-10 authority=static */
MflArtic *rwp_artic_sync_matrices(MflArtic *o)
{
  MflArticDef *d = o->def;
  unsigned int i;
  int parent;
  RwV3d back;

  for (i = 0; i < o->nparts; i++) {
    MflRwpSim *sim = o->parts[i].sim;
    RwMatrix *m = o->parts[i].matrix;

    *m = sim->set->matrices[0][sim->index];
    back.x = -sim->com.x;
    back.y = -sim->com.y;
    back.z = -sim->com.z;
    RwMatrixTranslate(m, &back, rwCOMBINEPRECONCAT);
    RwMatrixUpdate(m);
  }
  if (o->flags & RWP_ARTIC_HIERARCHY) {
    for (i = 0; i < d->nnodes; i++) {
      parent = d->parent[i];
      if (parent != -1) {
        o->world[i] = o->local[i];
        RwMatrixTransform(&o->world[i], &o->world[parent], rwCOMBINEPOSTCONCAT);
      }
    }
  }
  return o;
}
