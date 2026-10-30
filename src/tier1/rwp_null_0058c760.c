/* compile: /G5 */
#include "mfl_tier2.h"
/* Returns 1 when q lies on the non-negative side of the plane through o spanned by r - o and p - o, oriented
   like (r - o) x (p - o); the dot product of the two plane normals decides. (Rwp library) */
int rwp_null_0058c760(RwV3d *p, RwV3d *q, RwV3d *o, RwV3d *r)
{
  RwV3d d, e, f, n, g;

  RwV3dSubMacro(&d, r, o);
  RwV3dSubMacro(&e, p, o);
  RwV3dCrossProductMacro(&n, &d, &e);
  RwV3dSubMacro(&f, q, o);
  RwV3dCrossProductMacro(&g, &d, &f);
  if (RwV3dDotProductMacro(&g, &n) >= 0.0f) return 1;
  return 0;
}
