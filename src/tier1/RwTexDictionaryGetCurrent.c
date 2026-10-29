/* compile: /G5 */
#include "mfl_tier2.h"
/*007d2128*/ extern char *RwEngineInstance;
/*007d2184*/ extern int g_rw_7d2184;
#define FLD(p, T, off) (*(T *)((char *)(p) + (off)))
/* Current texture dictionary from the engine texture plugin. */
void *RwTexDictionaryGetCurrent(void)
{
  return FLD(RwEngineInstance + g_rw_7d2184, void *, 0x10);
}
