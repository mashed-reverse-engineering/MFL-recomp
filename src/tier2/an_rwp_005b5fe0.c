/* rwp_artic_enable (0x005b5fe0): put the parts and joints of an Rwp articulated object into its simulation space */
#include "mfl_tier2.h"
#include "rwp_common.h"

/*007daa88*/ extern int g_data_7daa88;
/*0056b530*/ extern unsigned int rwp_volume_0056b530(void *space);   /* free body slots */
/*0056b540*/ extern unsigned int rwp_volume_0056b540(void *space);   /* free joint slots */
/*0056b600*/ extern void *rwp_volume_0056b600(void *space, void *body, void *matrix);
/*0056e890*/ extern void *rwp_obj_0056e890(void *space, void *joint_def, MflAggHandle *a, MflAggHandle *b);
/*004d4490*/ extern RwMatrix *RwMatrixInvert(RwMatrix *out, RwMatrix *in);
/*004d49c0*/ extern RwMatrix *RwMatrixTransform(RwMatrix *matrix, RwMatrix *transform, RwOpCombineType combineOp);

#define RWP_ARTIC_HIERARCHY 0x10000   /* flags: the parts follow a matrix hierarchy */

typedef struct MflRwpClass {
  unsigned char pad00[8];
  unsigned int max;   /* 0x8 */
  unsigned int count;   /* 0xc */
} MflRwpClass;

typedef struct MflArticDef {
  int refs;   /* 0x0 */
  unsigned short nbodies;   /* 0x4 */
  unsigned short pad06;
  unsigned short nparts;   /* 0x8 */
  unsigned short njoints;   /* 0xa */
  void *anim;   /* 0xc */
  void *shape;   /* 0x10 */
  char *bodies;   /* 0x14: records of 0x4c bytes */
  char *joint_defs;   /* 0x18: records of 0xb4 bytes */
  unsigned short (*joint_parts)[2];   /* 0x1c: the two parts each joint links */
  unsigned int nnodes;   /* 0x20 */
  void *buf24;   /* 0x24 */
  void *buf28;   /* 0x28 */
  int *parent;   /* 0x2c: parent node per node, -1 = root */
} MflArticDef;

typedef struct MflArticPart {
  void *shape;   /* 0x0 */
  void *sim;   /* 0x4 */
  RwMatrix *matrix;   /* 0x8 */
} MflArticPart;

typedef struct MflArtic {
  MflRwpClass *cls;   /* 0x0 */
  MflArticDef *def;   /* 0x4 */
  unsigned int flags;   /* 0x8 */
  unsigned short nparts;   /* 0xc */
  unsigned short pad0e;
  MflArticPart *parts;   /* 0x10 */
  unsigned short njoints;   /* 0x14 */
  unsigned short pad16;
  void **joints;   /* 0x18 */
  unsigned char pad1c[8];
  void *space;   /* 0x24 */
  unsigned char pad28[0x10];
  RwMatrix *local;   /* 0x38 */
  RwMatrix *world;   /* 0x3c */
} MflArtic;

/* While the class has room for one more live object and the space has room for all parts and joints: every part body goes into the
   space (following its matrix), every joint is created between its two parts, and for a hierarchy the local matrix of every node with
   a parent is rebuilt as world * inverse(parent world).  Returns o, 0 when the space is full; when the class is full the result is the
   unset `ret` (as in the original). */
/* TIER2: unvalidated 2026-11-05 authority=diffemu */
MflArtic *rwp_artic_enable(MflArtic *o)
{
  MflArticDef *def;
  void *space;
  unsigned int nparts, njoints, i;
  MflAggHandle a, b;
  RwMatrix inv;
  MflArtic *ret;
  int k;

  if (o->cls->count < o->cls->max) {
    def = o->def;
    space = o->space;
    nparts = def->nparts;
    njoints = def->njoints;
    ret = o;
    if (rwp_volume_0056b530(space) < nparts) return 0;
    if (rwp_volume_0056b540(space) < njoints) return 0;
    o->cls->count++;
    g_data_7daa88 = 0;
    for (i = 0; i < nparts; i++) o->parts[i].sim = rwp_volume_0056b600(space, def->bodies + i * 0x4c, o->parts[i].matrix);
    a.ptr = o;
    b.ptr = o;
    for (i = 0; i < njoints; i++) {
      a.id = def->joint_parts[i][0];
      b.id = def->joint_parts[i][1];
      o->joints[i] = rwp_obj_0056e890(space, o->def->joint_defs + i * 0xb4, &a, &b);
    }
    o->nparts = nparts;
    o->njoints = njoints;
    if (o->flags & RWP_ARTIC_HIERARCHY) {
      for (i = 0; i < o->def->nnodes; i++) {
        k = def->parent[i];
        if (k != -1) {
          RwMatrixInvert(&inv, &o->world[k]);
          o->local[i] = o->world[i];
          RwMatrixTransform(&o->local[i], &inv, rwCOMBINEPOSTCONCAT);
        }
      }
    }
  }
  return ret;
}
