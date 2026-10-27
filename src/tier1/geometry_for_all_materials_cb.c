#include "mfl_tier2.h"
/*004f8b60*/ extern struct RpGeometry * RpGeometryForAllMaterials(struct RpGeometry *p0, RpMaterialCallBack p1, void *p2);
#define RP_ATOMIC_GEOMETRY_OFFSET 0x18


/* geometry_for_all_materials_cb */

/* RpAtomic callback that applies a material callback/data pair to all materials of the atomic's geometry. */
/* "RpGeometryForAllMaterials(callback from the pair)" */

int __cdecl geometry_for_all_materials_cb(int atomic,void **callback_pair)
{
  /* Invoke RpGeometryForAllMaterials on the atomic's geometry with the callback and user data */
  RpGeometryForAllMaterials
            (*(RpGeometry **)(atomic + RP_ATOMIC_GEOMETRY_OFFSET),(RpMaterialCallBack)*callback_pair,(void *)callback_pair[1]);
  return atomic;
}
