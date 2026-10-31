#include "mfl_tier2.h"
#include <math.h>

typedef struct RwpHit { RwV3d p; float dist; } RwpHit;
typedef struct RwpMatrixTable { RwMatrix *matrix; } RwpMatrixTable;
typedef struct RwpHitOwner { unsigned char pad00[0x10]; RwpMatrixTable *set; } RwpHitOwner;
typedef struct RwpHitNode { RwpHitOwner *owner; int index; } RwpHitNode;
typedef struct RwpPipeItem { void *shape; RwpHitNode *node; RwMatrix *xf; } RwpPipeItem;
typedef struct RwpPipeline { unsigned char pad00[0x10]; RwpPipeItem *items; } RwpPipeline;
/*0056c140*/ extern int rwp_sphere_0056c140(void *shape, void *arg, RwpHit *hit);

/* Casts against item `index`; on a hit moves the hit point from the item's frame to world space, adds the
   distance from there to the owner's reference position to hit->dist and returns the hit with that position. */
/* TIER2: unvalidated 2026-10-31 authority=diffemu */
RwpHit *rwp_pipeline_00572c80(RwpPipeline *pl, int index, RwpHit *hit)
{
  RwpPipeItem *item = pl->items + index;
  RwpHitNode *node = item->node;
  int mi = node->index;
  RwMatrix *base = node->owner->set->matrix;
  RwMatrix *xf;
  RwV3d *ref;
  double acc, x, y;
  float z, dx, dy, dz;

  if (!rwp_sphere_0056c140(item->shape, 0, hit))
    return 0;
  xf = (pl->items + index)->xf;
  acc = xf->up.x * hit->p.y;
  acc = acc + xf->at.x * hit->p.z;
  acc = acc + xf->right.x * hit->p.x;
  x = acc;
  acc = xf->up.y * hit->p.y;
  acc = acc + xf->right.y * hit->p.x;
  acc = acc + xf->at.y * hit->p.z;
  y = acc;
  acc = xf->up.z * hit->p.y;
  acc = acc + xf->right.z * hit->p.x;
  acc = acc + xf->at.z * hit->p.z;
  z = acc;
  hit->p.x = xf->pos.x + x;
  hit->p.y = (pl->items + index)->xf->pos.y + y;
  xf = (pl->items + index)->xf;
  hit->p.z = xf->pos.z + z;
  ref = &base[mi].pos;
  dz = ref->z - (xf->pos.z + z);
  dy = ref->y - hit->p.y;
  dx = ref->x - hit->p.x;
  acc = dz * dz;
  acc = acc + dx * dx;
  acc = acc + dy * dy;
  hit->dist = (float)sqrt(acc) + hit->dist;
  hit->p = *ref;
  return hit;
}
