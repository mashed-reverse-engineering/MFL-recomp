/* compile: /G5 */
#include "mfl_tier2.h"
/*007d2128*/ extern char *RwEngineInstance;
typedef int (__cdecl *FnShow)(void *, void *, int);
extern void __cdecl rwResourcesPurge(void);
#define ENG(off, T) (*(T *)(RwEngineInstance + (off)))
/* Purges resources, then shows the raster through the device callback; returns the raster or NULL. */
void *RwRasterShowRaster(void *r, void *a, int b)
{
  FnShow f = ENG(0x98, FnShow);
  rwResourcesPurge();
  return f(r, a, b) ? r : 0;
}
