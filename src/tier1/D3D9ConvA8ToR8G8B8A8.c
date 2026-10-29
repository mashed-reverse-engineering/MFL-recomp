/* compile: /G5 */
#include "mfl_tier2.h"
typedef struct RwPix { unsigned char r, g, b, a; } RwPix;
/* Pixel format converter (A8ToR8G8B8A8). */
void D3D9ConvA8ToR8G8B8A8(RwPix *p, unsigned char v)
{
  p->r = 255;
  p->g = 255;
  p->b = 255;
  p->a = v;
}
