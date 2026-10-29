/* compile: /G5 */
#include "mfl_tier2.h"
/* Packs an RGBA pixel as A8B8G8R8. */
unsigned D3D9ConvR8G8B8A8ToA8B8G8R8(unsigned char *p)
{
  return ((((p[3] << 8) | p[2]) << 8 | p[1]) << 8) | p[0];
}
