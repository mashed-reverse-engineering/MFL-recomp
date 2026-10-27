#include "mfl_tier2.h"
/*00496fc0*/ extern undefined __cdecl atomic_bbox_calculate_from_geometry(struct RwBBox *p0, int p1);
/*004d1990*/ extern struct RwBBox * RwBBoxInitialize(struct RwBBox *p0, struct RwV3d *p1);
/*004d19d0*/ extern struct RwBBox * RwBBoxAddPoint(struct RwBBox *p0, struct RwV3d *p1);
/*004d5670*/ extern struct RwV3d * RwV3dTransformPoints(struct RwV3d *p0, struct RwV3d *p1, RwInt32 p2, RwMatrix *p3);
/* bbox corners transformed as one array: inf, then sup (inferred) */
#define BBOX_POINT_COUNT (2)
/* offset in atomic of the pointer to its frame (inferred) */
#define ATOMIC_FRAME_PTR_OFFSET (4)
/* offset of the matrix inside the frame (inferred) */
#define FRAME_MATRIX_OFFSET (0x10)
/* atomic_world_bbox_update */
/* "(atomic, bbox): geometry bbox transformed into world space" */
/*
 * Recomputes bbox from the atomic's geometry, then rewrites it in world space.
 * atomic: atomic object; its frame pointer sits at ATOMIC_FRAME_PTR_OFFSET.
 * bbox:   output, receives the world-space bounds (inf/sup).
 * Transforms the local-space inf and sup corners by the frame matrix.
 */
void atomic_world_bbox_update(int atomic,RwBBox *bbox)
{
  RwV3d world_inf;
  RwV3d world_sup;

  /* local-space bbox from the geometry */
  atomic_bbox_calculate_from_geometry(bbox,atomic);
  /* copy the local corners out before bbox is overwritten */
  world_inf = bbox->inf;
  world_sup = bbox->sup;
  /* transform inf and sup in place (world_sup must directly follow world_inf in the frame) */
  RwV3dTransformPoints(&world_inf,&world_inf,BBOX_POINT_COUNT,(RwMatrix *)(*(char **)(atomic + ATOMIC_FRAME_PTR_OFFSET) + FRAME_MATRIX_OFFSET));
  /* rebuild bbox from the world-space corners */
  RwBBoxInitialize(bbox,&world_inf);
  RwBBoxAddPoint(bbox,&world_sup);
  return;
}
