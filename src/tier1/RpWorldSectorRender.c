/* compile: /G5 */
#include "mfl_tier2.h"
/*007d2128*/ extern char *RwEngineInstance;
typedef void *(__cdecl *V1)(void *);
#define FLD(p, T, off) (*(T *)((char *)(p) + (off)))
/* Renders a world sector through the current world's sector render callback (+0x68).
   Returns the callback's result; WorldSectorRender 0x4f57b0 tests it (VOID_w16: was void). */
void *RpWorldSectorRender(void *sector)
{
  return FLD(FLD(RwEngineInstance, void *, 4), V1, 0x68)(sector);
}
