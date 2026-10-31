#include "mfl_tier2.h"
/* Rwp collision: contact generation for one shape pair into the contact set (record layouts reconstructed from the code, unnamed bytes are padding) */
typedef struct RwpPoint { float v[4]; } RwpPoint;
typedef struct RwpMaterial {
  unsigned char pad00[64];
  unsigned int flags;   /* 0x40 */
} RwpMaterial;
typedef struct RwpShapeData {
  unsigned char pad00[16];
  RwV3d center;   /* 0x10 */
  unsigned char pad1c[20];
  RwV3d dir;   /* 0x30 */
  unsigned char pad3c[12];
  unsigned short kind;   /* 0x48 */
  unsigned char pad4a[2];
  float f4c;   /* 0x4c */
  float f50;   /* 0x50 */
  float f54;   /* 0x54 */
  unsigned char pad58[4];
  RwpMaterial *material;   /* 0x5c */
} RwpShapeData;
typedef struct RwpBodyRef {
  int id0;   /* 0x0 */
  int id1;   /* 0x4 */
  RwpShapeData *shape;   /* 0x8 */
  int id3;   /* 0xc */
  RwMatrix *xf;   /* 0x10 */
} RwpBodyRef;
typedef struct RwpPair {
  RwpBodyRef *a;   /* 0x0 */
  RwpBodyRef *b;   /* 0x4 */
} RwpPair;
typedef struct RwpContact {
  unsigned char pad00[80];
  RwpPoint *points;   /* 0x50 */
  unsigned short count_a;   /* 0x54 */
  unsigned char pad56[2];
  int a_id0;   /* 0x58 */
  int a_id1;   /* 0x5c */
  RwV3d normal;   /* 0x60 */
  int a_id3;   /* 0x6c */
  float a_f54;   /* 0x70 */
  float a_f50;   /* 0x74 */
  float a_f4c;   /* 0x78 */
  unsigned short a_flags;   /* 0x7c */
  unsigned short a_kind;   /* 0x7e */
  unsigned char pad80[84];
  unsigned short count_b;   /* 0xd4 */
  unsigned char padd6[2];
  int b_id0;   /* 0xd8 */
  int b_id1;   /* 0xdc */
  RwV3d b_normal;   /* 0xe0 */
  int b_id3;   /* 0xec */
  float b_f54;   /* 0xf0 */
  float b_f50;   /* 0xf4 */
  float b_f4c;   /* 0xf8 */
  unsigned short b_flags;   /* 0xfc */
  unsigned short b_kind;   /* 0xfe */
  unsigned char pad100[16];
  float distance;   /* 0x110 */
  unsigned char pad114[12];
} RwpContact;
typedef struct RwpContactSet {
  unsigned char pad00[44];
  RwpContact *contacts;   /* 0x2c */
  unsigned int count;   /* 0x30 */
  unsigned int capacity;   /* 0x34 */
  RwpPoint *points;   /* 0x38 */
  unsigned int points_used;   /* 0x3c */
  unsigned int points_capacity;   /* 0x40 */
  unsigned char pad44[48];
  float tolerance;   /* 0x74 */
  int full;   /* 0x78 */
} RwpContactSet;
typedef struct RwpQueryObj {
  unsigned char pad00[4];
  int shape;   /* 0x4 */
} RwpQueryObj;
typedef struct RwpQuery {
  unsigned char pad00[112];
  RwpQueryObj *obj;   /* 0x70 */
} RwpQuery;
#define SUM3(r, a, b, c) do { (r) = (a); (r) = (r) + (b); (r) = (r) + (c); } while (0)
/*0058b450*/ extern int rwp_gjk_0058b450(void *shape, RwpPair *pair, float limit, RwpContact *c, float tol);
/*005894f0*/ extern int rwp_aggregate_005894f0(void *shape, RwpPair *pair, float limit, RwpContact *c, float tol);
/*00586200*/ extern int rwp_aggregate_00586200(RwpBodyRef *b, RwpBodyRef *a, RwV3d *normal, unsigned short *kind);
/*0056c8a0*/ extern int rwp_sphere_0056c8a0(void *shape, RwMatrix *xf, RwV3d *dir, float *lo, float *hi);

