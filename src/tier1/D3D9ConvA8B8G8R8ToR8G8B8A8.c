/* compile: /G5 */
#include "mfl_tier2.h"
typedef struct RwPix { unsigned char r, g, b, a; } RwPix;
/* Pixel format converter (A8B8G8R8ToR8G8B8A8). */
void D3D9ConvA8B8G8R8ToR8G8B8A8(RwPix *p, unsigned v)
{
  p->g = v >> 8;
  p->r = v;
  p->b = v >> 16;
  p->a = v >> 24;
}
