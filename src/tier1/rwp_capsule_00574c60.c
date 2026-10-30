/* compile: /G5 */
#include "mfl_tier2.h"
typedef struct { float x, y, z; } RwpV3;
typedef struct RwpShape { char pad[0x20]; RwpV3 center; } RwpShape;
/* Copies the centre of a shape to the output vector; returns 1. (Rwp library) */
int rwp_capsule_00574c60(void *unused, RwpShape *s, int unused2, RwpV3 *out)
{
  *out = s->center;
  return 1;
}
