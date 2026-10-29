/* compile: /G5 */
#include "mfl_tier2.h"
/* Pixel format converter (R8G8B8A8ToA8R3G3B2). */
unsigned short D3D9ConvR8G8B8A8ToA8R3G3B2(unsigned char *p)
{
  unsigned x = (((p[1] & 0xe0) | (p[2] >> 3)) >> 3) | (p[3] << 8);
  return x | (p[0] & 0xe0);
}
