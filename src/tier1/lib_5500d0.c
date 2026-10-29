/* compile: /G5 */
#include "mfl_tier2.h"
/*007da730*/ extern int g_rp_7da730;
#define FLD(p, T, off) (*(T *)((char *)(p) + (off)))
/* Plugin copy: sets the second effect flag to 1 when the source has it set; returns the destination. */
void *lib_5500d0(void *o, void *src)
{
  if (FLD(src, int, g_rp_7da730)) FLD(o, int, g_rp_7da730) = 1;
  return o;
}
