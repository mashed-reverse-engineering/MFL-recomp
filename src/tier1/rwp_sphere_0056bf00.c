/* compile: /G5 */
#include "mfl_tier2.h"
extern void rwp_sphere_0056c0a0(void *shape);
/* Releases the shape that a reference points to (tail call). (Rwp library) */
void rwp_sphere_0056bf00(void **shape_ref)
{
  rwp_sphere_0056c0a0(*shape_ref);
}
