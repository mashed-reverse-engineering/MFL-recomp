#include "mfl_tier2.h"
/* Dot product of two vectors, clamped to [-1, 1] (RenderWare vector macro). */
float dot_product_3d_clamped(RwV3d *a, RwV3d *b)
{
  float d;
  float r;

  d = RwV3dDotProductMacro(a, b);
  if (d < -1.0f) {
    return -1.0f;
  }
  if (d > 1.0f) {
    r = 1.0f;
  } else {
    r = d;
  }
  return r;
}
