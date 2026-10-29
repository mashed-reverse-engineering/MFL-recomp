/* compile: /G5 */
#include "mfl_tier2.h"
/* Pixel format converter (R8G8B8A8ToV8U8). */
unsigned D3D9ConvR8G8B8A8ToV8U8(unsigned char *p)
{
  return p[0] | (p[1] << 8);
}
