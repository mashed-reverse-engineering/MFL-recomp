/* compile: /G5 */
#include "mfl_tier2.h"
typedef struct RwPix { unsigned char r, g, b, a; } RwPix;
/* Pixel format converter (X4R4G4B4ToR8G8B8A8). */
void D3D9ConvX4R4G4B4ToR8G8B8A8(RwPix *p, unsigned v)
{
  p->a = 0xff;
  p->r = (v >> 4) & 0xf0;
  p->g = v & 0xf0;
  p->b = v << 4;
}
