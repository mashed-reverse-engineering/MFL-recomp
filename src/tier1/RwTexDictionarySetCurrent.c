/* compile: /G5 */
#include "mfl_tier2.h"
/*007d2128*/ extern char *RwEngineInstance;
/*007d2184*/ extern int g_rw_7d2184;
#define FLD(p, T, off) (*(T *)((char *)(p) + (off)))
/* Sets the current texture dictionary in the engine texture plugin. */
void RwTexDictionarySetCurrent(void *d)
{
  FLD(RwEngineInstance + g_rw_7d2184, void *, 0x10) = d;
}
