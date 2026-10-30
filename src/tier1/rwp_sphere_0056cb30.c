/* compile: /G5 */
#include "mfl_tier2.h"
typedef struct RwpShape { char pad[0x4c]; int extent; } RwpShape;
typedef struct RwpSupport { int x, y, z; int extent; } RwpSupport;
/* Support point of a point shape: zero vector plus the shape's extent. (Rwp library) */
void rwp_sphere_0056cb30(RwpShape *s, RwpSupport *out)
{
  out->z = 0;
  out->y = 0;
  out->x = 0;
  out->extent = s->extent;
}
