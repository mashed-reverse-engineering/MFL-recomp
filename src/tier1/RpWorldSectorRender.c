/* compile: /G5 */
#include "mfl_tier2.h"
/*007d2128*/ extern char *RwEngineInstance;
typedef void (__cdecl *V1)(void *);
#define FLD(p, T, off) (*(T *)((char *)(p) + (off)))
/* Renders a world sector through the current world's sector render callback (+0x68). */
void RpWorldSectorRender(void *sector)
{
  FLD(FLD(RwEngineInstance, void *, 4), V1, 0x68)(sector);
}
