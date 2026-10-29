/* compile: /G5 */
#include "mfl_tier2.h"
typedef struct RwPix { unsigned char r, g, b, a; } RwPix;
/* Pixel format converter (X8B8G8R8ToR8G8B8A8). */
void D3D9ConvX8B8G8R8ToR8G8B8A8(RwPix *p, unsigned v)
{
  p->r = v;
  p->a = 255;
  p->g = v >> 8;
  p->b = v >> 16;
}
