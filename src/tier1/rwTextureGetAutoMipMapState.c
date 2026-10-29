/* compile: /G5 */
#include "mfl_tier2.h"
/*007d2128*/ extern char *RwEngineInstance;
/*007d2184*/ extern int g_rw_7d2184;
#define FLD(p, T, off) (*(T *)((char *)(p) + (off)))
/* Reads the engine texture plugin's auto-mipmap flag. */
int rwTextureGetAutoMipMapState(void)
{
  return FLD(RwEngineInstance + g_rw_7d2184, int, 0x20);
}
