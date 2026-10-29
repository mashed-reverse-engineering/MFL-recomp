/* compile: /G5 */
#include "mfl_tier2.h"
/*007d2128*/ extern char *RwEngineInstance;
/*007d2184*/ extern int g_rw_7d2184;
#define FLD(p, T, off) (*(T *)((char *)(p) + (off)))
/* Sets the engine auto-mipmap flag; returns TRUE. */
int RwTextureSetAutoMipmapping(int v)
{
  FLD(RwEngineInstance + g_rw_7d2184, int, 0x20) = v;
  return 1;
}
