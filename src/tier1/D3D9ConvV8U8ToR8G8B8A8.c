/* compile: /G5 */
#include "mfl_tier2.h"
typedef struct RwPix { unsigned char r, g, b, a; } RwPix;
/* Pixel format converter (V8U8ToR8G8B8A8). */
void D3D9ConvV8U8ToR8G8B8A8(RwPix *p, unsigned v)
{
  p->r = v;
  p->b = 0;
  p->g = v >> 8;
  p->a = 255;
}
