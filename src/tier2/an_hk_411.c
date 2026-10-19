#include "mfl_tier2.h"
extern char LAB_0049fdc0[];
/*004f8b60*/ extern struct RpGeometry * RpGeometryForAllMaterials(struct RpGeometry *p0, RpMaterialCallBack p1, void *p2);
#define GEOMETRY_FLAG_MODULATE_MATERIAL_COLOR (0x40)
#define ATOMIC_GEOMETRY_OFFSET (0x18)
/* geometry_set_flag_0x40_for_materials */

/* "sets geometry flag 0x40 and applies LAB_0049fdc0 to all materials" */

/*
 * Sets GEOMETRY_FLAG_MODULATE_MATERIAL_COLOR (0x40) on the geometry of an atomic, then
 * runs the material callback LAB_0049fdc0 over every material of that geometry.
 *   atomic        : atomic whose geometry is changed (inferred: RpAtomic, geometry at +0x18)
 *   callback_data : passed unchanged to the material callback (inferred)
 * Returns atomic.
 */
int geometry_set_flag_0x40_for_materials(int atomic,void *callback_data)

{
  RwUInt32 *flags_ptr;
  RpGeometry *geometry;

  /* read the geometry pointer from the atomic */
  geometry = *(RpGeometry **)(atomic + ATOMIC_GEOMETRY_OFFSET);
  flags_ptr = &RpGeometry_flags(geometry);
  *flags_ptr = *flags_ptr | GEOMETRY_FLAG_MODULATE_MATERIAL_COLOR;
  RpGeometryForAllMaterials(geometry,(RpMaterialCallBack)&LAB_0049fdc0,callback_data);
  return atomic;
}


