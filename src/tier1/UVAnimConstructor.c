/* compile: /G5 */
#include "mfl_tier2.h"
/*00947b58*/ extern int g_rp_947b58;
/* Atomic plugin constructor: clears the 0x28 byte UV animation data; returns the object. */
void *UVAnimConstructor(void *o)
{
  memset((char *)o + g_rp_947b58, 0, 0x28);
  return o;
}
