/* compile: /G5 */
#include "mfl_tier2.h"
typedef struct RwPix { unsigned char r, g, b, a; } RwPix;
/* Pixel format converter (A4L4ToR8G8B8A8). */
void D3D9ConvA4L4ToR8G8B8A8(RwPix *p, unsigned v)
{
  p->r = v << 4;
  p->g = v << 4;
  p->b = v << 4;
  if (v >= 0xf0) p->a = 255;
  else p->a = v & 0xf0;
}
