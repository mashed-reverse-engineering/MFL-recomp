/* compile: /G5 */
#include "mfl_tier2.h"
/*007d2128*/ extern char *RwEngineInstance;
typedef int (__cdecl *FnI3)(void *, void *, int);
extern void (__cdecl *g_rx_7d53c4)(void);
#define ENG(off, T) (*(T *)(RwEngineInstance + (off)))
/* Locks a raster level through the device callback; returns the pixel pointer or NULL. */
void *RwRasterLock(void *r, int level, int mode)
{
  void *px;
  return ((FnI3)ENG(0x84, void *))(&px, r, ((level & 0xff) << 8) + mode) ? px : 0;
}
