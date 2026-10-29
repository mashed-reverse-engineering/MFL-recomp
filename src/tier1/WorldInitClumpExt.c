/* compile: /G5 */
#include "mfl_tier2.h"
/*007d2128*/ extern char *RwEngineInstance;
/*007d52a4*/ extern int g_rp_7d52a4;
/*007d5294*/ extern int g_rp_7d5294;
#define FLD(p, T, off) (*(T *)((char *)(p) + (off)))
/* Clump plugin constructor: clears the world link and copies the default LOD value from the engine. */
void *WorldInitClumpExt(void *c)
{
  char *e = (char *)c + g_rp_7d52a4;
  FLD(e, int, 0) = 0;
  FLD(e, int, 4) = FLD(RwEngineInstance + g_rp_7d5294, int, 8);
  return c;
}
