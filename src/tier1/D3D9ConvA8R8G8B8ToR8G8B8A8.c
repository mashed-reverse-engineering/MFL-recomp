/* compile: /G5 */
#include "mfl_tier2.h"
typedef struct RwPix { unsigned char r, g, b, a; } RwPix;
/* Pixel format converter (A8R8G8B8ToR8G8B8A8). */
void D3D9ConvA8R8G8B8ToR8G8B8A8(RwPix *p, unsigned v)
{
  p->r = v >> 16;
  p->b = v;
  p->g = v >> 8;
  p->a = v >> 24;
}
