/* compile: /G5 */
#include "mfl_tier2.h"
/*007da87c*/ extern int g_rp_7da87c;
#define FLD(p, T, off) (*(T *)((char *)(p) + (off)))
/* Stores the patch pipeline in the atomic's pipeline field. */
void rpPatchPipelinesAttach(void *a)
{
  FLD(a, int, 0x6c) = g_rp_7da87c;
}
