/* compile: /G5 */
#include "mfl_tier2.h"
/*007da730*/ extern int g_rp_7da730;
#define FLD(p, T, off) (*(T *)((char *)(p) + (off)))
/* Stream size of the second MatFX flag: 4 when set, otherwise 0. */
int lib_550170(void *o)
{
  return FLD(o, int, g_rp_7da730) ? 4 : 0;
}
