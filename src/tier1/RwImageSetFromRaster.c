/* compile: /G5 */
#include "mfl_tier2.h"
/*007d2128*/ extern char *RwEngineInstance;
typedef int (__cdecl *FnI3)(void *, void *, int);
#define FLD(p, T, off) (*(T *)((char *)(p) + (off)))
#define ENG(off, T) (*(T *)(RwEngineInstance + (off)))
/* Copies a raster into an image through the device callback; copies the alpha flag; returns the image or NULL. */
void *RwImageSetFromRaster(void *img, void *r)
{
  if (((FnI3)ENG(0x60, void *))(img, r, 0)) { if (FLD(r, unsigned char, 0x22) & 1) FLD(img, unsigned, 0) |= 2; return img; }
  return 0;
}