/* Adds the contact between the two bodies of `pair` (distance <= limit) to the set; fails (0) when the set or the
   point pool is full or the narrow phase finds nothing. */
/* TIER2: unvalidated 2026-10-31 authority=diffemu */
int rwp_aggregate_00585f20(RwpContactSet *cs, RwpQuery *query, RwpPair *pair, float limit)
{
  RwpContact *c;
  RwpBodyRef *a, *b;
  RwpShapeData *sa, *sb;
  RwMatrix *xf;
  unsigned int n;
  float lo, hi;
  double centre, acc;
  int hit;

  if (cs->points_used + 0x10 > cs->points_capacity || cs->count >= cs->capacity) {
    cs->full = 1;
    return 0;
  }
  c = cs->contacts + cs->count;
  c->points = cs->points + cs->points_used;
  lo = cs->tolerance;
  if ((pair->b->shape->material->flags | pair->a->shape->material->flags) & 0x10)
    hit = rwp_gjk_0058b450(query->obj->shape, pair, limit, c, lo);
  else
    hit = rwp_aggregate_005894f0(query->obj->shape, pair, limit, c, lo);
  if (!hit)
    return 0;
  if (!(c->distance <= limit))
    return 0;
  n = cs->count;
  cs->count = n + 1;
  if (n + 1 >= cs->capacity)
    cs->full = 1;
  if (c->count_a > 4)
    cs->points_used += c->count_a;
  if (c->count_b > 4)
    cs->points_used += c->count_b;

  a = pair->a;
  sa = a->shape;
  c->a_id0 = a->id0;
  c->a_id1 = a->id1;
  c->a_id3 = a->id3;
  c->a_flags = sa->material->flags;
  c->a_f4c = sa->f4c;
  c->a_f54 = sa->f54;
  c->a_f50 = sa->f50;
  c->a_kind = 0;
  if (c->a_flags & 0x40) {
    b = pair->b;
    a = pair->a;
    sa = a->shape;
    c->a_kind = sa->kind;
    if (sa->kind != 0xe) {
      xf = a->xf;
      SUM3(acc, xf->at.x * sa->dir.z, xf->up.x * sa->dir.y, xf->right.x * sa->dir.x);
      c->normal.x = acc;
      xf = a->xf;
      SUM3(acc, xf->at.y * sa->dir.z, xf->right.y * sa->dir.x, xf->up.y * sa->dir.y);
      c->normal.y = acc;
      xf = a->xf;
      SUM3(acc, xf->at.z * sa->dir.z, xf->right.z * sa->dir.x, xf->up.z * sa->dir.y);
      c->normal.z = acc;
      if (!(c->a_kind & 1)) {
        rwp_sphere_0056c8a0(b->shape, b->xf, &c->normal, &lo, &hi);
        xf = a->xf;
        SUM3(centre, sa->center.y * sa->dir.y, sa->center.x * sa->dir.x, sa->center.z * sa->dir.z);
        SUM3(acc, xf->pos.y * c->normal.y, xf->pos.x * c->normal.x, xf->pos.z * c->normal.z);
        acc = centre + acc;
        if (acc - lo > hi - acc) {
          c->normal.x = -c->normal.x;
          c->normal.y = -c->normal.y;
          c->normal.z = -c->normal.z;
        }
      }
    }
  }

  b = pair->b;
  sb = b->shape;
  c->b_id0 = b->id0;
  c->b_id1 = b->id1;
  c->b_id3 = b->id3;
  c->b_flags = sb->material->flags;
  c->b_f4c = sb->f4c;
  c->b_f54 = sb->f54;
  c->b_f50 = sb->f50;
  c->b_kind = 0;
  if (c->b_flags & 0x40)
    rwp_aggregate_00586200(pair->b, pair->a, &c->b_normal, &c->b_kind);
  return 1;
}
