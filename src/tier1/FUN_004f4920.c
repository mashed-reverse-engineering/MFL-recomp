/* compile: /G5 */
#include "mfl_tier2.h"
/*007d52a4*/ extern int g_rp_7d52a4;
#define FLD(p, T, off) (*(T *)((char *)(p) + (off)))
/* Plugin data of an object at the world-plugin offset (second world plugin slot). */
void *FUN_004f4920(void *o)
{
  return FLD(o, void *, g_rp_7d52a4);
}
