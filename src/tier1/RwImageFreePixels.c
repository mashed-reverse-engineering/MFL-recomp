/* compile: /G5 */
#include "mfl_tier2.h"
/*007d2128*/ extern char *RwEngineInstance;
#define FLD(p, T, off) (*(T *)((char *)(p) + (off)))
/* Frees the pixel buffer of an image and clears the pixel pointer, stride and the own-pixels flag. */
void *RwImageFreePixels(void *img)
{
  RwFree(FLD(img, void *, 0x14));
  FLD(img, int, 0x14) = 0;
  FLD(img, int, 0x18) = 0;
  FLD(img, unsigned, 0) &= ~1u;
  return img;
}
