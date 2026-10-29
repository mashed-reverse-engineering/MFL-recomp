/* compile: /G5 */
#include "mfl_tier2.h"
typedef struct RwPix { unsigned char r, g, b, a; } RwPix;
/* Pixel format converter (R8G8B8A8ToR5G6B5). */
unsigned D3D9ConvR8G8B8A8ToR5G6B5(RwPix *p)
{
  return ((((p->r & 0xf8) << 5) | (p->g & 0xfc)) << 3) | (p->b >> 3);
}
