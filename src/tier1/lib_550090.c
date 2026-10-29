/* compile: /G5 */
#include "mfl_tier2.h"
/*007da72c*/ extern int g_rp_7da72c;
#define FLD(p, T, off) (*(T *)((char *)(p) + (off)))
/* Stream size of the MatFX flag: 4 when set, otherwise 0. */
int lib_550090(void *o)
{
  return FLD(o, int, g_rp_7da72c) ? 4 : 0;
}
