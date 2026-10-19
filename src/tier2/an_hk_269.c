#include "mfl_tier2.h"
/*00481040*/ extern int material_set_alpha_callback();
/*004c53e0*/ extern void FUN_004c53e0(undefined4 p0, undefined4 p1, undefined4 p2);
/*004f8b60*/ extern struct RpGeometry * RpGeometryForAllMaterials(struct RpGeometry *p0, RpMaterialCallBack p1, void *p2);

#define ATOMIC_GEOMETRY_OFFSET (0x18)

/* atomic_set_material_alpha */

/* "locks the geometry and sets the alpha of all materials" */

/* Atomic callback: takes the geometry of `atomic` (RpAtomic at ATOMIC_GEOMETRY_OFFSET), locks it,
   then runs material_set_alpha_callback over every material with `callback_data` as its argument.
   Returns `atomic` unchanged. */
int __cdecl atomic_set_material_alpha(int atomic,void *callback_data)

{
  RpGeometry *geometry;

  geometry = *(RpGeometry **)(atomic + ATOMIC_GEOMETRY_OFFSET);
  FUN_004c53e0(geometry,0x40,1);
  RpGeometryForAllMaterials(geometry,material_set_alpha_callback,callback_data);
  return atomic;
}


