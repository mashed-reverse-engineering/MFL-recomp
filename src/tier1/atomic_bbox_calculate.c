#include "mfl_tier2.h"
/*004d18d0*/ extern struct RwBBox * RwBBoxCalculate(struct RwBBox *p0, struct RwV3d *p1, RwInt32 p2);


enum {
    RP_ATOMIC_OFFSET_GEOMETRY = 0x18,
    RP_GEOMETRY_OFFSET_NUM_VERTICES = 0x14,
    RP_GEOMETRY_OFFSET_MORPH_TARGET = 0x5c,
    RP_MORPH_TARGET_OFFSET_VERTS = 0x14
};

/* atomic_bbox_calculate */

/* Calculate the bounding box of an atomic from its geometry vertices */
RwBBox * __cdecl atomic_bbox_calculate(RwBBox *bound_box,char *atomic)
{
  char *geometry;
  RwV3d *verts;
  
  geometry = *(char **)(atomic + RP_ATOMIC_OFFSET_GEOMETRY);
  /* Verify that the atomic geometry has a valid morph target and vertices */
  if (((geometry != 0) && (*(int *)(geometry + RP_GEOMETRY_OFFSET_MORPH_TARGET) != 0)) &&
     (verts = *(RwV3d **)(*(char **)(geometry + RP_GEOMETRY_OFFSET_MORPH_TARGET) + RP_MORPH_TARGET_OFFSET_VERTS), verts != (RwV3d *)0x0)) {
    /* Calculate bounding box from vertices and vertex count */
    RwBBoxCalculate(bound_box,verts,*(RwInt32 *)(geometry + RP_GEOMETRY_OFFSET_NUM_VERTICES));
    return bound_box;
  }
  return bound_box;
}
