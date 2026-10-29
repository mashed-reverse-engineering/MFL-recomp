/* compile: /G5 */
#include "mfl_tier2.h"
typedef struct RwPix { unsigned char r, g, b, a; } RwPix;
/* Pixel format converter (L6V5U5ToR8G8B8A8). */
void D3D9ConvL6V5U5ToR8G8B8A8(RwPix *p, unsigned v)
{
  p->r = v << 3;
  p->g = (v >> 2) & 0xf8;
  p->b = (v >> 8) & 0xfc;
  p->a = 0xff;
}
