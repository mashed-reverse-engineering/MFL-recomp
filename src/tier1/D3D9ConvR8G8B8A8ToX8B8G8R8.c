/* compile: /G5 */
#include "mfl_tier2.h"
/* Packs an RGBA pixel as X8B8G8R8 (alpha forced to 0xff). */
unsigned D3D9ConvR8G8B8A8ToX8B8G8R8(unsigned char *p)
{
  return ((((p[2] | 0xffffff00) << 8) | p[1]) << 8) | p[0];
}
