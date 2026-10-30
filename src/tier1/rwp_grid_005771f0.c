/* compile: /G5 */
#include "mfl_tier2.h"
#include <math.h>
extern int rwp_cylinder_0056fd90();
extern float rwp_cylinder_0056fd20();
/* Normalises in into out and returns the length; a zero vector is replaced by dflt scaled by fallback (or a cylinder default when fallback is 0). (Rwp library) */
float rwp_grid_005771f0(RwV3d *out, RwV3d *in, RwV3d *dflt, float fallback)
{
  float l2;
  double recip;

  l2 = RwV3dDotProductMacro(in, in);
  if (l2 > 1e-12f) {
    recip = 1.0f / (float)sqrt(l2);
    out->x = (float)recip * in->x;
    out->y = in->y * recip;
    out->z = in->z * recip;
    return l2 * recip;
  }
  if (fallback == 0.0f) {
    rwp_cylinder_0056fd90(out, dflt);
    rwp_cylinder_0056fd20(out, out);
    return 0.0f;
  }
  RwV3dScaleMacro(out, dflt, fallback);
  rwp_cylinder_0056fd20(out, out);
  return 0.0f;
}
