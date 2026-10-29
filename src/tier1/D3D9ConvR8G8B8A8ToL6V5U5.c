/* compile: /G5 */
#include "mfl_tier2.h"
/* Pixel format converter (R8G8B8A8ToL6V5U5). */
unsigned short D3D9ConvR8G8B8A8ToL6V5U5(unsigned char *p)
{
  return ((((p[2] & 0xfc) << 6) | (p[1] & 0xf8)) << 2) | (p[0] >> 3);
}
