/* compile: /G5 */
#include "mfl_tier2.h"
/* Packs an RGBA pixel as X8R8G8B8 (alpha forced to 0xff). */
unsigned D3D9ConvR8G8B8A8ToX8R8G8B8(unsigned char *p)
{
  return ((((p[0] | 0xffffff00) << 8) | p[1]) << 8) | p[2];
}
