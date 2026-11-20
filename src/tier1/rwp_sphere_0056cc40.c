/* compile: /G5 */
#include "mfl_tier2.h"
typedef struct { int x, y, z; } RwpZero3;
/* Clears an output vector (support direction of a point shape) and returns it, as the original does in EAX (shape vtable
   slot; v143 left another value there when this was `void`, agents/hang_w16/notes.md). (Rwp library) */
RwpZero3 *rwp_sphere_0056cc40(int a, int b, RwpZero3 *out)
{
  out->z = 0;
  out->y = 0;
  out->x = 0;
  return out;
}
