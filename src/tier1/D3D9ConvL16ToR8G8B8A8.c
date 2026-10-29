/* compile: /G5 */
#include "mfl_tier2.h"
typedef struct RwPix { unsigned char r, g, b, a; } RwPix;
/* Pixel format converter (L16ToR8G8B8A8). */
void D3D9ConvL16ToR8G8B8A8(RwPix *p, unsigned v)
{
  v >>= 8;
  p->r = v;
  p->g = v;
  p->b = v;
  p->a = 255;
}
