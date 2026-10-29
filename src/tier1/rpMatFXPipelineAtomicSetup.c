/* compile: /G5 */
#include "mfl_tier2.h"
/*007da734*/ extern int g_rp_7da734;
#define FLD(p, T, off) (*(T *)((char *)(p) + (off)))
/* Stores the MatFX atomic pipeline in the atomic's pipeline field. */
void rpMatFXPipelineAtomicSetup(void *a)
{
  FLD(a, int, 0x6c) = g_rp_7da734;
}
