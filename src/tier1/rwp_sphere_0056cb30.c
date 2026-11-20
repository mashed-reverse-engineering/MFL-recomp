/* compile: /G5 */
#include "mfl_tier2.h"
typedef struct RwpShape { char pad[0x4c]; int extent; } RwpShape;
typedef struct RwpSupport { int x, y, z; int extent; } RwpSupport;
/* Bounding sphere of a point shape: zero centre plus the shape's extent, and returns it.  Shape vtable slot +0x8: the caller
   (0x56c0b0) tests the result and reads the sphere through it, so the original's EAX = out is the return value; as `void` the
   v143 build returned the extent bits there (ret_scan.py FPTR row, agents/det_w17/notes.md). (Rwp library) */
RwpSupport *rwp_sphere_0056cb30(RwpShape *s, RwpSupport *out)
{
  out->z = 0;
  out->y = 0;
  out->x = 0;
  out->extent = s->extent;
  return out;
}
