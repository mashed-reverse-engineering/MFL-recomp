/* compile: /G5 */
#include "mfl_tier2.h"
/*007d2128*/ extern char *RwEngineInstance;
typedef int (__cdecl *FnI3)(void *, void *, int);
#define FLD(p, T, off) (*(T *)((char *)(p) + (off)))
#define ENG(off, T) (*(T *)(RwEngineInstance + (off)))
/* Unlocks the palette through the device callback and clears the palette-lock bits of the raster flags. */
void *RwRasterUnlockPalette(void *r)
{
  FnI3 f = ENG(0xa8, FnI3);
  f(0, r, 0);
  FLD(r, unsigned char, 0x22) &= 0xe7;
  return r;
}
