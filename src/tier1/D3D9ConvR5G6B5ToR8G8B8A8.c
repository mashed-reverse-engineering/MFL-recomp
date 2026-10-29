/* compile: /G5 */
#include "mfl_tier2.h"
typedef struct RwPix { unsigned char r, g, b, a; } RwPix;
/* Pixel format converter (R5G6B5ToR8G8B8A8). */
void D3D9ConvR5G6B5ToR8G8B8A8(RwPix *p, unsigned v)
{
  p->a = 0xff;
  p->r = (v >> 8) & 0xf8;
  p->g = (v >> 3) & 0xfc;
  p->b = v << 3;
}
