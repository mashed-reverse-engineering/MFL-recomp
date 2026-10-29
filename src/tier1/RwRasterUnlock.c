/* compile: /G5 */
#include "mfl_tier2.h"
/*007d2128*/ extern char *RwEngineInstance;
typedef int (__cdecl *FnI3)(void *, void *, int);
#define ENG(off, T) (*(T *)(RwEngineInstance + (off)))
/* Unlocks a raster through the device callback; returns the raster. */
void *RwRasterUnlock(void *r)
{
  FnI3 f = ENG(0x88, FnI3);
  f(0, r, 0);
  return r;
}
