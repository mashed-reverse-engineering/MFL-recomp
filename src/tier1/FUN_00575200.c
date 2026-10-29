/* compile: /G5 */
#include "mfl_tier2.h"
/*007d2128*/ extern char *RwEngineInstance;
#define FLD(p, T, off) (*(T *)((char *)(p) + (off)))
/* Releases a reference (count at +0x84); frees the object at zero. NULL-safe, returns TRUE. (Rwp library, proposed name rwp_capsule_00575200) */
int FUN_00575200(void *o)
{
  if (!o) return 0;
  if (--FLD(o, int, 0x84) == 0) RwFree(o);
  return 1;
}
