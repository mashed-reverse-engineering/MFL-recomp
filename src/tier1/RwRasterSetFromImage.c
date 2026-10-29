/* compile: /G5 */
#include "mfl_tier2.h"
/*007d2128*/ extern char *RwEngineInstance;
typedef int (__cdecl *FnI3)(void *, void *, int);
#define FLD(p, T, off) (*(T *)((char *)(p) + (off)))
#define ENG(off, T) (*(T *)(RwEngineInstance + (off)))
/* Copies an image into a raster through the device callback; marks the raster alpha flag when the image has alpha. */
void *RwRasterSetFromImage(void *r, void *img)
{
  if (((FnI3)ENG(0x64, void *))(r, img, 0)) { if (FLD(img, unsigned char, 0) & 2) FLD(r, unsigned char, 0x22) |= 1; return r; }
  return 0;
}
