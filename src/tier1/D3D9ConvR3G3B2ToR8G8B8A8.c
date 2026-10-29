/* compile: /G5 */
#include "mfl_tier2.h"
typedef struct RwPix { unsigned char r, g, b, a; } RwPix;
/* Pixel format converter (R3G3B2ToR8G8B8A8). */
void D3D9ConvR3G3B2ToR8G8B8A8(RwPix *p, unsigned char v)
{
  p->a = 0xff;
  p->r = v & 0xe0;
  p->g = (v & 0xfc) << 3;
  p->b = v << 6;
}
