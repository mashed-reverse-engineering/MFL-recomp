/* compile: /G5 */
#include "mfl_tier2.h"
typedef struct RwPix { unsigned char r, g, b, a; } RwPix;
/* Pixel format converter (A8L8ToR8G8B8A8). */
void D3D9ConvA8L8ToR8G8B8A8(RwPix *p, unsigned v)
{
  p->r = v;
  p->g = v;
  p->b = v;
  p->a = v >> 8;
}
