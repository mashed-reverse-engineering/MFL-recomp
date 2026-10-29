/* compile: /G5 */
#include "mfl_tier2.h"
/*007da72c*/ extern int g_rp_7da72c;
#define FLD(p, T, off) (*(T *)((char *)(p) + (off)))
/* Plugin copy: sets the effect flag to 1 when the source has it set; returns the destination. */
void *lib_54fff0(void *o, void *src)
{
  if (FLD(src, int, g_rp_7da72c)) FLD(o, int, g_rp_7da72c) = 1;
  return o;
}
