#include "mfl_tier2.h"
/*0040d040*/ extern undefined __cdecl material_set_envmap(struct RpMaterial *p0, RwReal *p1);
/*004f8b60*/ extern struct RpGeometry * RpGeometryForAllMaterials(struct RpGeometry *p0, RpMaterialCallBack p1, void *p2);
/*00550190*/ extern void FUN_00550190(undefined4 p0);
#define ATOMIC_GEOMETRY_OFFSET (0x18)
/* clump_set_envmap */
/* "applies material_set_envmap to every material of the geometry"
 * Per-atomic callback (inferred): applies the environment map to every material
 * of the atomic's geometry. atomic = RpAtomic * (inferred), envmap_data = RwReal *
 * environment map coefficient, passed on to material_set_envmap.
 * Returns atomic (the callback return value, non-NULL = keep iterating). (inferred) */
int __cdecl clump_set_envmap(int atomic,void *envmap_data)
{
  FUN_00550190(atomic);
  /* only atomics that have a geometry get their materials updated */
  if (*(RpGeometry **)(atomic + ATOMIC_GEOMETRY_OFFSET) != (RpGeometry *)0x0) {
    RpGeometryForAllMaterials(*(RpGeometry **)(atomic + ATOMIC_GEOMETRY_OFFSET),material_set_envmap,envmap_data);
  }
  return atomic;
}
